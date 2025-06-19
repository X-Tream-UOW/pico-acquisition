#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"

#include "gpio_init.h"
#include "pwm_trigger.h"

#include "pio_reader_sm.h"
#include "spi_slave_sm.h"

#include "reader.pio.h"
#include "spi_slave.pio.h"

#include "adc_dma.h"
#include "spi_dma.h"
#include "dma_irq_mux.h"

#define ACQ_FREQ 1000000.0f

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("PWM ADC acquisition\n");

    init_gpio();
    setup_pwm(ACQ_FREQ);

    setup_reader_sm();
    setup_reader_dma();

    setup_spi_sm();
    setup_spi_dma();
    setup_dma_irq_handler();

    dma_channel_start(spi_dma_chan);
    dma_channel_start(reader_dma_chan);

    while (true) {

        if (reader_dma_paused) {
            printf("[Reader] DMA paused — waiting for buffer release\n");
            sleep_ms(100);
        }

        if (buffer1_ready) {
            printf("[Main] Buffer 1 filled and ready to send\n");
            sleep_ms(100);
        }

        if (buffer2_ready) {
            printf("[Main] Buffer 2 filled and ready to send\n");
            sleep_ms(100);
        }

    }
}
