#include "myUsarthd.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/*
**************************************************
*                  ����ȫ�ֱ���
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
*��������@MY_UartRxStruct_Init
*���ܣ�	 �����ڵĲ�����ʼ��
*ע�ͣ�  Ĭ��ʹ�õ��Ǵ���һ����ʹ���������������и���
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
*��������@DealUartData
*���ܣ�	 �Խ��յ����ݽ��д���
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
		
		/* ���ݴ���start */
		SendFrame((uint8_t *)gsUartRx.processingBufPtr, tmpLen);
		/* ���ݴ���end */
	}
}


/*
**************************************************
*��������@IDLE_UART_IRQHandler
*���ܣ�	 ���ڿ����жϷ�����
*ע�ͣ�  ����ʹ�ô��ڿ����ж���Ҫ��stm32f4xx_it.c�ļ����µ�@USART1_IRQHandler@��������;
*        Ĭ��ʹ�õ��Ǵ���һ����ʹ���������������и���;
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
		/* ��⴮�ڿ����ж�---------------------------------------------------------*/
		if((tmp_flag != RESET) && (tmp_it_source != RESET))
		{
			gsUartRx.uRxDataCnt = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
			gsUartRx.bRxFinish = TRUE;
			
			//���IDLE�жϱ�־λ
			tmpData = USART1->SR;
			tmpData = USART1->DR;
			tmpData = tmpData;
		
			HAL_UART_DMAStop(&huart1);//�رմ���DMA
		}	
	}
}


/*
**************************************************
*��������@myPrintf
*���ܣ�	 �ַ����ĸ�ʽ�����
*������	 ������ʽ�ͱ�׼���printf��ʽ��ͬ
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
*��������@SendFrame
*���ܣ�  ֱ�ӷ���ָ�����ȵ��ַ���
*������  uint8_t *pData	@Ҫ���͵�����(ÿ����Ԫ��һ���ֽ�)
*        uint16_t Len		@Ҫ���͵����ݳ���(��λ���ֽ�)
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



