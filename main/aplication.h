#ifndef APLICATION_H
#define APLICATION_H

#include "HAL.h"

#if RTOS
    extern TaskHandle_t adcHandle;
    extern TaskHandle_t systemHandle;
    extern TaskHandle_t buttonHandle;
#endif

void systemInit();
void periphInit(void);

#if !RTOS
    void systemBehavior();
#elif RTOS
    void vADC(void *arg);
    void vSystem(void *arg);
    void vButton(void *arg);
#endif

#endif