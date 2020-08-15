/*
���õ�Ӳ����SPI1��GPIO
SPI1��PA5(SCK)��PA6(MISO)��PA7(MOSI)
GPIO:PA4(NRF_CE) PC4(NRF_CS)
*/

#include "nrf24l01.h"
//#include "lcd.h"
#include "delay.h"

static u8 tx_addr[TX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};

u8 tx_buf[TX_PLOAD_WIDTH];
u8 rx_buf[RX_PLOAD_WIDTH];
float DATA[3] = {28.5,11.22,33.44};
u8 rx_time = 0;

//��ʼ��Ӳ��SPI1
void spi1_init()
{
	//����ʱ��
	//GPIOAʱ�Ӻ�SPI1ʱ��
	RCC->APB2ENR |= 1<<12;
	RCC->APB2ENR |= 1<<2;
	
	GPIOA->CRL&=0X000FFFFF; 
	GPIOA->CRL|=0XBBB00000;//PA5.6.7���� 	    
	GPIOA->ODR|=0X7<<5;    //PA5.6.7����
	
	//SPI��������
	SPI1->CR1|=0<<10;//ȫ˫��ģʽ	
	SPI1->CR1|=1<<9; //���nss����
	SPI1->CR1|=1<<8;  
	
	SPI1->CR1|=1<<2; //SPI����
	SPI1->CR1|=0<<11;//8bit���ݸ�ʽ	
	SPI1->CR1|=1<<1; //����ģʽ��SCKΪ1 CPOL=1
	SPI1->CR1|=1<<0; //���ݲ����ӵڶ���ʱ����ؿ�ʼ,CPHA=1  
	SPI1->CR1|=7<<3; //Fsck=Fcpu/256
	SPI1->CR1|=0<<7; //MSBfirst   
	SPI1->CR1|=1<<6; //SPI�豸ʹ��
}

//SPI1 �ٶ����ú���
//SpeedSet:0~7
//SPI�ٶ�=fAPB2/2^(SpeedSet+1)
//APB2ʱ��һ��Ϊ72Mhz
void SPI1_SetSpeed(u8 SpeedSet)
{
	SpeedSet&=0X07;			//���Ʒ�Χ
	SPI1->CR1&=0XFFC7; 
	SPI1->CR1|=SpeedSet<<3;	//����SPI1�ٶ�  
	SPI1->CR1|=1<<6; 		//SPI�豸ʹ�� 
} 

//SPI1��д����
u8 spi1_rw(u8 data)
{
	u16 t = 0;
	while((SPI1->SR & 1<<1) == 0)//��������
	{
		t++;
		if(t>0xfffe)
			return 0;
	}
	SPI1->DR = data;
	t = 0;
	while((SPI1->SR & 1<<0) == 0)//��ȡ����
	{
		t++;
		if(t>0xfffe)
			return 0;
	}
	data = SPI1->DR;
	
	return data;
}

//NRF��ʼ������
void nrf_init()
{
	spi1_init();
	SPI1_SetSpeed(SPI_SPEED_8); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz�� 
	RCC->APB2ENR |= 1<<2;
	RCC->APB2ENR |= 1<<4;
	
	GPIOA->CRL &= 0XFFF0FFFF;
	GPIOA->CRL |= 0X00030000;
	GPIOA->ODR |= 1<<4;
	
	GPIOC->CRL &= 0XFFF0FFFF;
	GPIOC->CRL |= 0X00030000;
	GPIOC->ODR |= 1<<4;
	
	//���NRF���ص��޸�SPI������
 	SPI1->CR1&=~(1<<6); 	//SPI�豸ʧ��
	SPI1->CR1&=~(1<<1); 	//����ģʽ��SCKΪ0 CPOL=0
	SPI1->CR1&=~(1<<0); 	//���ݲ����ӵ�1��ʱ����ؿ�ʼ,CPHA=0  
	SPI1->CR1|=1<<6; 		//SPI�豸ʹ��   
	nrf_ce=0; 	//ʹ��24L01
	nrf_csn=1;	//SPIƬѡȡ��
}
/*------------------�ֽ���---------------------*/
//NRF��д����
//ͨ��SPI1ʵ��
u8 nrf_rw(u8 data)
{
	u8 dat;	
	dat = spi1_rw(data);
	return dat;
}

//NRFд�Ĵ�������
void nrf_writereg(u8 reg,u8 data)
{
	nrf_csn = 0;
	
	nrf_rw(reg);
	nrf_rw(data);
	
	nrf_csn = 1;
}

//NRF���Ĵ������ݺ���
u8 nrf_readdata(u8 reg)
{
	u8 data;
	nrf_csn = 0;
	
	nrf_rw(reg);
	data = nrf_rw(0);
	
	nrf_csn = 1;

	return data;
}

//д����ֽڵ�����
void nrf_writendata(u8 reg,u8* buf,u8 num)
{
	u8 t;
	nrf_csn = 0;
	
	nrf_rw(reg);
	for(t = 0;t<num;t++)
	{
		nrf_rw(buf[t]);
	}
	
	nrf_csn = 1;
}

//������ֽڵ�����
void nrf_readndata(u8 reg,u8 *buf,u8 num)
{
	u8 t;
	nrf_csn = 0;
	
	nrf_rw(reg);
	for(t=0;t<num;t++)
	{
		buf[t] = nrf_rw(0);
	}
	
	nrf_csn = 1;
}

//���NRF�Ƿ����
//������  ����0
//������  ����1
u8 nrf_check()
{
	u8 addr_ck[TX_ADR_WIDTH] = {0x55,0x55,0x55,0x55,0x55};
	u8 addr[TX_ADR_WIDTH];
	u8 t;
	
	nrf_writendata(NRF_WRITE_REG + TX_ADDR,addr_ck,TX_ADR_WIDTH);
	nrf_readndata(NRF_READ_REG + TX_ADDR,addr,TX_ADR_WIDTH);
	for(t=0;t<TX_ADR_WIDTH;t++)
	{
		if(addr[t] != 0x55)
		{
			break;
		}
	}
	
	if(t == 5)
	{
		return 0;
	}
	else 
	{
		return 1;
	}
		
}

//����ģʽ
//���ҷ���һ������
void tx_mode()
{
	u8 output[] = {"tx_"};
	u8 i;
	
	delay_ms(10);

//	LCD_ShowString(10,10,20,4,16,output);//������Ϣ
	
	nrf_ce = 0;
	nrf_writendata(NRF_WRITE_REG+TX_ADDR,(u8*)tx_addr,TX_ADR_WIDTH);//���÷��͵ĵ�ַ
	nrf_writendata(WR_TX_PLOAD,tx_buf,5);							//д���ݵ����͵ļĴ�������(����ɾ��)
	nrf_writereg(NRF_WRITE_REG+EN_AA,0x00);							//�����Զ�Ӧ��
	nrf_writereg(NRF_WRITE_REG + RF_CH,40);							//���ù���ͨ��Ƶ�ʣ������ն�ͳһ��
	nrf_writereg(NRF_WRITE_REG + RF_SETUP,0x07);					//���÷��书�ʣ������ն�ͳһ��
	nrf_writereg(NRF_WRITE_REG+CONFIG,0x02);						//���óɷ���ģʽ
	nrf_ce = 1;
}

//��������
void tx_send()
{
	nrf_ce = 0;
	nrf_writendata(WR_TX_PLOAD,(u8 *)tx_buf,TX_PLOAD_WIDTH);        //����tx_buf���������TX_PLOAD_WIDTH���ֽ�
	nrf_ce = 1;
//	LCD_ShowNum(80,200,11,4,16);//������Ϣ
}

//����ģʽ
void rx_mode()
{
	u8 output[] = {"rx_"};
//	LCD_ShowString(10,10,20,4,16,output);//������Ϣ
	nrf_ce = 0;
	nrf_writereg(NRF_WRITE_REG + CONFIG, 0x01);
	nrf_ce = 1;
	delay_ms(20);
	
	nrf_ce = 0;
	nrf_writendata(NRF_WRITE_REG + RX_ADDR_P0, tx_addr, TX_ADR_WIDTH);		//����ͨ��RX_ADDR_P0�ĵ�ַ
	nrf_writereg(NRF_WRITE_REG + EN_AA, 0x00);      						//�����Զ�Ӧ��
  	nrf_writereg(NRF_WRITE_REG + EN_RXADDR, 0x01); 							//����ͨ��0����
  	nrf_writereg(NRF_WRITE_REG + RF_CH, 40);       							//���ù���ͨ��Ƶ�ʣ������Ͷ�ͳһ��
  	nrf_writereg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);					//����ͨ��RX_PW_P0�������ݵĳ���ΪRX_PLOAD_WIDTH�ֽ�
  	nrf_writereg(NRF_WRITE_REG + RF_SETUP, 0x07);  							//���÷��书�ʣ������Ͷ�ͳһ��
  	nrf_writereg(NRF_WRITE_REG + CONFIG, 0x03);    			
	nrf_ce = 1;
	delay_ms(20);
}

//��������
u8 rx_read()
{
//	while(!(nrf_readdata(NRF_READ_REG + STATUS)&0x40));//����Ƿ���յ�����,�յ����ݲ�����ѭ��
//		nrf_writereg(NRF_WRITE_REG+STATUS,0x40);           
		
	nrf_readndata(RD_RX_PLOAD,rx_buf,RX_PLOAD_WIDTH);  //��ȡ����
	
//	if((nrf_readdata(NRF_READ_REG + STATUS)&0x40) == 1)//���յ�����
//	{
//		nrf_writereg(NRF_WRITE_REG+STATUS,0x40);		   //���״̬�Ĵ�����λΪ0
//		nrf_readndata(RD_RX_PLOAD,rx_buf,RX_PLOAD_WIDTH);  //��ȡ����
//		rx_time++;										   //��ȡ���ݴ�����1
//		return 1;
//	}
//	else//û�н��յ�����
//	{
//		return 0;
//	}
}

//��3�������͵�����ת��Ϊ5���ַ��͵�����Ȼ����
void FloatToChar()
{	
	tx_buf[0] = DATA[0];//С����ǰ
    tx_buf[1] = DATA[1];//С����ǰ
    tx_buf[2] = (int)(DATA[1]*100)%100;//С�������λ
    tx_buf[3] = DATA[2];//С����ǰ
    tx_buf[4] = (int)(DATA[2]*100)%100;//С�������λ
}
















