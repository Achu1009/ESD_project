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
#include "brick_game.h"
#include "flappy_game.h"
#include "2048_game.h"


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
  GUI_DispStringHCenterAt("Press 'A' to Start snake", xPos, yPos);
	
  xPos = xSize / 2;
  yPos = ySize / 3 + 20;
	
	GUI_DispStringHCenterAt("Press 'B' to Start brick", xPos, yPos);
	xPos = xSize / 2;
  yPos = ySize / 3 + 40;
	GUI_DispStringHCenterAt("Press 'C' to Start flappy", xPos, yPos);
	xPos = xSize / 2;
  yPos = ySize / 3 + 60;
	GUI_DispStringHCenterAt("Press 'D' to Start 2048", xPos, yPos);
	
	
  /* MAIN LOOP */
  while (1) {
    
    /* Check for Start Key '5' */
    if (Keypad_Get_Key() == 'A') {
        StartSnakeGame();
				GUI_SetBkColor(GUI_BLACK);
				GUI_Clear();
				xPos = xSize / 2;
				yPos = ySize / 3;
        GUI_SetColor(GUI_WHITE);
				GUI_SetTextMode(GUI_TM_REV);
				GUI_SetFont(GUI_FONT_20F_ASCII);
				GUI_DispStringHCenterAt("Press 'A' to Start snake", xPos, yPos);
				xPos = xSize / 2;
				yPos = ySize / 3 + 20;
				GUI_DispStringHCenterAt("Press 'B' to Start brick", xPos, yPos);
				xPos = xSize / 2;
				yPos = ySize / 3 + 40;
				GUI_DispStringHCenterAt("Press 'C' to Start flappy", xPos, yPos);
				xPos = xSize / 2;
				yPos = ySize / 3 + 60;
				GUI_DispStringHCenterAt("Press 'D' to Start 2048", xPos, yPos);
    }
		if (Keypad_Get_Key() == 'B') {
        StartBrickGame();
				GUI_SetBkColor(GUI_BLACK);
				GUI_Clear();
				xPos = xSize / 2;
				yPos = ySize / 3;
        GUI_SetColor(GUI_WHITE);
				GUI_SetTextMode(GUI_TM_REV);
				GUI_SetFont(GUI_FONT_20F_ASCII);
				GUI_DispStringHCenterAt("Press 'A' to Start snake", xPos, yPos);
				xPos = xSize / 2;
				yPos = ySize / 3 + 20;
				GUI_DispStringHCenterAt("Press 'B' to Start brick", xPos, yPos);
				xPos = xSize / 2;
				yPos = ySize / 3 + 40;
				GUI_DispStringHCenterAt("Press 'C' to Start flappy", xPos, yPos);
				xPos = xSize / 2;
				yPos = ySize / 3 + 60;
				GUI_DispStringHCenterAt("Press 'D' to Start 2048", xPos, yPos);
    }
		if (Keypad_Get_Key() == 'C') {
        StartFlappyGame();
				GUI_SetBkColor(GUI_BLACK);
				GUI_Clear();
				xPos = xSize / 2;
				yPos = ySize / 3;
        GUI_SetColor(GUI_WHITE);
				GUI_SetTextMode(GUI_TM_REV);
				GUI_SetFont(GUI_FONT_20F_ASCII);
				GUI_DispStringHCenterAt("Press 'A' to Start snake", xPos, yPos);
				xPos = xSize / 2;
				yPos = ySize / 3 + 20;
				GUI_DispStringHCenterAt("Press 'B' to Start brick", xPos, yPos);
				xPos = xSize / 2;
				yPos = ySize / 3 +40;
				GUI_DispStringHCenterAt("Press 'C' to Start flappy", xPos, yPos);
				xPos = xSize / 2;
				yPos = ySize / 3 + 60;
				GUI_DispStringHCenterAt("Press 'D' to Start 2048", xPos, yPos);
    }
		if (Keypad_Get_Key() == 'D') {
        Start2048Game();
				GUI_SetBkColor(GUI_BLACK);
				GUI_Clear();
				xPos = xSize / 2;
				yPos = ySize / 3;
        GUI_SetColor(GUI_WHITE);
				GUI_SetTextMode(GUI_TM_REV);
				GUI_SetFont(GUI_FONT_20F_ASCII);
				GUI_DispStringHCenterAt("Press 'A' to Start snake", xPos, yPos);
				xPos = xSize / 2;
				yPos = ySize / 3 + 20;
				GUI_DispStringHCenterAt("Press 'B' to Start brick", xPos, yPos);
				xPos = xSize / 2;
				yPos = ySize / 3 +40;
				GUI_DispStringHCenterAt("Press 'C' to Start flappy", xPos, yPos);
				xPos = xSize / 2;
				yPos = ySize / 3 + 60;
				GUI_DispStringHCenterAt("Press 'D' to Start 2048", xPos, yPos);
    }

    GUI_Delay(50); 
  }
}

