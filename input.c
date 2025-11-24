/* input.c */
#include "main.h"
#include "input.h"

/* * WARNING: PF0-PF9 are often used for FSMC (LCD/RAM) Address lines.
 * If your LCD stops working, move Keypad to Port A (PA0-PA7) or Port C (PC0-PC7).
 */

/* Keypad Mapping */
static const char keyMap[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

void Keypad_Init(void)
{
    /* Enable Clock for Port F */
    __HAL_RCC_GPIOF_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 1. Configure Rows (PF0 - PF3) as Outputs */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    /* Set all Rows HIGH initially (Inactive) */
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_SET);

    /* 2. Configure Columns (PF4 - PF7) as Inputs with Internal PULL-UP */
    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP; /* Critical for active-low scanning */
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
}

char Keypad_Get_Key(void)
{
    uint16_t rows[] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3};
    uint16_t cols[] = {GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7};

    for (int r = 0; r < 4; r++) {
        /* Set current Row LOW (Active) */
        HAL_GPIO_WritePin(GPIOF, rows[r], GPIO_PIN_RESET);

        /* Check Columns */
        for (int c = 0; c < 4; c++) {
            /* If Input is LOW, button is pressed */
            if (HAL_GPIO_ReadPin(GPIOF, cols[c]) == GPIO_PIN_RESET) {
                
                /* Debounce Wait */
                // HAL_Delay(20); 

                /* Restore Row HIGH before returning */
                HAL_GPIO_WritePin(GPIOF, rows[r], GPIO_PIN_SET);
                return keyMap[r][c];
            }
        }
        /* Set Row back to HIGH (Inactive) */
        HAL_GPIO_WritePin(GPIOF, rows[r], GPIO_PIN_SET);
    }

    return 0; /* No key pressed */
}
