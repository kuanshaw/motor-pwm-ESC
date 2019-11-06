#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm8s.h"
#include "stm8s_gpio.h"

/* FAULT FEEDBACK */

#define PWM_OUTPUT1_PORT        (GPIOC)
#define PWM_OUTPUT1_PIN         (GPIO_PIN_7)

#define PWM_OUTPUT2_PORT        (GPIOC)
#define PWM_OUTPUT2_PIN         (GPIO_PIN_6)

#define DIAG_OUT_PORT           (GPIOB)
#define DIAG_OUT_PIN            (GPIO_PIN_5)

#define SET_PWM_INPUT_CLOSE()   (GPIO_SetBits_redef(DIAG_OUT_PORT, DIAG_OUT_PIN))
#define SET_PWM_INPUT_OPEN()    (GPIO_ResetBits_redef(DIAG_OUT_PORT, DIAG_OUT_PIN))

#define FAULE_FB_COUNT_BASE_TIME            (100)       /* ms */

#define FAULE_FB_TIME_PWM_CLOSE             (20)        // 20*FAULE_FB_COUNT_BASE_TIME = 2s

#define FAULE_FB_TIME_SHORT_CIRCUIT         (20)
#define FAULE_FB_TIME_BLOCK                 (30)
#define FAULE_FB_TIME_OPEN_CIRCUIT          (40)
#define FAULE_FB_TIME_OVER_VOLTAGE          (50)
#define FAULE_FB_TIME_LOW_VOLTAGE           (60)
#define FAULE_FB_TIME_OVER_TEMP             (70)
#define FAULE_FB_TIME_OVER_CURRENT          (80)


void motor_fault_fb(void);
void set_fault_fb_time(uint8_t cnt);



#endif 


