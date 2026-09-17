#include "main.h"

// oled i2c address + protocol control bytes
#define OLED_ADDR         0x3C   // oled i2c address (set once at init by oled_init via oled_probe)
#define OLED_ADDR_ALT     0x3D   // alternate oled i2c address
#define OLED_COMMAND_BYTE 0x00   // control byte for commands
#define OLED_DATA_BYTE    0x40   // control byte for pixel stuff

static uint8_t oled_address;   // oled i2c address

// send cmd
static uint8_t oled_send_cmd(uint8_t cmd)
{
  uint8_t tx[2] = {OLED_COMMAND_BYTE, cmd};  // control byte 0x00 = {"next is a command", then the command}
  return i2c1_write(oled_address, tx, 2);
}

// send command with parameter
static uint8_t oled_send_cmd2(uint8_t cmd, uint8_t param)
{
  uint8_t tx[3] = {OLED_COMMAND_BYTE, cmd, param}; // same as above but with a parameter
  return i2c1_write(oled_address, tx, 3);
}

// probes oled board with manual i2c write
static uint8_t oled_probe(void)
{
  uint8_t temp = 0x00; // random byte

  for (int k = 0; k < 6; k++)
  {
    if (i2c1_write(OLED_ADDR, &temp, 1) == OLED_OK)     return OLED_ADDR;
    if (i2c1_write(OLED_ADDR_ALT, &temp, 1) == OLED_OK) return OLED_ADDR_ALT;
    HAL_Delay(100);
  }
  return 0;  // no display found
}

// basic init setup
static void oled_hw_init(void)
{
  oled_send_cmd (0xAE);        // display OFF while configuring
  oled_send_cmd2(0xD5, 0x80);  // oscillator / clock divide ratio
  oled_send_cmd2(0xA8, 0x3F);  // multiplex ratio 1/64 (64 rows)
  oled_send_cmd2(0xD3, 0x00);  // display offset 0
  oled_send_cmd (0x40);        // display start line 0
  oled_send_cmd2(0x8D, 0x14);  // charge pump ON (drives the panel)
  oled_send_cmd2(0x20, 0x00);  // memory mode: HORIZONTAL (page auto-advances)
  oled_send_cmd (0xA1);        // segment REMAPPED: fb byte 0 = right (panel mirrors x)
  oled_send_cmd (0xC8);        // COM scan reversed: page 0 = bottom (panel mirrors y)
  oled_send_cmd2(0xDA, 0x12);  // COM pins: 128x64 "alternative" mode
  oled_send_cmd2(0x81, 0xCF);  // contrast
  oled_send_cmd2(0xD9, 0xF1);  // precharge period
  oled_send_cmd2(0xDB, 0x40);  // VCOMH deselect level
  oled_send_cmd (0xA4);        // show RAM contents (not the test pattern)
  oled_send_cmd (0xA6);        // normal video (not inverted)
  oled_send_cmd (0xAF);        // display ON
}