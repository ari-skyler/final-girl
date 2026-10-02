#include "main.h"

#define MAX 8388607                   // 2^23. our audio fidelity is 24 bits in the math our sample oscilates between 2^23(+3v) and -2^23(-3v)
#define CENTER 30U                   // the vertical centre pixel (this row is the 0V line)
#define SCOPE_VIEW (TOTAL_FRAMES/2)   // the number of samples we use for the scope view      

uint16_t scope_trig;           // index where our trigger starts
const int32_t scope_trigger_level = 0;      // the trigger level in raw sample units (0 = the 0V line)

// scales our 24 bit sample to +/- 22 and place it around our center pixel
uint8_t scale_sample(int32_t sample) { return CENTER - ((sample * 22) / MAX); }

void scope_trigger_find(void)
{
  for (uint16_t k = 1; k <= SCOPE_VIEW; k++) // leaves at least 512 samples after the trigger (Total Frames/2)              
  {
    if (pcm5242_play_sample(k) >= scope_trigger_level && pcm5242_play_sample(k-1) < scope_trigger_level) // trace rising through the level?
    {
      scope_trig = k; 
      return; // lock the trace here
    }
  }
}

void scope_draw(void)
{
  update_coefficients(); // update filter (probably not the best place to put this)
  scope_trigger_find();                                   

  for (uint8_t col = 0; col < OLED_WIDTH; col++)           // one screen column at a time
  {
    // a and b define the range of samples in the scope window that map to this column
    uint32_t a = (col * SCOPE_VIEW) / OLED_WIDTH;          // first 0, then 5, then 10, 
    uint32_t b = ((col + 1U) * SCOPE_VIEW) / OLED_WIDTH;   // first 4, then 9, then 14, 

    uint8_t top = 255;                                     // init
    uint8_t bot = 0;                                       // init

    for (uint32_t k = a; k < b; k++)                       // scan the samples that land in this column
    {
      uint8_t row = scale_sample(pcm5242_play_sample(scope_trig + k));  // straight from the buffer
      
      if (row < top) top = row;                            // keep the highest
      if (row > bot) bot = row;                            // keep the lowest
    }

    gfx_fill_rect(col, top, 1U, (bot - top + 1U), true);   // draw the span: peak detect
  }

  char freq_cutoff[16];
  snprintf(freq_cutoff, sizeof freq_cutoff, "CUTOFF %d HZ", (int)fc);
  gfx_text(1U, 1U, freq_cutoff, DARK, SMALL);

  char output_tone[16];
  snprintf(output_tone, sizeof output_tone, "TONE %d HZ", (int)tone_hz);
  gfx_text(60U, 1U, output_tone, DARK, SMALL);
}