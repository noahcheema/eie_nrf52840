/*
* State Machine File
*/

#include <zephyr/smf.h>


#include "LED.h"
#include "BTN.h"
#include "my_state_machine.h"

// USE PWM

/*
Function Prototypes
*/

int press();
void all_LED(int state);
void cleararray();

static enum smf_state_result firstchar_run(void* o);
static enum smf_state_result strbuild_run(void* o);
static enum smf_state_result submission_run(void* o);
static enum smf_state_result standby_run(void* o);

static void firstchar_entry(void* o);
static void strbuild_entry(void* o);
static void submission_entry(void* o);
static void standby_entry(void* o);

/*
Typedefs
*/

enum program_states{
    FIRSTCHAR,
    STRBUILD,
    SUBMISSION,
    STANDBY
};

typedef struct {
    struct smf_ctx ctx;

    uint16_t count;

    int btn;

    uint8_t bit_index;

    uint8_t character;

    int string[128];
    
    uint8_t len;

    int prevstate;

    uint8_t current_duty_cycle;

} machine_state_object_t;

/*
Local Variables
*/

static const struct smf_state machine_states[] = {
    [FIRSTCHAR] = SMF_CREATE_STATE(firstchar_entry, firstchar_run, NULL, NULL, NULL),
    [STRBUILD] = SMF_CREATE_STATE(strbuild_entry, strbuild_run, NULL, NULL, NULL),
    [SUBMISSION] = SMF_CREATE_STATE(submission_entry, submission_run, NULL, NULL, NULL),
    [STANDBY] = SMF_CREATE_STATE(standby_entry, standby_run, NULL, NULL, NULL)
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

void cleararray(void *p, size_t len){
    unsigned char *b = p;
    for(size_t i = 0; i < len; i++){
        b[i] = 0;
    }
}

void all_LED(int state){
    for(int i = 0; i < 4; i++){
        LED_set(i, state);
    }
}

int held(){
    static int time = 0;

    if(BTN_is_pressed(BTN0) && BTN_is_pressed(BTN1)){
        if(time ==0)
            time = k_uptime_get();
    if(k_uptime_get() - time >= 3000)
        return 1;
    }

    else time = 0; 

    return 0;
}

void state_machine_init(){
    machine_state_object.count = 0;
    machine_state_object.btn = -1;
    machine_state_object.bit_index = 0;
    machine_state_object.character = 0;

    smf_set_initial(SMF_CTX(&machine_state_object), &machine_states[FIRSTCHAR]);
}

int state_machine_run(){
    return smf_run_state(SMF_CTX(&machine_state_object));
}

static void firstchar_entry(void* o){
    machine_state_object.count = 0;
    machine_state_object.btn = -1;
    machine_state_object.bit_index = 0;
    machine_state_object.character = 0;
    cleararray(machine_state_object.string, sizeof(machine_state_object.string));
    all_LED(LED_OFF);
    LED_set(LED3, LED_ON);
}

static enum smf_state_result firstchar_run(void* o){
    if(machine_state_object.count == 1000){
        LED_toggle(LED3);
        machine_state_object.count = 0;
    }

    if(machine_state_object.btn == 1)
        machine_state_object.bit_index++;
    
    else if(machine_state_object.btn == 2){
        machine_state_object.character |= (1 << (7 - machine_state_object.bit_index));
        machine_state_object.bit_index++;
    }

    else if(machine_state_object.btn == 3){
        printk("Button 3 pressed, clearing first character entry.\n");
        machine_state_object.bit_index = 0;
        machine_state_object.character = 0;
    }

    else if(machine_state_object.btn == 4){
        if(machine_state_object.bit_index != 0){
            printk("Invalid ASCII codes detected, restarting program\n");
            smf_set_state(SMF_CTX(&machine_state_object), &machine_states[FIRSTCHAR]);
        }
        printk("Button 4 Pressed, Entering string build state\n");
        smf_set_state(SMF_CTX(&machine_state_object), &machine_states[STRBUILD]);
    }

    else if(held() == 1){
        printk("Entering Standby State...\n");
        machine_state_object.prevstate = 1;
        smf_set_state(SMF_CTX(&machine_state_object), &machine_states[STANDBY]);
    }
    
    if(machine_state_object.bit_index == 8){
        machine_state_object.string[machine_state_object.len++] = (char) machine_state_object.character;
        machine_state_object.bit_index = 0;
        machine_state_object.character = 0;
    }

    machine_state_object.count++;
    machine_state_object.btn = press();
    return SMF_EVENT_HANDLED;
}

static void strbuild_entry(void* o){
    machine_state_object.count = 0;
    machine_state_object.btn = -1;
    machine_state_object.bit_index = 0;
    machine_state_object.character = 0;
    all_LED(LED_OFF);
}

static enum smf_state_result strbuild_run(void* o){
    if(machine_state_object.count == 250){
        LED_toggle(LED3);
        machine_state_object.count = 0;
    }
    
    if(machine_state_object.btn == 1)
        machine_state_object.bit_index++;
    
    else if(machine_state_object.btn == 2){
        machine_state_object.character |= (1 << (7 - machine_state_object.bit_index));
        machine_state_object.bit_index++;
    }

    else if(machine_state_object.btn == 3){
        printk("Clearing current entry and prior entrys...\nReturning to initial entry state.\n");
        machine_state_object.bit_index = 0;
        machine_state_object.character = 0;
        cleararray(machine_state_object.string, sizeof(machine_state_object.string));
        machine_state_object.len = 0; 
        smf_set_state(SMF_CTX(&machine_state_object), &machine_states[FIRSTCHAR]);
    }

    else if(machine_state_object.btn == 4){
        
        if(machine_state_object.bit_index != 0){
            printk("Invalid ASCII codes detected, restarting program\n");
            smf_set_state(SMF_CTX(&machine_state_object), &machine_states[FIRSTCHAR]);
        }

        printk("Entering Submission State...\nButton 4 to print string. Button 3 to restart.\n");
        machine_state_object.string[machine_state_object.len] = '\0';
        smf_set_state(SMF_CTX(&machine_state_object), &machine_states[SUBMISSION]);
    }

    else if(held() == 1){
        printk("Entering Standby State...\n");
        machine_state_object.prevstate = 2;
        smf_set_state(SMF_CTX(&machine_state_object), &machine_states[STANDBY]);
    }

    if(machine_state_object.bit_index == 8){
        machine_state_object.string[machine_state_object.len++] = (char) machine_state_object.character;
        machine_state_object.bit_index = 0;
        machine_state_object.character = 0;
    }


    machine_state_object.count++;
    machine_state_object.btn = press();
    return SMF_EVENT_HANDLED;
}

static void submission_entry(void* o){
    machine_state_object.count = 0;
    machine_state_object.btn = -1;
    machine_state_object.bit_index = 0;
    machine_state_object.character = 0;
    all_LED(LED_OFF);
}

static enum smf_state_result submission_run(void* o){
    if(machine_state_object.count == 63){
        LED_toggle(LED3);
        machine_state_object.count = 0;
    }

    if(machine_state_object.btn == 3){
        printk("Clearing all entries...\n Returning to first character entry state\n");
        smf_set_state(SMF_CTX(&machine_state_object), &machine_states[FIRSTCHAR]);
    }

    else if(machine_state_object.btn == 4){
        printk("Printing string to serial monitor...\n");
        for(int i = 0; machine_state_object.string[i] != '\0'; i++)
            printk("%d", machine_state_object.string[i]);
        printk("\nRestarting program(Returning to first character entry state)\n");
        smf_set_state(SMF_CTX(&machine_state_object), &machine_states[FIRSTCHAR]);
    }

    else if(held() == 1){
        printk("Entering Standby State...\n");
        machine_state_object.prevstate = 3;
        smf_set_state(SMF_CTX(&machine_state_object), &machine_states[STANDBY]);
    }
        
    machine_state_object.count++;
    machine_state_object.btn = press();
    
    return SMF_EVENT_HANDLED;
}

static void standby_entry(void* o){
    machine_state_object.count = 0;
    machine_state_object.btn = -1;
    machine_state_object.bit_index = 0;
    machine_state_object.character = 0;
    machine_state_object.current_duty_cycle = 0;
    all_LED(LED_OFF);
}

static enum smf_state_result standby_run(void* o){
  
    if(machine_state_object.current_duty_cycle >= 100){
        for(int i = 0; i < 100; i++){
            machine_state_object.current_duty_cycle = machine_state_object.current_duty_cycle - 1; 
            k_msleep(10);
            LED_pwm(LED0, machine_state_object.current_duty_cycle); LED_pwm(LED1, machine_state_object.current_duty_cycle); LED_pwm(LED2, machine_state_object.current_duty_cycle); LED_pwm(LED3, machine_state_object.current_duty_cycle);
        }
    }

    else{
        for(int j = 0; j < 100; j++){
            machine_state_object.current_duty_cycle = machine_state_object.current_duty_cycle + 1;
            k_msleep(10);
            LED_pwm(LED0, machine_state_object.current_duty_cycle); LED_pwm(LED1, machine_state_object.current_duty_cycle); LED_pwm(LED2, machine_state_object.current_duty_cycle); LED_pwm(LED3, machine_state_object.current_duty_cycle);
        }
    }

    
    if(machine_state_object.btn == 3 || machine_state_object.btn == 4){
        if(machine_state_object.prevstate == 1){
            printk("Returning to first character entry state\n");
            smf_set_state(SMF_CTX(&machine_state_object), &machine_states[FIRSTCHAR]);
        }

        else if(machine_state_object.prevstate == 2){
            printk("Returning to string build state\n");
            smf_set_state(SMF_CTX(&machine_state_object), &machine_states[STRBUILD]);
        }

        else if(machine_state_object.prevstate == 3){
            printk("Returning to submission state\n");
            smf_set_state(SMF_CTX(&machine_state_object), &machine_states[SUBMISSION]);
        }
    }
    
    machine_state_object.btn=press();
    
    return SMF_EVENT_HANDLED;
}

