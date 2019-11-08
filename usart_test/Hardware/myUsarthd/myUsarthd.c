#include "myUsarthd.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/*
**************************************************
*                  定义全局变量
**************************************************
*/
volatile uint8_t gArrUartRxBuffer[BUFFER_SIZE];

#if USART_RX_METHOD == DMA_DOUBLE_BUF
volatile uint8_t gArrUartRxBufferTmp[BUFFER_SIZE];
#endif /* USART_RX_METHOD == DMA_DOUBLE_BUF */

volatile UartRxStruct gsUartRx;
volatile uint8_t UartSendFinish;


/*
**************************************************
*函数名：@MY_UartRxStruct_Init
*功能：	 将串口的参数初始化
*注释：  默认使用的是串口一，如使用其他串口需自行更改
**************************************************
*/
void MY_UartRxStruct_Init(void)
{
	__HAL_UART_CLEAR_PEFLAG(&huart1);
	
	gsUartRx.uRxDataCnt = 0;
	gsUartRx.usingBufferPtr = (uint8_t *)gArrUartRxBuffer;
	gsUartRx.processingBufPtr = (uint8_t *)gArrUartRxBuffer;
	gsUartRx.bRxFinish = FALSE;
	memset((uint8_t *)gArrUartRxBuffer, 0, BUFFER_SIZE);
	memset((uint8_t *)gArrUartRxBufferTmp, 0, BUFFER_SIZE);

#if USART_PROTOCOLS==RS485_USED
	UART_RS485_RX;
#endif /* USART_PROTOCOLS==RS485_USED */
	
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart1, (uint8_t *)gsUartRx.usingBufferPtr, BUFFER_SIZE);
}


/*
**************************************************
*函数名：@DealUartData
*功能：	 对接收的数据进行处理
**************************************************
*/
void DealUartData(void)
{
	uint16_t tmpLen;
	
	if(gsUartRx.bRxFinish == TRUE)
	{
		gsUartRx.bRxFinish = FALSE;
		tmpLen = gsUartRx.uRxDataCnt;
		if(gsUartRx.usingBufferPtr == gArrUartRxBuffer)
		{
			gsUartRx.usingBufferPtr = (uint8_t *)gArrUartRxBufferTmp;
			gsUartRx.processingBufPtr = (uint8_t *)gArrUartRxBuffer;
		}else{
			gsUartRx.usingBufferPtr = (uint8_t *)gArrUartRxBuffer;
			gsUartRx.processingBufPtr = (uint8_t *)gArrUartRxBufferTmp;
		}
		HAL_UART_Receive_DMA(&huart1, (uint8_t *)gsUartRx.usingBufferPtr, BUFFER_SIZE);
		
		/* 数据处理start */
		SendFrame((uint8_t *)gsUartRx.processingBufPtr, tmpLen);
		/* 数据处理end */
	}
}


/*
**************************************************
*函数名：@IDLE_UART_IRQHandler
*功能：	 串口空闲中断服务函数
*注释：  如需使用串口空闲中断需要在stm32f4xx_it.c文件夹下的@USART1_IRQHandler@函数调用;
*        默认使用的是串口一，如使用其他串口需自行更改;
**************************************************
*/
void IDLE_UART_IRQHandler(UART_HandleTypeDef *huart)
{
	uint32_t tmp_flag = 0, tmp_it_source = 0;
	uint8_t tmpData;
	if(huart->Instance == USART1)
	{
		tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE);
		tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_IDLE);
		/* 检测串口空闲中断---------------------------------------------------------*/
		if((tmp_flag != RESET) && (tmp_it_source != RESET))
		{
			gsUartRx.uRxDataCnt = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
			gsUartRx.bRxFinish = TRUE;
			
			//清除IDLE中断标志位
			tmpData = USART1->SR;
			tmpData = USART1->DR;
			tmpData = tmpData;
		
			HAL_UART_DMAStop(&huart1);//关闭串口DMA
		}	
	}
}


/*
**************************************************
*函数名：@myPrintf
*功能：	 字符串的格式化输出
*参数：	 参数格式和标准库的printf格式相同
**************************************************
*/
uint16_t myPrintf(const char*frame,...)
{
	uint16_t lenFrame;
	uint8_t tmpFrame[100];
	
	va_list arg_ap;
	va_start(arg_ap, frame);
	
	lenFrame = vsprintf((char*)tmpFrame, frame, arg_ap);

#if USART_PROTOCOLS==RS485_USED
	UART_RS485_TX;
#endif /* USART_PROTOCOLS==RS485_USED */
	
	HAL_UART_Transmit(&huart1, tmpFrame, lenFrame, 1000);
	
#if USART_PROTOCOLS==RS485_USED
	UART_RS485_RX;
#endif /* USART_PROTOCOLS==RS485_USED */
	
	return lenFrame;
}


/*
**************************************************
*函数名：@SendFrame
*功能：  直接发送指定长度的字符串
*参数：  uint8_t *pData	@要发送的数据(每个单元是一个字节)
*        uint16_t Len		@要发送的数据长度(单位：字节)
**************************************************
*/
void SendFrame(uint8_t *pData, uint16_t Len)
{
#if USART_PROTOCOLS==RS485_USED
	UART_RS485_TX;
#endif /* USART_PROTOCOLS==RS485_USED */
	
	HAL_UART_Transmit(&huart1, pData, Len, 1000);
	
#if USART_PROTOCOLS==RS485_USED
	UART_RS485_RX;
#endif /* USART_PROTOCOLS==RS485_USED */
}



