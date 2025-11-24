#include "snake_game.h"
#include "GUI.h"
#include "LCD.h"
#include "cmsis_os2.h"
#include <stdint.h>
#include <stdio.h>
#include "input.h"
#include "sound.h"     // <-- ADDED

#define CELL_SIZE        12
#define MAX_SNAKE_LEN    128
#define INITIAL_SPEED_MS 160

typedef struct { int x, y; } cell_t;
typedef enum { DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT } dir_t;

static cell_t snake[MAX_SNAKE_LEN];
static int snake_len;
static cell_t fruit;
static dir_t cur_dir;

static int grid_w, grid_h, pixel_w, pixel_h;

/******** RNG *********/
static uint32_t rng_state = 0x12345678;
static uint32_t rng_next(void)
{
    uint32_t x = rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return rng_state = x;
}

static void init_game(void);
static void draw_scene(void);
static int move_snake(void);
static void place_fruit(void);
static int is_collision(cell_t h);
static void game_over_screen(void);

/************************************************************
 * PUBLIC ENTRY FUNCTION
 ************************************************************/
void StartSnakeGame(void)
{
    GUI_Clear();
    init_game();

    uint32_t speed = INITIAL_SPEED_MS;

    while (1)
    {
        char key = Keypad_Get_Key();

        if (key == '2' && cur_dir != DIR_DOWN)   cur_dir = DIR_UP;
        else if (key == '8' && cur_dir != DIR_UP) cur_dir = DIR_DOWN;
        else if (key == '4' && cur_dir != DIR_RIGHT) cur_dir = DIR_LEFT;
        else if (key == '6' && cur_dir != DIR_LEFT)  cur_dir = DIR_RIGHT;

        if (key == '5')
        {
            init_game();
            speed = INITIAL_SPEED_MS;
            osDelay(200);
            continue;
        }

        int result = move_snake();

        if (result < 0)
        {
            draw_scene();
            game_over_screen();

            while (Keypad_Get_Key() != '5')
                osDelay(50);

            init_game();
            speed = INITIAL_SPEED_MS;
            osDelay(200);
            continue;
        }

        if (result > 0 && speed > 60)
            speed -= 6;

        draw_scene();
        osDelay(speed);
    }
}

/************************************************************
 * INIT GAME
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
 * DRAW SCREEN
 ************************************************************/
static void draw_scene(void)
{
    GUI_SetBkColor(GUI_BLACK);
    GUI_Clear();

    GUI_SetColor(GUI_RED);
    GUI_FillRect(
        fruit.x * CELL_SIZE,
        fruit.y * CELL_SIZE,
        fruit.x * CELL_SIZE + CELL_SIZE - 1,
        fruit.y * CELL_SIZE + CELL_SIZE - 1);

    GUI_SetColor(GUI_GREEN);

    for (int i = 0; i < snake_len; i++)
    {
        GUI_FillRect(
            snake[i].x * CELL_SIZE,
            snake[i].y * CELL_SIZE,
            snake[i].x * CELL_SIZE + CELL_SIZE - 1,
            snake[i].y * CELL_SIZE + CELL_SIZE - 1);
    }

    GUI_SetColor(GUI_WHITE);
    GUI_SetFont(GUI_FONT_13_ASCII);
    char buf[32];
    sprintf(buf, "LEN: %d", snake_len);
    GUI_DispStringAt(buf, 4, 4);
}

/************************************************************
 * MOVE SNAKE
 ************************************************************/
static int move_snake(void)
{
    cell_t head = snake[0];

    switch (cur_dir)
    {
        case DIR_UP: head.y--; break;
        case DIR_DOWN: head.y++; break;
        case DIR_LEFT: head.x--; break;
        case DIR_RIGHT: head.x++; break;
    }

    if (is_collision(head))
        return -1;

    for (int i = snake_len - 1; i > 0; i--)
        snake[i] = snake[i - 1];

    snake[0] = head;

    if (head.x == fruit.x && head.y == fruit.y)
    {
        if (snake_len < MAX_SNAKE_LEN)
            snake[snake_len++] = snake[snake_len - 1];

        place_fruit();
        Sound_EatFruit();        // <-- ADDED
        return 1;
    }

    return 0;
}

/************************************************************/
static void place_fruit(void)
{
    while (1)
    {
        int rx = rng_next() % grid_w;
        int ry = rng_next() % grid_h;

        int collision = 0;
        for (int i = 0; i < snake_len; i++)
            if (snake[i].x == rx && snake[i].y == ry)
                collision = 1;

        if (!collision)
        {
            fruit.x = rx;
            fruit.y = ry;
            return;
        }
    }
}

/************************************************************/
static int is_collision(cell_t h)
{
    if (h.x < 0 || h.x >= grid_w || h.y < 0 || h.y >= grid_h)
        return 1;

    for (int i = 0; i < snake_len; i++)
        if (snake[i].x == h.x && snake[i].y == h.y)
            return 1;

    return 0;
}

/************************************************************/
static void game_over_screen(void)
{
    GUI_SetColor(GUI_WHITE);
    GUI_SetFont(GUI_FONT_20_ASCII);
    GUI_DispStringHCenterAt("GAME OVER",
            pixel_w / 2, pixel_h / 2 - 20);

    Sound_GameOver();      // <-- ADDED
}
