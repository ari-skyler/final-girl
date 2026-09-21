#include "main.h"
#include <stdio.h>
#include <stdlib.h>

void  eq_draw(void)
{
  // encoder values
  uint8_t enc1 = enc1_position >> 1U;
  uint8_t enc2 = enc2_position >> 1U;
  uint8_t enc3 = enc3_position >> 1U;

  // output = output_start + ((output_end - output_start) / (input_end - input_start)) * (input - input_start)
  uint8_t freq = 10 + 172 * enc1;
  uint8_t q = 10 + 14 * enc2;
  int8_t lvl = -20 + (int)lroundf((315 * enc3)/1000);
  char number[32];
  char result[64];

  snprintf(number, sizeof number, "%d", freq);
  snprintf(result, sizeof result, "%s HZ", number);
  gfx_text(1U,  1U, result, DARK, SMALL);

  snprintf(number, sizeof number, "%d", q);
  snprintf(result, sizeof result, "Q: %s", number);
  gfx_text(OLED_WIDTH/2-13U,  1U, result, DARK, SMALL);

  snprintf(number, sizeof number, "%d", lvl);
  snprintf(result, sizeof result, "%s DB", number);
  gfx_text(OLED_WIDTH-26U,  1U, result, DARK, SMALL);


  // STATIC COMPONENTS
  gfx_text(1U, OLED_HEIGHT-7U, "EQ", LIGHT, LARGE);
  gfx_fill_rect(0U, (OLED_HEIGHT/2)-2, OLED_WIDTH, 2U, true);
  gfx_fill_rect((OLED_WIDTH/2)-2, 8U, 2U, DISPLAY_AREA_HEIGHT, true);
}