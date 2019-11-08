#ifndef _MY_USART_HD_H_
#define _MY_USART_HD_H_

#include "includes.h"

/*
**************************************************
*宏定义 USART_PROTOCOLS 所使用的参数
**************************************************
*/
#define					RS232_USED					0
#define					RS485_USED					1


/*
**************************************************
*功能：这个宏定义用来定义这里使用的串口是RS485还是RS232协议
*参数：@USART_PROTOCOLS@定义为@RS232_USED@：使用的是RS232协议
*      @USART_PROTOCOLS@定义为@RS485_USED@：使用的是RS485协议
**************************************************
*/
#define					USART_PROTOCOLS			RS232_USED


/* 
**************************************************
*    如果使用半双工的RS485协议通信，
*则这里定义的是控制RS485芯片收发模式的引脚，
*需要根据实际使用的引脚来修改 
**************************************************
*/
#if USART_PROTOCOLS==RS485_USED /* 如果使用半双工的RS485协议通信，则这里定义的是控制RS485芯片收发模式的引脚，需要根据实际使用的引脚来修改 */

#define     		UART_RS485_RX				do{ \
																			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); \
																		}while(0)

#define					UART_RS485_TX				do{ \
																			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET); \
																		}while(0)

#endif /* USART_PROTOCOLS==RS485_USED */


#define					BUFFER_SIZE					500 /* 串口接收数据缓存区的长度 */


extern volatile uint8_t gArrUartRxBuffer[BUFFER_SIZE];
extern volatile uint8_t gArrUartRxBufferTmp[BUFFER_SIZE];


typedef struct{
	uint8_t bRxFinish;
	uint16_t uRxDataCnt;
	uint8_t *usingBufferPtr;
	uint8_t *processingBufPtr;
}UartRxStruct;
extern volatile UartRxStruct gsUartRx;

extern volatile uint8_t UartSendFinish;



/* 
**************************************************
*                   函数声明
**************************************************
*/
/* 串口发送函数 */
uint16_t myPrintf(const char*frame,...);
void SendFrame(uint8_t *pData, uint16_t Len);

/* 串口接收函数 */
void MY_UartRxStruct_Init(void);
void IDLE_UART_IRQHandler(UART_HandleTypeDef *huart);
void DealUartData(void);


#endif /* _MY_USART_HD_H_ */

