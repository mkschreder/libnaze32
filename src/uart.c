/*
 * Copyright (c) 2016 Martin Schröder <mkschreder.uk@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <string.h>
#include <errno.h>
#include <utype/cbuf.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <stm32/f10x/stm32f10x_usart.h>

#include "uart.h"

#ifndef UART1_GPIO
#define UART1_TX_PIN                GPIO_Pin_9  // PA9
#define UART1_RX_PIN                GPIO_Pin_10 // PA10
#define UART1_GPIO                  GPIOA
#define UART1_APB2_PERIPHERALS      RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA
#define UART1_AHB_PERIPHERALS       RCC_AHBPeriph_DMA1
#endif

#ifndef UART2_GPIO
#define UART2_TX_PIN                Pin_2 // PA2
#define UART2_RX_PIN                Pin_3 // PA3
#define UART2_GPIO                  GPIOA
#define UART2_APB1_PERIPHERALS      RCC_APB1Periph_USART2
#define UART2_APB2_PERIPHERALS      RCC_APB2Periph_GPIOA
#define UART2_AHB_PERIPHERALS       RCC_AHBPeriph_DMA1
#endif

#ifndef UART3_GPIO
#define UART3_TX_PIN                Pin_10 // PB10
#define UART3_RX_PIN                Pin_11 // PB11
#define UART3_GPIO                  GPIOB
#define UART3_APB1_PERIPHERALS      RCC_APB1Periph_USART3
#define UART3_APB2_PERIPHERALS      RCC_APB2Periph_GPIOB
#endif

// pins for UART4 are fixed by hardware
#define UART4_TX_PIN                Pin_10 // PC10
#define UART4_RX_PIN                Pin_11 // PC11
#define UART4_GPIO                  GPIOC
#define UART4_APB1_PERIPHERALS      RCC_APB1Periph_UART4
#define UART4_APB2_PERIPHERALS      RCC_APB2Periph_GPIOC

// pins for UART5 are fixed by hardware and on GPIOC and D
#define UART5_TX_PIN                Pin_12 // PC12
#define UART5_RX_PIN                Pin_2  // PD2
#define UART5_GPIO_TX               GPIOC
#define UART5_GPIO_RX               GPIOD
#define UART5_APB1_PERIPHERALS      RCC_APB1Periph_UART5
#define UART5_APB2_PERIPHERALS_TX   RCC_APB2Periph_GPIOC
#define UART5_APB2_PERIPHERALS_RX   RCC_APB2Periph_GPIOD

struct stm32_uart {
	struct cbuf rx_buf; 
	char rx_data[128]; 

	xSemaphoreHandle rx_sem;
}; 

static struct stm32_uart uart1; 

// USART3 - MAVLINK (RX - IRQ, TX - IRQ)
void uart_init(uint32_t baudRate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	memset(uart1.rx_data, 0, sizeof(uart1.rx_data)); 
	cbuf_init(&uart1.rx_buf, uart1.rx_data, sizeof(uart1.rx_data)); 

	vSemaphoreCreateBinary(uart1.rx_sem);

    RCC_APB2PeriphClockCmd(UART1_APB2_PERIPHERALS, ENABLE);
    RCC_AHBPeriphClockCmd(UART1_AHB_PERIPHERALS, ENABLE);

    // USART1_TX    PB10
    // USART1_RX    PB11
	GPIO_InitStructure.GPIO_Pin = UART1_RX_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
    GPIO_Init(UART1_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = UART1_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(UART1_GPIO, &GPIO_InitStructure);
	
	//reset USART
	USART_DeInit(USART1);
	
    // RX interrupt, doesn't need high performance
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	USART_InitStructure.USART_BaudRate = baudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

// USART1 Rx IRQ Handler
void USART1_IRQHandler(void){
	static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		cbuf_put(&uart1.rx_buf, USART_ReceiveData(USART1));

		xSemaphoreGiveFromISR(uart1.rx_sem, &xHigherPriorityTaskWoken);
    }
	
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

int uart_read(char *data, size_t size){
	// wait for at least one data byte to be available
	xSemaphoreTake(uart1.rx_sem, portMAX_DELAY);

	if(cbuf_empty(&uart1.rx_buf)) return -EAGAIN; 

	size_t c = 0; 
	for(c = 0; c < size; c++){
		int16_t d = cbuf_get(&uart1.rx_buf); 
		if((d & 0xff00) != 0) break; 

		*data = d & 0xff; 
		data++; 
	}
	return c; 
}

void uart_put(u8 ch){
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
	USART_SendData(USART1, (u8) ch);
}

