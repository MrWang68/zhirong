
#ifndef __FUNCTIONS_H
#define __FUNCTIONS_H

#include "gpio.h"
#include "dma.h"
#include "i2c.h"
#include "ov7725.h"
#include "common.h"
#include "uart.h"
#include "ftm.h"


#define IMAGE_SIZE 0

#if (IMAGE_SIZE  ==  0)
#define OV7725_W    (80)
#define OV7725_H    (60)
#define xSize 80
#define ySize 60
#define IMAGE_W_H H_80_W_60

#elif (IMAGE_SIZE == 1)
#define OV7725_W    (160)
#define OV7725_H    (120)
#define xSize 160
#define ySize 120
#define IMAGE_W_H H_120_W_160


#else
#error "Image Size Not Support!"
#endif

/* 引脚定义*///
#define BOARD_OV7725_PCLK_PORT      HW_GPIOD
#define BOARD_OV7725_PCLK_PIN       (11)
#define BOARD_OV7725_VSYNC_PORT     HW_GPIOD
#define BOARD_OV7725_VSYNC_PIN      (10)
#define BOARD_OV7725_HREF_PORT      HW_GPIOD
#define BOARD_OV7725_HREF_PIN       (21)
#define BOARD_OV7725_DATA_OFFSET    (8) /* 摄像头数据引脚PTA8-PTA15 只能为 0 8 16 24 */

//extern uint8_t * gpHREF[OV7725_H+1];
//extern uint8_t CameraData[OV7725_H][OV7725_W]; 
/* 状态机定义 */
typedef enum
{
    TRANSFER_IN_PROCESS, //数据正在处理
    NEXT_FRAME,          //下一帧数据
}OV7725_Status;

//????????
typedef enum
{
    IMG_NOTINIT = 0,
    IMG_FINISH,             //??????
    IMG_FAIL,               //??????(??????)
    IMG_GATHER,             //?????
    IMG_START,              //??????
    IMG_STOP,
    IMG_NOT_FINISH    //??????
} IMG_STATUS_e;

static uint8_t debug_light_staues = 0;
void SerialDispImage(uint8_t* srcData);
int SCCB_Init(uint32_t I2C_MAP);
void init_All(void);
void OV_HREF_ISR(uint32_t index);
void OV_VSYNC_ISR(uint32_t index);
void motor_steer_init(void);
void camera_init(void);
void uart_init(void);
void switch_init(void);
void DMA_init(void);
void UART_RX_ISR(uint16_t byteReceived);
void get_speed(void);
void camera_get_image(void);
void ov7725_DMA(void );
void ov7725_port_init(void);
#endif


