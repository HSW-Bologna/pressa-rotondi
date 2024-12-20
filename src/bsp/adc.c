#include <stdint.h>
#include "hal_data.h"
#include "adc.h"
#include "services/timestamp.h"


#define NUM_SAMPLES 10


static volatile uint16_t samples_index_0_10v         = 0;
static volatile uint16_t samples_0_10v[NUM_SAMPLES]  = {0};
static volatile uint16_t samples_index_4_20ma        = 0;
static volatile uint16_t samples_4_20ma[NUM_SAMPLES] = {0};


void bsp_adc_init(void) {
    fsp_err_t ret = FSP_SUCCESS;
    ret           = R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);
    assert(FSP_SUCCESS == ret);

    ret = R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);
    assert(FSP_SUCCESS == ret);

    /* triggers by calling R_ADC_ScanStart(). */
    //(void) R_ADC_ScanStart(&g_adc0_ctrl);
}


uint16_t bsp_adc_get(bsp_adc_t adc) {
    uint16_t *samples[BSP_ADC_NUM] = {(uint16_t *)samples_0_10v, (uint16_t *)samples_4_20ma};

    uint32_t sum = 0;
    for (uint16_t i = 0; i < BSP_ADC_NUM; i++) {
        sum += samples[adc][i];
    }

    return (uint16_t)(sum/NUM_SAMPLES);
}


void bsp_adc_manage(void) {
    static timestamp_t ts = 0;

    if (timestamp_is_expired(ts, 100)) {
        (void)R_ADC_ScanStart(&g_adc0_ctrl);
        ts = timestamp_get();
    }
}


void adc_sample_callback(adc_callback_args_t *p_arg) {
    switch (p_arg->event) {
        case ADC_EVENT_SCAN_COMPLETE: {
            switch (p_arg->channel) {
                case ADC_CHANNEL_0: {
                    uint16_t adc_data = 0;
                    R_ADC_Read(&g_adc0_ctrl, ADC_CHANNEL_0, &adc_data);
                    samples_0_10v[samples_index_0_10v] = adc_data;
                    samples_index_0_10v                = (uint16_t)(samples_index_0_10v + 1) % NUM_SAMPLES;
                    break;
                }

                case ADC_CHANNEL_2: {
                    uint16_t adc_data = 0;
                    R_ADC_Read(&g_adc0_ctrl, ADC_CHANNEL_0, &adc_data);
                    samples_4_20ma[samples_index_4_20ma] = adc_data;
                    samples_index_4_20ma                 = (uint16_t)(samples_index_4_20ma + 1) % NUM_SAMPLES;
                    break;
                }

                default:
                    break;
            }
            break;
        }

        default: {
            __NOP();
            __NOP();
            __NOP();
            __NOP();
            break;
        }
    }

    return;
}
