#include "naze32.h"
#include "leds.h"
#include "uart.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void vTaskMain(void *ptr){
	while(1){
		uart_put('A'); 
		vTaskDelay(500 / portTICK_RATE_MS); 
	}
}

void vTaskLed(void *ptr){
	while(1){
		naze32_led_on(); 
		vTaskDelay(500 / portTICK_RATE_MS); 
		naze32_led_off(); 
		vTaskDelay(500 / portTICK_RATE_MS); 
	}
}

void HardFault_Handler(void){
	naze32_led_on(); 
	for(;;); 
}

int main(int argc, char **argv){
	naze32_init(); 
	
	uart_put('A'); 

	xTaskCreate( vTaskMain, (const char*)"Main",
		512, NULL, 3, NULL );

	xTaskCreate( vTaskLed, (const char*)"Led",
		512, NULL, 1, NULL );
	
	uart_put('B'); 
	
	naze32_led_off(); 
	vTaskStartScheduler(); 

	while(1); 
	return 0; 
}

