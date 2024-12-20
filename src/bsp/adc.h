#ifndef BSP_ADC_H_
#define BSP_ADC_H_


#include <stdint.h>


typedef enum {
    BSP_ADC_FEEDBACK_0_10V = 0,
    BSP_ADC_4_20_MA,
#define BSP_ADC_NUM 2
} bsp_adc_t;


void     bsp_adc_init(void);
uint16_t bsp_adc_get(bsp_adc_t adc);
void     bsp_adc_manage(void);


#endif /* BSP_ADC_H_ */
