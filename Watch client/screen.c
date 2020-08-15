#include "screen.h"
#include "tft.h"
#include "nrf24l01.h"
#include <intrins.h>
#include "api.h"



uchar speed_num=0;//�����ٶȵĵ�λ
uchar up_data=0;//���Ƹ������ݵĳ���
uchar speed_alive=0;//�����ٶȸ��µĳ���
uchar direaction_alive=0;//���Ʒ�����ڳ���
uchar function_alive=0;//���ƹ�������ʹ��
uchar font=1;//������ҳ�治��ɨ��



uchar const TX_ADDRESS[TX_ADR_WIDTH]  = {0x34,0x43,0x10,0x10,0x01}; // ����TXָ��

uchar rx_buf[TX_PLOAD_WIDTH];
uchar tx_buf[TX_PLOAD_WIDTH];

uchar bdata sta;
sbit	RX_DR	=sta^6;
sbit	TX_DS	=sta^5;
sbit	MAX_RT	=sta^4;
/**************************************************/



/**************************************************
������: init_io();
����:
  ����led,��ʾͨ��,
  Spi ����,Spiʱ�ӳ�ʼ��Ϊ�ߵ�ƽ
/**************************************************/

void init_io(void)
{
	P0=0xaa;			// led����
	CE=0;			// chip ʹ��
	CSN=1;			// Spi����	
	SCK=0;			// Spi ʱ�ӳ�ʼ��Ϊ��
	P0=0xff;		// ledϨ��
}
/**************************************************/

/**************************************************/
void delay_nrf(unsigned int x)
{
    unsigned int i,j;
    i=0;
    for(i=0;i<x;i++)
    {
       j=108;
       while(j--);
    }
}
/**************************************************/

/**************************************************
������: SPI_RW();

����:
дһ���ֽڣ�8λ����MOSI��ͬʱ��MISO��ȡ8λ����
���н����
����byte�е�����д��MOSI�У�MISO�е�����д��byte�в���Ϊ�������ķ���ֵ
/**************************************************/
uchar SPI_RW(uchar byte)
{
	uchar bit_ctr;
   	for(bit_ctr=0;bit_ctr<8;bit_ctr++)   // ���8λ
   	{
   		MOSI = (byte & 0x80);         // ��� 'byte'
   		byte = (byte << 1);           // ��һλ
   		SCK = 1;                      // ����SCK
   		byte |= MISO;       		  // ��ȡ��ǰ����
   		SCK = 0;            		  // 
   	}
    return(byte);           		  // 
}
/**************************************************/

/**************************************************
������: SPI_RW_Reg();

����:
  д 'value' ���Ĵ��� 'reg'
/**************************************************/
uchar SPI_RW_Reg(BYTE reg, BYTE value)
{
	uchar status;

  	CSN = 0;                   // CSN ��, ʹ��SPI
  	status = SPI_RW(reg);      // ѡ��Ĵ���reg
  	SPI_RW(value);             // ��valueд��ȥ
  	CSN = 1;                   // CSN �ص��ߵ�ƽ ��SPIֹͣ

  	return(status);            // ���� nRF24L01 ״̬λ
}
/**************************************************/

/**************************************************
������: SPI_Read();

����:
  ��nRF24L01��һ���ֽڵ�'reg'
/**************************************************/
BYTE SPI_Read(BYTE reg)
{
	BYTE reg_val;

  	CSN = 0;                // CSN ��, ��ʼ�� SPI
  	SPI_RW(reg);            // ѡ��reg
  	reg_val = SPI_RW(0);    // ��ȡreg������
  	CSN = 1;                // CSN �ص���

  	return(reg_val);        // 
}
/**************************************************/

/**************************************************
Function: SPI_Read_Buf();

����:

/**************************************************/
uchar SPI_Read_Buf(BYTE reg, BYTE *pBuf, BYTE bytes)
{
	uchar status,byte_ctr;

  	CSN = 0;                    	
  	status = SPI_RW(reg);       		

  	for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
    	pBuf[byte_ctr] = SPI_RW(0);   

  	CSN = 1;                          

  	return(status);                   
}
/**************************************************/

/**************************************************
������: SPI_Write_Buf();

����:

/**************************************************/
uchar SPI_Write_Buf(BYTE reg, BYTE *pBuf, BYTE bytes)
{
	uchar status,byte_ctr;

  	CSN = 0;                   
  	status = SPI_RW(reg);    
  	for(byte_ctr=0; byte_ctr<bytes; byte_ctr++)
    	SPI_RW(*pBuf++);
  	CSN = 1;                
  	return(status);        
}
/**************************************************/

/**************************************************
������: RX_Mode();

����:
����Rxģʽ
/**************************************************/
void RX_Mode(void)
{
	CE=0;
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x01);     	  //��powerdown״̬�����ò���
	CE=1;
	delay_nrf(20);
	CE=0;
  	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);  //����RX��ַ����˵�ַ��ͬ�����ݱ�����

  	SPI_RW_Reg(WRITE_REG + EN_AA, 0x00);      			//�����Զ�Ӧ��AUTO ACK��
  	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01); 			//ͨ��0�������
  	SPI_RW_Reg(WRITE_REG + RF_CH, 40);       			//���ù���Ƶ��
  	SPI_RW_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);	//�������ݵ���Ч���
  	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);  			//���ݴ�����2Mbps�����书��0dBm
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x03);    			//��crcУ�죬powerup�Ľ���ģʽ

  	CE = 1; 
	delay_nrf(20);
}
/**************************************************/

/**************************************************
������: TX_Mode();

����:
����TXģʽ
/**************************************************/
void TX_Mode()
{	
    
	
	CE=0;
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x00);     	  //��powerdown�����磩״̬�����ò���
	CE=1;
	delay_nrf(20);
	CE=0;
  	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);      //����TX��ַ����˵�ַ��ͬ�Ľ��ն˽�����շ��ͳ�ȥ������
  	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);   //�Զ�Ӧ��Ľ��յ�ַ

	SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH); 			   //��Ҫ���͵�����tx_bufд��WR_TX_PLOAD��

  	SPI_RW_Reg(WRITE_REG + EN_AA, 0x00);      	  //�����Զ�Ӧ��AUTO ACK��
  	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  	  //ͨ��0�������
  	SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x1a); 	  //�Զ��ط�10�Σ�ÿ����ʱ586us
  	SPI_RW_Reg(WRITE_REG + RF_CH, 40);     		  //���ù���Ƶ��
  	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);   	  //���ݴ�����2Mbps�����书��0dBm
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x02);     	  //��crcУ�죬powerup���ϵ磩�ķ���ģʽ
	CE=1;
	delay_nrf(20);

}
/**************************************************/



























/*��ʼ����*/
void Begin(void)
{
    dsp_single_colour(BLACK,0,0,128,128);
	Fast_DrawFont_GBK16(50,50,WHITE,BLACK,"��ӭ") ;
}



/*���ݸ��º���*/
void Update(void)
{	 
      if(up_data==1)
	  {
	  char a[] = "28";
      Display_ASCII8X16_Color(50,30,a,BLACK,WHITE) ;
	  Display_ASCII8X16_Color(50,50,"23.5",BLACK,WHITE) ;
	  Display_ASCII8X16_Color(50,70,"116.8",BLACK,WHITE) ;
	  Display_ASCII8X16_Color(50,30,"29",BLACK,WHITE) ;
	  Display_ASCII8X16_Color(50,50,"24.5",BLACK,WHITE) ;
	  Display_ASCII8X16_Color(50,70,"117.8",BLACK,WHITE) ;
	  Display_ASCII8X16_Color(50,30,"30",BLACK,WHITE) ;
	  Display_ASCII8X16_Color(50,50,"25.5",BLACK,WHITE) ;
	  Display_ASCII8X16_Color(50,70,"118.8",BLACK,WHITE) ;
	  Display_ASCII8X16_Color(50,30,"31",BLACK,WHITE) ;
	  Display_ASCII8X16_Color(50,50,"26.5",BLACK,WHITE) ;
	  Display_ASCII8X16_Color(50,70,"119.8",BLACK,WHITE) ;											   
	  delay_ms(3800);
	  } 								   		
}


/*����������*/
void Function(void)
{	//dsp_single_colour1(BLUE);
    dsp_single_colour(YELLOW,0,0,128,128);
	Fast_DrawFont_GBK16(35,3,WHITE,BLUE,"������") ;
	//Fast_DrawFont_GBK16(3,70,YELLOW,BLACK,"��ӭ����") ;
	//Display_ASCII8X16_Color(3,50,"do you like me?",YELLOW,BLACK) ;
    Fast_DrawFont_GBK16(3,30,WHITE,RED,"����");
	Fast_DrawFont_GBK16(3,50,WHITE,RED,"���");
	Fast_DrawFont_GBK16(3,70,WHITE,RED,"����");
	Fast_DrawFont_GBK16(3,90,WHITE,RED,"����");
	Display_ASCII8X16_Color(36,30,":",BLACK,GRAY0);
	Display_ASCII8X16_Color(36,50,":",BLACK,GRAY0);
	Display_ASCII8X16_Color(36,70,":",BLACK,GRAY0);
	Display_ASCII8X16_Color(36,90,":",BLACK,GRAY0);
	Display_ASCII8X16_Color(50,30,"D  ==>>",BLACK,YELLOW);
	Display_ASCII8X16_Color(50,50,"#  ==>>",BLACK,YELLOW);
	Display_ASCII8X16_Color(50,70,"*  ==>>",BLACK,YELLOW);
	Display_ASCII8X16_Color(50,90,"C  ==>>",BLACK,YELLOW);
	delay_ms(3800);
	
	up_data=0;//�����Ը�������
	speed_alive=0;//�����Ե����ٶȵ�λ
	direaction_alive=0;//�����Ե��ڷ���
	function_alive=1;//����������ʹ��
	font=1;//����������
}

/*������*/
void Font_Test(void)
{	
    if(font==1)
	{
	dsp_single_colour(BLUE,0,0,128,20);
	dsp_single_colour(YELLOW,0,20,35,128);
	dsp_single_colour(WHITE,35,20,128,128);
	Fast_DrawFont_GBK16(28,3,WHITE,BLUE,"��Ӿ���ƶ�") ;
	Fast_DrawFont_GBK16(3,30,RED,YELLOW,"�¶�") ;
	Fast_DrawFont_GBK16(3,50,RED,YELLOW,"γ��") ;
	Fast_DrawFont_GBK16(3,70,RED,YELLOW,"����") ;
	Display_ASCII8X16_Color(35,22,"----------",RED,WHITE) ;
	Display_ASCII8X16_Color(35,40,"----------",RED,WHITE) ;
	Display_ASCII8X16_Color(35,60,"----------",RED,WHITE) ;
	Display_ASCII8X16_Color(35,80,"----------",RED,WHITE) ;
	Fast_DrawFont_GBK16(10,100,YELLOW,BLUE,"��һ���빦����") ;
	Display_ASCII8X16_Color(26,100," B",YELLOW,BLUE) ;
	Display_ASCII8X16_Color(36,30,":",BLACK,WHITE) ;
	Display_ASCII8X16_Color(36,50,":",BLACK,WHITE) ;
	Display_ASCII8X16_Color(36,70,":",BLACK,WHITE) ;
    Display_ASCII8X16_Color(80,30,"%C",BLACK,WHITE) ;
	Display_ASCII8X16_Color(90,50,"N",BLACK,WHITE) ;
	Display_ASCII8X16_Color(100,70,"E",BLACK,WHITE) ;
	
	up_data=1;//���Ը�������
	function_alive=0; //��ֹ������ɨ��
	font=0;//��ֹ��ҳ�治��ɨ��
	}
}

/*���������*/
void  Direction(void)
{	
   if(function_alive==1)
	{
	dsp_single_colour(GREEN,0,0,128,30);
	dsp_single_colour(YELLOW,0,30,128,128);
	Display_ASCII8X16_Color(25,3,"Direction",WHITE,GREEN) ;
	Fast_DrawFont_GBK16(15,40,RED,YELLOW,"��");
	Display_ASCII8X16_Color(36,40,":4",BLACK,YELLOW);
	Fast_DrawFont_GBK16(60,40,RED,YELLOW,"��λ");
	
	Fast_DrawFont_GBK16(15,65,RED,YELLOW,"��");
	Display_ASCII8X16_Color(36,65,":5",BLACK,YELLOW);
	Fast_DrawFont_GBK16(60,65,RED,YELLOW,"ת");
	Display_ASCII8X16_Color(80,65,"45",BLACK,YELLOW);
	Fast_DrawFont_GBK16(95,65,RED,YELLOW,"��");

	
	Fast_DrawFont_GBK16(15,90,RED,YELLOW,"��");
	Display_ASCII8X16_Color(36,90,":6",BLACK,YELLOW);
	Fast_DrawFont_GBK16(60,90,RED,YELLOW,"ת");
	Display_ASCII8X16_Color(80,90,"60",BLACK,YELLOW);
	Fast_DrawFont_GBK16(95,90,RED,YELLOW,"��");

	direaction_alive=1;//����ɵ���

	function_alive=0;//��ֹ����������ɨ��
	}

}


/*��Ƚ���*/
void Help_Me(void)
{  
  if(function_alive==1)
   {
	dsp_single_colour(RED,0,0,128,128);
	Display_ASCII8X16_Color(40,40,"SOS....",WHITE,RED);
	Display_ASCII8X16_Color(20,70,"^_^finished",WHITE,RED);
	function_alive=0;
   }
}


/*��������*/
void Air_Inflation(void)
{ 
  if(function_alive==1)
  {
    dsp_single_colour(GREEN,0,0,128,128);
    Display_ASCII8X16_Color(5,50,"INFLATTING....",BLACK,YELLOW);
	function_alive=0;
  }
}


/*���ٶȽ���*/
void Speed_Governing(void)
{	
    if(function_alive==1)
	{	
	dsp_single_colour(GREEN,0,0,128,30);
	dsp_single_colour(YELLOW,0,30,128,128);
	Display_ASCII8X16_Color(15,3,"Speed change",WHITE,GREEN) ;
	
	Fast_DrawFont_GBK16(15,40,RED,YELLOW,"��");
	Display_ASCII8X16_Color(36,40,":2",BLACK,YELLOW);
	Display_ASCII8X16_Color(60,40,"+",BLACK,YELLOW);
	Fast_DrawFont_GBK16(75,40,RED,YELLOW,"�ٶ�");

	
	Fast_DrawFont_GBK16(15,90,RED,YELLOW,"��");
	Display_ASCII8X16_Color(36,90,":3",BLACK,YELLOW);
	Display_ASCII8X16_Color(60,90,"-",BLACK,YELLOW);
	Fast_DrawFont_GBK16(75,90,RED,YELLOW,"�ٶ�");

    Fast_DrawFont_GBK16(15,65,RED,YELLOW,"�ٶ�");
	Display_ASCII8X16_Color(50,65,":",BLACK,YELLOW);
	Fast_DrawFont_GBK16(80,65,BLACK,YELLOW,"��");

	speed_num=0;//��������

	speed_alive=1;//�ٶȿ��Ե���
	function_alive=0;
    }
}


/*���ٶȵ��ں���*/
void Cont_Speed_Add()
{   	
	
	uchar *one=    "1";
	uchar *two=    "2";
	uchar *three=  "3";
	uchar *four=   "4";
	uchar *five=   "5";
	 
   speed_num=speed_num+1; //������һ
   
   if(speed_alive==1)
   {
    if(speed_num<6)
    { 
      switch(speed_num)
       {   case 1:
	              Display_ASCII8X16_Color(60,65,one,BLACK,YELLOW);break;
  	       case 2:
   	              Display_ASCII8X16_Color(60,65,two,BLACK,YELLOW);break;
   	       case 3:
	              Display_ASCII8X16_Color(60,65,three,BLACK,YELLOW);break;
           case 4:
	 	          Display_ASCII8X16_Color(60,65,four,BLACK,YELLOW);break;
	       case 5:
	              Display_ASCII8X16_Color(60,65,five,BLACK,YELLOW);break;
	   }
	 }
	else 
    speed_num=5;
    }
}

/*���ٶȵ��ں���*/
void Cont_Speed_Flow()
{   	
	
	uchar *one=    "1";
	uchar *two=    "2";
	uchar *three=  "3";
	uchar *four=   "4";
	uchar *five=   "5";
	 
   speed_num=speed_num-1; //������һ
   if(speed_alive==1)
   {
    if(speed_num<6)
    {
     switch(speed_num)
     {   case 1:
	           Display_ASCII8X16_Color(60,65,one,BLACK,YELLOW);break;
  	     case 2:
   	           Display_ASCII8X16_Color(60,65,two,BLACK,YELLOW);break;
   	     case 3:
	           Display_ASCII8X16_Color(60,65,three,BLACK,YELLOW);break;
         case 4:
	 	       Display_ASCII8X16_Color(60,65,four,BLACK,YELLOW);break;
	     case 5:
	           Display_ASCII8X16_Color(60,65,five,BLACK,YELLOW);break;
	 }
    }
    else 
        speed_num=1;
   }
   
}

/*������ں���*/
void Cont_Direction(unsigned int i)
{
   if(direaction_alive==1)
   {  
      
      switch(i)
       { 
	     case 0: {tx_buf[0]=0xff;TX_Mode();dsp_single_colour(RED,0,0,128,128); break; /*��λ*/   };

	     case 45:{tx_buf[0]=0xfe;TX_Mode();dsp_single_colour(YELLOW,0,0,128,128);break;/*ת��45��*/};

	     case 60:{tx_buf[0]=0xfd;TX_Mode();dsp_single_colour(BLACK,0,0,128,128);break;/*ת��60��*/};
	   }
   }
}


