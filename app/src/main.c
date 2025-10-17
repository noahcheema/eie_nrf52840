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
#define SLEEP_MS 1


int main(void) {

    int count = 0; 
    if(0 > BTN_init()){
      return 0;
    }
    if(0 > LED_init()){
      return 0; 
    }

  while(1) {
    if(BTN_check_clear_pressed(BTN0)){
      count++;
      printk("%d ", count);
      
    if(count > 15)
        count = 0;
      
    if(count & 0b0001)
        LED_set(LED0, LED_ON);
      else
        LED_set(LED0, LED_OFF);
      
      if(count & 0b0010)
         LED_set(LED1, LED_ON);
      else
        LED_set(LED1, LED_OFF);
      
      
      if(count & 0b0100)
         LED_set(LED2, LED_ON);
      else
        LED_set(LED2, LED_OFF);
      
      if(count & 0b1000)
         LED_set(LED3, LED_ON);
      else
        LED_set(LED3, LED_OFF);
      
      

    }
  }
	return 0;
}
