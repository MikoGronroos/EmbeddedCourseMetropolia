#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <time.h>
#include <string.h>    
#include "args.h"
#include <stdlib.h>
#include <math.h>
  
#define division 8
#define steps 8
#define pinOpto 28
#define pinA 2
#define pinB 3
#define pinC 6
#define pinD 13

uint numberOfSteps = 0;
uint amountOfArgs = 0;
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

arguments* parse_args(char* input){
  arguments* args = malloc(sizeof(arguments));
  if(args == NULL){
    return NULL;
  }
  amountOfArgs = 1;
  for(int i = 0; i < strlen(input); i++){
    if(input[i] == ' '){
      amountOfArgs++;
    }
  }
  args->length = amountOfArgs;
  args->args = malloc((amountOfArgs + 1) * sizeof(char *));
  int currentStrSize = 0;
  int offset = 0;
  int index = 0;
  for(int i = 0; i < strlen(input); i++){
    currentStrSize++;
    if(input[i+1] == ' ' || input[i+1] == '\0'){
      args->args[index] = (char *)malloc(currentStrSize+1);
      int k = 0;
      for(int j = offset; j < offset + currentStrSize; j++){
         args->args[index][k] = input[j];
         k++;
      }
      args->args[index][k] = '\0'; 
      index++;
      offset += currentStrSize;
      currentStrSize = 0;
    }
  }
  return args;
}

int calibrate(){
   numberOfSteps = 0;
   while(gpio_get(pinOpto) == true){
      turnRight();
   }

   for(int i = 0; i < 3; i++){
      int temp = 0;
      while(gpio_get(pinOpto) == false){
         turnRight();
         temp++;
      }
      while(gpio_get(pinOpto) == true){
         turnRight();
         temp++;
      }
      temp = temp * 8;
      numberOfSteps += temp;
   }
   numberOfSteps = numberOfSteps / 3;
   calibrated = true;
   return 1;
}

int run(int n){
   int cycle = n * numberOfSteps / division / steps;
   for(int i = 0; i < cycle; i++){
      turnRight();
   }
   return 1;
}

int status(){
   printf("The motor is calibrated: %s\n", calibrated ? "true" : "false");
   printf("Number of steps per revolution: ");
   if(calibrated == true){
      printf("%d\n", numberOfSteps);   
   }else{
      printf("Not available\n");
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
            arguments* args = parse_args(buffer);
            pos = 0;
            if(strcmp(args->args[0], "status") == 0){
               if(amountOfArgs == 1){
                  status();
               }
            }
            if(strcmp(args->args[0], "calibrate") == 0){
               if(amountOfArgs == 1){
                  calibrate();
               }
            }
            if(strcmp(args->args[0], "run") == 0){
               if(amountOfArgs > 1){
                  if(atoi(args->args[1]) >= 0){
                     run(atoi(args->args[1]));
                  }
               }else {
                  run(8);
               }
            }
            free(args);
         } else {
            if (pos < 64 - 1) {
                buffer[pos++] = c;
            }
         }
      }
   }
   return 1;
}
