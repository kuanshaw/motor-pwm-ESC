#ifndef __ADC_H
#define __ADC_H

#include "stm8s_adc1.h"

#define	ADC_BAT_CHANNEL       ADC1_CHANNEL_3
#define	ADC_BAT_PORT          GPIOD
#define ADC_BAT_PIN           GPIO_PIN_2 

#define	ADC_CURRENT_CHANNEL	  ADC1_CHANNEL_4
#define ADC_CURRENT_PORT      GPIOD
#define ADC_CURRENT_PIN       GPIO_PIN_3 

#define	ADC_TEMP_CHANNEL		  ADC1_CHANNEL_5
#define ADC_TEMP_PORT         GPIOD
#define	ADC_TEMP_PIN          GPIO_PIN_5 


#define ADC_SAMP_LENGTH         (30)
#define NUM_USED_ADC_CHANNEL    (3)

#define ADC_VREF_VOLTAGE        (5.0)
#define ADC_RESOLUTION_VALUE    (1024)

#define BAT_Q                   (0.3)
#define BAT_R                   (0.5)
#define BAT_P                   (1.0)
#define BAT_K                   (0)

#define CURRENT_Q               (0.3)
#define CURRENT_R               (0.5)
#define CURRENT_P               (1.0)
#define CURRENT_K               (0)

#define TEMP_Q                  (0.3)
#define TEMP_R                  (0.5)
#define TEMP_P                  (1.0)
#define TEMP_K                  (0)



typedef enum{
  BAT     = 0,
  CURRENT = 1,
  TEMP    = 2
} adc_channel_name_t;


typedef struct{
  volatile adc_channel_name_t curr_channel;    //current channel
  
  volatile float ave_value[NUM_USED_ADC_CHANNEL];
  volatile float voltage[NUM_USED_ADC_CHANNEL];
  volatile float voltage_flt[NUM_USED_ADC_CHANNEL];                 /**< ADC Voltage with Kalman Filter */

} adc_data_t;


void adc_init(void);
void adc_calc(void);
void adc_channel_set(adc_channel_name_t ch);
uint16_t read_adc_orig_value(void);
volatile float get_adc_value(adc_channel_name_t ch);
volatile float get_adc_voltage(adc_channel_name_t ch);


#endif /* __ADC_H */

