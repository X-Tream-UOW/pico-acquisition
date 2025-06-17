#include "hardware/pwm.h"
#include "pico/stdlib.h"

#define PWM_PIN 21

void setup_pwm(float acquisition_freq) {
    if (acquisition_freq < 1.0f || acquisition_freq > 1.25e6f)
        return;

    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN);

    const float sys_clk = 150e6f;
    float clkdiv = 1.0f;
    uint32_t wrap = (uint32_t)((sys_clk / (clkdiv * acquisition_freq)) - 1.0f);
    if (wrap > 65535) wrap = 65535;

    pwm_set_clkdiv(slice_num, clkdiv);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(PWM_PIN), wrap / 2);
    pwm_set_enabled(slice_num, true);
}
