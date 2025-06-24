#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"

#include "gpio_init.h"
#include "pwm_trigger.h"
#include "gpio_ack.h"

#include "pio_reader_sm.h"
#include "spi_slave_sm.h"

#include "reader.pio.h"
#include "spi_slave.pio.h"

#include "buffer_manager.h"
#include "reader_dma.h"
#include "spi_dma.h"
#include "dma_irq_mux.h"

#define ACQ_FREQ 1000000.0f

extern BufferManager buffer_mgr;

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("Connected\n");

    init_gpios();
    setup_ack_gpio();
    setup_pwm(ACQ_FREQ);

    sleep_ms(100);

    buffer_manager_init(buffer1, buffer2);

    setup_dma_irq_handler();

    setup_reader_sm();
    setup_spi_sm();

    setup_reader_dma();
    setup_spi_dma();
   
    dma_channel_start(spi_dma_chan);
    dma_channel_start(reader_dma_chan);

    while (true) {
        tight_loop_contents();
    }
}
