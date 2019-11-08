#ifndef _MY_USART_HD_H_
#define _MY_USART_HD_H_

#include "includes.h"

/*
**************************************************
*�궨�� USART_PROTOCOLS ��ʹ�õĲ���
**************************************************
*/
#define					RS232_USED					0
#define					RS485_USED					1


/*
**************************************************
*���ܣ�����궨��������������ʹ�õĴ�����RS485����RS232Э��
*������@USART_PROTOCOLS@����Ϊ@RS232_USED@��ʹ�õ���RS232Э��
*      @USART_PROTOCOLS@����Ϊ@RS485_USED@��ʹ�õ���RS485Э��
**************************************************
*/
#define					USART_PROTOCOLS			RS232_USED


/* 
**************************************************
*    ���ʹ�ð�˫����RS485Э��ͨ�ţ�
*�����ﶨ����ǿ���RS485оƬ�շ�ģʽ�����ţ�
*��Ҫ����ʵ��ʹ�õ��������޸� 
**************************************************
*/
#if USART_PROTOCOLS==RS485_USED /* ���ʹ�ð�˫����RS485Э��ͨ�ţ������ﶨ����ǿ���RS485оƬ�շ�ģʽ�����ţ���Ҫ����ʵ��ʹ�õ��������޸� */

#define     		UART_RS485_RX				do{ \
																			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); \
																		}while(0)

#define					UART_RS485_TX				do{ \
																			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET); \
																		}while(0)

#endif /* USART_PROTOCOLS==RS485_USED */


#define					BUFFER_SIZE					500 /* ���ڽ������ݻ������ĳ��� */


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
*                   ��������
**************************************************
*/
/* ���ڷ��ͺ��� */
uint16_t myPrintf(const char*frame,...);
void SendFrame(uint8_t *pData, uint16_t Len);

/* ���ڽ��պ��� */
void MY_UartRxStruct_Init(void);
void IDLE_UART_IRQHandler(UART_HandleTypeDef *huart);
void DealUartData(void);


#endif /* _MY_USART_HD_H_ */

