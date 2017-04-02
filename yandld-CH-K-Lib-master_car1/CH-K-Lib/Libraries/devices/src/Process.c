
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
double xl;
int xielvnum = -1;
int xielvnum2 = -1;
int xielvnum3 = -1;
    

int close=45,close_f=0;//取值范围
int l;//封闭标准
int left[61]={40,40},right[61]={40,40};

int leftXmen;

int rightXmen;

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

//绝对值
int myabs(int a){
    if(a >= 0)
        return a;
    else if(a < 0)
        return -a;
}
//比较大小
int findm(int a,int b,int flag){
    int rt;
    if(flag == 0) //max
    {
    if(a >= b)
        return a;
    else
        return b;
    }
    else if(flag == 1) //min
    {
    if(a >= b)
        return b;
    else
        return a;
    }
}



/*
**findD(18,20,left)
**求斜率（最小二乘法）
*/
double findD(uint8_t begin,uint8_t end,int *p)  
{
   int xsum=0,ysum=0,xysum=0,x2sum=0;
   uint8_t i=0;
   float result=0;
   static float resultlast,resultsum;
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
    
  if(xielvnum < 0)
      resultlast = result;
  
  else if(xielvnum >= 0 && xielvnum < 10){
      resultsum += result;
   }
  else {
     // if((myabs(resultsum/5-resultlast)) > 1)
            resultlast = resultsum/10;
      resultsum = 0;
      xielvnum = -1;
  }
      xielvnum++;
  result = resultlast;
  return result;

}



//??????
double findDD(int m, int n,double *p){
    double xielv = 0;
    static double xielvlast,xielvsum;
	
    int a[5],b[5],k[4] = {1,4,2,1};
    double xielvs,xl[5];
	a[0] = 0;
	b[0] = m-1;
	xl[0] = 0;
	for(int i=1;i<5;i++){
		a[i] = b[i-1]+1;
		b[i] = a[i]+(n-m)*k[i-1]/8;
		if(b[i] - a[i] == 0)
			xl[i] = 0;
		else
			xl[i] =(double)(p[b[i]] - p[a[i]])/(b[i] - a[i]);
		xielvs += xl[i];
	}
	xielv = xielvs/8;
	
    
     //   xielv =(double)(img[n] - img[m])/(n - m);
  if(xielvnum2 < 0)
      xielvlast = xielv;
  
  else if(xielvnum2 >= 0 && xielvnum2 < 10){
      xielvsum += xielv;
   }
  else {
    //  if((myabs(xielvsum/5-xielvlast)) > 1)
         xielvlast = xielvsum/10;
      xielvsum = 0;
      xielvnum2 = -1;
  }
      xielvnum2++;
    xielv = xielvlast;
  
    return xielv*100;
}


/*
double findDDD(int m, int n,int my,int ny)
    {
    double xielv2 = 0;
    static double xielv2last,xielv2sum;
		if(n - m == 0)
			xielv2 = 0;
		else
			xielv2 =(double)(ny - my)/(n - m);
        
  if(xielvnum3 < 0)
      xielv2last = xielv2;
  
  else if(xielvnum3 >= 0 && xielvnum3 < 10){
      xielv2sum += xielv2;
   }
  else {
    //  if((myabs(xielvsum/5-xielvlast)) > 1)
      xielv2last = xielv2sum/10;
      xielv2sum = 0;
      xielvnum3 = -1;
  }
      xielvnum3++;
    xielv2 = xielv2last;
  
    return xielv2;
}
*/



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
void CoverLine(int startLine,int endLine,int x,int lr,unsigned char **gpHREE)
    {
        int i,j;
        double slope=0;
    if(lr==1)//left
    {
        //slope=fineDDD(startLine,endLine,left[startLine],x);
        slope=(x -left[startLine])/(double)(endLine -startLine);
        
        for(i=startLine+1;i<endLine;i++)
        {
      left[i]=(int)(left[startLine]+(i-startLine)*slope);
        gpHREE[60-i][left[i]]=2;
        }
            left[endLine]=x;
         for(i=59-endLine;i>=0;i--)
       {

         if(get_gp(i,left[59-i],gpHREE)==true)
         {
            for(j=left[59-i];j<80;j++)
            {
             // printf("->left[%d]=%d \r\n",60-i,j); 
            if(get_gp(i,j,gpHREE)==false)
                   {
                       //set_gp(i,j,gpHREE,3);
                     left[60-i]=j-1;
                        break;
                   }
            }
         if(j==80){left[60-i]=79;j=0;}
           
         }
         
         
         if(get_gp(i,left[59-i],gpHREE)==false) 
            {
              
                for(j=left[59-i];j>=0;j--)
               {
                   //printf("<-left[%d]=%d \r\n",60-i,j);
                    if(get_gp(i,j,gpHREE))
                    {
                        //set_gp(i,j,gpHREE,3);
                        left[60-i]=j;
                        break;
                    }
                }
               if(j==-1){left[60-i]=0;j=0;} 
               
            }
        }
    }
    if(lr==2)    
    {
        //slope=fineDDD(startLine,endLine,left[startLine],x);
        slope=(x -right[startLine])/(double)(endLine -startLine);
        
        for(i=startLine+1;i<endLine;i++)
        {
      right[i]=(int)(right[startLine]+(i-startLine)*slope);
        gpHREE[60-i][right[i]]=2;
        }
            right[endLine]=x;
         for(i=59-endLine;i>=0;i--)
       {
            if(get_gp(i,right[59-i],gpHREE)==true)
          {
             
              for(j=right[59-i];j>=0;j--)
              {
                  if(get_gp(i,j,gpHREE)==false)
                  {
                      right[60-i]=j+1;
                      //set_gp(i,j,gpHREE,2);
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
                     //set_gp(i,j,gpHREE,2);
                     right[60-i]=j;
                     break;
                 }             
              }
              if(j==80){right[60-i]=j-1;j=0;}

          }

        }
    }
}

void findLine(unsigned char **gpHREE)
    {
        int i,j,k;
        int leftError=0;
        int leftChange=0;
        int leftChange_FirstFlag=0;
        double leftXmen_slope=0;
        int rightError=0;
        int rightChange=0;
        int rightChange_FirstFlag=0;
        double rightXmen_slope=0;
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
                       //set_gp(i,j,gpHREE,3);
                     left[60-i]=j-1;
                        break;
                   }
            }
         if(j==80){left[60-i]=79;j=0;}
           leftError=0;
         }
         
         
         if(get_gp(i,left[59-i],gpHREE)==false) 
            {
              
                for(j=left[59-i];j>=0;j--)
               {
                   //printf("<-left[%d]=%d \r\n",60-i,j);
                    if(get_gp(i,j,gpHREE))
                    {
                        //set_gp(i,j,gpHREE,3);
                        left[60-i]=j;
                        break;
                    }
                }
               if(j==-1){left[60-i]=0;j=0;} 
               if(leftError==0&&leftChange_FirstFlag==0)
               {
               leftChange=59-i;
                //   printf("leftChange=%d    j=%d\r\n",leftChange,j);
                   //OLED_showint2char(83,0,leftChange);
               }

               leftError++;
               // printf("leftChange=%d    j=%d\r\n",leftChange,j);   

               if(leftError==3&&leftChange_FirstFlag==0)
               {
                   leftChange_FirstFlag=1;
                   leftXmen=leftChange;
                 //  printf("leftXmen=%d\r\n",leftXmen);
                  // OLED_showint2char(83,1,leftXmen);
               }
            }
            
            if(get_gp(i,right[59-i],gpHREE)==true)
          {
             
              for(j=right[59-i];j>=0;j--)
              {
                  if(get_gp(i,j,gpHREE)==false)
                  {
                      right[60-i]=j+1;
                      //set_gp(i,j,gpHREE,2);
                      break;
                  }
              }
              rightError=0;
          }
          if(j==-1){right[60-i]=0;j=0;}
          if(get_gp(i,right[59-i],gpHREE)==false)
          {
              for(j=right[59-i];j<80;j++)
              {
                 if(get_gp(i,j,gpHREE))
                 {
                     //set_gp(i,j,gpHREE,2);
                     right[60-i]=j;
                     break;
                 }             
              }
              if(j==80){right[60-i]=j-1;j=0;}
              if(rightError==0&&rightChange_FirstFlag==0)
               {
               rightChange=59-i;
                //   printf("rightChange=%d    j=%d\r\n",rightChange,j);
                   //OLED_showint2char(83,0,rightChange);
               }

               rightError++;
               // printf("rightChange=%d    j=%d\r\n",rightChange,j);   

               if(rightError==3&&rightChange_FirstFlag==0)
               {
                   rightChange_FirstFlag=1;
                   rightXmen=rightChange;
                 //  printf("rightXmen=%d\r\n",rightXmen);
                  // OLED_showint2char(83,1,rightXmen);
               }
          }
        // printf("left[%d]=%d        right[%d]=%d    i=%d\r\n",60-i,left[60-i],60-i,right[60-i],i);
        }
       /**********************************************************************************************/
       //左边寻点补线
      if(leftChange_FirstFlag==1&&leftXmen>=4)
        {
           leftChange_FirstFlag=0;
            //leftXmen_slope=findD(leftXmen-4,leftXmen-1,left);
           leftXmen_slope=0;
           //printf("leftXmen_slope=%f\r\n",leftXmen_slope);
            int leftUpFlag;
           for(i=59-leftXmen;i>=0;i--)
           {
               leftUpFlag=(int)(left[leftXmen]+(60-i-leftXmen)*leftXmen_slope);
                //printf("1.gpHREE[%d][%d]=%d\r\n",i,leftUpFlag,gpHREE[i][leftUpFlag]); 
               //printf("gpHREE[%d][%d]=%d\r\n",i,leftUpFlag,gpHREE[i][leftUpFlag]);     
               if(get_gp(i,leftUpFlag,gpHREE)==true)
               {
                   if(right[60-i]<=leftUpFlag)
                   {
                       break;
                   }
               //printf("gpHREE[%d][%d]=%d\r\n",i,leftUpFlag,gpHREE[i][leftUpFlag]); 
                   i=i-5;
                   for(j=leftUpFlag;j<=79;j++)
                   {
                       if(get_gp(i,j,gpHREE)==false)
                       {
                           j=j-1;
                           break;
                       }
                       
                   }
                   //printf("2.gpHREE[%d][%d]=%d\r\n",i,j,gpHREE[i][j]); 
                   for(i=i+1;i<60-leftXmen;i++)
                   {
                       k=j;
                       //printf("3.gpHREE[%d][%d]=%d\r\n",i-1,k,gpHREE[i-1][k]); 
                       if(get_gp(i,j,gpHREE)==true)
                       {
                            for(;j<80;j++)
                            {
             // printf("->left[%d]=%d \r\n",60-i,j); 
                                if(get_gp(i,j,gpHREE)==false)
                                {
                       //set_gp(i,j,gpHREE,3);
                     
                                    break;
                                }
                            }
                            if(j==80){}
           
                      }        
                      if(get_gp(i,j,gpHREE)==false) 
                      {
                        for(;j>=0;j--)
                        {
                   //printf("<-left[%d]=%d \r\n",60-i,j);
                            if(get_gp(i,j,gpHREE))
                            {
                        //set_gp(i,j,gpHREE,3);  
                                break;
                            }
                        }
                        if(j==-1){break;} 
                        if(j-k>=3){break;}
                     }
                        
                       //if(get_gp(i,j,gpHREE)==false)
                      // for()
                 }
           // leftXmen_slope=findDDD(leftXmen,60-(i-1),left);
                if(j==-1)
             CoverLine(leftXmen,60-(i-1),k,1,gpHREE);      
     

                   break;
               }
           }
       }
 /**********************************************************************************************/       
       //右边寻点补线
      if(rightChange_FirstFlag==1&&rightXmen>=4)
        {
           rightChange_FirstFlag=0;
            //rightXmen_slope=findD(rightXmen-4,rightXmen-1,right);
           rightXmen_slope=0;
           //printf("rightXmen_slope=%f\r\n",rightXmen_slope);
            int rightUpFlag;
           for(i=59-rightXmen;i>=0;i--)
           {
               rightUpFlag=(int)(right[rightXmen]+(60-i-rightXmen)*rightXmen_slope);
                //printf("1.gpHREE[%d][%d]=%d\r\n",i,rightUpFlag,gpHREE[i][rightUpFlag]); 
               //printf("gpHREE[%d][%d]=%d\r\n",i,rightUpFlag,gpHREE[i][rightUpFlag]);     
               if(get_gp(i,rightUpFlag,gpHREE)==true)
               {
                   if(left[60-i]>=rightUpFlag)
                   {
                       break;
                   }
                   i=i-5;
               //printf("gpHREE[%d][%d]=%d\r\n",i,leftUpFlag,gpHREE[i][leftUpFlag]); 
                   for(j=rightUpFlag;j>=0;j--)
                   {
                       if(get_gp(i,j,gpHREE)==false)
                       {
                           j=j+1;
                           break;
                       }
                       
                   }
                   
                   //printf("2.gpHREE[%d][%d]=%d\r\n",i,j,gpHREE[i][j]); 
                   for(i=i+1;i<60-rightXmen;i++)
                   {
                       k=j;
                       //printf("3.gpHREE[%d][%d]=%d\r\n",i-1,k,gpHREE[i-1][k]); 
                       if(get_gp(i,j,gpHREE)==true)
                       {
                            for(;j>=0;j--)
                            {
             // printf("->left[%d]=%d \r\n",60-i,j); 
                                if(get_gp(i,j,gpHREE)==false)
                                {
                       //set_gp(i,j,gpHREE,3);
                     
                                    break;
                                }
                            }
                            if(j==-1){}
           
                      }        
                      if(get_gp(i,j,gpHREE)==false) 
                      {
                        for(;j<80;j++)
                        {
                   //printf("<-left[%d]=%d \r\n",60-i,j);
                            if(get_gp(i,j,gpHREE))
                            {
                        //set_gp(i,j,gpHREE,3);  
                                break;
                            }
                        }
                        if(j==80){break;} 
                        if(k-j>=3){break;}
                     }
                        
                       //if(get_gp(i,j,gpHREE)==false)
                      // for()
                 }
           // leftXmen_slope=findDDD(leftXmen,60-(i-1),left);
                if(j==80)
             CoverLine(rightXmen,60-(i-1),k,2,gpHREE);      
     

                   break;
               }
           }
       }
    }


int p;
int m,k,k0=STRAIGHT;
double ka,kdd;

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


    /*******************************************************************************************************/
     findLine(gpHREE);
    /***********************************************************************************************************************/
        int i,j;
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

//subtense=findD(3,10,img);
                   xl = findDD(5,25,img);
    if(average>0){
        q= 0.25*0.7;
        w= 0.3;
    k=q*average*average+w*xl;
        ka = q*average*average;
        kdd = w*xl;
    }
    
    else {
        q= 0.25*0.7;
        w= 0.35;
    k=-q*average*average+w*xl;
        ka = -q*average*average;
        kdd = w*xl;
        //kdd = tan(w);
    }
    
    m=e*average*average+f*subtense;
    //turnStr();
    //printf("k=%d\r\n",k);
    

    
    if(p==10)
    {
/*
    OLED_showint2char(83,1,k);
    OLED_showint2char(83,2,ka);
    OLED_showint2char(83,3,average);
    OLED_showint2char(83,4,kdd);
    OLED_showint2char(83,5,xl);
*/
         if(close_f==1)
{
   // OLED_ShowStr(83,1,"left ");
    //subtense=findR(left,close,close_f);
           // subtense=findD(3,6,img);
}
else if(close_f==2)
{
    // OLED_ShowStr(83,1,"right");
    //subtense=findR(right,close,close_f);
          //  subtense=findD(3,6,img);
}
else OLED_ShowStr(83,6,"     ");//subtense=0;
    //OLED_showint2char(83,7,k);
    //int2char(83,5,subtense);
    p=0;
    }
    else{
        p++;
    }
    
    //k = 1.5*xl;
    //k=1.8*xl;
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
