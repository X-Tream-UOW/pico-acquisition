#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#define BUFFER_SIZE 65536  // 2^16 samples

typedef struct {
    uint16_t* buffers[2];
    bool full[2];
} BufferManager;

extern BufferManager buffer_mgr;
extern uint16_t buffer1[BUFFER_SIZE];
extern uint16_t buffer2[BUFFER_SIZE];

void buffer_manager_init(uint16_t* buf0, uint16_t* buf1);
uint16_t* buffer_manager_get_free_buffer(void);
uint16_t* buffer_manager_get_full_buffer(void);
void buffer_manager_mark_full(uint16_t* buffer_ptr);
void buffer_manager_mark_sent(uint16_t* buffer_ptr);

#endif
