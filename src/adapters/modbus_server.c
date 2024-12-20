#define LIGHTMODBUS_IMPL
#define LIGHTMODBUS_SLAVE
#define LIGHTMODBUS_F03S
#define LIGHTMODBUS_F04S
#define LIGHTMODBUS_F16S
#include <lightmodbus/lightmodbus.h>

#include "services/timestamp.h"
#include "config/app_config.h"
#include "modbus_server.h"
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "bsp/rs232.h"


enum {
    MODBUS_IR_FIRMWARE_VERSION = 0,
    MODBUS_IR_INPUTS,
    MODBUS_IR_0_10V_ADC,
    MODBUS_IR_0_10V,
    MODBUS_IR_4_20MA_ADC,
    MODBUS_IR_4_20MA,
    MODBUS_IR_PROGRAM_ELAPSED_TIME_MILLISECONDS,
};


enum {
    MODBUS_HR_TEST_MODE = 0,
    MODBUS_HR_TEST_OUTPUTS,
    MODBUS_HR_TEST_PWM,
    MODBUS_HR_HEADGAP_OFFSET_UP,
    MODBUS_HR_HEADGAP_OFFSET_DOWN,
    MODBUS_HR_TIME_UNIT_DECISECONDS,
    MODBUS_HR_DAC_CHANNEL_LEVEL_1,
    MODBUS_HR_DAC_CHANNEL_LEVEL_2,
    MODBUS_HR_DAC_CHANNEL_LEVEL_3,
    MODBUS_HR_SENSOR_CHANNEL_LEVEL_1,
    MODBUS_HR_SENSOR_CHANNEL_LEVEL_2,
    MODBUS_HR_SENSOR_CHANNEL_LEVEL_3,
    MODBUS_HR_DIGITAL_CHANNEL_1_UNIT_1_to_16,
    MODBUS_HR_DIGITAL_CHANNEL_1_UNIT_17_to_25,
    MODBUS_HR_DIGITAL_CHANNEL_2_UNIT_1_to_16,
    MODBUS_HR_DIGITAL_CHANNEL_2_UNIT_17_to_25,
    MODBUS_HR_DIGITAL_CHANNEL_3_UNIT_1_to_16,
    MODBUS_HR_DIGITAL_CHANNEL_3_UNIT_17_to_25,
    MODBUS_HR_DIGITAL_CHANNEL_4_UNIT_1_to_16,
    MODBUS_HR_DIGITAL_CHANNEL_4_UNIT_17_to_25,
    MODBUS_HR_DIGITAL_CHANNEL_5_UNIT_1_to_16,
    MODBUS_HR_DIGITAL_CHANNEL_5_UNIT_17_to_25,
    MODBUS_HR_DIGITAL_CHANNEL_6_UNIT_1_to_16,
    MODBUS_HR_DIGITAL_CHANNEL_6_UNIT_17_to_25,
    MODBUS_HR_DIGITAL_CHANNEL_7_UNIT_1_to_16,
    MODBUS_HR_DIGITAL_CHANNEL_7_UNIT_17_to_25,
    MODBUS_HR_DIGITAL_CHANNEL_8_UNIT_1_to_16,
    MODBUS_HR_DIGITAL_CHANNEL_8_UNIT_17_to_25,
    MODBUS_HR_DIGITAL_CHANNEL_9_UNIT_1_to_16,
    MODBUS_HR_DIGITAL_CHANNEL_9_UNIT_17_to_25,
    MODBUS_HR_DIGITAL_CHANNEL_10_UNIT_1_to_16,
    MODBUS_HR_DIGITAL_CHANNEL_10_UNIT_17_to_25,
    MODBUS_HR_DIGITAL_CHANNEL_11_UNIT_1_to_16,
    MODBUS_HR_DIGITAL_CHANNEL_11_UNIT_17_to_25,
    MODBUS_HR_DIGITAL_CHANNEL_12_UNIT_1_to_16,
    MODBUS_HR_DIGITAL_CHANNEL_12_UNIT_17_to_25,
    MODBUS_HR_DIGITAL_CHANNEL_13_UNIT_1_to_16,
    MODBUS_HR_DIGITAL_CHANNEL_13_UNIT_17_to_25,
    MODBUS_HR_DIGITAL_CHANNEL_14_UNIT_1_to_16,
    MODBUS_HR_DIGITAL_CHANNEL_14_UNIT_17_to_25,
    MODBUS_HR_DAC_CHANNEL_UNIT_1_2_3_4,
    MODBUS_HR_DAC_CHANNEL_UNIT_5_6_7_8,
    MODBUS_HR_DAC_CHANNEL_UNIT_9_10_11_12,
    MODBUS_HR_DAC_CHANNEL_UNIT_13_14_15_16,
    MODBUS_HR_DAC_CHANNEL_UNIT_17_18_19_20,
    MODBUS_HR_DAC_CHANNEL_UNIT_21_22_23_24,
    MODBUS_HR_DAC_CHANNEL_UNIT_25,
    MODBUS_HR_SENSOR_CHANNEL_UNIT_1_2_3_4,
    MODBUS_HR_SENSOR_CHANNEL_UNIT_5_6_7_8,
    MODBUS_HR_SENSOR_CHANNEL_UNIT_9_10_11_12,
    MODBUS_HR_SENSOR_CHANNEL_UNIT_13_14_15_16,
    MODBUS_HR_SENSOR_CHANNEL_UNIT_17_18_19_20,
    MODBUS_HR_SENSOR_CHANNEL_UNIT_21_22_23_24,
    MODBUS_HR_SENSOR_CHANNEL_UNIT_25,
};


static ModbusError register_callback(const ModbusSlave *minion, const ModbusRegisterCallbackArgs *args,
                                     ModbusRegisterCallbackResult *result);
static ModbusError static_allocator(ModbusBuffer *buffer, uint16_t size, void *context);
static ModbusError exception_callback(const ModbusSlave *minion, uint8_t function, ModbusExceptionCode code);
static uint16_t    get_digital_channel_chunk(model_t *model, uint16_t chunk_index);
static void        set_digital_channel_chunk(model_t *model, uint16_t chunk_index, uint16_t chunk);
static void        set_analog_channel_chunk(uint8_t *analog_channel, uint16_t chunk_index, uint16_t chunk);
static uint16_t    get_analog_channel_chunk(const uint8_t *analog_channel, uint16_t chunk_index);


static ModbusSlave modbus_minion;


void modbus_server_init(void) {
    // Read the data from stdin
    ModbusErrorInfo err = modbusSlaveInit(&modbus_minion, register_callback, exception_callback, static_allocator,
                                          modbusSlaveDefaultFunctions, modbusSlaveDefaultFunctionCount);

    // Check for errors
    assert(modbusIsOk(err) && "modbusSlaveInit() failed!");
}

void modbus_server_manage(mut_model_t *model) {
    uint8_t request[BSP_RS232_MAX_PACKET_LEN] = {0};
    size_t  request_length                    = bsp_rs232_read(request, sizeof(request));

    if (request_length > 0 && bsp_rs232_timed_out(10)) {
        modbusSlaveSetUserPointer(&modbus_minion, model);
        ModbusErrorInfo err = modbusParseRequestRTU(&modbus_minion, 1, request, (uint8_t)request_length);

        if (modbusIsOk(err)) {
            const uint8_t *response        = modbusSlaveGetResponse(&modbus_minion);
            uint16_t       response_length = modbusSlaveGetResponseLength(&modbus_minion);
            bsp_rs232_write((uint8_t *)response, response_length);
            bsp_rs232_flush();
        } else {
            bsp_rs232_flush();
        }
    } else {
        // No data
    }
}

static ModbusError static_allocator(ModbusBuffer *buffer, uint16_t size, void *context) {
    (void)context;
#define MAX_RESPONSE 256

    static uint8_t response[MAX_RESPONSE];

    if (size != 0)     // Allocation reqest
    {
        if (size <= MAX_RESPONSE)     // Allocation request is within bounds
        {
            buffer->data = response;
            return MODBUS_OK;
        } else     // Allocation error
        {
            buffer->data = NULL;
            return MODBUS_ERROR_ALLOC;
        }
    } else     // Free request
    {
        buffer->data = NULL;
        return MODBUS_OK;
    }
}

static ModbusError register_callback(const ModbusSlave *minion, const ModbusRegisterCallbackArgs *args,
                                     ModbusRegisterCallbackResult *result) {
    mut_model_t *model = modbusSlaveGetUserPointer(minion);
    (void)model;

    switch (args->query) {
        // Pretend to allow all access
        case MODBUS_REGQ_R_CHECK:
            switch (args->type) {
                case MODBUS_INPUT_REGISTER:
                case MODBUS_HOLDING_REGISTER:
                    result->exceptionCode = MODBUS_EXCEP_NONE;
                    break;

                default:
                    result->exceptionCode = MODBUS_EXCEP_ILLEGAL_FUNCTION;
                    break;
            }
            break;

        case MODBUS_REGQ_W_CHECK:
            switch (args->type) {
                case MODBUS_HOLDING_REGISTER:
                    switch (args->index) {
                        case MODBUS_HR_TEST_MODE ... MODBUS_HR_SENSOR_CHANNEL_UNIT_25:
                            result->exceptionCode = MODBUS_EXCEP_NONE;
                            break;
                        default:
                            break;
                    }
                    break;

                default:
                    result->exceptionCode = MODBUS_EXCEP_ILLEGAL_FUNCTION;
                    break;
            }
            break;

            // Return 7 when reading
        case MODBUS_REGQ_R: {
            switch (args->type) {
                case MODBUS_INPUT_REGISTER: {
                    switch (args->index) {
                        case MODBUS_IR_FIRMWARE_VERSION:
                            result->value = ((APP_CONFIG_FIRMWARE_VERSION_MAJOR & 0x1F) << 11) |
                                            ((APP_CONFIG_FIRMWARE_VERSION_MINOR & 0x1F) << 6) |
                                            (APP_CONFIG_FIRMWARE_VERSION_PATCH & 0x3F);
                            break;

                        case MODBUS_IR_INPUTS:
                            result->value = model->run.sensors.inputs;
                            break;

                        case MODBUS_IR_4_20MA_ADC:
                            result->value = model->run.sensors.ma4_20;
                            break;

                        case MODBUS_IR_0_10V_ADC:
                            result->value = model->run.sensors.v0_10;
                            break;

                        case MODBUS_IR_PROGRAM_ELAPSED_TIME_MILLISECONDS:
                            result->value = model_get_program_elapsed_milliseconds(model);
                            break;

                        default:
                            result->value = 0;
                            break;
                    }
                    break;
                }

                case MODBUS_HOLDING_REGISTER: {
                    switch (args->index) {
                        case MODBUS_HR_TEST_MODE:
                            result->value = model->run.test.on;
                            break;

                        case MODBUS_HR_TEST_OUTPUTS:
                            result->value = model->run.test.outputs;
                            break;

                        case MODBUS_HR_TEST_PWM:
                            result->value = (uint16_t)model->run.test.pwm;
                            break;

                        case MODBUS_HR_HEADGAP_OFFSET_UP:
                            result->value = (uint16_t)model->run.program.headgap_offset_up;
                            break;

                        case MODBUS_HR_HEADGAP_OFFSET_DOWN:
                            result->value = (uint16_t)model->run.program.headgap_offset_down;
                            break;

                        case MODBUS_HR_TIME_UNIT_DECISECONDS:
                            result->value = (uint16_t)model->run.program.time_unit_milliseconds / 100;
                            break;

                        case MODBUS_HR_DAC_CHANNEL_LEVEL_1:
                        case MODBUS_HR_DAC_CHANNEL_LEVEL_2:
                        case MODBUS_HR_DAC_CHANNEL_LEVEL_3:
                            result->value = model->run.program.dac_levels[args->index - MODBUS_HR_DAC_CHANNEL_LEVEL_1];
                            break;

                        case MODBUS_HR_SENSOR_CHANNEL_LEVEL_1:
                        case MODBUS_HR_SENSOR_CHANNEL_LEVEL_2:
                        case MODBUS_HR_SENSOR_CHANNEL_LEVEL_3:
                            result->value =
                                model->run.program.sensor_levels[args->index - MODBUS_HR_SENSOR_CHANNEL_LEVEL_1];
                            break;

                        case MODBUS_HR_DIGITAL_CHANNEL_1_UNIT_1_to_16 ... MODBUS_HR_DIGITAL_CHANNEL_14_UNIT_17_to_25: {
                            uint16_t chunk_index = args->index - MODBUS_HR_DIGITAL_CHANNEL_1_UNIT_1_to_16;
                            result->value        = get_digital_channel_chunk(model, chunk_index);
                            break;
                        }


                        case MODBUS_HR_DAC_CHANNEL_UNIT_1_2_3_4 ... MODBUS_HR_DAC_CHANNEL_UNIT_21_22_23_24: {
                            uint16_t chunk_index = args->index - MODBUS_HR_DAC_CHANNEL_UNIT_1_2_3_4;
                            result->value =
                                get_analog_channel_chunk(model->run.program.dac_channel_states, chunk_index);
                            break;
                        }

                        case MODBUS_HR_SENSOR_CHANNEL_UNIT_1_2_3_4 ... MODBUS_HR_SENSOR_CHANNEL_UNIT_21_22_23_24: {
                            uint16_t chunk_index = args->index - MODBUS_HR_SENSOR_CHANNEL_UNIT_1_2_3_4;
                            result->value =
                                get_analog_channel_chunk(model->run.program.sensor_channel_thresholds, chunk_index);
                            break;
                        }

                        default:
                            result->value = 0;
                            break;
                    }
                    break;
                }

                default:
                    break;
            }
            break;
        }

        case MODBUS_REGQ_W: {
            switch (args->type) {
                case MODBUS_HOLDING_REGISTER: {
                    switch (args->index) {
                        case MODBUS_HR_TEST_MODE:
                            model->run.test.on = args->value > 0;
                            break;

                        case MODBUS_HR_TEST_OUTPUTS:
                            model->run.test.outputs = args->value;
                            break;

                        case MODBUS_HR_TEST_PWM:
                            model->run.test.pwm = (uint8_t)(args->value & 0xFF);
                            break;

                        case MODBUS_HR_HEADGAP_OFFSET_UP:
                            model->run.program.headgap_offset_up = args->value;
                            break;

                        case MODBUS_HR_HEADGAP_OFFSET_DOWN:
                            model->run.program.headgap_offset_down = args->value;
                            break;

                        case MODBUS_HR_TIME_UNIT_DECISECONDS:
                            model->run.program.time_unit_milliseconds = args->value * 100;
                            break;

                        case MODBUS_HR_DAC_CHANNEL_LEVEL_1:
                        case MODBUS_HR_DAC_CHANNEL_LEVEL_2:
                        case MODBUS_HR_DAC_CHANNEL_LEVEL_3:
                            model->run.program.dac_levels[args->index - MODBUS_HR_DAC_CHANNEL_LEVEL_1] = args->value;
                            break;

                        case MODBUS_HR_SENSOR_CHANNEL_LEVEL_1:
                        case MODBUS_HR_SENSOR_CHANNEL_LEVEL_2:
                        case MODBUS_HR_SENSOR_CHANNEL_LEVEL_3:
                            model->run.program.sensor_levels[args->index - MODBUS_HR_SENSOR_CHANNEL_LEVEL_1] =
                                args->value;
                            break;

                        case MODBUS_HR_DIGITAL_CHANNEL_1_UNIT_1_to_16 ... MODBUS_HR_DIGITAL_CHANNEL_14_UNIT_17_to_25: {
                            uint16_t chunk_index = args->index - MODBUS_HR_DIGITAL_CHANNEL_1_UNIT_1_to_16;
                            set_digital_channel_chunk(model, chunk_index, args->value);
                            break;
                        }

                        case MODBUS_HR_DAC_CHANNEL_UNIT_1_2_3_4 ... MODBUS_HR_DAC_CHANNEL_UNIT_21_22_23_24: {
                            uint16_t chunk_index = args->index - MODBUS_HR_DAC_CHANNEL_UNIT_1_2_3_4;
                            set_analog_channel_chunk(model->run.program.dac_channel_states, chunk_index, args->value);
                            break;
                        }

                        case MODBUS_HR_SENSOR_CHANNEL_UNIT_1_2_3_4 ... MODBUS_HR_SENSOR_CHANNEL_UNIT_21_22_23_24: {
                            uint16_t chunk_index = args->index - MODBUS_HR_SENSOR_CHANNEL_UNIT_1_2_3_4;
                            set_analog_channel_chunk(model->run.program.sensor_channel_thresholds, chunk_index,
                                                     args->value);
                            break;
                        }

                        default:
                            break;
                    }
                    break;
                }

                default:
                    break;
            }
            break;
        }

        default:
            break;
    }

    return MODBUS_OK;
}


static ModbusError exception_callback(const ModbusSlave *minion, uint8_t function, ModbusExceptionCode code) {
    (void)minion;
    (void)function;
    (void)code;
    return MODBUS_OK;
}


static void set_analog_channel_chunk(uint8_t *analog_channel, uint16_t chunk_index, uint16_t chunk) {
    uint16_t channel_index = chunk_index * 4;

    analog_channel[chunk_index] = (chunk >> 12) & 0xF;
    analog_channel[chunk_index] = (chunk >> 8) & 0xF;
    analog_channel[chunk_index] = (chunk >> 4) & 0xF;
    analog_channel[chunk_index] = chunk & 0xF;
}


static uint16_t get_analog_channel_chunk(const uint8_t *analog_channel, uint16_t chunk_index) {
    uint16_t channel_index = chunk_index * 4;

    return ((analog_channel[chunk_index] & 0xF) << 12) | ((analog_channel[chunk_index + 1] & 0xF) << 8) |
           ((analog_channel[chunk_index + 2] & 0xF) << 4) | (analog_channel[chunk_index + 3] & 0xF);
}


static uint16_t get_digital_channel_chunk(model_t *model, uint16_t chunk_index) {
    uint16_t ratio         = sizeof(digital_channel_schedule_t) / sizeof(uint16_t);
    uint16_t channel_index = chunk_index / ratio;
    uint16_t shift         = 16 * ((ratio % sizeof(uint16_t)) == 0);

    return (model->run.program.digital_channel_schedules[channel_index] >> shift) & 0xFFFF;
}


static void set_digital_channel_chunk(model_t *model, uint16_t chunk_index, uint16_t chunk) {
    uint16_t ratio         = sizeof(digital_channel_schedule_t) / sizeof(uint16_t);
    uint16_t channel_index = chunk_index / ratio;
    uint16_t shift         = 16 * ((ratio % 2) == 0);

    model->run.program.digital_channel_schedules[channel_index] &= ~((digital_channel_schedule_t)0xFFFF << shift);
    model->run.program.digital_channel_schedules[channel_index] |= chunk << shift;
}
