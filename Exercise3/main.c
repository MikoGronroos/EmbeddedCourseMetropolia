#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <time.h>
#include <string.h>

#define MAX_STEPS 4096     
   
uint pinOpto = 28;
uint pinA = 2;
uint pinB = 3;
uint pinC = 6;
uint pinD = 13;
uint numberOfSteps = 0;
bool calibrated = false;

int turnRight(){
   gpio_put(pinA, true);
   gpio_put(pinB, false);
   gpio_put(pinC, false);
   gpio_put(pinD, false);
   sleep_ms(1);
   gpio_put(pinA, true);
   gpio_put(pinB, true);
   gpio_put(pinC, false);
   gpio_put(pinD, false);     
   sleep_ms(1);
   gpio_put(pinA, false);
   gpio_put(pinB, true);
   gpio_put(pinC, false);
   gpio_put(pinD, false);
   sleep_ms(1); 
   gpio_put(pinA, false);
   gpio_put(pinB, true);
   gpio_put(pinC, true);
   gpio_put(pinD, false);
   sleep_ms(1);
   gpio_put(pinA, false);
   gpio_put(pinB, false);
   gpio_put(pinC, true);
   gpio_put(pinD, false);
   sleep_ms(1);
   gpio_put(pinA, false);
   gpio_put(pinB, false);
   gpio_put(pinC, true);
   gpio_put(pinD, true);
   sleep_ms(1);
   gpio_put(pinA, false);
   gpio_put(pinB, false);
   gpio_put(pinC, false);
   gpio_put(pinD, true);
   sleep_ms(1);
   gpio_put(pinA, true);
   gpio_put(pinB, false);
   gpio_put(pinC, false);
   gpio_put(pinD, true);
   sleep_ms(1);
   return 1; 
}

int calibrate(){
   while(gpio_get(pinOpto) == true){
      turnRight();
   }
   numberOfSteps = 0;
   return 1;
}

int run(int n){
   int cycle = n * 64;
   for(int i = 0; i < cycle; i++){
      turnRight();
   }
   numberOfSteps += cycle * 8;
   if(numberOfSteps > MAX_STEPS){
      numberOfSteps -= MAX_STEPS;
   }
   return 1;
}

int main(){
   stdio_init_all();

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
    
   char buffer[64];
   int pos = 0;   
   while(true){
      int c = getchar();
      if (c != EOF) {
        if (c == '\r' || c == '\n') {
            buffer[pos] = '\0';
            printf("received %s\n", buffer);
            pos = 0;
            if(strcmp(buffer, "status") == 0){
               printf("The motor is calibrated: %s\n", calibrated ? "true" : "false");
               printf("Number of steps per revolution: ");
               if(calibrated == true){
                  printf("%d\n", 1);   
               }else{
                  printf("Not available\n");
               }
            }
            if(strcmp(buffer, "calibrate") == 0){
               calibrate();
            }
            if(strcmp(buffer, "run") == 0){
               run(5);
            }
        } else {
            if (pos < 64 - 1) {
                buffer[pos++] = c;
            }
         }
      }
   }
   return 1;
}
