#ifndef PIO_READER_SM_H
#define PIO_READER_SM_H

#include "hardware/pio.h"

extern PIO pio;
extern uint sm_reader;

void setup_reader_sm(void);
void clean_and_start_reader_sm(void);
void stop_and_clear_reader_sm(void);

#endif
