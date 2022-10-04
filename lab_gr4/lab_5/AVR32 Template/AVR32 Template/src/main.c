#include <asf.h>
#include <board.h>
#include <gpio.h>
#include <sysclk.h>
#include "busy_delay.h"

#define CONFIG_USART_IF (AVR32_USART2)

// defines for BRTT interface
#define TEST_A      AVR32_PIN_PA31
#define RESPONSE_A  AVR32_PIN_PA30
#define TEST_B      AVR32_PIN_PA29
#define RESPONSE_B  AVR32_PIN_PA28
#define TEST_C      AVR32_PIN_PA27
#define RESPONSE_C  AVR32_PIN_PB00


__attribute__((__interrupt__)) static void interrupt_J3(void);

void init(){
    sysclk_init();
    board_init();
    busy_delay_init(BOARD_OSC0_HZ);

	gpio_configure_pin (TEST_A, GPIO_DIR_INPUT);
	gpio_configure_pin (RESPONSE_A, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	
	gpio_configure_pin (TEST_B, GPIO_DIR_INPUT);
	gpio_configure_pin (RESPONSE_B, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	
	gpio_configure_pin (TEST_C, GPIO_DIR_INPUT);
	gpio_configure_pin (RESPONSE_C, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	
	gpio_enable_pin_interrupt(TEST_A, GPIO_PIN_CHANGE);
	gpio_enable_pin_interrupt(TEST_B, GPIO_PIN_CHANGE);
	gpio_enable_pin_interrupt(TEST_C, GPIO_PIN_CHANGE);
    
    cpu_irq_disable();
    INTC_init_interrupts();
    INTC_register_interrupt(&interrupt_J3, AVR32_GPIO_IRQ_3, AVR32_INTC_INT1);
    cpu_irq_enable();
    
    stdio_usb_init(&CONFIG_USART_IF);

    #if defined(__GNUC__) && defined(__AVR32__)
        setbuf(stdout, NULL);
        setbuf(stdin,  NULL);
    #endif
}

void task_a(){
	gpio_toggle_pin(LED0_GPIO);
	while(1){
		//wait until testA = 0
		while(gpio_get_pin_value(TEST_A) != LOW){}
		gpio_toggle_pin(LED0_GPIO);
		//response A = 0
		gpio_set_pin_low(RESPONSE_A);
		//while testA = 0 do nothing // hold the signal low until the BRTT reads it
		while(gpio_get_pin_value(TEST_A) == LOW){}
		//response = 1	
		gpio_set_pin_high(RESPONSE_A);
		gpio_toggle_pin(LED0_GPIO);
	}
}

void task_b(){
	gpio_toggle_pin(LED0_GPIO);
	while(1){
		while(gpio_get_pin_value(TEST_A) != LOW){}
		gpio_set_pin_low(RESPONSE_A);
		while(gpio_get_pin_value(TEST_A) == LOW){}
		gpio_set_pin_high(RESPONSE_A);
		while(gpio_get_pin_value(TEST_B) != LOW){}
		gpio_set_pin_low(RESPONSE_B);
		while(gpio_get_pin_value(TEST_B) == LOW){}
		gpio_set_pin_high(RESPONSE_B);
		while(gpio_get_pin_value(TEST_C) != LOW){}
		gpio_set_pin_low(RESPONSE_C);
		while(gpio_get_pin_value(TEST_C) == LOW){}
		gpio_set_pin_high(RESPONSE_C);
	}
}

__attribute__((__interrupt__)) static void interrupt_J3(void){
	if (gpio_get_pin_interrupt_flag(TEST_A) == 1){
		if (gpio_get_pin_value(TEST_A) == LOW)
			gpio_set_pin_low(RESPONSE_A);
		else
			gpio_set_pin_high(RESPONSE_A);
	}
	if (gpio_get_pin_interrupt_flag(TEST_B) == 1){
		// task_d
		busy_delay_us(100);
		if (gpio_get_pin_value(TEST_B) == LOW)
		gpio_set_pin_low(RESPONSE_B);
		else
		gpio_set_pin_high(RESPONSE_B);
	}
	if (gpio_get_pin_interrupt_flag(TEST_C) == 1){
		if (gpio_get_pin_value(TEST_C) == LOW)
		gpio_set_pin_low(RESPONSE_C);
		else
		gpio_set_pin_high(RESPONSE_C);
	}
}

int main (void){
    init();
	//task_b();
    
    while(1){
        gpio_toggle_pin(LED0_GPIO);

        printf("tick\n");
        
        busy_delay_ms(500);
    }
}
