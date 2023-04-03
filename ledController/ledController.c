/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"
#include "pico/time.h"
#include <stdio.h>
#include <math.h>
#include "hardware/adc.h"


#define RED 10
#define GREEN 11
#define BLUE 12
#define PWM_MAX_VALUE 65535

#define RED_BUTTON 8
#define GREEN_BUTTON 7
#define BLUE_BUTTON 6 

#define SWITCH_PIN 15

#define REDSLICE  pwm_gpio_to_slice_num(RED)
#define REDCHANNEL  pwm_gpio_to_channel(RED)
#define GREENSLICE  pwm_gpio_to_slice_num(GREEN)
#define GREENCHANNEL  pwm_gpio_to_channel(GREEN)
#define BLUESLICE  pwm_gpio_to_slice_num(BLUE)
#define BLUECHANNEL  pwm_gpio_to_channel(BLUE)

static double red_value = 255;
static double green_value = 0;
static double blue_value = 0;
double* focused_colour = &red_value;
bool fade = true;  
int colourToPwm(int colour){
    return PWM_MAX_VALUE*(colour/255);
}


void pwm_wrap_handler(){
        

        pwm_clear_irq(REDSLICE);
        pwm_clear_irq(GREENSLICE);
        pwm_clear_irq(BLUESLICE);

        fade =gpio_get(SWITCH_PIN); 
        float increment = 0.2f;
        if (fade){
            if(red_value>0 && blue_value <=1){
                blue_value = 0;
                red_value -= increment;
                green_value+=increment;
            } else if (green_value > 0 && red_value <= 1){
                red_value = 0;
                green_value-=increment;
                blue_value+=increment;
            }else if (blue_value>0 && green_value <=1){
                green_value = 0;
                blue_value-=increment;
                red_value+=increment;
            }else {
                red_value = 255;
                blue_value = 0;
                green_value = 0;
            }
            pwm_set_gpio_level(RED,(red_value*red_value));
            pwm_set_gpio_level(GREEN,(green_value*green_value));
            pwm_set_gpio_level(BLUE,(blue_value*blue_value));
        }
        else{
            pwm_set_gpio_level(RED,(red_value*red_value)*8);
            pwm_set_gpio_level(GREEN,(green_value*green_value)*8);
            pwm_set_gpio_level(BLUE,(blue_value*blue_value)*8);
        }
        
        
}

int main() {

    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    gpio_init(RED_BUTTON);
    gpio_init(GREEN_BUTTON);
    gpio_init(BLUE_BUTTON);

    gpio_set_dir(RED_BUTTON,GPIO_IN);
    gpio_set_dir(GREEN_BUTTON,GPIO_IN);
    gpio_set_dir(BLUE_BUTTON,GPIO_IN);

    gpio_init(SWITCH_PIN);
    gpio_set_dir(SWITCH_PIN,GPIO_IN);

    gpio_set_function(RED, GPIO_FUNC_PWM);
    gpio_set_function(GREEN, GPIO_FUNC_PWM);
    gpio_set_function(BLUE, GPIO_FUNC_PWM);

    pwm_clear_irq(REDSLICE);
    pwm_set_irq_enabled(REDSLICE, true);
    pwm_clear_irq(BLUESLICE);
    pwm_set_irq_enabled(BLUESLICE, true);
    pwm_clear_irq(GREENSLICE);
    pwm_set_irq_enabled(GREENSLICE, true);

    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_wrap_handler);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.f);
    
    

    pwm_init(REDSLICE, &config, true);
    pwm_init(GREENSLICE, &config, true);
    pwm_init(BLUESLICE, &config, true);
    

    while (true) {
        
        if (gpio_get(SWITCH_PIN) == false){
            uint16_t result = adc_read();
            *focused_colour = sqrt(result)-10;
            if (gpio_get(GREEN_BUTTON)){
                focused_colour = &green_value;
            }
            if (gpio_get(BLUE_BUTTON)){
                focused_colour = &blue_value;
            }
            if (gpio_get(RED_BUTTON)){
                focused_colour = &red_value;
            }
        } 
            
            
        

    }

}

