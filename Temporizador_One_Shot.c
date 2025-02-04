#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"

// Definição dos pinos
#define LED_GREEN   11
#define LED_BLUE    12
#define LED_RED     13
#define BUTTON_PIN  5

// Definição dos estados
#define IDLE     0  // Nenhum LED ativo
#define ONE_ON   1  // Todos LEDs ligados
#define TWO_ON   2  // Dois LEDs ligados
#define ALL_ON   3  // Um LED ligado

// Variáveis de estado voláteis (compartilhadas com interrupções)
volatile int current_state = IDLE;
volatile uint64_t last_press_time = 0;

// Função para atualizar os LEDs baseado no estado
void update_leds() {
    // Lógica para ligar/desligar LEDs com base no estado
    switch (current_state) {
        case ALL_ON:
            gpio_put(LED_GREEN, 1); // Liga todos
            gpio_put(LED_BLUE, 1);
            gpio_put(LED_RED, 1);
            break;
        case TWO_ON:
            gpio_put(LED_GREEN, 0); // Desliga o verde
            gpio_put(LED_BLUE, 1);
            gpio_put(LED_RED, 1);
            break;
        case ONE_ON:
            gpio_put(LED_GREEN, 0); // Mantém o verde desligado
            gpio_put(LED_BLUE, 0);  // Desliga o azul
            gpio_put(LED_RED, 1);
            break;
        case IDLE:
            gpio_put(LED_GREEN, 0); // Desliga todos
            gpio_put(LED_BLUE, 0);
            gpio_put(LED_RED, 0);
            break;
    }
}

// Callback do alarme para transição de estados
int64_t alarm_callback(alarm_id_t id, void *user_data) {
    if (current_state > IDLE) {
        current_state--;
        update_leds();
        
        if (current_state > IDLE) {
            // Agenda próximo alarme se não estiver no estado final
            add_alarm_in_ms(3000, alarm_callback, NULL, false);
        }
    }
    return 0;
}

// Função para debounce do botão
bool debounce_button() {
    static uint64_t last_debounce_time = 0;
    const uint32_t debounce_delay = 50;
    
    if ((time_us_64() - last_debounce_time) > debounce_delay * 1000) {
        last_debounce_time = time_us_64();
        return gpio_get(BUTTON_PIN) == 0;
    }
    return false;
}

// Função principal
int main() {
    stdio_init_all();

    // Configuração dos LEDs
    gpio_init(LED_BLUE);
    gpio_init(LED_RED);
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_set_dir(LED_GREEN, GPIO_OUT);

    // Configuração do botão
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    while (true) {
        // Verificação de debounce
        if (debounce_button() && current_state == IDLE) {
            current_state = ALL_ON;
            update_leds();
            add_alarm_in_ms(3000, alarm_callback, NULL, false);
        }
        
        sleep_ms(10);
    }
    return 0;
}
