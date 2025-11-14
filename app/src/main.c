/*
 * main.c
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>
#include "BTN.h"
#include "LED.h"
#define SLEEP_MS 50

int press(){
  if (BTN_check_clear_pressed(BTN0)) return 1;
  if (BTN_check_clear_pressed(BTN1)) return 2;
  if (BTN_check_clear_pressed(BTN2)) return 3;
  if (BTN_check_clear_pressed(BTN3)) return 4;
  return -1;  
}

int main(void) {

  if(0 > BTN_init()) return 0;
  if(0 > LED_init()) return 0;

  int pass[3] = {1,2,3};
  int entry[3];

  int i=0, j;

  int btn = press();
  bool locked = true;
  
  printk("Starting\n");

  //Entry Mode 
  

  
  while(1) {


    if(locked == true){ 
    LED_set(LED0, LED_ON);

      while(btn != 4){

        if(btn>0){
          entry[i] = btn;
          printk("Button %d Pressed\n", btn);
          i++;
        }

        btn = press();
        k_msleep(SLEEP_MS);

      }

      for(j=0; j < sizeof(pass)/sizeof(pass[0]); j++){
        if(pass[j] != entry[j]){
        printk("Incorrect\n");
        locked = false;
        printk("In waiting state, press any button to lock.\n");
        break;
      }

        else{
          if(j == i-1){
            printk("Correct\n");
            locked = false;
            printk("In waiting state, press any button to lock.\n");
            break;

          }
        }
      }

    }

    else{
      LED_set(LED0, LED_OFF);
      btn = press();
      if(btn > 0){

        for(int c = 0; c< sizeof(entry)/size(entry[0]); c++) 
          entry[c] = 0;

        i = 0;
        btn = -1;
        locked = true;
        printk("Back to locked state.\n");
        k_msleep(SLEEP_MS);
      }
    }
  }

  return 0;

}

