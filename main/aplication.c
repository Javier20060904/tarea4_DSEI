#include "aplication.h"
#include "BSP.h"
#include "HAL.h"

#if RTOS
    TaskHandle_t adcHandle = NULL;
    TaskHandle_t systemHandle = NULL;
    TaskHandle_t buttonHandle = NULL;
    bool buttonState = true;
#elif !RTOS
#endif

static char message[50];

bool systemState = true, adcStatus[3];

#if !RTOS
    static void IRAM_ATTR systemInterrupt(void* arg){
        systemState = !systemState;
    }
#endif

void periphInit(void){
    ADC_Init();


    GPIO_Set(LED_PIN, GPIO_MODE_OUTPUT);
    UART_Init();
    #if RTOS
        GPIO_Set(BUTTON_PIN, GPIO_MODE_INPUT);
        GPIO_PullMode(BUTTON_PIN, GPIO_PULLUP_ONLY);
    #elif !RTOS     
        GPIO_Set_Interrupt(BUTTON_PIN, systemInterrupt);
    #endif
}

void systemInit(void){
    systemState = true;
    GPIO_Write(LED_PIN, systemState);
    #if RTOS
        if(adcHandle != NULL)
            vTaskResume(adcHandle);
    #elif !RTOS
    #endif
    //ESP_LOGI(TAG, "INICIA SISTEMA");
    UART_Write("INICIA SISTEMA\n");
}

#if !RTOS
    void systemBehavior(void){
        int32_t currentMillis = (int32_t) esp_timer_get_time()/1000;
        if(currentMillis - startMilis >= 1000){
            //ESP_LOGI(TAG, "ESTADO DEL SISTEMA: %s", systemState ? "ENCENDIDO" : "APAGADO");
            sprintf(message, "ESTADO DEL SISTEMA: %s\n", systemState ? "ENCENDIDO" : "APAGADO");
            if(!systemState)
                //ESP_LOGI(TAG, "NO DISPONIBLE");
                UART_Write("NO DISPONIBLE\n");
            else{
                //ESP_LOGI(TAG, "LECTURA DEL ADC: %d V", VOLTAGE_READ(ADC_CHANNEL));
                sprintf(message, "LECTURA DEL ADC: %d V\n", VOLTAGE_READ(ADC_CHANNEL));
                UART_Write(message);
            }
            startMilis = currentMillis;
        }
        GPIO_Write(LED_PIN, systemState ? LED_ON : LED_OFF);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

#elif RTOS
    void vADC(void *arg){
        static int adcRawRead[3];
        static float adcRead[3];
        while(1){
            ADC_Read(ADC_CHANNEL_6, &adcRawRead[0]);
            ADC_Read(ADC_CHANNEL_5, &adcRawRead[1]);
            ADC_Read(ADC_CHANNEL_4, &adcRawRead[2]);


            for(uint8_t i = 0; i < 3; i++){
                adcRead[i] = ((float)adcRawRead[i]) / 1000;
                adcStatus[i] = (adcRead[i] > 1);
                sprintf(message,"ADC %d: %.1f \n", i, adcRead[i]);
                UART_Write(message);
                sprintf(message,"ADC %d: %s \n", i, adcStatus[i] ? "ENCENDIDO" : "APAGADO");
                UART_Write(message);
                sprintf(message,"\n");
                UART_Write(message);
            }
            vTaskDelay(750 / portTICK_PERIOD_MS);
        }
    }

    void vSystem(void *arg){
        while (1)
        {        
            GPIO_Write(LED_PIN, systemState);
            //ESP_LOGI(TAG, "ESTADO DEL SISTEMA: %s", systemState ? "ENCENDIDO" : "APAGADO");
            //sprintf(message, "ESTADO DEL SISTEMA: %s\n", systemState ? "ENCENDIDO" : "APAGADO");
            //UART_Write(message);
            
            vTaskDelay(750 / portTICK_PERIOD_MS);
        }    
    }

    void vButton(void *arg){
        static int8_t buttonPressed = 0;
        static int32_t startMilis;
        while ((1))
        {
            int B = GPIO_Read(BUTTON_PIN);
            if (B != buttonState) {
                if(B == LOW){
                    if(buttonPressed == 0)
                        startMilis = (int32_t) esp_timer_get_time()/1000;
                    buttonPressed = buttonPressed * !(buttonPressed == 2);
                    buttonPressed++;  
                }
                buttonState = B;
            }

            int32_t currentMillis = (int32_t) esp_timer_get_time()/1000;;

            if(currentMillis - startMilis >= 1000 && buttonPressed != 0){
                systemState = (buttonPressed == 1);
                buttonPressed = 0;
            }

            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
#endif