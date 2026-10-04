void debug_draw() {

  gfx_text(1U, OLED_HEIGHT-7U, "DEBUG", LIGHT, LARGE);

  char result[64];
  char tact_switches[64];

//   PRINT ENC VALUES
  snprintf(result, sizeof result, "ENC 1: %d %d", enc1_position, enc1_sw_level);  snprintf(tact_switches, sizeof tact_switches, "SW 1: %d", tact1_level); 
  gfx_text(1U,  10U, result, LIGHT, SMALL);                                       gfx_text(60U,  10U, tact_switches, LIGHT, SMALL); 
  
  snprintf(result, sizeof result, "ENC 2: %d %d", enc2_position, enc2_sw_level);  snprintf(tact_switches, sizeof tact_switches, "SW 2: %d", tact2_level);
  gfx_text(1U,  16U, result, LIGHT, SMALL);                                       gfx_text(60U,  16U, tact_switches, LIGHT, SMALL);

  snprintf(result, sizeof result, "ENC 3: %d %d", enc3_position, enc3_sw_level);  snprintf(tact_switches, sizeof tact_switches, "SW 3: %d", tact3_level);
  gfx_text(1U,  22U, result, LIGHT, SMALL);                                       gfx_text(60U,  22U, tact_switches, LIGHT, SMALL);

  snprintf(result, sizeof result, "ENC 4: %d %d", enc4_position, enc4_sw_level);  snprintf(tact_switches, sizeof tact_switches, "SW 4: %d", tact4_level);
  gfx_text(1U,  28U, result, LIGHT, SMALL);                                       gfx_text(60U,  28U, tact_switches, LIGHT, SMALL);
}