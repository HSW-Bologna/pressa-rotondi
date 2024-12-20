#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED


#include <stdint.h>
#include "services/timestamp.h"


typedef struct {
    struct {
        timestamp_t last_communiocation_ts;

        struct {
            uint16_t inputs;
            uint16_t ma4_20;
            uint16_t v0_10;
        } sensors;

        struct {
            uint8_t  on;
            uint16_t outputs;
            uint8_t  pwm;
        } test;
    } run;
} mut_model_t;


typedef const mut_model_t model_t;


void     model_init(mut_model_t *model);
uint16_t model_get_relay_map(model_t *model);
uint8_t  model_get_pwm_percentage(model_t *model);


#endif
