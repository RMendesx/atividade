#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"



#define pino 22

void setup_pwm(uint servo_gpio) {
    gpio_set_function(servo_gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(servo_gpio);

    // Configuração do clock PWM
    pwm_set_clkdiv(slice_num, 100.0); // Divide o clock para 1.25MHz (125MHz / 100)
    pwm_set_wrap(slice_num, 25000);   // 1.25MHz / 50Hz = 25000
    pwm_set_enabled(slice_num, true);
}

void set_servo_angle(uint gpio, float angle) {
    uint slice = pwm_gpio_to_slice_num(gpio);
    float duty = 500 + (angle / 270.0) * 3000; // Converte ângulo em largura de pulso (500us a 3500us)
    pwm_set_chan_level(slice, PWM_CHAN_A, duty);
}

void mover_lentamente(uint gpio, uint start_duty, uint end_duty, int step, uint delay_ms) {
    uint slice = pwm_gpio_to_slice_num(gpio);
    uint current_duty = start_duty;

    if (start_duty < end_duty) {
        for (uint i = start_duty; i <= end_duty; i += step) {
            pwm_set_chan_level(slice, PWM_CHAN_A, i);
            sleep_ms(delay_ms);
        }
    } else {
        for (uint i = start_duty; i >= end_duty; i -= step) {
            pwm_set_chan_level(slice, PWM_CHAN_A, i);
            sleep_ms(delay_ms);
        }
    }
}

int main() {
    stdio_init_all();
    setup_pwm(pino);


    uint duty_0_degrees = 500;   // 500µs (0°)
    uint duty_270_degrees = 3500; // 3500µs (180°)

    while (1) {
        // Movimentação suave de 0° para 180°
        mover_lentamente(pino, duty_0_degrees, duty_270_degrees, 5, 10);
        sleep_ms(5000); // Aguarda 1 segundo na posição de 180°

        // Movimentação suave de 180° para 0°
        mover_lentamente(pino, duty_270_degrees, duty_0_degrees, 5, 10);
        sleep_ms(5000); // Aguarda 1 segundo na posição de 0°
    }
}