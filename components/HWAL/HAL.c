#include "HAL.h"

adc_oneshot_unit_handle_t adc_handle;

void GPIO_Set_Interrupt(gpio_num_t puerto, gpio_isr_t function){
    GPIO_Set(puerto, GPIO_MODE_INPUT);
    GPIO_PullMode(puerto, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(puerto, GPIO_INTR_POSEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(puerto, function, NULL);
    gpio_intr_enable(puerto);
}

void ADC_Init(adc_channel_t channel){
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    adc_oneshot_new_unit(&init_config, &adc_handle);

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };
    adc_oneshot_config_channel(adc_handle, channel, &config);
}

int ADC_Read(adc_channel_t channel){
    int adc_reading;
    adc_oneshot_read(adc_handle, channel, &adc_reading);
    return adc_reading;
}
