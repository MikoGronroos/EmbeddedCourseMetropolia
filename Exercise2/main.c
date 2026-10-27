#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "pico/util/queue.h"
#include <time.h>
#define TOP 999

static bool ledState = false;
static int channel_level = TOP;
static bool pressed = false;

//static uint channel_addon[2] = {4, -4};
static uint leds[3] = {20,21,22};
static uint slices[3] = {0,0,0};
static uint channels[3] = {0,0,0};

static queue_t events;

int modifyBrightness(uint pin, uint addon, int value){
   if(gpio_get(pin) == 0){
      value += addon;
      if(value > TOP){
         value = TOP;
      }
      if(value < 0){
         value = 0;
      }    
   }
   return value;
}
bool debounce_timer_callback(__unused struct repeating_timer *t) {
   pressed = false;
   return true;
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
   pwm_config_set_wrap(&config, TOP);
   pwm_init(slice_num, &config, false);
   pwm_set_chan_level(slice_num, channel, 0);
   pwm_set_enabled(slice_num, true);
}

void toggle_irq(uint gpio, uint32_t event_mask){
   if(pressed == false){
      int val = 1;
      queue_try_add(&events, &val);
   }
}

void toggle(){
   printf("xd\n");
   ledState = !ledState;
   if(ledState == false && channel_level == 0){
      ledState = true;
      channel_level = TOP / 2;
   }
   for(int i = 0; i < 3; i++){
      pwm_set_chan_level(slices[i], channels[i], ledState ? channel_level : 0);
   }
}

int main(){

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
   gpio_set_irq_enabled_with_callback(12, GPIO_IRQ_EDGE_FALL, true, toggle_irq);

   struct repeating_timer timer;
   add_repeating_timer_ms(220, debounce_timer_callback, NULL, &timer);

   for(int i = 0; i < 3; i++){
      init_led(leds[i]);
      slices[i] = pwm_gpio_to_slice_num(leds[i]);
      channels[i] = pwm_gpio_to_channel(leds[i]);
      init_pwm(slices[i], channels[i]);
   }

   stdio_init_all();
   
   queue_init(&events, sizeof(int), 10);
   int value = 0;
   while(true){
      
      while(queue_try_remove(&events, &value)){
         switch(value){
            case 1:
               toggle();
               break;
         }
      }
     /* 
      if(ledState){
         for(int i = 0; i < 2; i++){
            channel_level = modifyBrightness(btn[i], channel_addon[i], channel_level);
         }
         for(int i = 0; i < 3; i++){
            pwm_set_chan_level(slices[i], channels[i], channel_level);
         }
      }
*/
   }
   return 0;
}

