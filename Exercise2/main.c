#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "pico/util/queue.h"
#include <time.h>

static uint xd = 0;
static double interrupt_time = 0;
static double last_interrupt_time = 0;

void interrupt(uint gpio, uint32_t event_mask){
   interrupt_time = clock() / CLOCKS_PER_SEC;
   if (interrupt_time - last_interrupt_time > 0.25) 
   {
      xd++;
   }
   last_interrupt_time = interrupt_time;
}


int main(){
   stdio_init_all();

   uint rot_a = 10;
   uint rot_b = 11;
   uint rot_sw = 12;

   gpio_init(rot_a);
   gpio_set_dir(rot_a, GPIO_IN);

   gpio_init(rot_b);
   gpio_set_dir(rot_b, GPIO_IN);

   gpio_init(rot_sw);
   gpio_set_dir(rot_sw, GPIO_IN);
   gpio_pull_up(rot_sw);
   gpio_set_irq_enabled_with_callback(12, GPIO_IRQ_EDGE_FALL, true, interrupt);
   
   
   while(true){
      printf("xd %d\n", xd);
   }
   return 0;
}

