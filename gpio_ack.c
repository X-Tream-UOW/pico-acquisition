#include "gpio_ack.h"
#include "hardware/gpio.h"
#include "buffer_manager.h"
#include "spi_dma.h"
#include <stdio.h>
#include "pico/stdio.h"

#define ACK_PIN 26
#define READY_PIN 21

extern uint16_t* current_spi_buffer;

void __isr ack_gpio_irq_handler(uint gpio, uint32_t events) {
    if (current_spi_buffer) {
        printf("[ACK] ACK received, clearing buffer\n");
        stdio_flush();
        printf("[DEBUG] current_spi_buffer = %p\n", current_spi_buffer);
        stdio_flush();
        buffer_manager_mark_sent(current_spi_buffer);  // <-- crashes here
        current_spi_buffer = NULL;
    } else {
        printf("[ACK] Warning: no buffer to mark as sent\n");
    }
}

void setup_ack_gpio(void) {
    gpio_init(ACK_PIN);
    gpio_set_dir(ACK_PIN, GPIO_IN);
    gpio_pull_down(ACK_PIN);
    gpio_set_irq_enabled_with_callback(ACK_PIN, GPIO_IRQ_EDGE_RISE, true, &ack_gpio_irq_handler);
}
