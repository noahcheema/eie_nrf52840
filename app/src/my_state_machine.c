/**
 * @file my_state_machine.c
 */

#include <zephyr/smf.h>


#include "LED.h"
#include "BTN.h"
#include "my_state_machine.h"



/*
Function Protoypes
*/

int press();

void all_LED(int state);

static enum smf_state_result state0_run(void* o);
static enum smf_state_result state1_run(void* o);
static enum smf_state_result state2_run(void* o);
static enum smf_state_result state3_run(void* o);
static enum smf_state_result state4_run(void* o);


static void state0_entry(void* o);
static void state1_entry(void* o);
static void state2_entry(void* o);
static void state3_entry(void* o);
static void state4_entry(void* o);

/*
Typedefs
*/

enum led_state_machine_states{
    STATE0,
    STATE1,
    STATE2, 
    STATE3,
    STATE4
};

typedef struct {
    struct smf_ctx ctx;

    uint16_t count;

    int btn;
} machine_state_object_t;

/*
Local Variables
*/
static const struct smf_state machine_states[] = {
    [STATE0] = SMF_CREATE_STATE(state0_entry, state0_run, NULL, NULL, NULL),
    [STATE1] = SMF_CREATE_STATE(state1_entry, state1_run, NULL, NULL, NULL),
    [STATE2] = SMF_CREATE_STATE(state2_entry, state2_run, NULL, NULL, NULL),
    [STATE3] = SMF_CREATE_STATE(state3_entry, state3_run, NULL, NULL, NULL),
    [STATE4] = SMF_CREATE_STATE(state4_entry, state4_run, NULL, NULL, NULL)
};

static machine_state_object_t machine_state_object;

/*
Function Definitions
*/

int press(){
    if(BTN_check_clear_pressed(BTN0)) return 1;
    if(BTN_check_clear_pressed(BTN1)) return 2;
    if(BTN_check_clear_pressed(BTN2)) return 3;
    if(BTN_check_clear_pressed(BTN3)) return 4;
    return -1;
}

void all_LED(int state){
    for(int i = 0; i < 4; i++){
        LED_set(i, state);
    }
}

void state_machine_init(){
    machine_state_object.btn = -1;
    machine_state_object.count = 0;
    smf_set_initial(SMF_CTX(&machine_state_object), &machine_states[STATE0]);
}

int state_machine_run(){
    return smf_run_state(SMF_CTX(&machine_state_object));
}

static void state0_entry(void* o){
    machine_state_object.count = 0;
    machine_state_object.btn = -1;
    all_LED(LED_OFF);
}

static enum smf_state_result state0_run(void* o){
    if(machine_state_object.btn == 1){
        printk("Entering State 1\n");
        smf_set_state(SMF_CTX(&machine_state_object), &machine_states[STATE1]);
    } 
    else{
        machine_state_object.btn = press();
    }

    return SMF_EVENT_HANDLED;
}

static void state1_entry(void* o){
    machine_state_object.count = 0;
    machine_state_object.btn = -1;
    LED_set(LED0, LED_ON);
}

static enum smf_state_result state1_run(void* o){

    if(machine_state_object.count > 250){
        LED_toggle(LED0); 
        machine_state_object.count = 0; 
    }
    else{
        machine_state_object.count++;
    }

    if(machine_state_object.btn == 2){
        printk("Entering State 2\n");
        all_LED(LED_OFF);
        smf_set_state(SMF_CTX(&machine_state_object), &machine_states[STATE2]);
    }
        
    else if(machine_state_object.btn == 3){
        printk("Entering State 4\n");
        all_LED(LED_OFF);
        smf_set_state(SMF_CTX(&machine_state_object), &machine_states[STATE4]);
    }

    else if(machine_state_object.btn == 4){
        printk("Entering State 0\n");
        smf_set_state(SMF_CTX(&machine_state_object), &machine_states[STATE0]);
    }
    else if(machine_state_object.btn == 1)
        return SMF_EVENT_HANDLED;

    else{        
        machine_state_object.btn = press();
    }

    return SMF_EVENT_HANDLED;
}


static void state2_entry(void* o){
    machine_state_object.count = 0; 
    machine_state_object.btn = -1;
    LED_set(LED0, LED_ON);
    LED_set(LED2, LED_ON);
}

static enum smf_state_result state2_run(void* o){
    if(machine_state_object.count == 1000){
        printk("1 Second has Passed, Entering State 3\n");
        all_LED(LED_OFF);
        smf_set_state(SMF_CTX(&machine_state_object), &machine_states[STATE3]);
    }
    else{
        machine_state_object.count++;
    }

    if(machine_state_object.btn == 4){
        printk("Entering State 0\n");
        smf_set_state(SMF_CTX(&machine_state_object), &machine_states[STATE0]);
    }
    else{
        machine_state_object.btn = press();
    }

    return SMF_EVENT_HANDLED;

}

static void state3_entry(void* o){
    machine_state_object.count = 0;
    machine_state_object.btn = -1;
    LED_set(LED1, LED_ON);
    LED_set(LED3, LED_ON);
}

static enum smf_state_result state3_run(void* o){
    if(machine_state_object.count == 2000){
        printk("2 Seconds have Passed, Entering State 2\n");
        all_LED(LED_OFF);
        smf_set_state(SMF_CTX(&machine_state_object), &machine_states[STATE2]);
    }
    else{
        machine_state_object.count++;
    }

    if(machine_state_object.btn == 4){
        printk("Entering State 0\n");
        smf_set_state(SMF_CTX(&machine_state_object), &machine_states[STATE0]);
    }
    else{
        machine_state_object.btn = press();
    }
    
    return SMF_EVENT_HANDLED;
}

static void state4_entry(void* o){
    machine_state_object.count = 0;
    machine_state_object.btn = -1;
    all_LED(LED_ON);
}

static enum smf_state_result state4_run(void* o){
    if(machine_state_object.count == 63){
        LED_toggle(LED0);
        LED_toggle(LED1);
        LED_toggle(LED2);
        LED_toggle(LED3);
        machine_state_object.count = 0;
    }
    else{
        machine_state_object.count++;
    }
    
    machine_state_object.btn = press();

    if(machine_state_object.btn == 4){
        printk("Entering State 0\n");
        smf_set_state(SMF_CTX(&machine_state_object), &machine_states[STATE0]);
    }

    return SMF_EVENT_HANDLED;

}