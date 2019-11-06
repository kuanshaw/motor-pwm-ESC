#ifndef __PWM_INPUT_H
#define __PWM_INPUT_H

/******************************************************************************
* Include
******************************************************************************/

#include "stm8s.h"

/******************************************************************************
* Macro
******************************************************************************/

#define PWM_INPUT_PORT          (GPIOD)
#define PWM_INPUT_PIN           (GPIO_PIN_4)

#define PWM_INPUT_INVERSION     (1)

#define PWM_INPUT_COUNTER_MAX   (2000)
#define PWM_INPUT_SAMP_LENGTH   (10)


/******************************************************************************
* Structure
******************************************************************************/

typedef enum{
  HIGH_LEVEL     = 1,
  LOW_LEVEL      = 2,
  STOP_COUNTING  = 3
} counting_state_t;

typedef struct{
  volatile uint16_t capture_low_counter;
  volatile uint16_t capture_high_counter;
  
  volatile uint8_t edge_counter;         /**< 1-start, 2-duty cycle, 3-period */
  
  volatile counting_state_t counting_state;
  
  volatile float duty_cycle;
  volatile float last_duty_cycle;
  
  struct{
    uint8_t detect_over   :1;
    uint8_t edge_level    :1;
  }flag;
} pwm_input_state_t;


/******************************************************************************
* Function
******************************************************************************/
void pwm_input_init(void);
void pwm_input_calc(void);
void pwm_intput_detect(void);
float get_pwm_input_duty(void);
void set_pwm_input_duty(float duty);




#endif /* __PWM_INPUT_H */

