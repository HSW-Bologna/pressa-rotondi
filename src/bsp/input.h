#ifndef BSP_INPUT_H_INCLUDED
#define BSP_INPUT_H_INCLUDED


#include <stdint.h>


void     bsp_input_init(void);
void     bsp_input_manage(void);
uint16_t bsp_input_map(void);


#endif
