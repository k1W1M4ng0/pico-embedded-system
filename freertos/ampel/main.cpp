#include "FreeRTOS.h"
#include "GPIO.hpp"
#include "pico/stdlib.h"
#include "task.h"
#include <array>
#include <stdio.h>

#define nonBlockingDelay(x)                                                    \
    \ 
    TickType_t currentTick = xTaskGetTickCount();                              \
    while (xTaskGetTickCount() - currentTick < x)

#define WAIT 4000

// debug led
static pico_cpp::GPIO_Pin ledPin(25, pico_cpp::PinType::Output);

// pins for traffic light
static pico_cpp::GPIO_Pin redPin(6, pico_cpp::PinType::Output);
static pico_cpp::GPIO_Pin yellowPin(7, pico_cpp::PinType::Output);
static pico_cpp::GPIO_Pin greenPin(8, pico_cpp::PinType::Output);

TaskHandle_t xTaskOffHandle = NULL;
TaskHandle_t xTaskOnHandle = NULL;

TaskHandle_t haltHandle = NULL;
TaskHandle_t redYellowHandle = NULL;
TaskHandle_t goHandle = NULL;
TaskHandle_t goBlinkHandle = NULL;
TaskHandle_t yellowHandle = NULL;
TaskHandle_t dangerHandle = NULL;

#define RUNNING 3
#define READY 2
#define WAITING 1

void halt(void* params) {
    while (true) {
        printf("halt\n");
        vTaskPrioritySet(NULL, RUNNING);

        redPin.set_high();
        yellowPin.set_low();
        greenPin.set_low();

        nonBlockingDelay(WAIT);

        vTaskPrioritySet(redYellowHandle, READY);
        vTaskPrioritySet(NULL, WAITING);
    }
}

void redYellow(void* params) {
    while (true) {
        printf("halt 2\n");
        vTaskPrioritySet(NULL, RUNNING);

        redPin.set_high();
        yellowPin.set_high();
        greenPin.set_low();

        nonBlockingDelay(WAIT);
        vTaskPrioritySet(goHandle, READY);
        vTaskPrioritySet(NULL, WAITING);
    }
}

void go(void* params) {
    while (true) {
        printf("go\n");
        vTaskPrioritySet(NULL, RUNNING);

        redPin.set_low();
        yellowPin.set_low();
        greenPin.set_high();

        nonBlockingDelay(WAIT);
        vTaskPrioritySet(goBlinkHandle, READY);
        vTaskPrioritySet(NULL, WAITING);
    }
}

void goBlink(void* params) {
    while (true) {
        printf("blink go\n");
        vTaskPrioritySet(NULL, RUNNING);

        redPin.set_low();
        yellowPin.set_low();

        for (int i{}; i < 4; ++i) {
            greenPin.set_low();
            {
                nonBlockingDelay(WAIT / 8);
            }
            greenPin.set_high();
            {

                nonBlockingDelay(WAIT / 8);
            }
        }

        // switch
        vTaskPrioritySet(yellowHandle, READY);
        vTaskPrioritySet(NULL, WAITING);
    }
}

void onlyYellow(void* params) {
    while (true) {
        printf("halt again\n");
        vTaskPrioritySet(NULL, RUNNING);

        redPin.set_low();
        yellowPin.set_high();
        greenPin.set_low();

        nonBlockingDelay(WAIT);

        vTaskPrioritySet(haltHandle, READY);
        vTaskPrioritySet(NULL, WAITING);
    }
}

int main() {
    stdio_init_all();

    xTaskCreate(halt, "halt", 1024, (void*)1, READY, &haltHandle);

    xTaskCreate(redYellow, "redYellow", 1024, (void*)1, WAITING,
                &redYellowHandle);

    xTaskCreate(go, "go", 1024, (void*)1, WAITING, &goHandle);

    xTaskCreate(goBlink, "goBlink", 1024, (void*)1, WAITING, &goBlinkHandle);

    xTaskCreate(onlyYellow, "yellow", 1024, (void*)1, WAITING, &yellowHandle);

    // xTaskCreate(danger,
    //             "danger",   /* Text name for the task. */
    //             1024,     /* Stack size in words, not bytes. */
    //             (void*)1, /* Parameter passed into the task. */
    //             2,        /* Priority at which the task is created. */
    //             &dangerHandle);

    vTaskStartScheduler();

    while (1) {

        printf("danger!\n");

        redPin.set_low();
        greenPin.set_low();

        yellowPin.set_low();
        {
            nonBlockingDelay(WAIT / 8);
        }
        yellowPin.set_high();
        {
            nonBlockingDelay(WAIT / 8);
        }
        // configASSERT(0); /* We should never get here */
    }
}
