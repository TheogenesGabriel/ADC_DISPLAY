// Theógenes Gabriel Araújo de Andrade
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h" // Para funções de tempo
#include "lib/ssd1306.h"
#include "lib/font.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define JOYSTICK_X_PIN 26  // GPIO para eixo X
#define JOYSTICK_Y_PIN 27  // GPIO para eixo Y
#define JOYSTICK 22 // GPIO para botão do Joystick
#define Botao_A 5 // GPIO para botão A

#define PIN_VERDE 11
#define LED_RED 13
#define LED_BLUE 12

volatile bool estGreen = false;
volatile bool estado = true;

volatile uint32_t tempo_anterior = 0;

const uint16_t MAX = 65535; //define o máximo para o botão

void init_ADC(){
  adc_init();
  adc_gpio_init(JOYSTICK_X_PIN);
  adc_gpio_init(JOYSTICK_Y_PIN);  
}

void init_hardware(){
  gpio_init(PIN_VERDE);
  gpio_init(JOYSTICK);

  gpio_set_dir(PIN_VERDE, GPIO_OUT);
  gpio_set_dir(JOYSTICK, GPIO_IN);
  gpio_pull_up(JOYSTICK); 

  gpio_init(Botao_A);
  gpio_set_dir(Botao_A, GPIO_IN);
  gpio_pull_up(Botao_A);
}

// Configuração do PWM
void setup_pwm(bool estado) {

  gpio_set_function(LED_RED, GPIO_FUNC_PWM);
  gpio_set_function(LED_BLUE, GPIO_FUNC_PWM);

  uint slice_num_red = pwm_gpio_to_slice_num(LED_RED);
  uint slice_num_blue = pwm_gpio_to_slice_num(LED_BLUE);

  pwm_set_wrap(slice_num_red, MAX);
  pwm_set_wrap(slice_num_blue, MAX);

  pwm_set_enabled(slice_num_red, estado);
  pwm_set_enabled(slice_num_blue, estado);
}


// Interrupção para o botão A e o Joystick com debounce
void button_a_isr(uint gpio, uint32_t events) {
  uint32_t tempo_atual = to_us_since_boot(get_absolute_time());
  
  // Debounce: Verifica se o tempo desde o último pressionamento é maior que 200ms
  if (tempo_atual - tempo_anterior > 195) {
      if(!gpio_get(Botao_A)){
        estado = !estado;
        setup_pwm(estado);
      }else if(!gpio_get(JOYSTICK)){
        estGreen = !estGreen; // Alterna o estado do LED Verde
        setup_pwm(false);
      }
      tempo_anterior = tempo_atual; // Atualiza o tempo do último pressionamento
  }
  
}

void update_leds(uint16_t x_value, uint16_t y_value) {
  // Mapear valores do ADC para PWM (0-4095 -> 0-65535)
  uint16_t level_red = 0;
  uint16_t level_blue = 0;

  // Diferença entre x_value e y_value
  uint16_t diff = abs(x_value - y_value);

  // Ativar LEDs apenas se a diferença for significativa
  if (diff > 300) {
      // Eixo X (LED Vermelho)
      if(x_value > y_value){
        if (y_value < 1840) {
          level_blue = MAX - (2048 - y_value) * 32; // Movimento para cima
        }
        else if (x_value > 2070) {
          level_red = (x_value - 2048) * 32; // Movimento para a direita
        }
      }else{
        // Eixo Y (LED Azul)
        if (x_value < 1790) {
          level_red = MAX - (2048 - x_value) * 32; // Movimento para a esquerda
        }  else if (y_value > 1980) {
          level_blue = (y_value - 2048) * 32; // Movimento para baixo
        }
      }
  }
  // Aplicar os níveis de PWM aos LEDs
  pwm_set_gpio_level(LED_RED, level_red);
  pwm_set_gpio_level(LED_BLUE, level_blue);
}


int main()
{

  // Inicializações
  stdio_init_all();
  init_ADC();
  init_hardware();
  setup_pwm(estado);


  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA); // Pull up the data line
  gpio_pull_up(I2C_SCL); // Pull up the clock line
  ssd1306_t ssd; // Inicializa a estrutura do display
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd); // Configura o display
  ssd1306_send_data(&ssd); // Envia os dados para o display

  // Limpa o display. O display inicia com todos os pixels apagados.
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  // Configurar interrupções para os botões
  gpio_set_irq_enabled_with_callback(JOYSTICK, GPIO_IRQ_EDGE_FALL, true, &button_a_isr);
  gpio_set_irq_enabled_with_callback(Botao_A, GPIO_IRQ_EDGE_FALL, true, &button_a_isr);


  uint16_t adc_value_x;
  uint16_t adc_value_y;  

  while (true)
  {
    adc_select_input(1); // Seleciona o ADC para eixo X. O pino 26 como entrada analógica
    adc_value_x = adc_read();
    adc_select_input(0); // Seleciona o ADC para eixo Y. O pino 27 como entrada analógica
    adc_value_y = adc_read();    


    update_leds(adc_value_x, adc_value_y);


    //ssd1306_fill(&ssd, false);

    if(estGreen){
      ssd1306_rect(&ssd, 0, 0, 128, 64, true, false);
      uint8_t square_x = (adc_value_x * 120) / 4095;
      uint8_t square_y = (56 - ((adc_value_y * 56) / 4095));
      ssd1306_rect(&ssd, square_y, square_x, 8, 8, true, true); // Desenha um retângulo 
      ssd1306_send_data(&ssd);
      ssd1306_fill(&ssd, false);
    }else{
      uint8_t square_x = (adc_value_x * 120) / 4095;
      uint8_t square_y = (56 - ((adc_value_y * 56) / 4095));
      ssd1306_rect(&ssd, square_y, square_x, 8, 8, true, true); // Desenha um retângulo 
      ssd1306_send_data(&ssd);
      ssd1306_fill(&ssd, false);
    }
    gpio_put(PIN_VERDE, estGreen);
    sleep_ms(100);
    
  }
}
