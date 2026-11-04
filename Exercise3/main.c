#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "pico/util/queue.h"
#include <time.h>

int main(){
   uint pinOpto = 28;
   uint pinA = 2;
   uint pinB = 3;
   uint pinC = 6;
   uint pinD = 13;

   gpio_init(pinOpto);
   gpio_set_dir(pinOpto, GPIO_IN);
   gpio_pull_up(pinOpto);

   gpio_init(pinA);
   gpio_init(pinB);
   gpio_init(pinC);
   gpio_init(pinD);

   gpio_set_dir(pinA, GPIO_OUT);
   gpio_set_dir(pinB, GPIO_OUT);
   gpio_set_dir(pinC, GPIO_OUT);
   gpio_set_dir(pinD, GPIO_OUT);

   char buffer[256];

   while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
     buffer[strcspn(buffer, "\n")] = '\0';
     if (strlen(buffer) == 0) {
         continue;
     }
   }

   return 1;
}
