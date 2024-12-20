#include "services/timestamp.h"
#include "heartbit.h"
#include "hal_data.h"
#include "bsp_pin_cfg.h"
#include "input.h"
#include "debounce.h"


static debounce_filter_t filter = {0};


void bsp_input_init(void) {
    debounce_filter_init(&filter);
}


void bsp_input_manage(void) {
    static timestamp_t ts = 0;

    if (timestamp_is_expired(ts, 10)) {
        uint16_t bitmap = 0;

        const bsp_io_port_pin_t pins[] = {
            BSP_PIN_P_IN1, BSP_PIN_P_IN2, BSP_PIN_P_IN3, BSP_PIN_P_IN4,  BSP_PIN_P_IN5,  BSP_PIN_P_IN6,
            BSP_PIN_P_IN7, BSP_PIN_P_IN8, BSP_PIN_P_IN9, BSP_PIN_P_IN10, BSP_PIN_P_IN11, BSP_PIN_P_IN12,
        };

        for (size_t i = 0; i < sizeof(pins) / sizeof(pins[0]); i++) {
            bsp_io_level_t level = BSP_IO_LEVEL_LOW;
            g_ioport.p_api->pinRead(g_ioport.p_ctrl, pins[i], &level);
            if (level == BSP_IO_LEVEL_LOW) {
                bitmap |= 1 << i;
            }
        }

        debounce_filter(&filter, bitmap, 5);

        ts = timestamp_get();
    }
}


uint16_t bsp_input_map(void) {
    return (uint16_t)debounce_value(&filter);
}


uint8_t bsp_get_input(bsp_input_t input) {
    return debounce_read(&filter, input);
}
