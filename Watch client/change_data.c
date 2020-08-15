#include "tft.h"
#include "change_data.h"


#define uchar unsigned char
#define uint unsigned int


uchar *num_emger[10]={"0","1","2","3","4","5","6","7","8","9"};//��ʾ��ʮ���ַ�
uchar rx_buf_c[5];//���������������ַ���


void ballx(uchar a,uint b,uint c)//�ѷֱ���İ�λ����Ӧ��Ӧ�ַ�����ʾ����
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


void bally(uchar a,uint b,uint c)//�ѷֱ����ʮλ����Ӧ��Ӧ�ַ�����ʾ����
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


void ballz(uchar a,uint b,uint c)//�ѷֱ���ĸ�λ����Ӧ��Ӧ�ַ�����ʾ����
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


void classify(uchar a,uint b,uint c) //�����ݷֳɰ٣�ʮ����������
{
	uchar x,y,z; //�ó����ݵİ٣�ʮ����λ
	

	x=a/100;//��λ
    if(x==0)
	{
	  y=a/10;//ʮλ
	  z=a%10; //��λ
	}
	else
	{
	 y=(a-100)/10;//ʮλ
	 z=(a-100)%10;//��λ
	}
	switch(b)
	{
	   case 30:	//�¶�����λ��
	   {
	//   ballx(x,b,c);//��ʾ��λ
	     bally(y,b,c);//��ʾʮλ
         ballz(z,b,c+12);//��ʾ��λ
		 break;
	   }
	   case 50: //γ������λ��
	   {
 //	     ballx(x,b);//��ʾ��λ
	     bally(y,b,c);//��ʾʮλ
         ballz(z,b,c+8);//��ʾ��λ
		 break;
	   }
	   case 70:	//��������λ��
	   {
		 ballx(x,b,c);//��ʾ��λ
	     bally(y,b,c+8);//��ʾʮλ
         ballz(z,b,c+8+8);//��ʾ��λ
		 break;
	   }
	}
   

}


void emger(uchar a,uchar b,uchar c,uchar d,uchar e)	//�ѽ����������ݽ��д���
 {
	uint g;//ȷ��y������λ��
	uint h=50;//ȷ�����ݵ�x��λ��

	rx_buf_c[0]=a; //������������������
	rx_buf_c[1]=b;
	rx_buf_c[2]=c;
	rx_buf_c[3]=d;
	rx_buf_c[4]=e;

	g=30;h=50;//ȷ���¶�����λ��
	classify(rx_buf_c[0],g,h);
	Display_ASCII8X16_Color(80,30,"%C",BLACK,WHITE) ;

	g=50;h=50;//ȷ��γ������λ��
	classify(rx_buf_c[3],g,h);
	Display_ASCII8X16_Color(68,50,".",BLACK,WHITE);
	h=50+8+20;//ȷ��γ��С����֮�������λ��
	classify(rx_buf_c[4],g,h);
	Display_ASCII8X16_Color(96,50,"N",BLACK,WHITE) ;
	
	g=70;h=50;//ȷ����������λ��
	classify(rx_buf_c[1],g,h);
	Display_ASCII8X16_Color(74,70,".",BLACK,WHITE);
	h=50+8+8+8;//ȷ������С����֮�������λ��
	classify(rx_buf_c[2],g,h);
	Display_ASCII8X16_Color(100,70,"E",BLACK,WHITE) ;

	
 }

