#include <intrins.h>
#include "screen.h"
#include "tft.h"
#include "nrf24l01.h"
#include "api.h"
#include "change_data.h"



uchar speed_num=0;//控制速度的档位
uchar up_data=0;//限制更新数据的场合
uchar speed_alive=0;//控制速度更新的场合
uchar direaction_alive=0;//控制方向调节场合
uchar function_alive=0;//限制功能区的使用
uchar font=1;//限制主页面不断扫描
uint  yposition=40;//调节方向界面标记
uchar hypercritical=0;//关闭或开启充气
uchar inflate=0;//能否充气






uchar const TX_ADDRESS[TX_ADR_WIDTH]  = {0x34,0x43,0x10,0x10,0x01}; // 定义TX指针

uchar rx_buf[TX_PLOAD_WIDTH];
uchar tx_buf[TX_PLOAD_WIDTH];

uchar bdata sta;
sbit	RX_DR	=sta^6;
sbit	TX_DS	=sta^5;
sbit	MAX_RT	=sta^4;
/**************************************************/



/**************************************************
函数名: init_io();
描述:
  点亮led,表示通电,
  Spi 禁用,Spi时钟初始化为高电平
/**************************************************/

void init_io(void)
{
	P0=0xaa;			// led点亮
	CE=0;			// chip 使能
	CSN=1;			// Spi禁用	
	SCK=0;			// Spi 时钟初始化为高
	P0=0xff;		// led熄灭
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
函数名: SPI_RW();

描述:
写一个字节（8位）到MOSI，同时从MISO读取8位数据
运行结果：
参数byte中的数据写到MOSI中，MISO中的数据写到byte中并作为本函数的返回值
/**************************************************/
uchar SPI_RW(uchar byte)
{
	uchar bit_ctr;
   	for(bit_ctr=0;bit_ctr<8;bit_ctr++)   // 输出8位
   	{
   		MOSI = (byte & 0x80);         // 输出 'byte'
   		byte = (byte << 1);           // 下一位
   		SCK = 1;                      // 设置SCK
   		byte |= MISO;       		  // 获取当前输入
   		SCK = 0;            		  // 
   	}
    return(byte);           		  // 
}
/**************************************************/

/**************************************************
函数名: SPI_RW_Reg();

描述:
  写 'value' 到寄存器 'reg'
/**************************************************/
uchar SPI_RW_Reg(BYTE reg, BYTE value)
{
	uchar status;

  	CSN = 0;                   // CSN 低, 使能SPI
  	status = SPI_RW(reg);      // 选择寄存器reg
  	SPI_RW(value);             // 把value写进去
  	CSN = 1;                   // CSN 回到高电平 ，SPI停止

  	return(status);            // 返回 nRF24L01 状态位
}
/**************************************************/

/**************************************************
函数名: SPI_Read();

描述:
  从nRF24L01读一个字节到'reg'
/**************************************************/
BYTE SPI_Read(BYTE reg)
{
	BYTE reg_val;

  	CSN = 0;                // CSN 低, 初始化 SPI
  	SPI_RW(reg);            // 选择reg
  	reg_val = SPI_RW(0);    // 读取reg的数据
  	CSN = 1;                // CSN 回到高

  	return(reg_val);        // 
}
/**************************************************/

/**************************************************
Function: SPI_Read_Buf();

描述:

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
函数名: SPI_Write_Buf();

描述:

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
函数名: RX_Mode();

描述:
配置Rx模式
/**************************************************/
void RX_Mode(void)
{
	CE=0;
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x01);     	  //在powerdown状态中配置参数
	CE=1;
	delay_nrf(20);
	CE=0;
  	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);  //设置RX地址，与此地址相同的数据被接收

  	SPI_RW_Reg(WRITE_REG + EN_AA, 0x00);      			//禁能自动应答（AUTO ACK）
  	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01); 			//通道0允许接收
  	SPI_RW_Reg(WRITE_REG + RF_CH, 40);       			//设置工作频率
  	SPI_RW_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);	//接收数据的有效宽度
  	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);  			//数据传输率2Mbps，发射功率0dBm
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x03);    			//无crc校检，powerup的接收模式

  	CE = 1; 
	delay_nrf(20);
}
/**************************************************/

/**************************************************
函数名: TX_Mode();

描述:
配置TX模式
/**************************************************/
void TX_Mode()
{	
    
	
	CE=0;
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x00);     	  //在powerdown（掉电）状态中配置参数
	CE=1;
	delay_nrf(20);
	CE=0;
  	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);      //设置TX地址，与此地址相同的接收端将会接收发送出去的数据
  	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);   //自动应答的接收地址

	SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH); 			   //把要发送的数据tx_buf写到WR_TX_PLOAD中

  	SPI_RW_Reg(WRITE_REG + EN_AA, 0x00);      	  //禁能自动应答（AUTO ACK）
  	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  	  //通道0允许接收
  	SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x1a); 	  //自动重发10次，每次延时586us
  	SPI_RW_Reg(WRITE_REG + RF_CH, 40);     		  //设置工作频率
  	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);   	  //数据传输率2Mbps，发射功率0dBm
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x02);     	  //无crc校检，powerup（上电）的发送模式
	CE=1;
	delay_nrf(20);

}
/**************************************************/


























/**************************************************/
/*开始界面*/
void Begin(void)
{
    dsp_single_colour(BLACK,0,0,128,128);
	Fast_DrawFont_GBK16(50,50,WHITE,BLACK,"欢迎") ;
}



/*数据更新函数*/
void Update(void)
{	 
	  uchar tg[5];

      if(up_data==1)
	  {
		 tx_buf[0]=0x09; //发送0x09，更新数据，并之后变回接受状态
		 TX_Mode();
//		 Display_ASCII8X16_Color(60,65,"2",BLACK,YELLOW);
		 rx_buf[0] = 0;	   //清空接受缓冲区
		 RX_Mode();		   //切换为接受模式
		{
		 SPI_Read_Buf(RD_RX_PLOAD,rx_buf,5);	//接受数据，并检查数据
				
					tg[0]=rx_buf[0];
					tg[1]=rx_buf[1];
					tg[2]=rx_buf[2];
					tg[3]=rx_buf[3];
					tg[4]=rx_buf[4]; 	
					emger(tg[0],tg[1],tg[2],tg[3],tg[4]);//显示数据
			
		}  
	 } 								   		
}


/*功能区函数*/
void Function(void)
{	//dsp_single_colour1(BLUE);
    dsp_single_colour(YELLOW,0,0,128,128);
	Fast_DrawFont_GBK16(35,3,WHITE,BLUE,"功能区") ;
	//Fast_DrawFont_GBK16(3,70,YELLOW,BLACK,"欢迎按键") ;
	//Display_ASCII8X16_Color(3,50,"do you like me?",YELLOW,BLACK) ;
    Fast_DrawFont_GBK16(3,30,WHITE,RED,"调速");
	Fast_DrawFont_GBK16(3,50,WHITE,RED,"求救");
	Fast_DrawFont_GBK16(3,70,WHITE,RED,"充气");
	Fast_DrawFont_GBK16(3,90,WHITE,RED,"方向");
	Display_ASCII8X16_Color(36,30,":",BLACK,GRAY0);
	Display_ASCII8X16_Color(36,50,":",BLACK,GRAY0);
	Display_ASCII8X16_Color(36,70,":",BLACK,GRAY0);
	Display_ASCII8X16_Color(36,90,":",BLACK,GRAY0);
	Display_ASCII8X16_Color(50,30,"D  ==>>",BLACK,YELLOW);
	Display_ASCII8X16_Color(50,50,"#  ==>>",BLACK,YELLOW);
	Display_ASCII8X16_Color(50,70,"*  ==>>",BLACK,YELLOW);
	Display_ASCII8X16_Color(50,90,"C  ==>>",BLACK,YELLOW);
	delay_ms(3800);
	
	up_data=0;//不可以更新数据
	speed_alive=0;//不可以调节速度档位
	direaction_alive=0;//不可以调节方向
	function_alive=1;//功能区可以使用
	font=1;//返回主界面
	inflate=1;//进入充气界面
}

/*主界面*/
void Font_Test(void)
{	
    if(font==1)
	{
	dsp_single_colour(BLUE,0,0,128,20);
	dsp_single_colour(YELLOW,0,20,35,128);
	dsp_single_colour(WHITE,35,20,128,128);
	Fast_DrawFont_GBK16(28,3,WHITE,BLUE,"游泳控制端") ;
	Fast_DrawFont_GBK16(3,30,RED,YELLOW,"温度") ;
	Fast_DrawFont_GBK16(3,50,RED,YELLOW,"纬度") ;
	Fast_DrawFont_GBK16(3,70,RED,YELLOW,"经度") ;
	Display_ASCII8X16_Color(35,22,"----------",RED,WHITE) ;
	Display_ASCII8X16_Color(35,40,"----------",RED,WHITE) ;
	Display_ASCII8X16_Color(35,60,"----------",RED,WHITE) ;
	Display_ASCII8X16_Color(35,80,"----------",RED,WHITE) ;
	Fast_DrawFont_GBK16(10,100,YELLOW,BLUE,"按一进入功能区") ;
	Display_ASCII8X16_Color(26,100," B",YELLOW,BLUE) ;
	Display_ASCII8X16_Color(36,30,":",BLACK,WHITE) ;
	Display_ASCII8X16_Color(36,50,":",BLACK,WHITE) ;
	Display_ASCII8X16_Color(36,70,":",BLACK,WHITE) ;
   
	
	
	up_data=1;//可以更新数据
	function_alive=0; //禁止功能区扫面
	inflate=0;
	font=0;//禁止主页面不断扫面
	}
}

/*调方向界面*/
void  Direction(void)
{	
   if(function_alive==1)
	{
	dsp_single_colour(GREEN,0,0,128,30);
	dsp_single_colour(YELLOW,0,30,128,128);
	Display_ASCII8X16_Color(25,3,"Direction",WHITE,GREEN) ;
		
	Display_ASCII8X16_Color(6,yposition,"*",BLACK,YELLOW);
	Fast_DrawFont_GBK16(15,40,RED,YELLOW,"按");
	Display_ASCII8X16_Color(36,40,":4",BLACK,YELLOW);
	Fast_DrawFont_GBK16(60,40,RED,YELLOW,"复位");
	
	Fast_DrawFont_GBK16(15,65,RED,YELLOW,"按");
	Display_ASCII8X16_Color(36,65,":5",BLACK,YELLOW);
	Fast_DrawFont_GBK16(60,65,RED,YELLOW,"转");
	Display_ASCII8X16_Color(80,65,"45",BLACK,YELLOW);
	Fast_DrawFont_GBK16(95,65,RED,YELLOW,"度");

	
	Fast_DrawFont_GBK16(15,90,RED,YELLOW,"按");
	Display_ASCII8X16_Color(36,90,":6",BLACK,YELLOW);
	Fast_DrawFont_GBK16(60,90,RED,YELLOW,"转");
	Display_ASCII8X16_Color(80,90,"60",BLACK,YELLOW);
	Fast_DrawFont_GBK16(95,90,RED,YELLOW,"度");

	direaction_alive=1;//方向可调节
	inflate=0;
	function_alive=0;//禁止其他功能区扫描
	}

}


/*求救界面*/
void Help_Me(void)
{  
  if(function_alive==1)
   {
    tx_buf[0]=0x11;//发送0x11，求救
	TX_Mode();
	dsp_single_colour(RED,0,0,128,128);
	Display_ASCII8X16_Color(40,50,"SOS....",WHITE,RED);

//	rx_buf[0]=0;
//	RX_Mode();
//	if(rx_buf[0]=0xc5)
//	 {
//	 Display_ASCII8X16_Color(40,50,"get it",WHITE,RED);
//	 }
   }
   tx_buf[0]=0xFF;//发送0x11，求救
	TX_Mode();
   inflate=0;
   function_alive=0;
}


/*充气界面*/
void Air_Inflation(void)
{ 
  if(inflate==1)
  {
    tx_buf[0]=0x12;//发送0x12，充气
	TX_Mode();
	dsp_single_colour(GREEN,0,0,128,128);
   
    hypercritical++;
	
	if(hypercritical>1)
	hypercritical=0;
	switch(hypercritical)
	  {
      case 0: Display_ASCII8X16_Color(40,50,"close",BLACK,YELLOW);break;
	  case 1: Display_ASCII8X16_Color(40,50,"oopen",BLACK,YELLOW);break;
	  }
	
	function_alive=0;
  }
}


/*调速度界面*/
void Speed_Governing(void)
{	
    if(function_alive==1)
	{	
	dsp_single_colour(GREEN,0,0,128,30);
	dsp_single_colour(YELLOW,0,30,128,128);
	Display_ASCII8X16_Color(15,3,"Speed change",WHITE,GREEN) ;
	
	Fast_DrawFont_GBK16(15,40,RED,YELLOW,"按");
	Display_ASCII8X16_Color(36,40,":2",BLACK,YELLOW);
	Display_ASCII8X16_Color(60,40,"+",BLACK,YELLOW);
	Fast_DrawFont_GBK16(75,40,RED,YELLOW,"速度");

	
	Fast_DrawFont_GBK16(15,90,RED,YELLOW,"按");
	Display_ASCII8X16_Color(36,90,":3",BLACK,YELLOW);
	Display_ASCII8X16_Color(60,90,"-",BLACK,YELLOW);
	Fast_DrawFont_GBK16(75,90,RED,YELLOW,"速度");

    Fast_DrawFont_GBK16(15,65,RED,YELLOW,"速度");
	Display_ASCII8X16_Color(50,65,":",BLACK,YELLOW);
	Display_ASCII8X16_Color(60,65,num_emger[speed_num],BLACK,YELLOW);
	Fast_DrawFont_GBK16(80,65,BLACK,YELLOW,"档");

//	speed_num=0;//计数清零

	speed_alive=1;//速度可以调节
	function_alive=0;
	inflate=0;
    }
}


/*加速度调节函数*/
void Cont_Speed_Add()
{ 		
   if(speed_alive==1)
   { 
     speed_num++; //计数加一  
    
	if(speed_num<6)
    { 
      switch(speed_num)
       {  
	       case 1:
	            { tx_buf[0]=0x04;TX_Mode(); Display_ASCII8X16_Color(60,65,num_emger[1],BLACK,YELLOW);break;}//发送0x04,为一档
  	       case 2:
   	            { tx_buf[0]=0x05;TX_Mode(); Display_ASCII8X16_Color(60,65,num_emger[2],BLACK,YELLOW);break;}//发送0x05,为二档
   	       case 3:
	             {tx_buf[0]=0x06;TX_Mode(); Display_ASCII8X16_Color(60,65,num_emger[3],BLACK,YELLOW);break;}//发送0x06,为三档
           case 4:
	 	         {tx_buf[0]=0x07;TX_Mode(); Display_ASCII8X16_Color(60,65,num_emger[4],BLACK,YELLOW);break;}//发送0x07,为四档
	       case 5:
	             {tx_buf[0]=0x08;TX_Mode(); Display_ASCII8X16_Color(60,65,num_emger[5],BLACK,YELLOW);break;}//发送0x08,为五档
	   }
	 }
	else 
     {
	   speed_num=0;
	   tx_buf[0]=0x10;TX_Mode(); Display_ASCII8X16_Color(60,65,num_emger[0],BLACK,YELLOW);//发送0x10,关闭推进器
	   
     }
	}

}

/*减速度调节函数*/
void Cont_Speed_Flow()
{   	

   if(speed_alive==1)
   {
	speed_num--;//计数减一
    
	if(speed_num<6)
    {

     switch(speed_num)
     {   
	     case 0:
	          { tx_buf[0]=0x10;TX_Mode(); Display_ASCII8X16_Color(60,65,num_emger[0],BLACK,YELLOW);break;}//发送0x10,关闭推进器
		 case 1:
	          {tx_buf[0]=0x04;TX_Mode(); Display_ASCII8X16_Color(60,65,num_emger[1],BLACK,YELLOW);break;} //发送0x04,为一档
  	     case 2:
   	          {tx_buf[0]=0x05;TX_Mode(); Display_ASCII8X16_Color(60,65,num_emger[2],BLACK,YELLOW);break;}//发送0x05,为二档
   	     case 3:
	          {tx_buf[0]=0x06;TX_Mode(); Display_ASCII8X16_Color(60,65,num_emger[3],BLACK,YELLOW);break;}//发送0x06,为三档
         case 4:
	 	      {tx_buf[0]=0x07;TX_Mode(); Display_ASCII8X16_Color(60,65,num_emger[4],BLACK,YELLOW);break;} //发送0x07,为四档
	     case 5:
	          {tx_buf[0]=0x08;TX_Mode(); Display_ASCII8X16_Color(60,65,num_emger[5],BLACK,YELLOW);break;}//发送0x08,为五档
	 }
    }
    else 
        speed_num=0;
   }
   
}

/*方向调节函数*/
void Cont_Direction(unsigned int i)
{
   if(direaction_alive==1)
   {  
      
      switch(i)
       { 
	     case 10:{ Display_ASCII8X16_Color(6,yposition," ",BLACK,YELLOW); 
		           tx_buf[0]=0x01;TX_Mode();yposition=40;break; /*复位*/   };//发送0x01,为复位

	     case 45:{ Display_ASCII8X16_Color(6,yposition," ",BLACK,YELLOW); 
		           tx_buf[0]=0x02;TX_Mode();yposition=65;break;/*转动45度*/};//发送0x02,为45度

	     case 60:{ Display_ASCII8X16_Color(6,yposition," ",BLACK,YELLOW); 
		           tx_buf[0]=0x03;TX_Mode();yposition=90;break;/*转动60度*/};//发送0x03,为60度
	   }
	   
	  Display_ASCII8X16_Color(6,yposition,"*",BLACK,YELLOW); 																							
   }
}


