
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
double  average,carI,subtense;


bool get_gp(int i,int j,unsigned char **gpHREE){
    unsigned char c=gpHREE[i][j];
if(c) return true;
    else return false;
}
void set_gp(int i,int j,unsigned char **gpHREE,int z){
gpHREE[i][j]=z;
}

double findPP(int i){
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
    return b;
}

double findP(int i)
    {
    double b=0,c=0;
    int x,y,z=1,d=0;
    //printf("i=%d\r\n",i);
    x=i/3;
    y=i%3;
    if(y!=0)
    {
        x++;
    }
    
    for(int a=1;a<=3;a++)
    {
       
        for(c=0;c<x;c++)
        {
        //printf("img[%d]=%f\r\n",z,img[z]);
        b=b+img[z++]*a;
        d=d+a;
       // printf("b=%f d=%d\r\n",b,d);
        }
        if(z>i) break;
    }

    b=b/d-40;
  //  printf("b=%f\r\n",b);
   // printf("\r\n");
    return b;
}

/*
**findD(18,20,left)
**求斜率（最小二乘法）
*/
double findD(uint8_t begin,uint8_t end,double *p)  
{
   int xsum=0,ysum=0,xysum=0,x2sum=0;
   uint8_t i=0;
   float result=0;
   static float resultlast;
   p=p+begin;
   for(i=begin;i<end;i++)
   {
	   xsum+=i;
	   ysum+=*p;
	   xysum+=i*(*p);
	   x2sum+=i*i;
	   p=p+1;
   }
  if((end-begin)*x2sum-xsum*xsum) //分母不为零时
  {
    result=(float)((end-begin)*xysum-xsum*ysum)/((end-begin)*x2sum-xsum*xsum);
    resultlast=result;
  }
  else
  {
   result=resultlast;
  }
  result =(float) 1/result;
  result *= 100;
  return result/10;

}
double findR(int* a,int i,int v)
{
    //if(i>40)i=40;
  // int b=a[i]-a[4];
    
  //  if(b<10&&b>-10) return 0;
   // b=b/4.0;
    double d=0;
   // if(v==0)
   //     d=a[4]+0.1;
   // else d=a[4]-0.1;
    double e[6]={0};
    int f=0,c;
    if(v==1)
    {
        d=0.25;
    for(c=4;c<=i;c++)
    {
        if(a[c]>d)
        {
           if(a[c-1]<d)
            {
                e[f]=c-1;
                d=d+16;
                f++;
            }
        }
    }
    e[5]=i;
    }
    else
    {
        d=78.75;
        for(c=4;c<=i;c++)
    {

        if(a[c]<d)
        {
           if(a[c-1]>d)
            {
                e[f]=c;
                d=d-16;
                f++;
            }
        }
    }
    e[5]=i;
    }
  int b=0;
    for(c=5;c>=3;c--)
    {
        b=b+e[c]-e[c-3];
       //printf("e[%d]=%f\r\n",c,e[c]);
    }
   return b;
}
/*
void handle(unsigned char **gpHREE)
{ 
    int i,j,k;
    for(i=0;i<60;i++)
    {
        for(j=0;j<80;j++)
        {
        get_gp(i,j,gpHREE);
            
        }
        printf("\r\n");
    }
}
*/
void CoverLine(){
    
}

int p;
int m,k,k0=STRAIGHT,e=0;

int handle(unsigned char **gpHREE,int PWM_Motor,double q,double w,double e,double f)
{  
    /*
    if((gpHREE[5][3]!=0&&gpHREE[5][8]!=0&&gpHREE[4][3]!=0&&gpHREE[4][8]!=0)||e==1)
    {
        e=1;
        FTM_PWM_ChangeDuty(HW_FTM2, HW_FTM_CH0,0);
        return 0;
    }
    */

    int i,j;
    int close=45,close_f=0;//取值范围
    int l;//封闭标准
    /*******************************************************************************************************/
    int left[61]={40,40},right[61]={40,40};
   // for(i=0;i<61;i++)
   // printf("left[%d]=%d  \r\n",i,left[i]);
    //if(get_gp(59,40,gpHREE)==true)
    //   left[0]=0,right[0]=79;
    /*    if(get_gp(45,40,gpHREE)==true)
    {
        if(k>STRAIGHT)k=LEFT;
        else k=RIGHT;
       FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH3,k);
        return 0;
    }*/
    if(gpHREE[59][40]!=0)
    {
        left[0]=0;right[0]=79;
    }
    for(i=59;i>=0;i--)
       {
         if(get_gp(i,left[59-i],gpHREE)==true)
         {
            for(j=left[59-i];j<80;j++)
            {
             // printf("->left[%d]=%d \r\n",60-i,j); 
            if(get_gp(i,j,gpHREE)==false)
                   {
                       set_gp(i,j,gpHREE,3);
                     left[60-i]=j;
                        break;
                   }
            }
         }
         if(j==80){left[60-i]=79;j=0;}
         
         if(get_gp(i,left[59-i],gpHREE)==false) 
            {
              
                for(j=left[59-i];j>=0;j--)
               {
                   //printf("<-left[%d]=%d \r\n",60-i,j);
                    if(get_gp(i,j,gpHREE))
                    {
                        set_gp(i,j,gpHREE,2);
                        left[60-i]=j+1;
                        break;
                    }
                }
            }
            if(j==-1){left[60-i]=0;j=0;} 
            if(get_gp(i,right[59-i],gpHREE)==true)
          {
             
              for(j=right[59-i];j>=0;j--)
              {
                  if(get_gp(i,j,gpHREE)==false)
                  {
                      right[60-i]=j;
                      set_gp(i,j,gpHREE,2);
                      break;
                  }
              }
          }
          if(j==-1){right[60-i]=0;j=0;}
          if(get_gp(i,right[59-i],gpHREE)==false)
          {
              for(j=right[59-i];j<80;j++)
              {
                 if(get_gp(i,j,gpHREE))
                 {
                     set_gp(i,j,gpHREE,2);
                     right[60-i]=j-1;
                     break;
                 }             
              }
              if(j==80){right[60-i]=j-1;j=0;}
              
          }

              CoverLine();//补线
         // printf("left[%d]=%d        right[%d]=%d\r\n",60-i,left[60-i],60-i,right[60-i]);
        }
    /***********************************************************************************************************************/
        int aaa=0,bbb=0,Dmunber=0;
       for(i=1,l=0;i<=45&&l==0;i++)
        {
            if(bbb==0&&left[i]>right[i])
            {
               aaa++;
            }
            else bbb++;
         img[i]=(left[i]+right[i])/2.0;
            set_gp(60-i,img[i],gpHREE,1);
            if(left[i]==79&&right[i]==0&&l==0)
            {
                close=i-1;
                Dmunber=i-1;
                l=1;
            }
                    if(left[i]==right[i]&&l==0)
             {
                 l=1;
                 close=i;
                 Dmunber=i;
                 //if(img[i]==0)
                 //{
                 //    close_f=1;
                 //}
                 //if(img[i]==79)
                 //{
                 //    close_f=2;
                 //}
             }
        }
        for(i=close,close_f=0;i>close-10;i--)
        {
            if(left[i]==0&&right[i]!=79)
            {
                close_f=1;
                break;
            }
            if(right[i]==79&&left[i]!=0)
            {
                close_f=2;
                break;
            }
        }
  /*****************************************************************************************************************************/    
     //printf("left[%d]=%d        right[%d]=%d         img[%d]=%f\r\n",i,left[i],i,right[i],i,img[i]);
        
        //printf("\r\n\r\n");
        //int c,d=4;
        //if(close>60) c=20;
        //else c=close;
        //if(((left[3]+left[c])/2.0-left[c/2+1]<=d&&(left[3]+left[c])/2.0-left[c/2+1]>=-d)&&
            //((right[3]+right[c])/2.0-right[c/2+1]<=d&&(right[3]+right[c])/2.0-right[c/2+1]>=-d))
        //if(((left[3]+(left[c]-left[3])/3.0-left[c/3+1]<=d&&left[3]+(left[c]-left[3])/3.0-left[c/3+1]>=-d)&&
          //  (right[3]+(right[c]-right[3])/3.0-right[c/3+1]<=d&&right[3]+(right[c]-right[3])/3.0-right[c/3+1]>=-d))||
        //((left[3]+2*(left[c]-left[3])/3.0-left[c/3*2+1]<=d&&left[3]+2*(left[c]-left[3])/3.0-left[c/3*2+1]>=-d)&&
        //    (right[3]+2*(right[c]-right[3])/3.0-right[c/3*2+1]<=d&&right[3]+2*(right[c]-right[3])/3.0-right[c/3*2+1]>=-d)))
        //if(findR(left,close,0)||findR(right,close,1))
       /* if(0)
        {
        GPIO_WriteBit(HW_GPIOC, 2, 1);
        //FTM_PWM_ChangeDuty(HW_FTM2, HW_FTM_CH0,5500);
        }
        else 
        {
            GPIO_WriteBit(HW_GPIOC, 2, 0);
           // D=0,q=1,w=0;
            //FTM_PWM_ChangeDuty(HW_FTM2, HW_FTM_CH0,5500);
        }*/

        if(aaa<=3)
{
                    // printf("close=%d\r\n",close);
    average=findP(close);
subtense=findD(5,16,img);
                   
   
   //printf("average=%f             subtense=%f\r\n",average,subtense);


    if(average>0)
    k=q*average*average+w*subtense;
    else 
    k=-q*average*average+w*subtense;
    m=e*average*average+f*subtense;
    //turnStr();
    //printf("k=%d\r\n",k);
    
    if(p==10)
    {
         if(close_f==1)
{
    OLED_ShowStr(83,12,"left ");
    //subtense=findR(left,close,close_f);
           // subtense=findD(3,6,img);
}
else if(close_f==2)
{
     OLED_ShowStr(83,12,"right");
    //subtense=findR(right,close,close_f);
          //  subtense=findD(3,6,img);
}
else OLED_ShowStr(83,6,"     ");//subtense=0;
    OLED_showint2char(83,7,k);
    //int2char(83,5,subtense);
    p=0;
    }
    else{
        p++;
    }
    
    /*
    if(k>0&&k0>0)
    {
        if(k>k0)
        {
    k=(D+1)*k-D*k0;
    k0=(k+D*k0)/(D+1);
        }
     else
     {
    k=(k+k0)/2;
    k0=2*k-k0;
     }         
    }        
    else if(k<=0&&k0<=0)
    {
        if(k<k0)
        {
    k=(D+1)*k-D*k0;
    k0=(k+D*k0)/(D+1);   
        }
        else
        { 
       k=(k+k0)/2;
       k0=2*k-k0;
        }
    }
    else
    {
    k=(D+1)*k-D*k0;
    k0=(k+D*k0)/(D+1); 
    }
    */
    k=k+STRAIGHT;
    

      //OLED_ShowStr(x,y,*str);
    //printf("k=%d\r\n",k);
    if(k>LEFT)
        k=LEFT;
    if(k<RIGHT)
        k=RIGHT;
}
 /* else
  {
      if(k>STRAIGHT)
          k=LEFT;
      if(k<=STRAIGHT)
          k=RIGHT;
  } */
  //printf("k=%d\r\n",k);
   FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH3,k);

if(average>0)
{
    FTM_PWM_ChangeDuty(HW_FTM1, HW_FTM_CH0,PWM_Motor+m); // 0 - 10000  to 0% - 100% 右轮
    FTM_PWM_ChangeDuty(HW_FTM1, HW_FTM_CH1,PWM_Motor); // 0 - 10000  to 0% - 100%   左轮
}
else
{
    FTM_PWM_ChangeDuty(HW_FTM1, HW_FTM_CH0,PWM_Motor); // 0 - 10000  to 0% - 100% 右轮
    FTM_PWM_ChangeDuty(HW_FTM1, HW_FTM_CH1,PWM_Motor+m); // 0 - 10000  to 0% - 100%   左轮
}

    return 0;
}
