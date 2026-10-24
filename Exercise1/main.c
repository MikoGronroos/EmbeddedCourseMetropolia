#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

void modifyBrightness(uint pin, int value){
   if(gpio_get(pin) == 0){
      while(gpio_get(pin) == 0){
      
      }
   }
}

void init_btn(const uint pin){
   gpio_init(pin);
   gpio_set_dir(pin, GPIO_IN);
   gpio_pull_up(pin);
}

void init_led(const uint pin){
   gpio_init(pin);
   gpio_set_dir(pin, GPIO_OUT);
}

int main(){
   
   const uint led1 = 20;
   const uint led2 = 21;
   const uint led3 = 22;

   const uint lowerBrightnessBtn = 7;
   const uint toggleBtn = 8;
   const uint higherBrightnessBtn = 9;

   init_btn(toggleBtn);

   init_btn(lowerBrightnessBtn);

   init_btn(higherBrightnessBtn);

   init_led(led1);

   init_led(led2);
   
   init_led(led3);

   stdio_init_all();

   gpio_put(led1, false);
   gpio_put(led2, false);
   gpio_put(led3, false);


   bool ledState = false;
   bool pressed = false;

   while(true){
      if(gpio_get(toggleBtn) == 0 && pressed == false){ 
         pressed = true;
         ledState = !ledState; 
         gpio_put(led1, ledState);
         gpio_put(led2, ledState);
         gpio_put(led3, ledState);
      }else if(gpio_get(toggleBtn) == 1 && pressed == true){
         pressed = false;
      }
      
      if(ledState){
         
      }
      sleep_ms(10);
   }
   return 0;
}
