#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <time.h>
#include <string.h>    
#include "hardware/uart.h"
#include <stdlib.h>
#include <math.h>

#define UART_ID uart1
#define BAUD_RATE 9600
#define UART_TX_PIN 4
#define UART_RX_PIN 5
#define SW_0_PIN 9

#define TIMEOUT 500000
#define BUFFER_SIZE 128

typedef enum {
   STATE_WAIT = 1,
   STATE_CHECK_CONNECTION = 2,
   STATE_READ_VERSION = 3,
   STATE_READ_DEVUI = 4
} appstate;

void uart_flush(uart_inst_t *uart){
   while(uart_is_readable(uart)){
      (void)uart_getc(uart);
   }
}

int main(){
   stdio_init_all();

   gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
   gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

   gpio_init(SW_0_PIN);
   gpio_set_dir(SW_0_PIN, GPIO_IN);
   gpio_pull_up(SW_0_PIN);   

   appstate state = STATE_WAIT;
   char rx_buffer[BUFFER_SIZE];

   while(true){
      switch(state){
         case STATE_WAIT:
            if(!gpio_get(SW_0_PIN)){
               sleep_ms(22);
               if(!gpio_get(SW_0_PIN)){
                  while(!gpio_get(SW_0_PIN)){
                     
                  }
                  state = STATE_CHECK_CONNECTION;
               }
            }
            break;
         case STATE_CHECK_CONNECTION:
            printf("Hei\n");
            break;
         case STATE_READ_VERSION:
            break;
         case STATE_READ_DEVUI:
            break;
      }
   }   

}
