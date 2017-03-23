#include "Functions.h"
#include "ssd1306.h"

uint8_t gCCD_RAM[(OV7725_H)*((OV7725_W/8))];   
uint8_t * gpHREF;
extern uint8_t* img[OV7725_H];  
volatile IMG_STATUS_e      img_flag;   

/**
* @brief 
* @param  
* @retval 
* @note
*/
void init_All()
{
    
    DelayInit();
    //GPIO_QuickInit(HW_GPIOD, 10, kGPIO_Mode_OPP);
    
    motor_steer_init();
    uart_init();
   
    camera_init();
    
}
/**
 
* @brief é€šè¿‡è“ç‰™ä¸²å£å‘é€å›¾åƒ
* @param  srcDataï¼šå­˜æ”¾èµ›é“å›¾åƒçš„äºŒç»´æ•°ç»„
* @retval 60*80çš„01çŸ©é˜µ
* @note
*/
void SerialDispImage(uint8_t* srcData)
{
    uint32_t x, y;
    for(y = 0; y < ySize; y++)
    {
        for(x = 0; x < (xSize); x++)
        {
           
            printf("%d",srcData[y*OV7725_W+x]);
             
        }
        printf("\r\n");
        if(y==ySize -1)
        {
            printf("                                                                                ");
            printf("\r\n"); 
            printf("\r\n"); 
        }				
    }
    
}

/**
 
* @brief ¶ÔÉãÏñÍ·Ä£¿é½øĞĞ³õÊ¼»¯
* @param  
* @retval 
* @note
*/
void camera_init()
{
    
    
   // uint32_t i;
    if(SCCB_Init(I2C0_SCL_PD08_SDA_PD09))
    {
        
        printf("no ov7725device found!\r\n");
        while(1);
    }
  
    printf("OV7620 setup complete\r\n");
    gpHREF = (uint8_t *) &gCCD_RAM;
    ov7725_port_init();
    
}

/**
 
* @brief ÉãÏñÍ·SCCB³õÊ¼»¯
* @param  I2C_MAP£ºÉãÏñÍ·Òı½ÅÖĞµÄSCLÓëSDAÁ½¸öÒı½Å
    eg:I2C0_SCL_PB00_SDA_PB01
* @retval 
* @note
*/
int SCCB_Init(uint32_t I2C_MAP)
{
    int r;
    uint32_t instance;
    instance = I2C_QuickInit(I2C_MAP, 40*1000);  //
    r = ov7725_probe(instance);
    if(r)
    {
        return 1;
    }
    r = ov7725_set_image_size(0);
    if(r)
    {
        printf("OV7725 set image error\r\n");
        return 1;
    }
    return 0;
}

void ov7725_port_init()
{
     /* ÏñËØÖĞ¶Ï ³¡ÖĞ¶Ï ĞĞÖĞ¶ÏÒı½Å³õÊ¼»¯ */
    GPIO_QuickInit(BOARD_OV7725_PCLK_PORT, BOARD_OV7725_PCLK_PIN, kGPIO_Mode_IPD);
    GPIO_QuickInit(BOARD_OV7725_VSYNC_PORT, BOARD_OV7725_VSYNC_PIN, kGPIO_Mode_IPD);
  
    
     /* install callback */
    //Í¬Ê±×¢²á³¡ÖĞ¶ÏºÍĞĞÖĞ¶ÏµÄ·şÎñº¯Êı
    
    GPIO_CallbackInstall(BOARD_OV7725_VSYNC_PORT, OV_VSYNC_ISR);
    for(uint8_t i=0;i<8;i++)
    {
        GPIO_QuickInit(HW_GPIOC, BOARD_OV7725_DATA_OFFSET+i, kGPIO_Mode_IFT);
    }
    //DMA³õÊ¼»¯
     DMA_init();
     
    
     GPIO_ITDMAConfig(BOARD_OV7725_VSYNC_PORT, BOARD_OV7725_VSYNC_PIN, kGPIO_IT_FallingEdge, true);
    
     GPIO_ITDMAConfig(BOARD_OV7725_PCLK_PORT, BOARD_OV7725_PCLK_PIN, kGPIO_DMA_RisingEdge, true);

}
/**
 
* @brief ĞĞ³¡ÖĞ¶Ï·şÎñ³ÌĞò
* @param  index ĞĞ³¡ÖĞ¶Ï¶ÔÓ¦µÄÒı½Å
* @retval 
* @note
*/
//ĞĞ³¡ÖĞ¶Ï¶¼Ê¹ÓÃPTAÖĞ¶Ï
void OV_VSYNC_ISR(uint32_t index)
{
    if(index & (1 << BOARD_OV7725_VSYNC_PIN))
    {
       
        if(img_flag == IMG_START)                   
        {
            //printf("arrive here!\n");
            img_flag = IMG_GATHER;                 
           // NVIC_DisableIRQ (PORTC_IRQn);
            //GPIO_ITDMAConfig(BOARD_OV7725_HREF_PORT, BOARD_OV7725_HREF_PIN, kGPIO_IT_FallingEdge, false);
					
            PORTD ->ISFR |= 1<<BOARD_OV7725_PCLK_PIN;      

            DMA_EnableRequest (HW_DMA_CH2 );                  
           // PORTA ->ISFR |= 1 <<  BOARD_OV7725_PCLK_PIN; 
          
            DMA_SetDestAddress (HW_DMA_CH2,(uint32_t)gCCD_RAM );   
             OLED_DrawBMP(0,0,80,60,gpHREF);
        }
        else                                      
        {
           // NVIC_DisableIRQ (PORTC_IRQn);   
          //GPIO_ITDMAConfig(BOARD_OV7725_HREF_PORT, BOARD_OV7725_HREF_PIN, kGPIO_IT_FallingEdge, false);					
            img_flag = IMG_FAIL;                   
        }
    }
    
    /* ĞĞÖĞ¶Ï */
//    if(index & (1 << BOARD_OV7725_HREF_PIN))
//    {
// 
//            //printf("HREF: %d  \r\n",h_counter);
//           if(h_counter<60)
//           {
//            //½øÈëĞĞÖĞ¶Ï¾ÍÉè¶¨DMA´«Êäµ±Ç°ĞĞµÄÄ¿µÄµØÖ· 
//            DMA_SetDestAddress(HW_DMA_CH2, (uint32_t)gpHREF[h_counter++]);
//            //i = DMA_GetMajorLoopCount(HW_DMA_CH2);
//            DMA_SetMajorLoopCounter(HW_DMA_CH2, OV7725_W/8+1);
//            DMA_EnableRequest(HW_DMA_CH2);
//               printf("HREF: %d  \r\n",h_counter);
//           }
//           else 
//           {
//               printf("HREF: %d  \r\n",h_counter++);
//               NVIC_DisableIRQ(PORTB_IRQn);//close HREF interrupt
//               NVIC_EnableIRQ(PORTC_IRQn);//open VSYNC interrupt
//           }
            //if(h_counter>=60)
            //{
             //   img_flag = IMG_FINISH;
             //   NVIC_DisableIRQ(PORTB_IRQn);
               // GPIO_ITDMAConfig(BOARD_OV7725_HREF_PORT, BOARD_OV7725_HREF_PIN, kGPIO_IT_FallingEdge, false);
                //PORTA->ISFR |= (1<<BOARD_OV7725_HREF_PIN);
           // }
           // PORTA->ISFR |= (1<<BOARD_OV7725_HREF_PIN);
                      
                //img_flag = IMG_FINISH;
                //printf("IMG_FINISH\r\n");
                //GPIO_ITDMAConfig(BOARD_OV7725_HREF_PORT, BOARD_OV7725_HREF_PIN, kGPIO_IT_FallingEdge, false);
                //DMA_DisableRequest(HW_DMA_CH2);
                //GPIO_ITDMAConfig(BOARD_OV7725_PCLK_PORT, BOARD_OV7725_PCLK_PIN, kGPIO_DMA_RisingEdge, false);
           
        
        //return;
    //}
   
}

//void OV_VSYNC_ISR(uint32_t index)
//{
//    
//    
//    if(index & (1 << BOARD_OV7725_VSYNC_PIN))
//    {
//        NVIC_DisableIRQ(PORTC_IRQn);
//         printf("VSYN\r\n");
//     
//        if(img_flag == IMG_FINISH)
//        {
//            img_flag = IMG_GATHER;
//            NVIC_EnableIRQ(PORTB_IRQn);
//            printf("IMG_GATHER\r\n");
//            h_counter = 0;
//           
//        }
//        h_counter = 0;
////        else 
////        {
////            img_flag = IMG_FAIL;
////            printf("IMG_FAIL\r\n");
////            //GPIO_ITDMAConfig(BOARD_OV7725_VSYNC_PORT, BOARD_OV7725_VSYNC_PIN, kGPIO_IT_FallingEdge, false);
////            
////        }
//        //ÔÚÃ¿¸öĞĞÖĞ¶ÏÖĞ¶¼Òª½«ĞĞÊıÖÃ0
//        //return;
//    }
//}
/* Í¼Ïñ´¦Àíº¯Êı */
//void image_process(uint32_t vcount)
//{
//    //printf("Vcounts:%d \r\n", vcount);
//    //image_transfer(gpHREF, CameraData);
//    //OLED_PrintImage(gpHREF,60,80);
//    //SerialDispImage(gpHREF);
//}

/**
 
* @brief ¶Ô¶æ»úºÍµç»úµÄÈıÂ·FTM²¨³õÊ¼»¯
* @param  
* @retval 
* @note
*/
void motor_steer_init(void)
{
    //  FTM_PWM_QuickInit(FTM2_CH0_PB18, kPWM_EdgeAligned, 5000);//µç»úFTM³õÊ¼»¯
	 // FTM_PWM_QuickInit(FTM2_CH1_PB19, kPWM_EdgeAligned, 5000);//µç»úFTM³õÊ¼»¯
	 // FTM_PWM_QuickInit(FTM0_CH7_PD07, kPWM_EdgeAligned, 100);//¶æ»úFTM³õÊ¼»¯
	 // GPIO_QuickInit(HW_GPIOB, 10, kGPIO_Mode_OPP);//·½Ïò¿ØÖÆ¿Ú³õÊ¼»¯
	 // GPIO_QuickInit(HW_GPIOB, 11, kGPIO_Mode_OPP); //·½Ïò¿ØÖÆ¿Ú³õÊ¼»¯

}
/**
 
* @brief uartÄ£¿é³õÊ¼»¯
* @param  
* @retval 
* @note
*/
void uart_init(void)
{
   UART_QuickInit(UART4_RX_PC14_TX_PC15, 115200); 
  // UART_CallbackRxInstall(HW_UART5, UART_RX_ISR);
    /* ´ò¿ª´®¿Ú½ÓÊÕÖĞ¶Ï¹¦ÄÜ IT ¾ÍÊÇÖĞ¶ÏµÄÒâË¼*/
   // UART_ITDMAConfig(HW_UART5, kUART_IT_Rx, false);
}
/**
 
* @brief ´®¿ÚÖĞ¶Ï·şÎñ³ÌĞò
* @param  
* @retval 
* @note
*/
void UART_RX_ISR(uint16_t byteReceived)
{
    /* ½«½ÓÊÕµ½µÄÊı¾İ·¢ËÍ»ØÈ¥ */
    //UART_WriteByte(HW_UART5, byteReceived+12);
    if(byteReceived =='a')
    {
        debug_light_staues = 1;
        printf("receive :%d",debug_light_staues);
        
        
    }
    else if (byteReceived =='b')
    {
        debug_light_staues = 2;
        printf("receive :%d",debug_light_staues);
        
    }
}
/**
 
* @brief Êı¾İ´«ÊäDMAÍ¨µÀºÍ²âËÙDMAÍ¨µÀ³õÊ¼»¯
* @param  
* @retval 
* @note
*/
void DMA_init(void)
{
    //Í¼ÏñÊı¾İ´«ÊäDMAÍ¨µÀ³õÊ¼»¯
    DMA_InitTypeDef DMA_InitStruct1 = {0};
    //DMAÅäÖÃ
    DMA_InitStruct1.chl = HW_DMA_CH2;//DMA Í¨µÀ2
    DMA_InitStruct1.chlTriggerSource = PORTD_DMAREQ;//PortAÎªDMA´¥·¢Ô´
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
    DMA_InitStruct1.minorLoopByteCnt = 1;//minorLoopÖĞÒ»´Î´«ÊäÒ»¸ö×Ö½Ú
    //MajorLoopµÄ´«Êä´ÎÊı:???????/??MinorLoop??????(8?)
    DMA_InitStruct1.majorLoopCnt = ((OV7725_W*OV7725_H/8));
    DMA_InitStruct1.sAddr = (uint32_t)&PTC->PDIR + BOARD_OV7725_DATA_OFFSET/8;//DMA??????
    DMA_InitStruct1.sLastAddrAdj = 0;//??MajorLoop??????????????0,???????
    DMA_InitStruct1.sAddrOffset = 0;
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.sMod = kDMA_ModuloDisable;
    DMA_InitStruct1.dAddr = (uint32_t)gpHREF;
    DMA_InitStruct1.dLastAddrAdj = 0; 
    DMA_InitStruct1.dAddrOffset = 1;
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.dMod = kDMA_ModuloDisable;
    /* initialize DMA moudle */
    DMA_Init(&DMA_InitStruct1);
    //DMA_ITConfig (HW_DMA_CH2 ,kDMA_IT_Major ,true); //??DMA??
    //DMA_CallbackInstall (HW_DMA_CH2 ,ov7725_DMA);
    //DMA_DisableRequest (HW_DMA_CH2 );
}
/**
 
* @brief è·å–å°è½¦é€Ÿåº¦å‡½æ•°
* @param  
* @retval 
* @note
*/
void get_speed(void)
{

}

void ov7725_DMA(void )
{
    img_flag = IMG_FINISH ;
    DMA_DisableRequest (HW_DMA_CH2 );
   // DMA_IRQ_CLEAN (HW_DMA_CH2) ;  
}

void camera_get_image()
{
 
    img_flag = IMG_START;
    printf("IMG_START--1\r\n");
    //PORTA->ISFR |= (1<<BOARD_OV7725_VSYNC_PIN);
    NVIC_EnableIRQ(PORTD_IRQn);
   
    
    while(img_flag != IMG_FINISH)
    {
        if(img_flag == IMG_FAIL)
        {
            img_flag = IMG_START;
            printf("IMG_START--fail\r\n");
            //PORTA->ISFR |= (1<<BOARD_OV7725_VSYNC_PIN);
					//GPIO_CallbackInstall(BOARD_OV7725_VSYNC_PORT, OV_VSYNC_ISR);
           //(BOARD_OV7725_HREF_PORT, BOARD_OV7725_HREF_PIN, kGPIO_IT_FallingEdge, true);    
            
        }

    }
    
    
}




