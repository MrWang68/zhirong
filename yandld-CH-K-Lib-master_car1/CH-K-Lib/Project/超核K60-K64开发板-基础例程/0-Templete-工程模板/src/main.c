#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "dma.h"
#include "ftm.h"
#include "ssd1306.h"
#include "ov7725.h"
#include "i2c.h"
//#include "Process.h"
#include "Process.h"
#include "lptmr.h"
#include "pit.h"

//0: 80x60
//1: 160x120
//2: 240x180
#define IMAGE_SIZE  0

#if (IMAGE_SIZE  ==  0)
#define OV7620_W    (80)
#define OV7620_H    (60)

#elif (IMAGE_SIZE == 1)
#define OV7620_W    (160)
#define OV7620_H    (120)

#elif (IMAGE_SIZE == 2)
#define OV7620_W    (240)
#define OV7620_H    (180)

#else
#error "Image Size Not Support!"
#endif
static PORT_Type * const PORT_InstanceTable[] = PORT_BASES;
// ?????
uint8_t gCCD_RAM[(OV7620_H)*((OV7620_W/8)+1)];
uint8_t upDateImage[OV7620_H*OV7620_W+1];
//????RAM
//double carP,carI,carD;
/* ??? */
uint8_t * gpHREF[OV7620_H+1];
uint8_t * up_gpHREF[OV7620_H+1];
/* ???? PCLK VSYNC HREF ?????PORT? */
#define BOARD_OV7620_PCLK_PORT      HW_GPIOD
#define BOARD_OV7620_PCLK_PIN       (11)
#define BOARD_OV7620_VSYNC_PORT     HW_GPIOD
#define BOARD_OV7620_VSYNC_PIN      (10)
#define BOARD_OV7620_HREF_PORT      HW_GPIOD
#define BOARD_OV7620_HREF_PIN       (12)
/* 
???????PTA8-PTA15 ???? 0 8 16??? 
0 :PTA0-PTA7
8 :PTA8-PTA15
16:PTA16-PTA24
*/
#define BOARD_OV7620_DATA_OFFSET    (8) 
//double  img[OV7620_H+1];
/* ????? */
typedef enum
{
    TRANSFER_IN_PROCESS, //?????
    NEXT_FRAME,          //?????
    IMG_FAIL,              //??
}OV7620_Status;
  uint8_t status = TRANSFER_IN_PROCESS;

int PWM_Motor=7100;
double steer_q=0.25,steer_w=0,motor_e=1,motor_f=0;


void SerialDispCCDImage(int xSize, int ySize, uint8_t** ppData,uint8_t* upDateImage)
{
    int x,y;
    for(y = 0; y < ySize; y++)
    {
        for(x = 0; x < (xSize/8); x++)
        {
            *(upDateImage+(x-1)*8+y*xSize+0)=(ppData[y][x]>>7) & 0x01;
            *(upDateImage+(x-1)*8+y*xSize+1)=(ppData[y][x]>>6) & 0x01;
            *(upDateImage+(x-1)*8+y*xSize+2)=(ppData[y][x]>>5) & 0x01;
            *(upDateImage+(x-1)*8+y*xSize+3)=(ppData[y][x]>>4) & 0x01;
            *(upDateImage+(x-1)*8+y*xSize+4)=(ppData[y][x]>>3) & 0x01;
            *(upDateImage+(x-1)*8+y*xSize+5)=(ppData[y][x]>>2) & 0x01;
            *(upDateImage+(x-1)*8+y*xSize+6)=(ppData[y][x]>>2) & 0x01;
            *(upDateImage+(x-1)*8+y*xSize+7)=(ppData[y][x]>>0) & 0x01;
        }
    }
    

    
}
void show(uint8_t* upDateImage,uint8_t** ppData){
    int y,x;
    #if 1
        for(y = 0; y < 60; y++)
    {
        for(x = 0; x < 80; x++)
        {
            printf("%d",*(upDateImage+x+y*80));
        }
        printf("\r\n");
    }
    #endif
    #if 0
    for(y = 0; y < 60; y++)
    {
        for(x = 1; x < (80/8)+1; x++)
        {
            printf("%d",(ppData[y][x]>>7) & 0x01);
            printf("%d",(ppData[y][x]>>6) & 0x01);
            printf("%d",(ppData[y][x]>>5) & 0x01);
            printf("%d",(ppData[y][x]>>4) & 0x01);
            printf("%d",(ppData[y][x]>>3) & 0x01);
            printf("%d",(ppData[y][x]>>2) & 0x01);
            printf("%d",(ppData[y][x]>>1) & 0x01);
            printf("%d",(ppData[y][x]>>0) & 0x01);
            if(x == 80/8)
                printf("\r\n");   
        }
        if(y==60 -1)
        {
            printf("                                                                                ");
            printf("\r\n");  
        }				
    }
    #endif
}


float g_fDirectionControlOut2=0,g_fDirectionControlOutOld=0,g_fDirectionControlOutNew=0,V_error=0;
typedef struct PID 
{  
    float V_lasterror; //Error[-1]    
    float V_preserror; //Error[-2]
}PID;
 
 static PID sPID; 
 static PID *sptr=&sPID;
 
 
void PID_Init(void)  
{ 
    sptr->V_lasterror = 0; //Error[-1] 
    sptr->V_preserror = 0; //Error[-2]   
}
float P=0.1,I=0,D=0;
void PID_caculate(uint16_t Set_Value,float Get_Value,int c) 
{ 
    float g_fDirectionControlOut1=0;
    V_error=Set_Value-Get_Value;
    g_fDirectionControlOut1=P*(V_error-sptr->V_lasterror)+I*V_error+D*(V_error-2*sptr->V_lasterror+sptr->V_preserror);
    sptr->V_preserror =sptr->V_lasterror;       
    sptr->V_lasterror = V_error;  
   // if(g_fDirectionControlOut1>200)g_fDirectionControlOut2  +=200;
    //else if(g_fDirectionControlOut1<-200)g_fDirectionControlOut2  +=-200;
   // else
    g_fDirectionControlOut2  +=  g_fDirectionControlOut1;
    g_fDirectionControlOutOld = g_fDirectionControlOutNew;
    g_fDirectionControlOutNew =g_fDirectionControlOut2;
    //if(g_fDirectionControlOutNew<6000)g_fDirectionControlOutNew=6000;
    //else if(g_fDirectionControlOutNew>8000)g_fDirectionControlOutNew=8000;
   // printf(" 2=%f  new=%f  out1=%f     V_error=%f  \r\n",g_fDirectionControlOut2,g_fDirectionControlOutNew,g_fDirectionControlOut1,V_error);
    //FTM_PWM_ChangeDuty(HW_FTM2, HW_FTM_CH0,g_fDirectionControlOutNew); 
    if(c==1) 
    FTM_PWM_ChangeDuty(HW_FTM1, HW_FTM_CH1,g_fDirectionControlOutNew); // 0 - 10000  to 0% - 100%   左轮
    if(c==2)
    FTM_PWM_ChangeDuty(HW_FTM1, HW_FTM_CH0,g_fDirectionControlOutNew); // 0 - 10000  to 0% - 100% 右轮

}
    uint32_t LeftSpeed;
    int RightSpeed;//乘以0.06为当前速度
static void PIT_ISR(void)
{
    uint8_t a;
    FTM_QD_GetData(HW_FTM2, &RightSpeed, &a);
    FTM_QD_ClearCount(HW_FTM2);
    LeftSpeed = LPTMR_PC_ReadCounter(); //获得LPTMR模块的计数值
    LPTMR_ClearCounter();  //计数器归零
    PID_caculate(33,LeftSpeed,1);
    //PID_caculate(50,RightSpeed,2);
    //OLED_showint2char(83,1,value);
       
}

int SCCB_Init(uint32_t I2C_MAP)
{
    
    int r;
    uint32_t instance;
    instance = I2C_QuickInit(I2C_MAP, 50*1000);
    r = ov7725_probe(instance);
    if(r)
    {
        return 1;
    }
    r = ov7725_set_image_size(IMAGE_SIZE);
    if(r)
    {
    
        //printf("OV7725 set image error\r\n");
        return 1;
    }

    return 0;
}

//??????????PTA??steer_q=0.25,steer_w=0,motor_e=1,motor_f=0;
int y=0;
int z=0;
void par_ISR(uint32_t index)
{
            DelayMs(500);
    if(index &(1 <<22))
    {
        if(z<3)
        {
            z++;
        }
        else z=0;
    }
    else if(index & (1 <<21))
    {
       switch(z)
        {
            case 0:     steer_q+=0.02;break;
            case 1:     steer_w+=0.02;break;
            case 2:     motor_e+=0.02;break;
            case 3:     motor_f+=0.02;break;
        }
    }
    else if(index & (1 <<20))
    {
       // GPIO_ToggleBit(HW_GPIOC, 6);
        switch(z)
        {
            case 0:     steer_q-=0.02;break;
            case 1:     steer_w-=0.02;break;
            case 2:     motor_e-=0.02;break;
            case 3:     motor_f-=0.02;break;
        }
    }
}
void OV_ISR(uint32_t index)
{
    
    if(index & (1 << BOARD_OV7620_VSYNC_PIN))
    {
       
        if(status == TRANSFER_IN_PROCESS)                   
        {
            //printf("arrive here!\n");
            status = NEXT_FRAME;                 
           // NVIC_DisableIRQ (PORTC_IRQn);
            //GPIO_ITDMAConfig(BOARD_OV7725_HREF_PORT, BOARD_OV7725_HREF_PIN, kGPIO_IT_FallingEdge, false);
					
            PORTD ->ISFR |= 1<<BOARD_OV7620_PCLK_PIN;      

                              
           // PORTA ->ISFR |= 1 <<  BOARD_OV7725_PCLK_PIN; 
          
            DMA_SetDestAddress (HW_DMA_CH2,(uint32_t)gCCD_RAM );   
           DMA_EnableRequest (HW_DMA_CH2 );
        }
        else                                      
        {
            NVIC_DisableIRQ (PORTD_IRQn);   
          //GPIO_ITDMAConfig(BOARD_OV7725_HREF_PORT, BOARD_OV7725_HREF_PIN, kGPIO_IT_FallingEdge, false);					
            status = IMG_FAIL;                   
        }
    }
# if 0
    static uint8_t status = TRANSFER_IN_PROCESS;
    static uint32_t h_counter, v_counter;
   // uint32_t i;
    
    /* ??? */
    if(index & (1 << BOARD_OV7620_HREF_PIN))
    {
  
        DMA_SetDestAddress(HW_DMA_CH2, (uint32_t)gpHREF[h_counter++]);
        //i = DMA_GetMajorLoopCount(HW_DMA_CH2);
        DMA_SetMajorLoopCounter(HW_DMA_CH2, (OV7620_W/8)+1);
        DMA_EnableRequest(HW_DMA_CH2);
        return;
    }
    /* ??? */
    if(index & (1 << BOARD_OV7620_VSYNC_PIN))
    { 
        GPIO_ITDMAConfig(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_IT_FallingEdge, false);
      //  GPIO_ITDMAConfig(BOARD_OV7620_HREF_PORT, BOARD_OV7620_HREF_PIN, kGPIO_IT_FallingEdge, false);
        switch(status)
        {
            case TRANSFER_IN_PROCESS: //?????????????
                    UserApp(v_counter++);
                    //printf("i:%d %d\r\n", h_counter, i);
                    status = NEXT_FRAME;
                break;
            case NEXT_FRAME: //??????
                status =  TRANSFER_IN_PROCESS;
                break;
            default:
                break;
        }
        
        GPIO_ITDMAConfig(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_IT_FallingEdge, true);
        //GPIO_ITDMAConfig(BOARD_OV7620_HREF_PORT, BOARD_OV7620_HREF_PIN, kGPIO_IT_FallingEdge, true);
        PORTD->ISFR = 0xFFFFFFFF;
        
        h_counter = 0;
        return;
    }
#endif
   // OLED_DrawBMP(0,0,OV7620_H,OV7620_W,gCCD_RAM);
   
    

}

void camera_get_image()
{
 
    status = TRANSFER_IN_PROCESS;
   // printf("IMG_START--1\r\n");
    PORTD->ISFR = ~0;
    NVIC_EnableIRQ(PORTD_IRQn);
   
    
    while(status != NEXT_FRAME)
    {
        if(status == IMG_FAIL)
        {
            status = TRANSFER_IN_PROCESS;
            PORTD->ISFR = ~0;
           // printf("IMG_START--fail\r\n");
            //PORTA->ISFR |= (1<<BOARD_OV7725_VSYNC_PIN);
					//GPIO_CallbackInstall(BOARD_OV7725_VSYNC_PORT, OV_VSYNC_ISR);
           //(BOARD_OV7725_HREF_PORT, BOARD_OV7725_HREF_PIN, kGPIO_IT_FallingEdge, true);    
            
        }

    }
    
    
}
void camera_init(){
   
    uint32_t i;
    DelayInit();
    /* ??????? */
    //GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    //UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    printf("OV7725 test\r\n");
    
    //???GUI
    //CHGUI_Init();
   // GUI_printf(0, 0, "OV7725 test");
    ssd1306_init();
    //?????
     // OLED_ShowStr(10, 0, "1");  
    if(SCCB_Init(I2C0_SCL_PD08_SDA_PD09))
    {
        printf("no ov7725device found!\r\n");
        while(1);
    }
    // OLED_ShowStr(10, 0, "3"); 
     printf("OV7620 setup complete\r\n");
    //??????
   for(i=0; i<OV7620_H+1; i++)
    {
        gpHREF[i] = (uint8_t*)&gCCD_RAM[i*OV7620_W/8];
        up_gpHREF[i]=(uint8_t*)&upDateImage[i*OV7620_W];
    }
    DMA_InitTypeDef DMA_InitStruct1 = {0};
    
    /* ???  ??? ???? */
    GPIO_QuickInit(BOARD_OV7620_PCLK_PORT, BOARD_OV7620_PCLK_PIN, kGPIO_Mode_IPD);
    GPIO_QuickInit(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_Mode_IPD);
   // GPIO_QuickInit(BOARD_OV7620_HREF_PORT, BOARD_OV7620_HREF_PIN, kGPIO_Mode_IPD);
      PORT_InstanceTable[BOARD_OV7620_VSYNC_PORT]->PCR[BOARD_OV7620_VSYNC_PIN] |=(1<<PORT_PCR_PFE_SHIFT)&PORT_PCR_PFE_MASK;  
    /* install callback */
    GPIO_CallbackInstall(BOARD_OV7620_VSYNC_PORT, OV_ISR);
    
   // GPIO_ITDMAConfig(BOARD_OV7620_HREF_PORT, BOARD_OV7620_HREF_PIN, kGPIO_IT_FallingEdge, true);
    //GPIO_ITDMAConfig(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_IT_FallingEdge, true);
    //GPIO_ITDMAConfig(BOARD_OV7620_PCLK_PORT, BOARD_OV7620_PCLK_PIN, kGPIO_DMA_RisingEdge, true);
    GPIO_ITDMAConfig(BOARD_OV7620_VSYNC_PORT, BOARD_OV7620_VSYNC_PIN, kGPIO_IT_FallingEdge, true);
    GPIO_ITDMAConfig(BOARD_OV7620_PCLK_PORT, BOARD_OV7620_PCLK_PIN, kGPIO_DMA_FallingEdge, true);
    /* ??????? */
    for(i=0;i<8;i++)
    {
        GPIO_QuickInit(HW_GPIOC, BOARD_OV7620_DATA_OFFSET+i, kGPIO_Mode_IFT);
    }
    
    //DMA??
    DMA_InitStruct1.chl = HW_DMA_CH2;
    DMA_InitStruct1.chlTriggerSource = PORTD_DMAREQ;
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
    DMA_InitStruct1.minorLoopByteCnt = 1;
    DMA_InitStruct1.majorLoopCnt = ((OV7620_W/8)*OV7620_H);
    
    DMA_InitStruct1.sAddr = (uint32_t)&PTC->PDIR + BOARD_OV7620_DATA_OFFSET/8;
    DMA_InitStruct1.sLastAddrAdj = 0;
    DMA_InitStruct1.sAddrOffset = 0;
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.sMod = kDMA_ModuloDisable;
    
    DMA_InitStruct1.dAddr = (uint32_t)gpHREF[0];
    DMA_InitStruct1.dLastAddrAdj = 0;
    DMA_InitStruct1.dAddrOffset = 1;
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.dMod = kDMA_ModuloDisable;

    
    
    /* initialize DMA moudle */
    DMA_Init(&DMA_InitStruct1);
}















/*
double findP(int i){
    double b,c=3;
    for(int a=c;a<i;a++)
    {
        //printf("img[%d]=%f\r\n",a,img[a]);
        b+=img[a];
    }
    b=b/(i-c)-40;
    //printf("b=%f\r\n",b);
    return b;
}
*/










int main(void)
{
    
        /* 使用简易初始化初始化PTE端口的6引脚作为IO使用 设置为推挽输出模式 */
    GPIO_QuickInit(HW_GPIOC, 6, kGPIO_Mode_OPP);
    /* 另外一种初始化方式: 结构体模式的初始化 有点类似STM32固件库*/
//    GPIO_InitTypeDef GPIO_InitStruct1;
//    GPIO_InitStruct1.instance = HW_GPIOE;
//    GPIO_InitStruct1.mode = kGPIO_Mode_OPP;
//    GPIO_InitStruct1.pinx = 6;
//    GPIO_Init(&GPIO_InitStruct1);
    
    /* 控制PTE端口的6引脚输出低电平 */
    GPIO_WriteBit(HW_GPIOC, 6, 1);    /* 使用简易初始化初始化PTE端口的6引脚作为IO使用 设置为推挽输出模式 */
   // GPIO_QuickInit(HW_GPIOC, 6, kGPIO_Mode_OPP);
    /* 另外一种初始化方式: 结构体模式的初始化 有点类似STM32固件库*/
//    GPIO_InitTypeDef GPIO_InitStruct1;
//    GPIO_InitStruct1.instance = HW_GPIOE;
//    GPIO_InitStruct1.mode = kGPIO_Mode_OPP;
//    GPIO_InitStruct1.pinx = 6;
//    GPIO_Init(&GPIO_InitStruct1);
    
    /* 控制PTE端口的6引脚输出低电平 */
    GPIO_WriteBit(HW_GPIOC, 6, 1);
    GPIO_QuickInit(HW_GPIOC, 2, kGPIO_Mode_OPP);
    GPIO_WriteBit(HW_GPIOC, 2, 0);
     //UART_QuickInit(UART1_RX_PC03_TX_PC04, 115200);
    UART_QuickInit(UART0_RX_PA15_TX_PA14, 115200);
    
    /* 在PC01 上产生一定频率的方波 便于脉冲计数测量 */
    FTM_PWM_QuickInit(FTM1_CH1_PA13, kPWM_EdgeAligned, 1000);
    /* 设置FTM 的占空比 */
    FTM_PWM_ChangeDuty(HW_FTM1, HW_FTM_CH1, 5000); // 50%占空比 0-10000 对应 0-100%
    
    /*测速驱动*/
    //LPTMR_PC_QuickInit(LPTMR_ALT2_PC05); /* 脉冲计数 */
    LPTMR_PC_QuickInit(LPTMR_ALT1_PA19);
    GPIO_QuickInit(HW_GPIOA,12, kGPIO_Mode_OPP);
    GPIO_WriteBit(HW_GPIOA,12, 1);  
    FTM_QD_QuickInit(FTM2_QD_PHA_PA10_PHB_PA11, kFTM_QD_NormalPolarity,kQD_CountDirectionEncoding);
    /* 开启一个PIT定时器 产生1S中断 在中断中读取LPTMR的计数值 */
    PIT_QuickInit(HW_PIT_CH1, 1000 * 10);
    PIT_CallbackInstall(HW_PIT_CH1, PIT_ISR);
    PIT_ITDMAConfig(HW_PIT_CH1, kPIT_IT_TOF, true);
   
    /*摄像头，oled*/
    camera_init();
    //OLED_ShowStr(10, 9, "5");
    /*舵机驱动*/
       FTM_PWM_QuickInit(FTM0_CH3_PC04, kPWM_EdgeAligned,100);
     //FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH3,8640);//right
     FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH3,8480);//stright
     //FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH3,8400); //leftn
    
    /*电机驱动*/
    //GPIO_QuickInit(HW_GPIOA, 1, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOB, 10, kGPIO_Mode_OPP);//DIR1
    GPIO_QuickInit(HW_GPIOB, 11, kGPIO_Mode_OPP);//DIR2
    GPIO_QuickInit(HW_GPIOB, 16, kGPIO_Mode_OPP);//DIR3
    GPIO_QuickInit(HW_GPIOB, 17, kGPIO_Mode_OPP);//DIR4  
    FTM_PWM_QuickInit(FTM1_CH0_PB00, kPWM_EdgeAligned,20000);
    FTM_PWM_QuickInit(FTM1_CH1_PB01, kPWM_EdgeAligned,20000);
    //FTM_PWM_QuickInit(FTM1_CH1_PB13, kPWM_EdgeAligned,20000);
    GPIO_WriteBit(HW_GPIOB, 10, 0);//DIR1
    GPIO_WriteBit(HW_GPIOB, 11, 1);//DIR2
    GPIO_WriteBit(HW_GPIOB, 16, 1);//DIR3
    GPIO_WriteBit(HW_GPIOB, 17, 0);//DIR4
    FTM_PWM_ChangeDuty(HW_FTM1, HW_FTM_CH0,PWM_Motor); // 0 - 10000  to 0% - 100% 右轮
    FTM_PWM_ChangeDuty(HW_FTM1, HW_FTM_CH1,PWM_Motor); // 0 - 10000  to 0% - 100%   左轮
    //参数调整

    GPIO_QuickInit(HW_GPIOB, 22, kGPIO_Mode_IFT);//PQS1
    GPIO_QuickInit(HW_GPIOB, 21, kGPIO_Mode_IFT);//PQS2
    GPIO_QuickInit(HW_GPIOB, 20, kGPIO_Mode_IFT);//PQS3

    GPIO_CallbackInstall(HW_GPIOB, par_ISR);
    
    GPIO_ITDMAConfig(HW_GPIOB, 22, kGPIO_IT_RisingEdge, true);
    GPIO_ITDMAConfig(HW_GPIOB, 21, kGPIO_IT_RisingEdge, true);
    GPIO_ITDMAConfig(HW_GPIOB, 20, kGPIO_IT_RisingEdge, true);
    /*******************************************************************/
    int a=0;
    GPIO_WriteBit(HW_GPIOA, 17, 1);
    while(1)
    {
        # if 0
           if(b==100)
        {
        FTM_PWM_ChangeDuty(HW_FTM1, HW_FTM_CH0,7100);
        FTM_PWM_ChangeDuty(HW_FTM1, HW_FTM_CH1,7100);
        }
        else
        {
        b++;
        }
        # endif
         camera_get_image();
         SerialDispCCDImage(80,60,gpHREF,upDateImage);
        //OLED_DrawBMP(80,60,gpHREF);
        handle(up_gpHREF,PWM_Motor,steer_q,steer_w,motor_e,motor_f);
        //show(upDateImage,gpHREF);
        if(a==20)
        {
        OLED_DrawBMP(0,0,OV7620_H,OV7620_W,upDateImage);
        //printf("LPTMR:%dHz\r\n", value);
        OLED_showint2char(83,0,(int)(LeftSpeed));
        OLED_showint2char(83,1,(int)(RightSpeed));   
        //OLED_showint2char(83,0,z);
        //OLED_showint2char(83,1,(int)(steer_q*100));
        //OLED_showint2char(83,2,(int)(steer_w*100));
        //OLED_showint2char(83,3,(int)(motor_e*100));
        //OLED_showint2char(83,4,(int)(motor_f*100));       
        a=0;
        }
        else
        {
        a++;
        }
    //printf("%x ",gpHREF[0][1]);
    }
}
