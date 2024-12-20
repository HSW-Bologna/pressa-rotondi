#ifndef BSP_RELAY_H_INCLUDED
#define BSP_RELAY_H_INCLUDED


#include <stdint.h>


typedef enum {
    BSP_RELAY_1 = 0,
    BSP_RELAY_2,
    BSP_RELAY_3,
    BSP_RELAY_4,
    BSP_RELAY_5,
    BSP_RELAY_6,
    BSP_RELAY_7,
    BSP_RELAY_8,
    BSP_RELAY_9,
    BSP_RELAY_10,
    BSP_RELAY_11,
    BSP_RELAY_12,
    BSP_RELAY_13,
    BSP_RELAY_14,
    BSP_RELAY_15,
    BSP_RELAY_16,
#define BSP_RELAY_NUM 16
} bsp_relay_t;


void bsp_relay_update(bsp_relay_t relay, uint8_t level);


#endif
