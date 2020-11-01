  
#include "mbed.h"
#include <iostream>
#include <string>

DigitalOut red_(p5); //Representando a cor laranja
DigitalOut blue_(p6);
InterruptIn button(p7);
PwmOut intensity(p8);
Timeout press_timer, led_out;

enum States { 
    UP, 
    DOWN, 
    RISE, 
    FALL, 
    MAX,
    MIN, 
};

States state = UP;
bool press_ = false;
float led_intensity;
float blink_timer;
char* name_ ;


void start_button();
void end_button();
void switcht_state();
void state_name();
void blink();


int main() {
    red_ = 1;
    blue_ = 0;
    led_intensity = 0.5;
    button.rise(&start_button);
    button.fall(&end_button);
    
    while(1) {
        intensity = led_intensity;
        state_name();
        printf( "State: %s\n" , name_);
        printf("Intensity: %.2f\n", led_intensity);
        wait_ms(100);
    }
}



void start_button() {
    press_ = true;
    press_timer.attach(&switcht_state, 1.0);
}

void end_button() {
    press_ = false;
    switch (state) {
        case DOWN:
        case RISE:
        case MIN:
            if (led_intensity < 1) {
                blue_ = 1;
                red_ = 0;
                state = UP;
            }
            break;
        case UP:
        case FALL:
        case MAX:
            if (led_intensity > 0) {
                blue_ = 0;
                red_ = 1;
                state = DOWN;
            }
            break;
    }
}



void blink(){
    switch(state) {
        case MIN:
        case FALL:
            red_ = !red_;
            break;
        case MAX:
        case RISE:
            blue_ = !blue_;
            break;
        default:
            break;
    }

    switch(state) {
        case MAX:
        case MIN:
            blink_timer = 0.1;
            break;
        case RISE:
            blink_timer = 0.2 * (1 + led_intensity / 0.05);
            break;
        case FALL:
            blink_timer = (0.2 * (led_intensity / 0.05 - 1));
            break;
        default:
            break;
    }
    led_out.attach(&blink, blink_timer);
}

void state_name() {
    
    switch(state) {
        case RISE: name_ = "RISING"; break;
        case FALL: name_ = "FALLING"; break;
        case UP: name_ = "UP"; break;
        case DOWN: name_ = "DOWN"; break;
        case MAX: name_ = "MAX"; break;
        case MIN: name_ = "MIN"; break;
    }
    
    
}

void switcht_state(){
    press_timer.attach(&switcht_state, 1.0);
    led_out.attach(&blink, 0.2);

        if (press_) {
            switch (state) {
                case RISE:
                    if (led_intensity == 1) state = MAX;
                    else led_intensity += 0.05;
                    break;
                case FALL:
                    if (led_intensity == 0) state = MIN;
                    else led_intensity -= 0.05;
                    break;
                case UP:
                    state = RISE;
                    led_intensity += 0.05;
                    break;
                case DOWN:
                    state = FALL;
                    led_intensity -= 0.05;
                    break;
                default:
                    break;
            }

            if (led_intensity >= 1) {
                state = MAX;
                led_intensity = 1;
            } else if (led_intensity <= 0) {
                state = MIN;
                led_intensity = 0;
            }
    }
}
