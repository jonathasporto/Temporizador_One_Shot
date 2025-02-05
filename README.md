# Controle de LEDs com Botão no Raspberry Pi Pico

Este projeto implementa um sistema de controle de LEDs utilizando um Raspberry Pi Pico. O sistema alterna entre diferentes padrões de iluminação ao pressionar um botão, utilizando interrupções e temporizadores.

## Componentes Utilizados

- Raspberry Pi Pico
- LEDs (Verde, Azul e Vermelho)
- Resistores (330Ω para cada LED)
- Botão
- Jumpers
- Protoboard

## Esquema de Conexão

| Componente | GPIO do Pico |
|-----------|-------------|
| LED Verde  | 11          |
| LED Azul   | 12          |
| LED Vermelho | 13       |
| Botão      | 5          |

## Funcionamento

O sistema funciona baseado em diferentes estados:

1. **IDLE**: Nenhum LED aceso.
2. **ALL_ON**: Todos os LEDs acendem.
3. **TWO_ON**: Apenas dois LEDs permanecem acesos.
4. **ONE_ON**: Apenas um LED permanece aceso.
5. O botão é utilizado para iniciar o ciclo, e os LEDs desligam gradativamente após 3 segundos.

## Código-fonte Explicado

### Definições e Configuração

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
```

Incluímos as bibliotecas padrão do Raspberry Pi Pico para manipulação de GPIO e temporizadores.

### Definição dos GPIOs dos LEDs e Botão

```c
#define LED_GREEN   11
#define LED_BLUE    12
#define LED_RED     13
#define BUTTON_PIN  5
```

Os pinos GPIO 11, 12 e 13 são usados para controlar os LEDs, enquanto o botão está no GPIO 5.

### Definição dos Estados

```c
#define IDLE     0  // Nenhum LED ativo
#define ONE_ON   1  // Um LED ligado
#define TWO_ON   2  // Dois LEDs ligados
#define ALL_ON   3  // Todos os LEDs ligados
```

Criamos constantes para representar os diferentes estados do sistema.

### Atualização dos LEDs

```c
void update_leds() {
    switch (current_state) {
        case ALL_ON:
            gpio_put(LED_GREEN, 1);
            gpio_put(LED_BLUE, 1);
            gpio_put(LED_RED, 1);
            break;
        case TWO_ON:
            gpio_put(LED_GREEN, 0);
            gpio_put(LED_BLUE, 1);
            gpio_put(LED_RED, 1);
            break;
        case ONE_ON:
            gpio_put(LED_GREEN, 0);
            gpio_put(LED_BLUE, 0);
            gpio_put(LED_RED, 1);
            break;
        case IDLE:
            gpio_put(LED_GREEN, 0);
            gpio_put(LED_BLUE, 0);
            gpio_put(LED_RED, 0);
            break;
    }
}
```

Esta função altera os LEDs conforme o estado atual do sistema.

### Callback do Temporizador

```c
int64_t alarm_callback(alarm_id_t id, void *user_data) {
    if (current_state > IDLE) {
        current_state--;
        update_leds();
        if (current_state > IDLE) {
            add_alarm_in_ms(3000, alarm_callback, NULL, false);
        }
    }
    return 0;
}
```

Sempre que o botão for pressionado, os LEDs começam a se apagar progressivamente a cada 3 segundos.

### Debounce do Botão

```c
bool debounce_button() {
    static uint64_t last_debounce_time = 0;
    const uint32_t debounce_delay = 50;
    if ((time_us_64() - last_debounce_time) > debounce_delay * 1000) {
        last_debounce_time = time_us_64();
        return gpio_get(BUTTON_PIN) == 0;
    }
    return false;
}
```

Esta função evita leituras erradas devido a ruídos mecânicos do botão.

### Função `main()`

```c
int main() {
    stdio_init_all();

    gpio_init(LED_BLUE);
    gpio_init(LED_RED);
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_set_dir(LED_GREEN, GPIO_OUT);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    while (true) {
        if (debounce_button() && current_state == IDLE) {
            current_state = ALL_ON;
            update_leds();
            add_alarm_in_ms(3000, alarm_callback, NULL, false);
        }
        sleep_ms(10);
    }
    return 0;
}
```

A função principal:

1. Configura os pinos dos LEDs como saída e o botão como entrada com pull-up.
2. Monitora o botão e, ao ser pressionado, inicia o ciclo de acendimento e desligamento progressivo dos LEDs.

## Como Compilar e Executar

1. Instale o **SDK do Raspberry Pi Pico** em seu ambiente de desenvolvimento.
2. Compile o código utilizando o CMake e GCC para Pico.
3. Transfira o arquivo `.uf2` gerado para o Raspberry Pi Pico.
4. Conecte um monitor serial para visualizar os logs do sistema.

---

Projeto desenvolvido para demonstração do uso de GPIOs, botões e temporizadores no Raspberry Pi Pico.

