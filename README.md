# Projeto de Controle com Joystick e Display SSD1306 no RP2040

## Descrição
Este projeto utiliza um microcontrolador RP2040 para controlar LEDs RGB e um display OLED SSD1306 através de um joystick analógico. O sistema permite:
- Controle dos LEDs baseado na posição do joystick.
- Interrupção com debounce para botões.
- Atualização do display OLED com a posição do joystick.

## Hardware Utilizado
- RP2040 (exemplo: Raspberry Pi Pico)
- Display OLED SSD1306 (I2C)
- Joystick analógico
- LEDs RGB (Vermelho, Azul e Verde)
- Botões para interação

## Bibliotecas Necessárias
- `pico/stdlib.h`
- `hardware/adc.h`
- `hardware/i2c.h`
- `hardware/pwm.h`
- `hardware/irq.h`
- `hardware/gpio.h`
- `hardware/clocks.h`
- `lib/ssd1306.h`
- `lib/font.h`

## Configuração dos Pinos
| Componente  | GPIO |
|------------|------|
| Joystick X | 26   |
| Joystick Y | 27   |
| Botão Joystick | 22   |
| Botão A   | 5    |
| LED Verde  | 11   |
| LED Azul   | 12   |
| LED Vermelho | 13  |
| I2C SDA    | 14   |
| I2C SCL    | 15   |

## Como Compilar e Executar
1. Instale o SDK do Raspberry Pi Pico.
2. Configure o ambiente de desenvolvimento.
3. Compile o código utilizando CMake.
4. Carregue o binário gerado no RP2040.

## Funcionamento
- O joystick controla a intensidade dos LEDs RGB.
- O display OLED mostra um quadrado se movendo conforme a entrada do joystick.
- O botão A alterna entre ligado e desligado.
- O botão do joystick ativa/desativa o LED verde.

## Vídeo Demonstrativo
[https://www.youtube.com/shorts/mXb2OkFiIRU]

## Autor
- **Theógenes Gabriel Araújo de Andrade**

