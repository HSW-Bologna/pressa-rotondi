#include "heartbit.h"
#include "hal_data.h"
#include "bsp_pin_cfg.h"
#include "relay.h"


void bsp_relay_update(bsp_relay_t relay, uint8_t level) {
    const bsp_io_port_pin_t pins[] = {
        BSP_PIN_REL0,  BSP_PIN_REL1,  BSP_PIN_REL2,  BSP_PIN_REL3,  BSP_PIN_REL4,  BSP_PIN_REL5,
        BSP_PIN_REL6,  BSP_PIN_REL7,  BSP_PIN_REL8,  BSP_PIN_REL9,  BSP_PIN_REL10, BSP_PIN_REL11,
        BSP_PIN_REL12, BSP_PIN_REL13, BSP_PIN_REL14, BSP_PIN_REL15,
    };

    g_ioport.p_api->pinWrite(g_ioport.p_ctrl, pins[relay], level ? BSP_IO_LEVEL_HIGH : BSP_IO_LEVEL_LOW);
}
