#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include "eq.c"
#include "comp.c"
#include "splash.c"

uint8_t scene_fb[OLED_FB_BYTES]; // static frame buffer

// top-level scene list
#define SPLASH  0U
#define EQ  1U
#define COMP 2U

uint8_t active_frame = EQ;

// single click button logic
bool left_press = false; 
bool right_press = false;
void update_nav_key_press_state(void)
{
  left_press = !tact2_level;
  right_press = !tact3_level;
}

void listen_for_nav(void) {
    // fn + left to go to previous module
    if (!tact1_level && !tact2_level && !left_press && active_frame > SPLASH){
      active_frame = active_frame - 1;
    }
        // fn + right to go to next module
    if (!tact1_level && !tact3_level && !right_press && active_frame < COMP){
      active_frame = active_frame + 1;
    }
    update_nav_key_press_state();
}

void switch_scene(void (*callback)())
{
  memcpy(frame_buffer[back], scene_fb, OLED_FB_BYTES);
  callback();
}

// the worlds most robust menu system
void frame_draw(void)
{
  listen_for_nav();
  switch (active_frame)
  {
    case SPLASH:
      switch_scene(splash_draw);
      break;
    case EQ:
      switch_scene(eq_draw);
      break;
    case COMP:
      switch_scene(comp_draw);

    default: break; // draw nothing
  }
}

// called on every pass of the main loop's while(1) loop 
// renders one frame when its turn comes every 25ms abt 40fps  
uint32_t next_tick = 0U;
void scene_tick(void)
{
  if ((int32_t)(HAL_GetTick() - next_tick) < 0) return;  // not a new 25 ms frame yet
  
  next_tick += 25U;                                    // advance it 25ms 
  frame_draw();                                       // paint active frame into the back buffer
  oled_push();                                       // pushes the back buffer to the oled
}