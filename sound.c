#include "sound.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h" // For osDelay

/* * HARDWARE CONFIGURATION:
 * Timer: TIM4
 * Channel: 1
 * Pin: PB6 (Alternate Function 2)
 *
 * NOTE: If using the onboard Codec Speaker (SPK1), you would typically need 
 * the BSP Audio drivers. This implementation generates a raw PWM signal 
 * suitable for a Piezo buzzer connected to PB6, or as an input source 
 * if you route it correctly.
 */

TIM_HandleTypeDef htim4;

void Sound_Init(void)
{
    // 1. Enable Clocks
    __HAL_RCC_TIM4_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // 2. Configure GPIO (PB6 as Alternate Function for TIM4_CH1)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 3. Configure Timer Base
    // Assuming APB1 clock is roughly 42MHz or 84MHz. 
    // We set Prescaler to run timer at 1MHz (1 tick = 1us) for easy math.
    uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
    uint32_t prescaler = (pclk1 / 1000000) - 1; // 1 MHz counter clock

    htim4.Instance = TIM4;
    htim4.Init.Prescaler = prescaler;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 1000; // Default, will change per tone
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&htim4);

    // 4. Configure PWM Channel
    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 500; // 50% duty cycle
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1);
}

void Sound_Tone(uint32_t frequency, uint32_t duration_ms)
{
    if (frequency == 0) return;

    // Calculate Period (ARR) based on 1MHz timer clock
    // Formula: Period = (Timer_Freq / Tone_Freq) - 1
    uint32_t period = (1000000 / frequency) - 1;

    // Update Timer Period (Pitch)
    __HAL_TIM_SET_AUTORELOAD(&htim4, period);
    
    // Update Pulse Width (Volume/Duty Cycle) -> 50% usually gives best sound
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, period / 2);

    // Start PWM
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

    // Wait (Non-blocking to RTOS if possible, otherwise HAL_Delay)
    osDelay(duration_ms);

    // Stop PWM
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
}

void Sound_EatFruit(void)
{
    // High pitched "Ding"
    Sound_Tone(2000, 50); // 2kHz for 50ms
}

void Sound_GameOver(void)
{
    // "Sad" descending tones
    Sound_Tone(1000, 150);
    osDelay(50);
    Sound_Tone(800, 150);
    osDelay(50);
    Sound_Tone(400, 300);
}