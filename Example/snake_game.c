#include "snake_game.h"
#include "GUI.h"
#include "LCD.h"
#include "cmsis_os2.h"
#include <stdint.h>
#include <stdio.h>    // Added for sprintf
#include "input.h"    // Includes Keypad functions

/************************************************************
 * SNAKE GAME – COMPLETE STANDALONE ENGINE
 ************************************************************/

#define CELL_SIZE        12
#define MAX_SNAKE_LEN    128
#define INITIAL_SPEED_MS 160

typedef struct { int x, y; } cell_t;
typedef enum { DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT } dir_t;

/*********** GLOBAL GAME STATE  ***********/
static cell_t snake[MAX_SNAKE_LEN];
static int snake_len;
static cell_t fruit;
static dir_t cur_dir;

static int grid_w, grid_h, pixel_w, pixel_h;

/*********** INTERNAL FUNCTION PROTOTYPES  ***********/
static void     init_game(void);
static void     draw_scene(void);
static int      move_snake(void);
static void     place_fruit(void);
static int      is_collision(cell_t h);
static uint32_t rng_next(void);
static void     game_over_screen(void);

/*********** PSEUDO-RNG  ***********/
static uint32_t rng_state = 0x12345678;
static uint32_t rng_next(void)
{
    uint32_t x = rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return rng_state = x;
}

/************************************************************
 * PUBLIC FUNCTION – ENTRY POINT FROM app_main.c
 ************************************************************/
void StartSnakeGame(void)
{
    GUI_Clear();
    init_game();
		static uint32_t last_input_time = 0;
    uint32_t current_time;
    uint32_t debounce_delay = 150; // 150ms between inputs
    uint32_t speed = INITIAL_SPEED_MS;

    while (1)
    {
        /* ------------------------------
         * MATRIX KEYPAD CONTROL
         * ------------------------------ */
        char key = Keypad_Get_Key();
        current_time = osKernelGetTickCount();
			if (key != 0 && (current_time - last_input_time > debounce_delay))
        {
            
         if (key == '2' && cur_dir != DIR_DOWN) {
            cur_dir = DIR_UP;
        }
        else if (key == '8' && cur_dir != DIR_UP) {
            cur_dir = DIR_DOWN;
        }
        else if (key == '4' && cur_dir != DIR_RIGHT) {
            cur_dir = DIR_LEFT;
        }
        else if (key == '6' && cur_dir != DIR_LEFT) {
            cur_dir = DIR_RIGHT;
        }

        /* Hard reset (Key '5') */
        if (key == '#')
        {
            return;
        }
				 if (key == '#')
        {
            return;
        }
				 if (key == 'A')
        {
						init_game();
            speed = INITIAL_SPEED_MS;
            osDelay(200); // Wait for key release
            continue;
        }
            last_input_time = current_time;
        }

        /* Direction Control - Maps 2,4,6,8 to directions */
        /* Logic ensures we cannot reverse directly into ourselves */
        

        /* Move snake */
        int result = move_snake();

        /* Game Over */
        if (result < 0)
        {
            draw_scene();
            game_over_screen();

            // Wait for Key '5' to restart
            while (Keypad_Get_Key() != 'A') {                
							if (Keypad_Get_Key() == '#'){return;}
							osDelay(50);
							
            }

            init_game();
            speed = INITIAL_SPEED_MS;
            osDelay(200); // Wait for key release
            continue;
        }

        /* Fruit eaten - speed up */
        if (result > 0 && speed > 60)
            speed -= 5;

        draw_scene();
        osDelay(speed);
    }
}

/************************************************************
 * INITIALIZE GAME
 ************************************************************/
static void init_game(void)
{
    pixel_w = LCD_GetXSize();
    pixel_h = LCD_GetYSize();

    grid_w = pixel_w / CELL_SIZE;
    grid_h = pixel_h / CELL_SIZE;

    if (grid_w < 10) grid_w = 10;
    if (grid_h < 10) grid_h = 10;

    int cx = grid_w / 2;
    int cy = grid_h / 2;

    snake_len = 2;

    for (int i = 0; i < snake_len; i++)
    {
        snake[i].x = cx - i;
        snake[i].y = cy;
    }

    cur_dir = DIR_RIGHT;
    place_fruit();
}

/************************************************************
 * DRAW EVERYTHING
 ************************************************************/
static void draw_scene(void)
{
    GUI_SetBkColor(GUI_BLACK);
    GUI_Clear();

    /* Fruit */
    GUI_SetColor(GUI_RED);
    GUI_FillRect(
        fruit.x * CELL_SIZE,
        fruit.y * CELL_SIZE,
        fruit.x * CELL_SIZE + CELL_SIZE - 1,
        fruit.y * CELL_SIZE + CELL_SIZE - 1
    );

    /* Snake */
    GUI_SetColor(GUI_GREEN);
    for (int i = 0; i < snake_len; i++)
    {
        GUI_FillRect(
            snake[i].x * CELL_SIZE,
            snake[i].y * CELL_SIZE,
            snake[i].x * CELL_SIZE + CELL_SIZE - 1,
            snake[i].y * CELL_SIZE + CELL_SIZE - 1
        );
    }

    /* Score */
    GUI_SetColor(GUI_WHITE);
    GUI_SetFont(GUI_FONT_13_ASCII);
    char buf[32];
    sprintf(buf, "LEN: %d", snake_len);
    GUI_DispStringAt(buf, 4, 4);
}

/************************************************************
 * MOVE SNAKE FORWARD ONE STEP
 ************************************************************/
static int move_snake(void)
{
    cell_t head = snake[0];

    /* Direction logic */
    switch (cur_dir)
    {
        case DIR_UP:    head.y--; break;
        case DIR_DOWN:  head.y++; break;
        case DIR_LEFT:  head.x--; break;
        case DIR_RIGHT: head.x++; break;
    }
		/* WRAPPING LOGIC: Loop to opposite side */
    if (head.x < 0)             head.x = grid_w - 1;
    else if (head.x >= grid_w)  head.x = 0;

    if (head.y < 0)             head.y = grid_h - 1;
    else if (head.y >= grid_h)  head.y = 0;

    if (is_collision(head))
        return -1;   // hit wall or itself

    for (int i = snake_len - 1; i > 0; i--)
        snake[i] = snake[i - 1];

    snake[0] = head;

    /* Fruit eaten */
    if (head.x == fruit.x && head.y == fruit.y)
    {
        if (snake_len < MAX_SNAKE_LEN)
            snake[snake_len++] = snake[snake_len - 1];

        place_fruit();
        return 1;
    }

    return 0;
}

/************************************************************
 * RANDOM FRUIT POSITION
 ************************************************************/
static void place_fruit(void)
{
    while (1)
    {
        int rx = rng_next() % grid_w;
        int ry = rng_next() % grid_h;

        int collision = 0;
        for (int i = 0; i < snake_len; i++)
        {
            if (snake[i].x == rx && snake[i].y == ry)
            {
                collision = 1;
                break;
            }
        }

        if (!collision)
        {
            fruit.x = rx;
            fruit.y = ry;
            return;
        }
    }
}

/************************************************************
 * COLLISION CHECK
 ************************************************************/
static int is_collision(cell_t h)
{
    for (int i = 0; i < snake_len; i++)
        if (snake[i].x == h.x && snake[i].y == h.y)
            return 1;

    return 0;
}

/************************************************************
 * GAME OVER SCREEN
 ************************************************************/
static void game_over_screen(void)
{
    GUI_SetColor(GUI_WHITE);
    GUI_SetFont(GUI_FONT_20_ASCII);
    GUI_DispStringHCenterAt("GAME OVER", pixel_w / 2, pixel_h / 2 - 20);
}
