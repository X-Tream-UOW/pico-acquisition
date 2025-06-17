#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/irq.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "reader.pio.h"

#define BUFFER_SIZE 65536

#define ACQ_FREQ 1000000.0f
 
#define DB_STARTS  0
#define DB_COUNT   16
#define BUSY_PIN   16
#define CS_PIN     17
#define CONVST_PIN 18
#define RD_PIN     19

#define PWM_PIN  23

PIO pio = pio0;
uint sm_reader;
uint offset_reader;

uint16_t buffer1[BUFFER_SIZE];
uint16_t buffer2[BUFFER_SIZE];
volatile bool buffer1_ready = false;
volatile bool buffer2_ready = false;
int dma_chan;

void setup_pwm(float acquisition_freq) {
    if (acquisition_freq < 1.0f || acquisition_freq > 1.25e6f) {
        return;
    }

    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN);

    const float sys_clk = 150e6f;
    const float clkdiv = 1.0f;

    uint32_t wrap = (uint32_t)((sys_clk / (clkdiv * acquisition_freq)) - 1.0f);
    if (wrap > 65535) wrap = 65535;

    pwm_set_clkdiv(slice_num, clkdiv);
    pwm_set_wrap(slice_num, wrap);

    // Set duty cycle to ~50%
    uint32_t level = wrap / 2;
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(PWM_PIN), level);

    pwm_set_enabled(slice_num, true);
}

void __isr dma_handler() {
    dma_hw->ints0 = 1u << dma_chan;
    static bool ping = false;
    if (ping) {
        buffer2_ready = true;
        dma_channel_set_write_addr(dma_chan, buffer1, true);
    } else {
        buffer1_ready = true;
        dma_channel_set_write_addr(dma_chan, buffer2, true);
    }
    ping = !ping;
}

void setup_dma() {
    dma_chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(dma_chan);

    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
    channel_config_set_dreq(&cfg, pio_get_dreq(pio, sm_reader, false));

    dma_channel_configure(
        dma_chan,
        &cfg,
        buffer1,
        (const volatile uint16_t *)&pio->rxf[sm_reader],
        BUFFER_SIZE,
        false
    );

    dma_channel_set_irq0_enabled(dma_chan, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);
}

void init_db_lines() {
    for (int i = 0; i < DB_COUNT; i++) {
        pio_gpio_init(pio, DB_STARTS + i);
    }
}

int main(){
    stdio_init_all();
    sleep_ms(2000);
    printf("PWM ADC acquisition\n");

    setup_pwm(ACQ_FREQ);

    // Load PIO program
    offset_reader = pio_add_program(pio, &reader_program);
    sm_reader = pio_claim_unused_sm(pio, true);

    // Init GPIOs
    init_db_lines();
    pio_gpio_init(pio, BUSY_PIN);
    pio_gpio_init(pio, CS_PIN);
    pio_gpio_init(pio, CONVST_PIN);
    pio_gpio_init(pio, RD_PIN); 

    // Configure reader SM 
    pio_sm_config c_reader = reader_program_get_default_config(offset_reader);
    sm_config_set_set_pins(&c_reader, CS_PIN, 3);     // CS, CONVST, RD
    sm_config_set_in_pins(&c_reader, DB_STARTS);      // DB[15:0] 
    sm_config_set_in_shift(&c_reader, false, true, 16);
    sm_config_set_fifo_join(&c_reader, PIO_FIFO_JOIN_RX);
    pio_sm_init(pio, sm_reader, offset_reader, &c_reader);

    // Set pin directions for reader SM
    pio_sm_set_consecutive_pindirs(pio, sm_reader, CS_PIN, 3, true);  // CS, CONVST, RD as outputs
    pio_sm_set_consecutive_pindirs(pio, sm_reader, DB_STARTS, DB_COUNT, false);  // DB pins as inputs
    pio_sm_set_consecutive_pindirs(pio, sm_reader, BUSY_PIN, 1, false);  // BUSY as input

    pio_sm_set_enabled(pio, sm_reader, true);

    setup_dma();
    dma_channel_start(dma_chan);

    uint64_t dma_start_time = time_us_64();

    while (true) {
        if (buffer1_ready) {
            buffer1_ready = false;
            uint64_t now = time_us_64();
            printf("Buffer 1 filled in %llu us\n", now - dma_start_time);
            dma_start_time = now;
        }

        if (buffer2_ready) {
            buffer2_ready = false;
            uint64_t now = time_us_64();
            printf("Buffer 2 filled in %llu us\n", now - dma_start_time);
            dma_start_time = now;
        }
    }
}