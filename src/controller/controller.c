#include "controller.h"
#include "model/model.h"
#include "adapters/modbus_server.h"
#include "observer.h"
#include "bsp/input.h"
#include "bsp/adc.h"


void controller_init(mut_model_t *model) {
    model_init(model);

    observer_init(model);

    modbus_server_init();
}


void controller_manage(mut_model_t *model) {
    {
        static timestamp_t ts = 0;
        if (timestamp_is_expired(ts, 500)) {
            model->run.sensors.inputs = bsp_input_map();
            model->run.sensors.v0_10  = bsp_adc_get(BSP_ADC_FEEDBACK_0_10V);
            model->run.sensors.ma4_20 = bsp_adc_get(BSP_ADC_4_20_MA);

            ts = timestamp_get();
        }
    }

    observer_manage(model);
    modbus_server_manage(model);
}
