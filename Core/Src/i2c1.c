#include "main.h"

/*
    This file handles function of i2c1... this stm32 has a few i2c's we can use and 
    we are using i2c1 for the oled display. but you can also communicate w multiple devices
    on the same i2c bus if their address's are different. i figure that's why the oled can 
    have settable addresses  
*/

// i2c transfer codes
#define OLED_OK       0U    // operation completed
#define OLED_NACK     1U    // display did not ACK (display is there but not responding)
#define OLED_TIMEOUT  2U    // bus timed out (display faulted in the middle of a transfer)

// stock i2c init function
static void i2c1_bus_init(void)
{
  __HAL_RCC_I2C1_CLK_ENABLE();
  __HAL_RCC_I2C1_FORCE_RESET();
  __HAL_RCC_I2C1_RELEASE_RESET();

  I2C1->TIMINGR = 0x60442831U;                    // 400kHz baud (oled's limit)
  I2C1->OAR1    = 0U;                             // setup as master
  I2C1->CR2     = I2C_CR2_AUTOEND | I2C_CR2_NACK; // set i2c auto generates nack and stop flag
  I2C1->CR1     = I2C_CR1_PE;                     // enable the i2c peripheral

  HAL_NVIC_SetPriority(I2C1_EV_IRQn, 6U, 0U); // set priority for I2C1 event interrupt
  HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);           // enable the I2C1 event interrupt
}

// waits for either the i2c bus to ask for the next byte (txis) or to have failed (nack)
static int i2c1_wait_tx_ready(uint32_t timeout_ms)
{
  uint32_t time_0 = HAL_GetTick();

  while ((I2C1->ISR & (I2C_ISR_TXIS | I2C_ISR_NACKF)) == 0) // exit while loop if transmit wants next byte or nacked
    if ((HAL_GetTick() - time_0) >= timeout_ms) return -1;  // error code if timeout reached

  return 0; // woohoo
}

// this function waits for the i2c bus to become idle within a specified timeout
static int i2c1_wait_idle(uint32_t timeout_ms)
{
  uint32_t time_0 = HAL_GetTick(); // get the current time
  while ((I2C1->ISR & I2C_ISR_BUSY) != 0) // check to see if the i2c bus is busy
  {
    if ((HAL_GetTick() - time_0) >= timeout_ms) // if it is busy check to see if its been busy for as long as the input timeout
    {
      return -1; // if it has return error code
    }
  }
  return 0; // bus is chill and available
}

// configure the i2c control register 2 to start a transfer with the specified address and length
static void i2c1_cr2_start(uint8_t addr, uint16_t len)
{
  I2C1->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_AUTOEND | I2C_CR2_START | I2C_CR2_STOP); // clear relevant CR2 data
  I2C1->CR2 |= (addr << 1U & I2C_CR2_SADD);          // set the target address (our oled address left shifted by 1 to leave room for the read/(write) bit)
  I2C1->CR2 |= (len << I2C_CR2_NBYTES_Pos);          // set the number of bytes to transmit
  I2C1->CR2 |= I2C_CR2_AUTOEND | I2C_CR2_START;      // enable autoend and start the transfer
}

// blocking write on the i2c bus
static uint8_t i2c1_write(uint8_t addr, const uint8_t *bytes, uint16_t len)
{
  if (i2c1_wait_idle(50U) < 0U) return OLED_TIMEOUT; // wait up to 50ms for oled to be available (if it's not, we timeout)

  I2C1->ICR = I2C_ICR_NACKCF | I2C_ICR_STOPCF;     // disable interupt flags in anticipation of blocking i2c transfer

  i2c1_cr2_start(addr, len);                       // configure i2c bus and starts the transfer with the address byte
  for (uint16_t k = 0U; k < len; k++)
  {
    if (i2c1_wait_tx_ready(50U) < 0U)  // waits for the i2c to request the next byte (good) or hit a nack (bad)
      return OLED_TIMEOUT;             // if it recieves neither in the timeframe

    if ((I2C1->ISR & I2C_ISR_NACKF) != 0U) // checks if nack was set
    {
      I2C1->ICR = I2C_ICR_NACKCF;          // clear the nack flag
      return OLED_NACK;                    // return nack error
    }

    I2C1->TXDR = bytes[k];                   // add byte to data register which automatically clears the txis bit -
  }

  if (i2c1_wait_idle(200U) < 0U)             // waiting for transmission to complete
    return OLED_TIMEOUT;                     // timed out transmitting

  if ((I2C1->ISR & I2C_ISR_NACKF) != 0U)     // checks if the last byte was recieved or triggered a NACK
  {
    I2C1->ICR = I2C_ICR_NACKCF;              // clear nack flag
    return OLED_NACK;
  }

  I2C1->ICR = I2C_ICR_STOPCF;                // clear the stop flag when things finish up
  return OLED_OK;                            // success
}
