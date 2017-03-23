#include "Process.h"
#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dma.h"
#include "ftm.h"
#include "ssd1306.h"
#include "ov7725.h"
#include "i2c.h"

double  img[60+1];



bool get_gp(int i,int j,unsigned char **gpHREE){
    int a=j/8,b=j%8;
    unsigned char c=gpHREE[i][a]>>(7-b)&0x01;
if(c) return true;
    else return false;
}


 void ifGreat(int i)
   {
       double b=0,c=3;
    //printf("i=%d\r\n",i);
    for(int a=c;a<i;a++)
    {
        
        //printf("img[%d]=%f\r\n",a,img[a]);
        b=b+img[a];
        //printf("b=%f\r\n",b);
    }

    b=b/(i-c)-40;
   // printf("b=%f\r\n",b);
    if(b>5)
        printf("Æ«×ó\r\n");
    else if(b<-5)
        printf("Æ«ÓÒ\r\n");
   }


void right(unsigned char **gpHREE)
{
    int i,j;
    int close=45,close_f=0,Dmunber=20;//È¡Öµ·¶Î§
    int l;//·â±Õ±ê×¼
    int last[61]={40},right[61]={40};
    //if(get_gp(59,40,gpHREE)==true)
    //   last[0]=0,right[0]=79;
    
    for(i=59;i>=0;i--)
       {
         if(get_gp(i,last[59-i],gpHREE))
         {
            for(j=last[59-i];j>=0;j--)
            {
            if(get_gp(i,j,gpHREE)==false)
                   {
                     last[60-i]=j;
                        break;
                   }
            }
         }
         if(get_gp(i,last[59-i],gpHREE)==false) 
            {
             for(j=last[59-i];j<80;j++)
               {
                    if(get_gp(i,j,gpHREE))
                    {
                        last[60-i]=j-1;
                        break;
                    }
                }
            }
            if(get_gp(i,right[59-i],gpHREE))
          {
             
              for(j=right[59-i];j<80;j++)
              {
                  if(get_gp(i,j,gpHREE)==false)
                  {
                      
                      right[60-i]=j;
                      break;
                  }
              }
          }
          if(get_gp(i,right[59-i],gpHREE)==false)
          {
              
              for(j=right[59-i];j>=0;j--)
              {
                 if(get_gp(i,j,gpHREE))
                 {
                     right[60-i]=j-1;
                     break;
                 }
              
              }
              if(j==80)right[60-i]=j-1;
          }
        // printf("last[%d]=%d        right[%d]=%d\r\n",60-i,last[60-i],60-i,right[60-i]);
        }
        //printf("close=%d\r\n",close);
        int aaa=0,bbb=0;
       for(i=1,l=0;i<=60;i++)
        {
            if(bbb==0&&last[i]>right[i])
            {
               aaa++;
            }
            else bbb++;
         img[i]=(last[i]+right[i])/2.0;
            if(last[i]==79&&right[i]==0&&l==0)
            {
                close=i-1;
                Dmunber=i-1;
                l=1;
            }
            if(last[i]==right[i]&&l==0)
     {
         l=1;
         close=i;
         Dmunber=i;
         if(img[i]==0)
         {
             close_f=1;
         }
         if(img[i]==79)
         {
             close_f=2;
         }
     }
        }
   ifGreat(close);
  }

   
  