#include <stdio.h>
#include "pico/stdlib.h"
#include "adc_dma.h"
#include "pio_reader_sm.h"
#include "pwm_trigger.h"
#include "gpio_init.h"
#include "reader.pio.h"
#include "spi_slave.h"
#include "spi_dma.h"

#define ACQ_FREQ 50000.0f

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("PWM ADC acquisition\n");

    init_gpio();
    setup_pwm(ACQ_FREQ);

    setup_reader_sm();
    setup_reader_dma();

    setup_spi_sm();
    setup_sp_dma();

    while (true) {

        if (reader_dma_paused) {
            printf("DMA paused — waiting for buffer release\n");
            sleep_ms(1000);
        }

        if (buffer1_ready && gpio_get(ACK_PIN)) { // The Pi just polled out the buffer 1
            buffer1_ready = false;  // Mark buffer as available for filling
            clear_signals();  // Reset READY and ACK
            printf("Buffer 1 consumed\n");
            resume_reader_dma();
        }

        if (buffer2_ready && gpio_get(ACK_PIN)) {
            buffer2_ready = false;
            clear_signals();
            printf("Buffer 2 consumed\n");
            resume_reader_dma();
        }
    }
}
