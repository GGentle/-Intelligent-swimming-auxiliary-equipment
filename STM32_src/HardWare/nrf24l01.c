/*
所用的硬件：SPI1和GPIO
SPI1：PA5(SCK)，PA6(MISO)，PA7(MOSI)
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

//初始化硬件SPI1
void spi1_init()
{
	//开启时钟
	//GPIOA时钟和SPI1时钟
	RCC->APB2ENR |= 1<<12;
	RCC->APB2ENR |= 1<<2;
	
	GPIOA->CRL&=0X000FFFFF; 
	GPIOA->CRL|=0XBBB00000;//PA5.6.7复用 	    
	GPIOA->ODR|=0X7<<5;    //PA5.6.7上拉
	
	//SPI参数设置
	SPI1->CR1|=0<<10;//全双工模式	
	SPI1->CR1|=1<<9; //软件nss管理
	SPI1->CR1|=1<<8;  
	
	SPI1->CR1|=1<<2; //SPI主机
	SPI1->CR1|=0<<11;//8bit数据格式	
	SPI1->CR1|=1<<1; //空闲模式下SCK为1 CPOL=1
	SPI1->CR1|=1<<0; //数据采样从第二个时间边沿开始,CPHA=1  
	SPI1->CR1|=7<<3; //Fsck=Fcpu/256
	SPI1->CR1|=0<<7; //MSBfirst   
	SPI1->CR1|=1<<6; //SPI设备使能
}

//SPI1 速度设置函数
//SpeedSet:0~7
//SPI速度=fAPB2/2^(SpeedSet+1)
//APB2时钟一般为72Mhz
void SPI1_SetSpeed(u8 SpeedSet)
{
	SpeedSet&=0X07;			//限制范围
	SPI1->CR1&=0XFFC7; 
	SPI1->CR1|=SpeedSet<<3;	//设置SPI1速度  
	SPI1->CR1|=1<<6; 		//SPI设备使能 
} 

//SPI1读写函数
u8 spi1_rw(u8 data)
{
	u16 t = 0;
	while((SPI1->SR & 1<<1) == 0)//发送数据
	{
		t++;
		if(t>0xfffe)
			return 0;
	}
	SPI1->DR = data;
	t = 0;
	while((SPI1->SR & 1<<0) == 0)//读取数据
	{
		t++;
		if(t>0xfffe)
			return 0;
	}
	data = SPI1->DR;
	
	return data;
}

//NRF初始化函数
void nrf_init()
{
	spi1_init();
	SPI1_SetSpeed(SPI_SPEED_8); //spi速度为9Mhz（24L01的最大SPI时钟为10Mhz） 
	RCC->APB2ENR |= 1<<2;
	RCC->APB2ENR |= 1<<4;
	
	GPIOA->CRL &= 0XFFF0FFFF;
	GPIOA->CRL |= 0X00030000;
	GPIOA->ODR |= 1<<4;
	
	GPIOC->CRL &= 0XFFF0FFFF;
	GPIOC->CRL |= 0X00030000;
	GPIOC->ODR |= 1<<4;
	
	//针对NRF的特点修改SPI的设置
 	SPI1->CR1&=~(1<<6); 	//SPI设备失能
	SPI1->CR1&=~(1<<1); 	//空闲模式下SCK为0 CPOL=0
	SPI1->CR1&=~(1<<0); 	//数据采样从第1个时间边沿开始,CPHA=0  
	SPI1->CR1|=1<<6; 		//SPI设备使能   
	nrf_ce=0; 	//使能24L01
	nrf_csn=1;	//SPI片选取消
}
/*------------------分界线---------------------*/
//NRF读写函数
//通过SPI1实现
u8 nrf_rw(u8 data)
{
	u8 dat;	
	dat = spi1_rw(data);
	return dat;
}

//NRF写寄存器函数
void nrf_writereg(u8 reg,u8 data)
{
	nrf_csn = 0;
	
	nrf_rw(reg);
	nrf_rw(data);
	
	nrf_csn = 1;
}

//NRF读寄存器数据函数
u8 nrf_readdata(u8 reg)
{
	u8 data;
	nrf_csn = 0;
	
	nrf_rw(reg);
	data = nrf_rw(0);
	
	nrf_csn = 1;

	return data;
}

//写多个字节的命令
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

//读多个字节的数据
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

//检测NRF是否存在
//若存在  返回0
//不存在  返回1
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

//发送模式
//并且发送一次数据
void tx_mode()
{
	u8 output[] = {"tx_"};
	u8 i;
	
	delay_ms(10);

//	LCD_ShowString(10,10,20,4,16,output);//调试信息
	
	nrf_ce = 0;
	nrf_writendata(NRF_WRITE_REG+TX_ADDR,(u8*)tx_addr,TX_ADR_WIDTH);//设置发送的地址
	nrf_writendata(WR_TX_PLOAD,tx_buf,5);							//写数据到发送的寄存器当中(可以删除)
	nrf_writereg(NRF_WRITE_REG+EN_AA,0x00);							//除能自动应答
	nrf_writereg(NRF_WRITE_REG + RF_CH,40);							//设置工作通道频率（跟接收端统一）
	nrf_writereg(NRF_WRITE_REG + RF_SETUP,0x07);					//设置发射功率（跟接收端统一）
	nrf_writereg(NRF_WRITE_REG+CONFIG,0x02);						//设置成发送模式
	nrf_ce = 1;
}

//发送数据
void tx_send()
{
	nrf_ce = 0;
	nrf_writendata(WR_TX_PLOAD,(u8 *)tx_buf,TX_PLOAD_WIDTH);        //发送tx_buf数组里面的TX_PLOAD_WIDTH个字节
	nrf_ce = 1;
//	LCD_ShowNum(80,200,11,4,16);//调试信息
}

//接收模式
void rx_mode()
{
	u8 output[] = {"rx_"};
//	LCD_ShowString(10,10,20,4,16,output);//调试信息
	nrf_ce = 0;
	nrf_writereg(NRF_WRITE_REG + CONFIG, 0x01);
	nrf_ce = 1;
	delay_ms(20);
	
	nrf_ce = 0;
	nrf_writendata(NRF_WRITE_REG + RX_ADDR_P0, tx_addr, TX_ADR_WIDTH);		//接收通道RX_ADDR_P0的地址
	nrf_writereg(NRF_WRITE_REG + EN_AA, 0x00);      						//除能自动应答
  	nrf_writereg(NRF_WRITE_REG + EN_RXADDR, 0x01); 							//允许通道0接收
  	nrf_writereg(NRF_WRITE_REG + RF_CH, 40);       							//设置工作通道频率（跟发送端统一）
  	nrf_writereg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);					//设置通道RX_PW_P0接收数据的长度为RX_PLOAD_WIDTH字节
  	nrf_writereg(NRF_WRITE_REG + RF_SETUP, 0x07);  							//设置发射功率（跟发送端统一）
  	nrf_writereg(NRF_WRITE_REG + CONFIG, 0x03);    			
	nrf_ce = 1;
	delay_ms(20);
}

//接收数据
u8 rx_read()
{
//	while(!(nrf_readdata(NRF_READ_REG + STATUS)&0x40));//检查是否接收到数据,收到数据才跳过循环
//		nrf_writereg(NRF_WRITE_REG+STATUS,0x40);           
		
	nrf_readndata(RD_RX_PLOAD,rx_buf,RX_PLOAD_WIDTH);  //读取数据
	
//	if((nrf_readdata(NRF_READ_REG + STATUS)&0x40) == 1)//接收到数据
//	{
//		nrf_writereg(NRF_WRITE_REG+STATUS,0x40);		   //清空状态寄存器该位为0
//		nrf_readndata(RD_RX_PLOAD,rx_buf,RX_PLOAD_WIDTH);  //读取数据
//		rx_time++;										   //读取数据次数加1
//		return 1;
//	}
//	else//没有接收到数据
//	{
//		return 0;
//	}
}

//把3个浮点型的数据转换为5个字符型的数据然后发送
void FloatToChar()
{	
	tx_buf[0] = DATA[0];//小数点前
    tx_buf[1] = DATA[1];//小数点前
    tx_buf[2] = (int)(DATA[1]*100)%100;//小数点后两位
    tx_buf[3] = DATA[2];//小数点前
    tx_buf[4] = (int)(DATA[2]*100)%100;//小数点后两位
}
















