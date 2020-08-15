#include "tft.h"
#include "change_data.h"


#define uchar unsigned char
#define uint unsigned int


uchar *num_emger[10]={"0","1","2","3","4","5","6","7","8","9"};//显示的十个字符
uchar rx_buf_c[5];//储存接受来的五个字符数


void ballx(uchar a,uint b,uint c)//把分辨出的百位，对应相应字符，显示出来
{
   switch(a)
   {
	  case 1:{ Display_ASCII8X16_Color(c,b,num_emger[1],BLACK,WHITE) ; break;}
	  case 2:{ Display_ASCII8X16_Color(c,b,num_emger[2],BLACK,WHITE) ; break;}
	  case 3:{ Display_ASCII8X16_Color(c,b,num_emger[3],BLACK,WHITE) ; break;}
	  case 4:{ Display_ASCII8X16_Color(c,b,num_emger[4],BLACK,WHITE) ; break;}
	  case 5:{ Display_ASCII8X16_Color(c,b,num_emger[5],BLACK,WHITE) ; break;}
	  case 6:{ Display_ASCII8X16_Color(c,b,num_emger[6],BLACK,WHITE) ; break;}
	  case 7:{ Display_ASCII8X16_Color(c,b,num_emger[7],BLACK,WHITE) ; break;}
	  case 8:{ Display_ASCII8X16_Color(c,b,num_emger[8],BLACK,WHITE) ; break;}
	  case 9:{ Display_ASCII8X16_Color(c,b,num_emger[9],BLACK,WHITE) ; break;}
	  default:{ break;}
   }
}


void bally(uchar a,uint b,uint c)//把分辨出的十位，对应相应字符，显示出来
{
   switch (a)
   {
	  case 0:{ Display_ASCII8X16_Color(c,b,num_emger[0],BLACK,WHITE) ;  break;}
	  case 1:{ Display_ASCII8X16_Color(c,b,num_emger[1],BLACK,WHITE) ; break;}
	  case 2:{ Display_ASCII8X16_Color(c,b,num_emger[2],BLACK,WHITE) ; break;}
	  case 3:{ Display_ASCII8X16_Color(c,b,num_emger[3],BLACK,WHITE) ; break;}
	  case 4:{ Display_ASCII8X16_Color(c,b,num_emger[4],BLACK,WHITE) ; break;}
	  case 5:{ Display_ASCII8X16_Color(c,b,num_emger[5],BLACK,WHITE) ; break;}
	  case 6:{ Display_ASCII8X16_Color(c,b,num_emger[6],BLACK,WHITE) ; break;}
	  case 7:{ Display_ASCII8X16_Color(c,b,num_emger[7],BLACK,WHITE) ; break;}
	  case 8:{ Display_ASCII8X16_Color(c,b,num_emger[8],BLACK,WHITE) ; break;}
	  case 9:{ Display_ASCII8X16_Color(c,b,num_emger[9],BLACK,WHITE) ; break;}
   }		 
}


void ballz(uchar a,uint b,uint c)//把分辨出的个位，对应相应字符，显示出来
{
   switch(a)
   {
	  case 0:{ Display_ASCII8X16_Color(c,b,num_emger[0],BLACK,WHITE) ; break;}
	  case 1:{ Display_ASCII8X16_Color(c,b,num_emger[1],BLACK,WHITE) ; break;}
	  case 2:{ Display_ASCII8X16_Color(c,b,num_emger[2],BLACK,WHITE) ; break;}
	  case 3:{ Display_ASCII8X16_Color(c,b,num_emger[3],BLACK,WHITE) ; break;}
	  case 4:{ Display_ASCII8X16_Color(c,b,num_emger[4],BLACK,WHITE) ; break;}
	  case 5:{ Display_ASCII8X16_Color(c,b,num_emger[5],BLACK,WHITE) ; break;}
	  case 6:{ Display_ASCII8X16_Color(c,b,num_emger[6],BLACK,WHITE) ; break;}
	  case 7:{ Display_ASCII8X16_Color(c,b,num_emger[7],BLACK,WHITE) ; break;}
	  case 8:{ Display_ASCII8X16_Color(c,b,num_emger[8],BLACK,WHITE) ; break;}
	  case 9:{ Display_ASCII8X16_Color(c,b,num_emger[9],BLACK,WHITE) ; break;}
   }
}


void classify(uchar a,uint b,uint c) //把数据分成百，十，个三个数
{
	uchar x,y,z; //得出数据的百，十，个位
	

	x=a/100;//百位
    if(x==0)
	{
	  y=a/10;//十位
	  z=a%10; //个位
	}
	else
	{
	 y=(a-100)/10;//十位
	 z=(a-100)%10;//个位
	}
	switch(b)
	{
	   case 30:	//温度数据位置
	   {
	//   ballx(x,b,c);//显示百位
	     bally(y,b,c);//显示十位
         ballz(z,b,c+12);//显示个位
		 break;
	   }
	   case 50: //纬度数据位置
	   {
 //	     ballx(x,b);//显示百位
	     bally(y,b,c);//显示十位
         ballz(z,b,c+8);//显示个位
		 break;
	   }
	   case 70:	//经度数据位置
	   {
		 ballx(x,b,c);//显示百位
	     bally(y,b,c+8);//显示十位
         ballz(z,b,c+8+8);//显示个位
		 break;
	   }
	}
   

}


void emger(uchar a,uchar b,uchar c,uchar d,uchar e)	//把接受来的数据进行处理
 {
	uint g;//确定y轴数据位置
	uint h=50;//确定数据的x轴位置

	rx_buf_c[0]=a; //处理接受来的五个数据
	rx_buf_c[1]=b;
	rx_buf_c[2]=c;
	rx_buf_c[3]=d;
	rx_buf_c[4]=e;

	g=30;h=50;//确定温度数据位置
	classify(rx_buf_c[0],g,h);
	Display_ASCII8X16_Color(80,30,"%C",BLACK,WHITE) ;

	g=50;h=50;//确定纬度数据位置
	classify(rx_buf_c[3],g,h);
	Display_ASCII8X16_Color(68,50,".",BLACK,WHITE);
	h=50+8+20;//确定纬度小数点之后的数据位置
	classify(rx_buf_c[4],g,h);
	Display_ASCII8X16_Color(96,50,"N",BLACK,WHITE) ;
	
	g=70;h=50;//确定经度数据位置
	classify(rx_buf_c[1],g,h);
	Display_ASCII8X16_Color(74,70,".",BLACK,WHITE);
	h=50+8+8+8;//确定经度小数点之后的数据位置
	classify(rx_buf_c[2],g,h);
	Display_ASCII8X16_Color(100,70,"E",BLACK,WHITE) ;

	
 }

