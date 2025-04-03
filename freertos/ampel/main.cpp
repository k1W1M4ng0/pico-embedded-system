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

void halt(void* params) {
    while (true) {
        printf("halt\n");
        vTaskPrioritySet(NULL, 3);

        redPin.set_high();
        yellowPin.set_low();
        greenPin.set_low();

        nonBlockingDelay(WAIT);

        vTaskPrioritySet(redYellowHandle, 2);
        vTaskPrioritySet(NULL, 1);
    }
}

void redYellow(void* params) {
    while (true) {
        printf("halt 2\n");
        vTaskPrioritySet(NULL, 3);

        redPin.set_high();
        yellowPin.set_high();
        greenPin.set_low();

        nonBlockingDelay(WAIT);
        vTaskPrioritySet(goHandle, 2);
        vTaskPrioritySet(NULL, 1);
    }
}

void go(void* params) {
    while (true) {
        printf("go\n");
        vTaskPrioritySet(NULL, 3);

        redPin.set_low();
        yellowPin.set_low();
        greenPin.set_high();

        nonBlockingDelay(WAIT);
        vTaskPrioritySet(goBlinkHandle, 2);
        vTaskPrioritySet(NULL, 1);
    }
}

void goBlink(void* params) {
    while (true) {
        printf("blink go\n");
        vTaskPrioritySet(NULL, 3);

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
        vTaskPrioritySet(yellowHandle, 2);
        vTaskPrioritySet(NULL, 1);

    }
}

void onlyYellow(void* params) {
    while (true) {
        printf("halt again\n");
        vTaskPrioritySet(NULL, 3);

        redPin.set_low();
        yellowPin.set_high();
        greenPin.set_low();

        nonBlockingDelay(WAIT);

        vTaskPrioritySet(haltHandle, 2);
        vTaskPrioritySet(NULL, 1);
    }
}


int main() {
    stdio_init_all();

    xTaskCreate(halt,     // function
                "halt",   /* Text name for the task. */
                1024,     /* Stack size in words, not bytes. */
                (void*)1, /* Parameter passed into the task. */
                2,        /* Priority at which the task is created. */
                &haltHandle);

    xTaskCreate(redYellow,     // function
                "redYellow",   /* Text name for the task. */
                1024,     /* Stack size in words, not bytes. */
                (void*)1, /* Parameter passed into the task. */
                1,        /* Priority at which the task is created. */
                &redYellowHandle);

    xTaskCreate(go,     // function
                "go",   /* Text name for the task. */
                1024,     /* Stack size in words, not bytes. */
                (void*)1, /* Parameter passed into the task. */
                1,        /* Priority at which the task is created. */
                &goHandle);

    xTaskCreate(goBlink,     // function
                "goBlink",   /* Text name for the task. */
                1024,     /* Stack size in words, not bytes. */
                (void*)1, /* Parameter passed into the task. */
                1,        /* Priority at which the task is created. */
                &goBlinkHandle);

    xTaskCreate(onlyYellow,     // function
                "yellow",   /* Text name for the task. */
                1024,     /* Stack size in words, not bytes. */
                (void*)1, /* Parameter passed into the task. */
                1,        /* Priority at which the task is created. */
                &yellowHandle);

    // xTaskCreate(danger,     // function
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
