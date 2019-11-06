#include "kalman.h"

float kalman_filter(kalman_struct_t * kalman_struct, float value)
{
  //formula 1
  if(kalman_struct->old_value <= 0)
  {
    kalman_struct->old_value = value;
    return value;
  }

  //formula 2
  kalman_struct->para.P = kalman_struct->para.P + kalman_struct->para.Q;

  //formula 3
  if( (kalman_struct->para.P + kalman_struct->para.R) != 0)
  {
    kalman_struct->para.K = kalman_struct->para.P/(kalman_struct->para.P + kalman_struct->para.R);
  }

  //formula 4
  kalman_struct->old_value = kalman_struct->old_value + kalman_struct->para.K*(value - kalman_struct->old_value);

  //formula 5
  kalman_struct->para.P = kalman_struct->para.P - kalman_struct->para.P*kalman_struct->para.P;

  return kalman_struct->old_value;
}












