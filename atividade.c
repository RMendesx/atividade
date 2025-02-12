#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

// Definição do pino do servo
#define SERVO_PIN 12

// Função para configurar o PWM
void setup_pwm(uint servo_gpio)
{
    gpio_set_function(servo_gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(servo_gpio);

    // Configuração do clock PWM
    pwm_set_clkdiv(slice_num, 100.0); // Divide o clock para 1.25MHz (125MHz / 100)
    pwm_set_wrap(slice_num, 25000);   // 1.25MHz / 50Hz = 25000
    pwm_set_enabled(slice_num, true);
}

void angulo(uint gpio, float angle)
{
    // Define o ciclo de trabalho do PWM com base no ângulo desejado
    // Servo modificado: Pulso de 0.5ms a 3.5ms em um período de 20ms
    uint slice = pwm_gpio_to_slice_num(gpio);
    float duty = 500 + (angle / 270.0) * 3000; // Converte ângulo em largura de pulso
    pwm_set_chan_level(slice, PWM_CHAN_A, duty);
}

// Função para movimentação suave do servomotor
void lentamente(uint gpio, uint start_duty, uint end_duty, int step, uint delay_ms)
{
    uint slice = pwm_gpio_to_slice_num(gpio);
    uint current_duty = start_duty;

    // Movimentação do servo de start_duty para end_duty
    if (start_duty < end_duty)
    {
        for (uint i = start_duty; i <= end_duty; i += step)
        {
            pwm_set_chan_level(slice, PWM_CHAN_A, i);
            sleep_ms(delay_ms);
        }
    }
    else
    {
        for (uint i = start_duty; i >= end_duty; i -= step)
        {
            pwm_set_chan_level(slice, PWM_CHAN_A, i);
            sleep_ms(delay_ms);
        }
    }
}
int main()
{
    stdio_init_all();
    setup_pwm(SERVO_PIN);

    // Definição dos ciclos ativos para 0°, 90°, 180° e 270°
    uint duty_0_degrees = 500;    // (0°)
    uint duty_180_degrees = 3500; // (180°)

    while (1)
    {
        // 1) Move para 180° e aguarda 5 segundos
        pwm_set_chan_level(pwm_gpio_to_slice_num(SERVO_PIN), PWM_CHAN_A, 3000);
        sleep_ms(5000); // Aguarda 5 segundos

        // 2) Move para 90°e aguarda 5 segundos
        pwm_set_chan_level(pwm_gpio_to_slice_num(SERVO_PIN), PWM_CHAN_A, 1840);
        sleep_ms(5000); // Aguarda 5 segundos

        // 3) Move para 0° (500µs) e aguarda 5 segundos
        pwm_set_chan_level(pwm_gpio_to_slice_num(SERVO_PIN), PWM_CHAN_A, 600);
        sleep_ms(5000); // Aguarda 5 segundos

        // 4) Movimentação suave periódica entre 0° e 180°
        lentamente(SERVO_PIN, duty_0_degrees, duty_180_degrees, 5, 10); // 0° → 180°
        sleep_ms(1000);                                                  // Aguarda 1 segundo
        lentamente(SERVO_PIN, duty_180_degrees, duty_0_degrees, 5, 10); // 180° → 0°
        sleep_ms(1000);                                                  // Aguarda 1 segundo
    }
}