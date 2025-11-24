#ifndef __SOUND_H
#define __SOUND_H

#include <stdint.h>

// Initialize the Sound hardware (Timer/PWM)
void Sound_Init(void);

// Play the "Eat Fruit" sound effect (Short, high pitch)
void Sound_EatFruit(void);

// Play the "Game Over" sound effect (Descending tones)
void Sound_GameOver(void);

// Generic function to play a tone
// frequency: in Hz
// duration_ms: in milliseconds
void Sound_Tone(uint32_t frequency, uint32_t duration_ms);

#endif /* __SOUND_H */