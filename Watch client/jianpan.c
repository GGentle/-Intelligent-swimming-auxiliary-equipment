#include "jianpan.h"
#include "screen.h"
#include "tft.h"



uchar j,k,temp;



/*延时函数*/
void delay(unsigned char i)
{
  for(j=i;j>0;j--)
    for(k=125;k>0;k--);
}





/*键盘程序*/
void  Jianpan()
{
  
  while(1)
  {
    P3=0x7f;
    temp=P3;
    temp=temp&0x0f;
    if(temp!=0x0f)
    {
      delay(10);
      if(temp!=0x0f)
      {
        temp=P3;
        switch(temp)
        { 
		  delay_ms(500);
          case 0x77:
			   Update();//更新GPS、温度的信息
               break;

          case 0x7b:
               delay_ms(500);
			   Cont_Speed_Add();//加速
               break;

          case 0x7d:
               delay_ms(500);
			   Cont_Speed_Flow();//减速
               break;

          case 0x7e:
              Font_Test();//返回主界面
               break;
         }
         while(temp!=0x0f)
         {
           temp=P3;
           temp=temp&0x0f;
     
         }
       
      }
    }
    P3=0xbf;
    temp=P3;
    temp=temp&0x0f;
    if(temp!=0x0f)
    {
      delay(10);
      if(temp!=0x0f)
      {
        temp=P3;
        switch(temp)
        {
          case 0xb7:
               Cont_Direction(10);///推进器转动角度为0，复位，垂直于水平线
               break;

          case 0xbb:
               Cont_Direction(45);//推进器转动角度为45
               break;

          case 0xbd:
               Cont_Direction(60);//推进器转动角度为60
               break;

          case 0xbe:
               Function();//进去功能区，调速，调方向，充气，求救
               break;
         }
         while(temp!=0x0f)
         {
           temp=P3;
           temp=temp&0x0f;
       
         }
      }
      }
    P3=0xdf;
    temp=P3;
    temp=temp&0x0f;
    if(temp!=0x0f)
    {
      delay(10);
      if(temp!=0x0f)
      {
        temp=P3;
        switch(temp)
        {
          case 0xd7:
               //待定
               break;

          case 0xdb:
               //待定
               break;

          case 0xdd:
               //待定
               break;

          case 0xde:
               Direction();//方向调节界面
               break;
         }
         while(temp!=0x0f)
         {
           temp=P3;
           temp=temp&0x0f;
         
         }
      }
      }
    P3=0xef;
    temp=P3;
    temp=temp&0x0f;
    if(temp!=0x0f)
    {
      delay(10);
      if(temp!=0x0f)
      {
        temp=P3;
        switch(temp)
        {
          case 0xe7:
               Air_Inflation();//充气界面
               break;

          case 0xeb:
               //待定
               break;

          case 0xed:
               Help_Me();//求救界面
               break;

          case 0xee:
               Speed_Governing();//调速度界面
               break;
         }
         while(temp!=0x0f)
         {
           temp=P3;
           temp=temp&0x0f;
     
         }
      }
    }
  }
}


















































