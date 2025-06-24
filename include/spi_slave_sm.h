#pragma once

extern PIO spi_pio;
extern uint sm_spi;

void setup_spi_sm(void);
void clean_and_start_spi_sm(void);
void stop_and_clear_spi_sm(void);