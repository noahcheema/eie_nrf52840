/*
 * main.c
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>
#include<stdlib.h>
#include<stdio.h>
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

  int i = 0, p = 0, j;
  int btn = press();
  bool locked = false, set = true, begin = false;

  int space = 3, pass_len = 0;
  int *pass = malloc(space * sizeof(int)); 
  int *entry = NULL;

  printk("Beginning Program.\n");

  while(1) {

    if(set == true){

      printk("Press Button 4 to set a password.\n");

      int64_t start = k_uptime_get();

      while(k_uptime_get()-start < 3000){
        btn = press();
        if(btn == 4){
          begin = true;
          set = false;
          LED_set(LED3, LED_ON);
          printk("Use buttons 1 to 3 to set a password and use button 4 to submit.\n");
          break;
        }
        k_msleep(SLEEP_MS);
      }
      if(begin == false){
        printk("Restarting program.\n");
        continue;
      }

      btn = -1;
    }

    if(begin == true){
      btn = press();
    
      if(btn == 4){
        begin = false;
        locked = true;
        LED_set(LED3, LED_OFF);
        
        pass_len = p;
        if(pass_len > 0){
          int *temp = realloc(pass, pass_len * sizeof(int));
          if(temp)
            pass = temp;
          entry = malloc(pass_len*sizeof(int));
          }
        printk("Password has been set.\nEnter Password.\n");
      }

      else if(btn>0){
        pass[p] = btn;
        printk("Entered %d\n", btn);
        p++;

        if(p == space){
          space += 1;
          int *temp = realloc(pass, space * sizeof(int));
          if(temp)
            pass = temp;
        } 
      }

      btn = press();
      k_msleep(SLEEP_MS);

    }

    if(locked == true){ 

    LED_set(LED0, LED_ON);
    i = 0;

      while(btn != 4){

        if(btn>0){
          entry[i] = btn;
          printk("Button %d Pressed\n", btn);
          i++;
        }

        btn = press();
        k_msleep(SLEEP_MS);

      }

      for(j=0; j < pass_len; j++){
        if(pass[j] != entry[j] || i != pass_len){
        printk("Incorrect\n");
        printk("In waiting state, press any button to lock.\n");
        locked = false;
        break;
      }

        if(j == pass_len-1){
          printk("Correct\n");
          printk("In waiting state, press any button to lock.\n");
          locked = false;
          break;
        }
      }

    }

    else if(locked == false && begin == false){
      LED_set(LED0, LED_OFF);
      btn = press();
      if(btn > 0){

        for(int c = 0; c < pass_len; c++) 
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


