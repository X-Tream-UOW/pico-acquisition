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
    printf("[BUFFER] Initialized buffer manager\n");
}

uint16_t* buffer_manager_get_free_buffer(void) {
    if (!buffer_mgr.full[0]) {
        printf("[BUFFER] Providing buffer 0 for writing\n");
        return buffer_mgr.buffers[0];
    }
    if (!buffer_mgr.full[1]) {
        printf("[BUFFER] Providing buffer 1 for writing\n");
        return buffer_mgr.buffers[1];
    }
    printf("[BUFFER] No free buffer available\n");
    return NULL;
}

uint16_t* buffer_manager_get_full_buffer(void) {
    if (buffer_mgr.full[0]) {
        printf("[BUFFER] Buffer 0 is ready for sending\n");
        return buffer_mgr.buffers[0];
    }
    if (buffer_mgr.full[1]) {
        printf("[BUFFER] Buffer 1 is ready for sending\n");
        return buffer_mgr.buffers[1];
    }
    printf("[BUFFER] No full buffer available\n");
    return NULL;
}

void buffer_manager_mark_full(uint16_t* buffer_ptr) {
    if (buffer_ptr == buffer_mgr.buffers[0]) {
        buffer_mgr.full[0] = true;
        printf("[BUFFER] Buffer 0 marked full → READY=1\n");
    } else if (buffer_ptr == buffer_mgr.buffers[1]) {
        buffer_mgr.full[1] = true;
        printf("[BUFFER] Buffer 1 marked full → READY=1\n");
    } else {
        printf("[ERROR] Unknown buffer pointer in mark_full\n");
        return;
    }

    gpio_put(READY_PIN, 1);
    trigger_spi_dma(buffer_ptr);
}

void buffer_manager_mark_sent(uint16_t* buffer_ptr) {
    printf("[DEBUG] mark_sent called with ptr: %p\n", buffer_ptr);
    stdio_flush();
    printf("[DEBUG] buffer0=%p, buffer1=%p\n", buffer_mgr.buffers[0], buffer_mgr.buffers[1]);
    stdio_flush();
    if (buffer_ptr == buffer_mgr.buffers[0]) {
        buffer_mgr.full[0] = false;
        printf("[BUFFER] Buffer 0 marked as sent\n");
    } else if (buffer_ptr == buffer_mgr.buffers[1]) {
        buffer_mgr.full[1] = false;
        printf("[BUFFER] Buffer 1 marked as sent\n");
    } else {
        printf("[ERROR] Unknown buffer pointer in mark_sent\n");
        return;
    }

    if (!buffer_mgr.full[0] && !buffer_mgr.full[1]) {
        gpio_put(READY_PIN, 0);
        printf("[BUFFER] All buffers sent → READY=0\n");
    } else {
        printf("[BUFFER] Remaining ready buffers: [%d, %d]\n", buffer_mgr.full[0], buffer_mgr.full[1]);
    }
}
