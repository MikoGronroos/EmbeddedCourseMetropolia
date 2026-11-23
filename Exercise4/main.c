#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <time.h>
#include <string.h>    
#include "args.h"
#include <stdlib.h>
#include <math.h>

#define UART_ID uart1
#define BAUD_RATE 9600
#define UART_TX_PIN 4
#define UART_RX_PIN 5
#define SW_0_PIN 9

#define TIMEOUT 500000
#define BUFFER_SIZE 128

