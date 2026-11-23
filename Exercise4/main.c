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

int uart_read_line_timeout(uart_inst_t *uart, char *buffer, size_t max_len, uint32_t timeout) {
   size_t idx = 0;
   absolute_time_t deadline = make_timeout_time_us(timeout);

   while (!time_reached(deadline)) {
      if (uart_is_readable(uart)) {
         char c = uart_getc(uart);
         if (idx < max_len - 1) {
            buffer[idx++] = c;
         }
         if (c == '\n') {
            buffer[idx] = '\0';
            return (int)idx;
         }
      }
   }
   buffer[idx] = '\0';
   return -1;
}

int main(){
   stdio_init_all();

   uart_init(UART_ID, BAUD_RATE);
   gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
   gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

   gpio_init(SW_0_PIN);
   gpio_set_dir(SW_0_PIN, GPIO_IN);
   gpio_pull_up(SW_0_PIN);   

   appstate state = STATE_WAIT;
   char buffer[BUFFER_SIZE];

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
            printf("xd 1\n");
            bool connected = false;

            for (int attempt = 0; attempt < 5; attempt++) {
               printf("attempt %d\n", attempt);

               uart_flush(UART_ID);
               printf("amen 1\n");
               uart_puts(UART_ID, "AT\r\n");
               printf("amen 2\n");
               int len = uart_read_line_timeout(UART_ID, buffer, BUFFER_SIZE, TIMEOUT);
               printf("amen 3\n");

               if (len > 0 && strstr(buffer, "+AT: OK") != NULL) {
                  connected = true;
                  break;
               }
               printf("amen 4\n");
            }
             
            if (connected) {
               printf("Connected to LoRa module\n");
               state = STATE_READ_VERSION;
            } else {
               printf("module not responding\n");
               state = STATE_WAIT;
            }
            break;
         case STATE_READ_VERSION:
            uart_flush(UART_ID);
            uart_puts(UART_ID, "AT+VER\r\n");
            int len = uart_read_line_timeout(UART_ID, buffer, BUFFER_SIZE, TIMEOUT);
            if (len > 0 && strstr(buffer, "+VER:") != NULL) {
               buffer[strcspn(buffer, "\r\n")] = '\0';
               printf("Firmware version: %s\n", buffer);
               state = STATE_READ_DEVEUI;
            } else {
               printf("Module stopped responding\n");
               state = STATE_WAIT_BUTTON;
            }
            break;
         case STATE_READ_DEVUI:
            printf("Hei\n");
            break;
      }
   }   

}
