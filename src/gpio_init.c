#include "pico/stdlib.h"
#include "gpio_init.h"

void init_gpios(void) {
    gpio_init(READY_PIN);
    gpio_set_dir(READY_PIN, GPIO_OUT);
    gpio_pull_down(READY_PIN);
    gpio_put(READY_PIN, 0);

    gpio_init(ACK_PIN);
    gpio_set_dir(ACK_PIN, GPIO_IN);
    gpio_pull_down(ACK_PIN);

    gpio_init(ACQ_PIN);
    gpio_set_dir(ACQ_PIN, GPIO_IN);
    gpio_pull_down(ACQ_PIN);

    gpio_init(PIN_LED_ON);
    gpio_set_dir(PIN_LED_ON, GPIO_OUT);
    gpio_put(PIN_LED_ON, 1);
}
