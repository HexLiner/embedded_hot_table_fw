//  ***************************************************************************
/// @file    indicators_driver.c
//  ***************************************************************************
#include "indicators_driver.h"


#define LEDS_BUZZER_PIN (PA4)

static bool is_led_process, is_led_error_en;


typedef enum {
    BUZZER_STATE_IDLE,
    BUZZER_STATE_BEEP_PROC,
} buzzer_process_state_t;

typedef struct {
    uint8_t  repeats_qty;
    uint8_t  durations_qty;
    const uint16_t *durations_ms;
} buzzer_melody_t;

static buzzer_process_state_t buzzer_process_state;
static timer_t buzzer_timer;
static uint8_t buzzer_duration_counter;
static uint8_t buzzer_repeat_counter;
static uint8_t buzzer_task;
static bool buzzer_state;

static const uint16_t beep_melody_durations[] = {500};
static const uint16_t process_done_melody_durations[] = {1000, 800};
static const uint16_t error_melody_durations[] = {200, 100, 200, 100, 200, 1000};

static const buzzer_melody_t buzzer_melodyes[] = {
    {1, (sizeof(beep_melody_durations) / sizeof(uint16_t)), beep_melody_durations},
    {5, (sizeof(process_done_melody_durations) / sizeof(uint16_t)), process_done_melody_durations},
    {5, (sizeof(error_melody_durations) / sizeof(uint16_t)), error_melody_durations},
};


static void led_process(void);
static void buzzer_process(void);
static void buzzer_en(void);
static void buzzer_dis(void);




void indicators_init(void) {
    gpio_config_pins(LEDS_BUZZER_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULL_NONE, GPIO_SPEED_HIGH, 0, true);

    // TIM14_CH1
    sysclk_enable_peripheral(TIM14);
    TIM14->CCMR1 = 4 << TIM_CCMR1_OC1M_Pos; //  disable
    TIM14->CCER = 1 << TIM_CCER_CC1E_Pos;
    TIM14->PSC = 0;
    TIM14->ARR = 6857;   // 7 KHz
    TIM14->CCR1 = 6857 / 2;
    TIM14->CR1 = TIM_CR1_CEN;

    is_led_process = false;
    is_led_error_en = false;

    buzzer_process_state = BUZZER_STATE_IDLE;
    buzzer_task = 0xFF;
}


void indicators_process(void) {
    led_process();
    buzzer_process();
}


void indicators_led_process(bool en_dis) {
    is_led_process = en_dis;
}

void indicators_led_error(bool en_dis) {
    is_led_error_en = en_dis;
}


void indicators_buzzer_beep_terminate(void) {
    buzzer_task = 0xFF;
    buzzer_process_state = BUZZER_STATE_IDLE;
    buzzer_state = false;
    buzzer_dis();
}


void indicators_buzzer_short_beep(void) {
    buzzer_task = 0;
}


void indicators_buzzer_process_done_beep(void) {
    buzzer_task = 1;
}


void indicators_buzzer_error_beep(void) {
    buzzer_task = 2;
}




static void buzzer_process(void) {
    switch (buzzer_process_state) {
        case BUZZER_STATE_IDLE:
            if (buzzer_task != 0xFF) {
                buzzer_duration_counter = 0;
                buzzer_repeat_counter = 0;
                buzzer_timer = timer_start_ms(buzzer_melodyes[buzzer_task].durations_ms[0]);
                buzzer_process_state = BUZZER_STATE_BEEP_PROC;
                buzzer_state = true;
                buzzer_en();
            }
            break;


        case BUZZER_STATE_BEEP_PROC:
            if (timer_triggered(buzzer_timer)) {
                buzzer_duration_counter++;
                if (buzzer_duration_counter >= buzzer_melodyes[buzzer_task].durations_qty) {
                    buzzer_duration_counter = 0;
                    buzzer_repeat_counter++;
                    if (buzzer_repeat_counter >= buzzer_melodyes[buzzer_task].repeats_qty) {
                        buzzer_process_state = BUZZER_STATE_IDLE;
                        buzzer_task = 0xFF;
                        buzzer_dis();
                        return;
                    }
                }

                buzzer_timer = timer_start_ms(buzzer_melodyes[buzzer_task].durations_ms[buzzer_duration_counter]);
                if (buzzer_state) {
                    buzzer_state = false;
                    buzzer_dis();
                }
                else {
                    buzzer_state = true;
                    buzzer_en();
                }
            }
            break;
    }
}


static void buzzer_en(void) {
    gpio_config_pins(LEDS_BUZZER_PIN, GPIO_MODE_ALT_FUNCTION_PP, GPIO_PULL_NONE, GPIO_SPEED_HIGH, 4, false);
    TIM14->CCMR1 = 6 << TIM_CCMR1_OC1M_Pos;    // PWM mode 1 - Channel 1 is active as long as TIMx_CNT < TIMx_CCR1 else inactive
}


static void buzzer_dis(void) {
    gpio_config_pins(LEDS_BUZZER_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULL_NONE, GPIO_SPEED_HIGH, 0, true);
    TIM14->CCMR1 = 4 << TIM_CCMR1_OC1M_Pos;  // disable
}


static void led_process(void) {
    static timer_t led_animation_timer = 0;
    static bool is_led_en = false;


    if (buzzer_process_state != BUZZER_STATE_IDLE) return;

    
    if (is_led_error_en) {
        if (timer_triggered(led_animation_timer)) {
            if (is_led_en) gpio_reset_pins(LEDS_BUZZER_PIN);
            else gpio_set_pins(LEDS_BUZZER_PIN);
            is_led_en = !is_led_en;
            led_animation_timer = timer_start_ms(600);
        }
    }
    else if (is_led_process) {
        gpio_set_pins(LEDS_BUZZER_PIN);
    }
    else {
        gpio_reset_pins(LEDS_BUZZER_PIN);
    }
}
