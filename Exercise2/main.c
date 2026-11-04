#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "pico/util/queue.h"
#include <time.h>
#define TOP 999

static bool ledState = false;
static int channel_level = TOP;
static uint leds[3] = {20,21,22};
static uint slices[3] = {0,0,0};
static uint channels[3] = {0,0,0};
static uint debounce = 0;
static uint rot_a = 10;
static uint rot_b = 11;
static uint rot_sw = 12;

static queue_t events;

int modifyBrightness(uint addon, int value){
   value += addon;
   if(value > TOP){
      value = TOP;
   }
   if(value < 0){
      value = 0;
   }   
   return value;
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

bool debounce_timer_callback(__unused struct repeating_timer *t) {
   debounce++;
   return true;
}

void interrupt(uint gpio, uint32_t event_mask){
   if(gpio == rot_sw){
      if(debounce >= 220){
         int val = 1;
         queue_try_add(&events, &val);
         debounce = 0;
      }
   }
   if(gpio == rot_a){
      int val = 0;
      if(gpio_get(rot_b) == 0){
         val = 3;
      }else if(gpio_get(rot_a) == 1){
         val = 2;
      }
      queue_try_add(&events, &val);
   }
}

void moved(int value){
   if(ledState){
      for(int i = 0; i < 2; i++){
         channel_level = modifyBrightness(value, channel_level);
      }
      for(int i = 0; i < 3; i++){
         pwm_set_chan_level(slices[i], channels[i], channel_level);
      }
   }
}

void toggle(){
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

   gpio_init(rot_a);
   gpio_set_dir(rot_a, GPIO_IN);
   gpio_set_irq_enabled_with_callback(rot_a, GPIO_IRQ_EDGE_RISE, true, &interrupt);

   gpio_init(rot_b);
   gpio_set_dir(rot_b, GPIO_IN);

   gpio_init(rot_sw);
   gpio_set_dir(rot_sw, GPIO_IN);
   gpio_pull_up(rot_sw);
   gpio_set_irq_enabled_with_callback(rot_sw, GPIO_IRQ_EDGE_FALL, true, &interrupt);

   struct repeating_timer timer;
   add_repeating_timer_ms(1, debounce_timer_callback, NULL, &timer);

   for(int i = 0; i < 3; i++){
      init_led(leds[i]);
      slices[i] = pwm_gpio_to_slice_num(leds[i]);
      channels[i] = pwm_gpio_to_channel(leds[i]);
      init_pwm(slices[i], channels[i]);
   }

   stdio_init_all();
   
   queue_init(&events, sizeof(int), 8);
   int value = 0;
   while(true){
      
      while(queue_try_remove(&events, &value) == true){
         switch(value){
            case 1:
               toggle();
               break;
            case 2:
               moved(-10);
               break;
            case 3:
               moved(10);
               break;
         }
      }
   }
   queue_free(&events);
   return 0;
}

