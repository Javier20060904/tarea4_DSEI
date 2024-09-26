#include "aplication.h"
#include "BSP.h"

#if RTOS
    TaskHandle_t adcHandle = NULL;
    TaskHandle_t systemHandle = NULL;
    TaskHandle_t buttonHandle = NULL;
    bool buttonState = true;
#elif !RTOS
#endif


static const char *TAG = "example";

bool systemState = true;

int8_t buttonPressed = 0;
int32_t startMilis;


#if !RTOS
    static void IRAM_ATTR systemInterrupt(void* arg){
        systemState = !systemState;
    }
#endif

void periphInit(void){
    ADC_Init(ADC_CHANNEL);
    GPIO_Set(LED_PIN, GPIO_MODE_INPUT_OUTPUT_OD);
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
    ESP_LOGI(TAG, "INICIA SISTEMA");
}

#if !RTOS
    void systemBehavior(void){
        int32_t currentMillis = (int32_t) esp_timer_get_time()/1000;
        if(currentMillis - startMilis >= 1000){
            ESP_LOGI(TAG, "ESTADO DEL SISTEMA: %s", systemState ? "ENCENDIDO" : "APAGADO");
            if(!systemState)
                ESP_LOGI(TAG, "NO DISPONIBLE");
            else
                ESP_LOGI(TAG, "LECTURA DEL ADC: %d V", VOLTAGE_READ(ADC_CHANNEL));
            startMilis = currentMillis;
        }
        GPIO_Write(LED_PIN, systemState ? LED_ON : LED_OFF);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

#elif RTOS
    void vADC(void *arg){
        while(1){
            if(!systemState){
                ESP_LOGI(TAG, "NO DISPONIBLE");
            }
            else{
                ESP_LOGI(TAG, "LECTURA DEL ADC: %d V", VOLTAGE_READ(ADC_CHANNEL));
            }
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }

    void vSystem(void *arg){
        while (1)
        {        
            GPIO_Write(LED_PIN, systemState);

            ESP_LOGI(TAG, "ESTADO DEL SISTEMA: %s", systemState ? "ENCENDIDO" : "APAGADO");
            
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }    
    }

    void vButton(void *arg){
        while ((1))
        {
            int B = GPIO_Read(BUTTON_PIN);
            if (B != buttonState) {
                ESP_LOGI(TAG, "Boton");
                if(B == LOW){
                    vTaskDelay(50 / portTICK_PERIOD_MS);
                    if(buttonPressed == 0)
                        startMilis = (int32_t) esp_timer_get_time()/1000;
                    buttonPressed = buttonPressed * !(buttonPressed == 2);
                    buttonPressed++;
                }
                buttonState = B;
            }
        
            int32_t currentMillis = (int32_t) esp_timer_get_time()/1000;;

            if(currentMillis - startMilis >= 5000 && buttonPressed != 0){
                systemState = systemState && (buttonPressed == 1);
                buttonPressed = 0;
            }

            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
#endif