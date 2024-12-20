#ifndef BSP_INPUT_H_INCLUDED
#define BSP_INPUT_H_INCLUDED


#include <stdint.h>


typedef enum {
    BSP_INPUT_PEDAL = 0,
} bsp_input_t;


void     bsp_input_init(void);
void     bsp_input_manage(void);
uint16_t bsp_input_map(void);
uint8_t  bsp_get_input(bsp_input_t input);


#endif
