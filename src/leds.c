#include <stm32/f10x/stm32f10x_gpio.h>
#include <stm32/f10x/stm32f10x_rcc.h>
	
#include "leds.h"

#define LED0_PIN GPIO_Pin_13
#define LED0_GPIO GPIOC

void naze32_led_init(void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
	gpio.GPIO_Pin = LED0_PIN; 
    GPIO_Init(GPIOC, &gpio);

	naze32_led_off(); 
}

void naze32_led_on(void){
	GPIO_ResetBits(LED0_GPIO, LED0_PIN);
}

void naze32_led_off(void){
	GPIO_SetBits(LED0_GPIO, LED0_PIN); 
}
