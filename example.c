/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG               
*        Solutions for real time microcontroller applications        
**********************************************************************
*                                                                    *
*        (c) 1996 - 2018  SEGGER Microcontroller GmbH & Co. KG       
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    
*                                                                    *
**********************************************************************

** emWin V5.48 - Graphical user interface for embedded applications **
---------------------------------------------------------------------- 
*/

/* example.c */
#include "main.h"
#include "GUI.h"
#include "snake_game.h"
#include "input.h" 

#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

__NO_RETURN void app_main (void *argument) {
  int32_t xPos, yPos, xSize, ySize;
  int32_t i = 0;

  (void)argument;

  /* --- INITIALIZE KEYPAD (PF0-PF7) --- */
  Keypad_Init(); 
  /* ----------------------------------- */

  GUI_Init();

  GUI_SetBkColor(GUI_BLACK);
  GUI_Clear();

  xSize = LCD_GetXSize();
  ySize = LCD_GetYSize();

  xPos = xSize / 2;
  yPos = ySize / 3;

  GUI_SetColor(GUI_WHITE);
  GUI_SetTextMode(GUI_TM_REV);
  GUI_SetFont(GUI_FONT_20F_ASCII);
  GUI_DispStringHCenterAt("Press '5' to Start", xPos, yPos);

  /* MAIN LOOP */
  while (1) {
    
    /* Check for Start Key '5' */
    if (Keypad_Get_Key() == '5') {
        StartSnakeGame();
        
        // Redraw Menu after game
        GUI_SetBkColor(GUI_BLACK);
        GUI_Clear();
        GUI_SetColor(GUI_WHITE);
        GUI_DispStringHCenterAt("Press '5' to Start", xPos, yPos);
    }

    GUI_Delay(50); 
  }
}

