#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"


int modifyBrightness(uint pin, uint addon, int value){
   if(gpio_get(pin) == 0){
      value += addon;
      if(value > 999){
         value = 999;
      }
      if(value < 0){
         value = 0;
      }    
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

void init_pwm(const uint slice_num, const uint channel){
   pwm_set_enabled(slice_num, false);
   pwm_config config = pwm_get_default_config();
   pwm_config_set_clkdiv_int(&config, 125);
   pwm_config_set_wrap(&config, 999);
   pwm_init(slice_num, &config, false);
   pwm_set_chan_level(slice_num, channel, 0);
   pwm_set_enabled(slice_num, true);
}

int main(){
   
   const uint toggleBtn = 8;
   int channel_level = 999;
   uint channel_addon[2] = {4, -4};
   uint btn[2] = {9, 7};
   uint leds[3] = {20,21,22};
   uint slices[3] = {0,0,0};
   uint channels[3] = {0,0,0};
   
   init_btn(toggleBtn);

   for(int i = 0; i < 2; i++){
      init_btn(btn[i]);
   }

   for(int i = 0; i < 3; i++){
      init_led(leds[i]);
      slices[i] = pwm_gpio_to_slice_num(leds[i]);
      channels[i] = pwm_gpio_to_channel(leds[i]);
      init_pwm(slices[i], channels[i]);
   }

   stdio_init_all();

   bool ledState = false;
   bool pressed = false;

   while(true){
      if(gpio_get(toggleBtn) == 0 && pressed == false){ 
         pressed = true;
         ledState = !ledState;
         if(ledState == false && channel_level == 0){
            ledState = true;
            channel_level = 499;
         }
         for(int i = 0; i < 3; i++){
            pwm_set_chan_level(slices[i], channels[i], ledState ? channel_level : 0);
         }
      }else if(gpio_get(toggleBtn) == 1 && pressed == true){
         pressed = false;
      }
      
      if(ledState){
         for(int i = 0; i < 2; i++){
            channel_level = modifyBrightness(btn[i], channel_addon[i], channel_level);
         }
         for(int i = 0; i < 3; i++){
            pwm_set_chan_level(slices[i], channels[i], channel_level);
         }
      }
      sleep_ms(10);
   }
   return 0;
}
