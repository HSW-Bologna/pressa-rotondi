#include <string.h>
#include "services/timestamp.h"
#include "model.h"


void model_init(mut_model_t *model) {
    memset(model, 0, sizeof(mut_model_t));
}


uint8_t model_get_pwm_percentage(model_t *model) {
    if (model->run.test.on) {
        return model->run.test.pwm;
        // The program is running
    } else if (model->run.state.running) {
        if (model_is_program_done(model)) {
            return 0;
        } else {
            uint16_t elapsed_ms      = model_get_program_elapsed_milliseconds(model);
            uint16_t time_unit_index = elapsed_ms / NUM_TIME_UNITS;
            uint8_t  dac_state       = model->run.program.dac_channel_states[time_unit_index];

            if (dac_state > 0) {
                return model->run.program.dac_levels[dac_state];
            } else {
                return 0;
            }
        }
    }
    // Everything off
    else {
        return 0;
    }
}



uint16_t model_get_program_duration_milliseconds(model_t *model) {
    return model->run.program.time_unit_milliseconds * NUM_TIME_UNITS;
}


uint8_t model_is_program_done(model_t *model) {
    if (model->run.state.running) {
        uint16_t duration = model_get_program_duration_milliseconds(model);
        return timestamp_is_expired(model->run.state.start_ts, duration);
    } else {
        return 0;
    }
}


uint16_t model_get_program_elapsed_milliseconds(model_t *model) {
    if (model_is_program_done(model)) {
        return model_get_program_duration_milliseconds(model);
    } else {
        return timestamp_interval(model->run.state.start_ts, timestamp_get());
    }
}


uint16_t model_get_relay_map(model_t *model) {
    // Test mode
    if (model->run.test.on) {
        return model->run.test.outputs;
    }
    // The program is running
    else if (model->run.state.running) {
        if (model_is_program_done(model)) {
            return 0;
        } else {
            uint16_t elapsed_ms      = model_get_program_elapsed_milliseconds(model);
            uint16_t time_unit_index = elapsed_ms / NUM_TIME_UNITS;
            uint16_t result          = 0;

            for (uint16_t i = 0; i < NUM_DIGITAL_CHANNELS; i++) {
                if (model->run.program.digital_channel_schedules[i] & (1 << time_unit_index)) {
                    result |= (1 << i);
                }
            }

            return result;
        }
    }
    // Everything off
    else {
        return 0;
    }
}


void model_start_program(mut_model_t *model) {
    if (model_get_program_duration_milliseconds(model) != 0) {
        model->run.state.running  = 1;
        model->run.state.start_ts = timestamp_get();
    }
}
