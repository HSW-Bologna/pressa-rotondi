/* generated configuration header file - do not edit */
#ifndef BSP_PIN_CFG_H_
#define BSP_PIN_CFG_H_
#include "r_ioport.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

#define BSP_PIN_FEEDBACK_0_10V_M (BSP_IO_PORT_00_PIN_00) /* Analog feedback on the 0/10 Volt ouptut */
#define BSP_PIN_4_20MA (BSP_IO_PORT_00_PIN_02) /* 4-20 ma input */
#define BSP_PIN_P_IN9 (BSP_IO_PORT_00_PIN_13) /* Input 9 */
#define BSP_PIN_P_IN10 (BSP_IO_PORT_00_PIN_14) /* Input 10 */
#define BSP_PIN_P_IN11 (BSP_IO_PORT_00_PIN_15) /* Input 11 */
#define BSP_PIN_P_IN6 (BSP_IO_PORT_01_PIN_00) /* Input 6 */
#define BSP_PIN_P_IN5 (BSP_IO_PORT_01_PIN_01) /* Input 5 */
#define BSP_PIN_P_IN4 (BSP_IO_PORT_01_PIN_02) /* Input 4 */
#define BSP_PIN_P_IN3 (BSP_IO_PORT_01_PIN_03) /* Input 3 */
#define BSP_PIN_P_IN2 (BSP_IO_PORT_01_PIN_04) /* Input 2 */
#define BSP_PIN_P_IN1 (BSP_IO_PORT_01_PIN_05) /* Input 1 */
#define BSP_PIN_REL5 (BSP_IO_PORT_01_PIN_06) /* Relay 6 */
#define BSP_PIN_REL6 (BSP_IO_PORT_01_PIN_07) /* Relay 7 */
#define BSP_PIN_REL0 (BSP_IO_PORT_01_PIN_09) /* Relay 1 */
#define BSP_PIN_REL1 (BSP_IO_PORT_01_PIN_10) /* Relay 2 */
#define BSP_PIN_REL2 (BSP_IO_PORT_01_PIN_11) /* Relay 3 */
#define BSP_PIN_REL3 (BSP_IO_PORT_01_PIN_12) /* Relay 4 */
#define BSP_PIN_REL4 (BSP_IO_PORT_01_PIN_13) /* Relay 5 */
#define BSP_PIN_REL9 (BSP_IO_PORT_02_PIN_04) /* Relay 10 */
#define BSP_PIN_REL8 (BSP_IO_PORT_02_PIN_05) /* Relay 9 */
#define BSP_PIN_REL7 (BSP_IO_PORT_02_PIN_06) /* Relay 8 */
#define BSP_PIN_REL14 (BSP_IO_PORT_02_PIN_07) /* Relay 15 */
#define BSP_PIN_REL15 (BSP_IO_PORT_02_PIN_08) /* Relay 16 */
#define BSP_PIN_REL12 (BSP_IO_PORT_03_PIN_01) /* Relay 13 */
#define BSP_PIN_REL13 (BSP_IO_PORT_03_PIN_02) /* Relay 14 */
#define BSP_PIN_REL11 (BSP_IO_PORT_03_PIN_03) /* Relay 12 */
#define BSP_PIN_REL10 (BSP_IO_PORT_03_PIN_04) /* Relay 11 */
#define BSP_PIN_RUN (BSP_IO_PORT_04_PIN_00) /* Heartbit led */
#define BSP_PIN_RX2 (BSP_IO_PORT_04_PIN_10) /* RS232 receive line */
#define BSP_PIN_P_IN7 (BSP_IO_PORT_05_PIN_00) /* Input 7 */
#define BSP_PIN_P_IN8 (BSP_IO_PORT_05_PIN_01) /* Input 8 */
#define BSP_PIN_P_IN12 (BSP_IO_PORT_05_PIN_02) /* Input 12 */
extern const ioport_cfg_t g_bsp_pin_cfg; /* R7FA2L1AB3CFM.pincfg */

void BSP_PinConfigSecurityInit();

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER
#endif /* BSP_PIN_CFG_H_ */
