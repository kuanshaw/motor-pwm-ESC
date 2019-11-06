#include "adc.h"
#include "gpio.h"
#include "kalman.h"
#include "motor.h"

adc_data_t adc_data;
kalman_struct_t kalman[NUM_USED_ADC_CHANNEL];


void adc_kalman_init(void)
{
  /************* Value Init **************/

  kalman[BAT].old_value = 0;
  kalman[BAT].para.Q = BAT_Q;
  kalman[BAT].para.R = BAT_R;
  kalman[BAT].para.P = BAT_Q;
  kalman[BAT].para.K = BAT_K;

  kalman[CURRENT].old_value = 0;
  kalman[CURRENT].para.Q = CURRENT_Q;
  kalman[CURRENT].para.R = CURRENT_R;
  kalman[CURRENT].para.P = CURRENT_Q;
  kalman[CURRENT].para.K = CURRENT_K;

  kalman[TEMP].old_value = 0;
  kalman[TEMP].para.Q = TEMP_Q;
  kalman[TEMP].para.R = TEMP_R;
  kalman[TEMP].para.P = TEMP_Q;
  kalman[TEMP].para.K = TEMP_K;
}


void adc_init(void)
{
  uint8_t i = 0;
  
  GPIO_Init(ADC_BAT_PORT, ADC_BAT_PIN, GPIO_MODE_IN_FL_NO_IT); 
  GPIO_Init(ADC_TEMP_PORT, ADC_TEMP_PIN, GPIO_MODE_IN_FL_NO_IT);
  GPIO_Init(ADC_CURRENT_PORT, ADC_CURRENT_PIN, GPIO_MODE_IN_FL_NO_IT);

  /* Init ADC1 peripheral----block and over current protect */
  ADC1_Init(ADC1_CONVERSIONMODE_SINGLE, ADC_CURRENT_CHANNEL, ADC1_PRESSEL_FCPU_D2, \
      ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL0,\
      DISABLE);


  /* Init ADC1 peripheral---temperature */
  ADC1_Init(ADC1_CONVERSIONMODE_SINGLE, ADC_TEMP_CHANNEL, ADC1_PRESSEL_FCPU_D2, \
      ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL5,\
      DISABLE);

    /* Init ADC1 peripheral---battery voltage */
  ADC1_Init(ADC1_CONVERSIONMODE_SINGLE, ADC_BAT_CHANNEL, ADC1_PRESSEL_FCPU_D2, \
      ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL3,\
      DISABLE);


  /* Enable EOC interrupt */
  //ADC1_ITConfig(ADC1_IT_EOCIE,ENABLE);   
	//ADC1->CSR |= (uint8_t)ADC1_IT_EOCIE;  /* Enable the ADC1 interrupts */
  
  /************* Value Init **************/
  for(i=0; i<NUM_USED_ADC_CHANNEL; i++){
    adc_data.ave_value[i] = 0;
    adc_data.voltage[i] = 0;
    adc_data.voltage_flt[i] = 0;
  }
  
  adc_data.curr_channel = BAT;  
  adc_channel_set(adc_data.curr_channel);

  adc_kalman_init();
}


void adc_calc(void)
{
  uint16_t temp = 0;
  static uint8_t  cnt[NUM_USED_ADC_CHANNEL] = {0, 0};                /**< Counter */
  static uint16_t sum[NUM_USED_ADC_CHANNEL] = {0, 0};
  static uint16_t max[NUM_USED_ADC_CHANNEL] = {0, 0};
  static uint16_t min[NUM_USED_ADC_CHANNEL] = {0xFFFF, 0xFFFF};

  /* When ADC end of Conversion. */
  if((ADC1->CSR & ADC1_CSR_EOC) == 0){
     return;
  }
  
  temp = (read_adc_orig_value() +1 );
  cnt[adc_data.curr_channel]++;
  
  /*********** Calculate the Ave Value **************/
  sum[adc_data.curr_channel] += temp;
  if(temp > max[adc_data.curr_channel]){
    max[adc_data.curr_channel] = temp;
  }
  if(temp < min[adc_data.curr_channel]){
    min[adc_data.curr_channel] = temp;
  }

  /* Calculate */
  if(cnt[adc_data.curr_channel] >= (ADC_SAMP_LENGTH)){
    cnt[adc_data.curr_channel] = 0;
    
    /* Calculate the Ave Value */
    adc_data.ave_value[adc_data.curr_channel] = 
      (sum[adc_data.curr_channel]-max[adc_data.curr_channel]-min[adc_data.curr_channel]) / (ADC_SAMP_LENGTH - 2);
    
    /* Calculate the Voltage */
    adc_data.voltage[adc_data.curr_channel] = (adc_data.ave_value[adc_data.curr_channel]) * ADC_VREF_VOLTAGE / ADC_RESOLUTION_VALUE;
    
    /* Filter */
    adc_data.voltage_flt[adc_data.curr_channel] = kalman_filter(&kalman[adc_data.curr_channel], adc_data.voltage[adc_data.curr_channel]);
    
    /* Updata Motor Info */
    switch(adc_data.curr_channel){
      case BAT:
        calc_Bat_voltage();
        break;
      case CURRENT:
        calc_Current();
        break;
      case TEMP:
        calc_Temp();
        break;
    }

    /* Reset the Value */
    sum[adc_data.curr_channel] = 0;
    max[adc_data.curr_channel] = 0;
    min[adc_data.curr_channel] = 0xFFFF;
  }
 
  /* Change the Channel */
  adc_data.curr_channel++;
  if(adc_data.curr_channel >= NUM_USED_ADC_CHANNEL){
    adc_data.curr_channel = 0;    // BAT-CURRENT-TEMP
  }
  adc_channel_set(adc_data.curr_channel);
}



void adc_channel_set(adc_channel_name_t ch)
{
  ADC1->CSR &= 0xF0;

  switch(ch){
    case BAT:
      ADC1->CSR |= (uint8_t)(ADC_BAT_CHANNEL);
      break;
    case CURRENT:
      ADC1->CSR |= (uint8_t)(ADC_CURRENT_CHANNEL);
      break;
    case TEMP:
      ADC1->CSR |= (uint8_t)(ADC_TEMP_CHANNEL);
      break;
  }
  ADC1->CR1 |= (uint8_t)ADC1_CR1_ADON;
}


uint16_t read_adc_orig_value(void)
{
  uint16_t temph = 0;
  uint8_t templ = 0;
  
  templ = ADC1->DRL;
  temph = ADC1->DRH;
  temph = (uint16_t)(templ | (uint16_t)(temph << (uint8_t)8));
  
  return temph;
}


volatile float get_adc_value(adc_channel_name_t ch)
{
  return adc_data.ave_value[ch];
}


volatile float get_adc_voltage(adc_channel_name_t ch)
{
  return (adc_data.voltage_flt[ch]);                   /**< Uint: V */
}


