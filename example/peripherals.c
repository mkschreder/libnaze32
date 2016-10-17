#include "naze32.h"
#include "leds.h"
#include "uart.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "../src/usb_vcp.h"

void vTaskMain(void *ptr){
	while(1){
		//uart_put('A'); 
		vTaskDelay(100 / portTICK_RATE_MS); 
		static uint8_t buf[32]; 
		int count = usb_vcp_read((char*)buf, 10); 
		if(count > 0)
			usb_vcp_write((char*)buf, count); 
		//usb_vcp_write("Hello", 6); 
	/*	
		static u8 packet[8] = {1, 2, 3, 4, 5, 6, 7, 8}; 

		while (usbCanTransfer() == 0) portYIELD(); 

		USB_SIL_Write(EP1_IN, packet, 8);
		SetEPTxValid(ENDP1);
*/
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

#define CYCLES_PER_LOOP 3
static inline void wait_cycles( uint32_t n ) {
	uint32_t l = n/CYCLES_PER_LOOP;
	asm volatile( "0:" "SUBS %[count], 1;" "BNE 0b;" :[count]"+r"(l) );
}

void HardFault_Handler(void){
	for(;;){
		naze32_led_on(); 
		wait_cycles(2000000UL); 
		naze32_led_off(); 
		wait_cycles(2000000UL); 
	}
}

int main(int argc, char **argv){
	naze32_init(); 
	
	//uart_put('A'); 

	xTaskCreate( vTaskMain, (const char*)"Main",
		512, NULL, 1, NULL );

	xTaskCreate( vTaskLed, (const char*)"Led",
		512, NULL, 1, NULL );
	
	//uart_put('B'); 
	
	naze32_led_off(); 
	vTaskStartScheduler(); 

	while(1); 
	return 0; 
}

