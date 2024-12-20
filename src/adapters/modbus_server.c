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
};


enum {
    MODBUS_HR_TEST_MODE = 0,
    MODBUS_HR_TEST_OUTPUTS,
    MODBUS_HR_TEST_PWM,
};


static ModbusError register_callback(const ModbusSlave *minion, const ModbusRegisterCallbackArgs *args,
                                     ModbusRegisterCallbackResult *result);
static ModbusError static_allocator(ModbusBuffer *buffer, uint16_t size, void *context);
static ModbusError exception_callback(const ModbusSlave *minion, uint8_t function, ModbusExceptionCode code);

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
                        case MODBUS_HR_TEST_MODE:
                        case MODBUS_HR_TEST_OUTPUTS:
                        case MODBUS_HR_TEST_PWM:
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
