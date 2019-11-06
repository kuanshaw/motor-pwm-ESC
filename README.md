# motor-pwm-ESC

PWM为输入信号，实现电机转速控制。一些主要的函数。

## adc.c 
- 使用一个ADC的不同通道，轮循采样；
- 采用Kalman滤波；

## pwm_input.c
- PWM信号输入模块，采用高频率的中断采集引脚高低电平进行计算真空比。
- pwm_intput_detect()一般置于中断函数中；pwm_input_calc()一般置于主函数中。（对于裸机程序）

## fault_fb.c
- 故障反馈功能。
- 实现对信号线的固定时序拉高拉低。
