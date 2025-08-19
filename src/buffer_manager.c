/* This file contains all the logic to fill, send and clear the buffers. They provide high level functions 
that can be called by the other components of the system to obtain an adress were the data can be written or sent from. */

#include "buffer_manager.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include "gpio_init.h"
#include "spi_dma.h"
#include "pico/stdio.h"

BufferManager buffer_mgr;

uint16_t buffer1[BUFFER_SIZE];
uint16_t buffer2[BUFFER_SIZE];

void buffer_manager_init(uint16_t* buf0, uint16_t* buf1) {
    buffer_mgr.buffers[0] = buf0;
    buffer_mgr.buffers[1] = buf1;
    buffer_mgr.full[0] = false;
    buffer_mgr.full[1] = false;
}

uint16_t* buffer_manager_get_free_buffer(void) {
    if (!buffer_mgr.full[0]) {
        return buffer_mgr.buffers[0];
    }
    if (!buffer_mgr.full[1]) {
        return buffer_mgr.buffers[1];
    }
    return NULL;
}

uint16_t* buffer_manager_get_full_buffer(void) {
    if (buffer_mgr.full[0]) {
        return buffer_mgr.buffers[0];
    }
    if (buffer_mgr.full[1]) {
        return buffer_mgr.buffers[1];
    }
    return NULL;
}

void buffer_manager_mark_full(uint16_t* buffer_ptr) {

    for (int i = 0; i < BUFFER_SIZE; ++i) { // Dummy data for test purpose
        buffer_ptr[i] = 0xAAAC;
    }

    if (buffer_ptr == buffer_mgr.buffers[0]) {
        buffer_mgr.full[0] = true;
    } else if (buffer_ptr == buffer_mgr.buffers[1]) {
        buffer_mgr.full[1] = true;
    } else {
        return;
    }

    gpio_put(READY_PIN, 1);
    trigger_spi_dma(buffer_ptr);
}

void buffer_manager_mark_sent(uint16_t* buffer_ptr) {
    if (buffer_ptr == buffer_mgr.buffers[0]) {
        buffer_mgr.full[0] = false;
    } else if (buffer_ptr == buffer_mgr.buffers[1]) {
        buffer_mgr.full[1] = false;
    } else {
        return;
    }

    if (!buffer_mgr.full[0] && !buffer_mgr.full[1]) {
        gpio_put(READY_PIN, 0);
    } 
}
