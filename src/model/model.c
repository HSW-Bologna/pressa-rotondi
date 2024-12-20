#include <string.h>
#include "model.h"


void model_init(mut_model_t *model) {
    memset(model, 0, sizeof(mut_model_t));
}


uint8_t model_get_pwm_percentage(model_t *model) {
    if (model->run.test.on) {
        return model->run.test.pwm;
    } else {
        return 0;
    }
}


uint16_t model_get_relay_map(model_t *model) {
    if (model->run.test.on) {
        return model->run.test.outputs;
    } else {
        return 0;
    }
}
