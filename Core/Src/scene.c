#include "main.h"

uint8_t scene_fb[OLED_FB_BYTES]; // static frame buffer

// called once to set a static image in the scene buffer
void example_scene_init(void)
{
  // rendering data into the back buffer
  gfx_clear(0x00);                         // whole screen black
  gfx_text(0U,  0U,  "FINAL GIRL");       // Render text starting at (0, 0) top left corner 
  gfx_text(0U,  16U, "SATURATION 1");
  gfx_text(0U,  32U, "LIMITER 2");

  // copy the back buffer into the scene buffer
  memcpy(scene_fb, frame_buffer[back], OLED_FB_BYTES); 
}

uint8_t  demo_ball_height = 20U;         // ball height
uint8_t  demo_ball_width = 20U;         // ball width
uint8_t  demo_ball_x;                  // bouncing-ball state
uint8_t  demo_ball_y;                 // bouncing-ball state
uint8_t  demo_ball_x_inc;  
uint8_t  demo_ball_y_inc;  

uint8_t  demo_text_x;  
uint8_t  demo_text_inc;

// demo scene creator -> bouncing square and encoder 1 level indicator 
void frame_menu_draw(void)
{
  // copy the scene background into the frame buffer
  // clears previous frame and starts us fresh with are scene image 
  memcpy(frame_buffer[back], scene_fb, OLED_FB_BYTES); 

  // text rendering
  if (demo_text_x >= OLED_WIDTH-(FONT_W*14U)) demo_text_inc = -1; 
  if (demo_text_x <= 0U) demo_text_inc = 1;
  demo_text_x = demo_text_x + demo_text_inc;
  
  gfx_text(demo_text_x,  48U, "COMPRESSOR 3");

  // bouncing ball
  if (demo_ball_x >= (OLED_WIDTH - demo_ball_width)) demo_ball_x_inc = -1;
  if (demo_ball_x <= 0U) demo_ball_x_inc = 1;
  demo_ball_x = demo_ball_x + demo_ball_x_inc;
  
  if (demo_ball_y >= (OLED_HEIGHT - demo_ball_height)) demo_ball_y_inc = -1;  
  if (demo_ball_y <= 0U)  demo_ball_y_inc = 1; 
  demo_ball_y = demo_ball_y + demo_ball_y_inc;

  gfx_fill_rect(demo_ball_x, demo_ball_y, demo_ball_width, demo_ball_height, true);

   // render encoder 1 value
  uint8_t lvl = enc1_position >> 1U; // enc1_position is uint8. oled width is 128
  gfx_fill_rect(0U, OLED_HEIGHT - 8U, lvl, 8U, true);

  if(!tact1_level) // if tact 1 is pressed draw a square
    gfx_fill_rect(100U, 20U, 20U, 20U, true);
  
  if(!enc1_sw_level) // if encoder 1 switch is pressed draw a square
    gfx_fill_rect(50U, 20U, 20U, 20U, true);
}

// scene frame controller 
#define FRAME_MENU  0U
#define SOME_OTHER_FRAME  1U
uint8_t active_frame = FRAME_MENU;   

// the worlds most robust menu system
void frame_draw(void)
{
  switch (active_frame)
  {
    case FRAME_MENU: 
      frame_menu_draw(); 
      break;

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