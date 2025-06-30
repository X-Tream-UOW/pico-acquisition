#include "acquisition_control.h"

#include "buffer_manager.h"
#include "reader_dma.h"
#include "spi_dma.h"
#include "pio_reader_sm.h"
#include "spi_slave_sm.h"
#include "dma_irq_mux.h"

#include "hardware/dma.h"

extern uint16_t buffer1[BUFFER_SIZE];
extern uint16_t buffer2[BUFFER_SIZE];

void start_acquisition(void) {
    buffer_manager_init(buffer1, buffer2);

    setup_dma_irq_handler();

    setup_reader_dma();
    setup_spi_dma();

    dma_channel_start(spi_dma_chan);
    dma_channel_start(reader_dma_chan);

    clean_and_start_reader_sm();
    clean_and_start_spi_sm();
}

void stop_acquisition(void) {
    stop_spi_dma();
    stop_reader_dma();

    stop_and_clear_reader_sm();
    stop_and_clear_spi_sm();
}
