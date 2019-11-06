#include "pwm_input.h"
#include "gpio.h"

pwm_input_state_t   pwm_input;


void pwm_input_init(void)
{
  pwm_input.capture_high_counter = 0;
  pwm_input.capture_low_counter = 0;
  pwm_input.counting_state = STOP_COUNTING;
  pwm_input.edge_counter = 0;
  
  pwm_input.flag.detect_over = 0;
  pwm_input.flag.edge_level = 0;

  pwm_input.duty_cycle = 10;
  pwm_input.last_duty_cycle = 0;
}


void pwm_input_calc(void)
{
  if(pwm_input.flag.detect_over == 0){
    return;
  }

  static uint8_t cnt = 0;
  static float duty_sum = 0;
  volatile float temp;
  
  pwm_input.flag.detect_over = 0;       /**< Clear the flag */

  /* If out of Counter */
  if(pwm_input.capture_high_counter >= PWM_INPUT_COUNTER_MAX){
    pwm_input.duty_cycle = 100.0;
  }
  else if(pwm_input.capture_low_counter >= PWM_INPUT_COUNTER_MAX){
    pwm_input.duty_cycle = 0;
  }
  else{

#if PWM_INPUT_INVERSION
    temp = (pwm_input.capture_low_counter*100.0) /(pwm_input.capture_high_counter + pwm_input.capture_low_counter);
#else
    temp = (pwm_input.capture_high_counter*100.0) /(pwm_input.capture_high_counter + pwm_input.capture_low_counter);
#endif

    duty_sum += temp;
   
    cnt++;
    if( cnt >= PWM_INPUT_SAMP_LENGTH){
      cnt = 0;              /**< Clear the counter */
      
      pwm_input.last_duty_cycle = pwm_input.duty_cycle;
      pwm_input.duty_cycle = (float)(duty_sum * 1.0 / PWM_INPUT_SAMP_LENGTH);

      duty_sum = 0;
    }
  }
  pwm_input.capture_low_counter = 0;
  pwm_input.capture_high_counter = 0;
  
}


void pwm_intput_detect(void)
{
  static uint16_t high_counter = 0;
  static uint16_t low_counter = 0;
  
  /* Low level */
  if((BitStatus)(PWM_INPUT_PORT->IDR & (uint8_t)PWM_INPUT_PIN) == RESET){
    if(pwm_input.counting_state != LOW_LEVEL){
      pwm_input.counting_state = LOW_LEVEL;
      pwm_input.edge_counter ++;
    }
  }
  /* High level */
  else{
    if(pwm_input.counting_state != HIGH_LEVEL){
      pwm_input.counting_state = HIGH_LEVEL;
      pwm_input.edge_counter ++;
    }
  }

  switch(pwm_input.counting_state){
  case LOW_LEVEL:
    low_counter++;
    break;
  case HIGH_LEVEL:
    high_counter++;
    break;
  case STOP_COUNTING:
    low_counter = 0;
    high_counter = 0;
    break;
  default:
    break;
  }

  if((pwm_input.edge_counter >= 2 ) || (high_counter > PWM_INPUT_COUNTER_MAX) || (low_counter > PWM_INPUT_COUNTER_MAX)){
    pwm_input.edge_counter = 0;
      
    pwm_input.capture_high_counter = high_counter;
    pwm_input.capture_low_counter = low_counter;
    
    high_counter = 0;
    low_counter = 0;
    
    pwm_input.flag.detect_over = 1;
  }
}


float get_pwm_input_duty(void)
{
  return pwm_input.duty_cycle;
}


/* Not use! For Debug. */
void set_pwm_input_duty(float duty)
{
  pwm_input.duty_cycle = duty;

}




