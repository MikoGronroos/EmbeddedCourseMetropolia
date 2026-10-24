#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"


int modifyBrightness(uint pin, uint addon, int value, uint slice_num, uint channel){
   if(gpio_get(pin) == 0){
      while(gpio_get(pin) == 0){
         value += addon;
         if(value > 999){
            value = 999;
         }
         if(value < 0){
            value = 0;
         }
         printf("Pressing %d %d\n", value, addon);
         pwm_set_chan_level(slice_num, channel, value);
      }
      sleep_ms(10);
   }
   return value;
}

void init_btn(const uint pin){
   gpio_init(pin);
   gpio_set_dir(pin, GPIO_IN);
   gpio_pull_up(pin);
}

void init_led(const uint pin){
   gpio_init(pin);
   gpio_set_dir(pin, GPIO_OUT);
   gpio_set_function(pin, GPIO_FUNC_PWM);
}

int main(){
   
   const uint led1 = 20;
   const uint led2 = 21;
   const uint led3 = 22;

   const uint toggleBtn = 8;

   int channel_level = 999;
   uint channel_addon[2] = {5, -5};
   uint btn[2] = {9, 7};
   
   init_btn(toggleBtn);

   init_btn(btn[1]);

   init_btn(btn[0]);

   init_led(led1);

   init_led(led2);
   
   init_led(led3);

   uint slice_num = pwm_gpio_to_slice_num(led2);
   uint channel = pwm_gpio_to_channel(led2);

   pwm_set_enabled(slice_num, false);
   pwm_config config = pwm_get_default_config();
   pwm_config_set_clkdiv_int(&config, 125);
   pwm_config_set_wrap(&config, 999);
   pwm_init(slice_num, &config, true);
   pwm_set_chan_level(slice_num, channel, 0);
   pwm_set_enabled(slice_num, true);
   stdio_init_all();

   //gpio_put(led1, false);
   //gpio_put(led2, false);
   //gpio_put(led3, false);

   bool ledState = false;
   bool pressed = false;

   while(true){
      if(gpio_get(toggleBtn) == 0 && pressed == false){ 
         pressed = true;
         ledState = !ledState; 
         //gpio_put(led1, ledState);
         //gpio_put(led2, ledState);
         for(int i = 0; i < 2; i++){
            pwm_set_chan_level(slice_num, channel, ledState ? channel_level : 0);
         }
         //gpio_put(led3, ledState);
      }else if(gpio_get(toggleBtn) == 1 && pressed == true){
         pressed = false;
      }
      
      if(ledState){
         for(int i = 0; i < 2; i++){
            channel_level = modifyBrightness(btn[i], channel_addon[i], channel_level, slice_num, channel);
         }
      }
   }
   return 0;
}
