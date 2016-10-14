#include <stm32/f10x/stm32f10x_rcc.h>
#include <stm32/f10x/misc.h>
#include "naze32.h"
#include "leds.h"
#include "uart.h"

void naze32_init(void){
	//SystemCoreClockUpdate();

    // turn on peripherals needed by all
	/* RCC system reset(for debug purpose). */
	RCC_DeInit ();                        

    /* Enable HSE. */
	RCC_HSEConfig( RCC_HSE_ON );           
	
	/* Wait till HSE is ready. */
	while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);
	
    /* HCLK = SYSCLK. */
	RCC_HCLKConfig( RCC_SYSCLK_Div1 );   

    /* PCLK2  = HCLK. */
	RCC_PCLK2Config( RCC_HCLK_Div1 );     

    /* PCLK1  = HCLK/2. */
	RCC_PCLK1Config( RCC_HCLK_Div2 );     

	/* ADCCLK = PCLK2/4. */
	RCC_ADCCLKConfig( RCC_PCLK2_Div4 );    
	
    /* Flash 2 wait state. */
	*( volatile unsigned long  * )0x40022000 = 0x01;           
	
	/* PLLCLK = 8MHz * 9 = 72 MHz */
	RCC_PLLConfig( RCC_PLLSource_HSE_Div1, RCC_PLLMul_9 );
	
    /* Enable PLL. */
	RCC_PLLCmd( ENABLE );
	
	/* Wait till PLL is ready. */
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
	
	/* Select PLL as system clock source. */
	RCC_SYSCLKConfig (RCC_SYSCLKSource_PLLCLK);
	
	/* Wait till PLL is used as system clock source. */
	while (RCC_GetSYSCLKSource() != 0x08);

	/* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE and AFIO clocks */
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC
							| RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE );

	/* Set the Vector Table base address at 0x08000000. */
	NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x0 );

	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

	/* Configure HCLK clock as SysTick clock source. */
	SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );
	SysTick_Config(SystemCoreClock / 1000); 

	naze32_led_init(); 

	uart_init(115200); 
}

