#include <stdio.h>
#include "pico/stdlib.h"
#include "adc_dma.h"
#include "gpio_init.h"
#include "pwm_trigger.h"
#include "reader.pio.h"

#define ACQ_FREQ 1000000.0f

extern volatile bool buffer1_ready;
extern volatile bool buffer2_ready;
extern uint16_t buffer1[];
extern uint16_t buffer2[];

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("PWM ADC acquisition\n");

    setup_pwm(ACQ_FREQ);
    sleep_ms(100);
    init_pio_and_gpio();
    setup_dma();

    uint64_t dma_start_time = time_us_64();

    while (true) {
        if (buffer1_ready) {
            buffer1_ready = false;
            uint64_t now = time_us_64();
            printf("Buffer 1 filled in %llu us\n", now - dma_start_time);
            dma_start_time = now;
        }

        if (buffer2_ready) {
            buffer2_ready = false;
            uint64_t now = time_us_64();
            printf("Buffer 2 filled in %llu us\n", now - dma_start_time);
            dma_start_time = now;
        }
    }
}
