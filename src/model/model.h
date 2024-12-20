#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED


#include <stdint.h>
#include "services/timestamp.h"


#define NUM_TIME_UNITS       25
#define NUM_DIGITAL_CHANNELS 14
#define NUM_ANALOG_LEVELS    3


typedef
#if NUM_TIME_UNITS <= 8
    uint8_t
#elif NUM_TIME_UNITS <= 16
    uint16_t
#elif NUM_TIME_UNITS <= 32
    uint32_t
#elif NUM_TIME_UNITS <= 64
    uint64_t
#else
#error "Too many time units!"
#endif
        digital_channel_schedule_t;


typedef struct {
    struct {
        timestamp_t last_communication_ts;

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

        struct {
            uint16_t                   headgap_offset_up;
            uint16_t                   headgap_offset_down;
            uint16_t                   time_unit_milliseconds;
            uint16_t                   dac_levels[NUM_ANALOG_LEVELS];
            uint16_t                   sensor_levels[NUM_ANALOG_LEVELS];
            digital_channel_schedule_t digital_channel_schedules[NUM_DIGITAL_CHANNELS];
            uint8_t                    dac_channel_states[NUM_TIME_UNITS];
            uint8_t                    sensor_channel_thresholds[NUM_TIME_UNITS];
        } program;

        struct {
            timestamp_t start_ts;
            uint8_t     running;
        } state;
    } run;
} mut_model_t;


typedef const mut_model_t model_t;


void     model_init(mut_model_t *model);
uint16_t model_get_relay_map(model_t *model);
uint8_t  model_get_pwm_percentage(model_t *model);
uint16_t model_get_program_elapsed_milliseconds(model_t *model);
void     model_start_program(mut_model_t *model);
uint8_t  model_is_program_done(model_t *model);


#endif
