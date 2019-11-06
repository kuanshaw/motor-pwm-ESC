#include "fault_fb.h"

uint8_t fault_fb_time = 0;
extern motor_info_t motor;

// Executed per 100mms
void motor_fault_fb(void)
{
  static uint8_t diag_edge = 0;
  static uint8_t cnt = 0;

  if((motor.state == MOTOR_RUN) && (diag_edge == 0)){
    return;
  }
  
  if((diag_edge == 1) && (cnt == FAULE_FB_TIME_PWM_CLOSE)){
    SET_PWM_INPUT_OPEN();
    cnt = 0;
    diag_edge++;
  }

  if((diag_edge == 2) && (cnt == fault_fb_time)){
    diag_edge = 0;
  }

  if((motor.state == MOTOR_FAULT) && (diag_edge == 0)){
    SET_PWM_INPUT_CLOSE();
    cnt = 0;
    diag_edge++;
  }

  cnt++;

}


void set_fault_fb_time(uint8_t cnt)
{
  fault_fb_time = (uint8_t)cnt;
}


