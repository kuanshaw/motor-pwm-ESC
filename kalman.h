#ifndef __KALMAN_H
#define __KALMAN_H


typedef struct{
  float old_value;
  struct{
    float Q;
    float R;
    float P;
    float K;
  } para;

}kalman_struct_t;


float kalman_filter(kalman_struct_t *kalman_struct, float value);


#endif /* __KALMAN_H */

