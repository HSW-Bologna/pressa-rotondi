#include <stdint.h>
#include "heartbit.h"
#include "hal_data.h"
#include "bsp_pin_cfg.h"
#include "relay.h"


static uint32_t pwm_periph_period;


void bsp_pwm_init(void) {
    fsp_err_t    err = FSP_SUCCESS;
    timer_info_t info;

    /* Initializes the module. */
    err = R_GPT_Open(&g_timer_pwm_ctrl, &g_timer_pwm_cfg);
    assert(FSP_SUCCESS == err);

    /* Start the timer. */
    R_GPT_Start(&g_timer_pwm_ctrl);

    /* Get the current period setting. */
    R_GPT_InfoGet(&g_timer_pwm_ctrl, &info);
    pwm_periph_period = info.period_counts;
}


void bsp_pwm_update(uint16_t pctx10) {
    fsp_err_t    err = FSP_SUCCESS;
    uint32_t     width_cycles;
    gpt_io_pin_t io_name = GPT_IO_PIN_GTIOCB;

    /* Calculate the desired duty cycle based on the current period. Note that if the period could be larger than
     * UINT32_MAX / 100, this calculation could overflow. A cast to uint64_t is used to prevent this. The cast is
     * not required for 16-bit timers. */
    width_cycles = (uint32_t)(((uint64_t)pwm_periph_period * pctx10) / 1000);

    /* Set the calculated duty cycle. */
    err = R_GPT_DutyCycleSet(&g_timer_pwm_ctrl, width_cycles, io_name);
    assert(FSP_SUCCESS == err);
}
