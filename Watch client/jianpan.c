#include "jianpan.h"
#include "screen.h"
#include "tft.h"



uchar j,k,temp;



/*��ʱ����*/
void delay(unsigned char i)
{
  for(j=i;j>0;j--)
    for(k=125;k>0;k--);
}





/*���̳���*/
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
			   Update();//����GPS���¶ȵ���Ϣ
               break;

          case 0x7b:
               delay_ms(500);
			   Cont_Speed_Add();//����
               break;

          case 0x7d:
               delay_ms(500);
			   Cont_Speed_Flow();//����
               break;

          case 0x7e:
              Font_Test();//����������
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
               Cont_Direction(10);///�ƽ���ת���Ƕ�Ϊ0����λ����ֱ��ˮƽ��
               break;

          case 0xbb:
               Cont_Direction(45);//�ƽ���ת���Ƕ�Ϊ45
               break;

          case 0xbd:
               Cont_Direction(60);//�ƽ���ת���Ƕ�Ϊ60
               break;

          case 0xbe:
               Function();//��ȥ�����������٣������򣬳��������
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
               //����
               break;

          case 0xdb:
               //����
               break;

          case 0xdd:
               //����
               break;

          case 0xde:
               Direction();//������ڽ���
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
               Air_Inflation();//��������
               break;

          case 0xeb:
               //����
               break;

          case 0xed:
               Help_Me();//��Ƚ���
               break;

          case 0xee:
               Speed_Governing();//���ٶȽ���
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


















































