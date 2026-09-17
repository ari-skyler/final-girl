#include "main.h"

#define DEBOUNCE_TIME  3U   // 3 ms debounce at 1 kHz

// magic encoder table 
static const int8_t enc_table[16] =
{
   0,  1, -1,  0,
  -1,  0,  0,  1,
   1,  0,  0, -1,
   0, -1,  1,  0
};

static volatile uint8_t enc1_position;    // encoders hold value between 0-255
static uint8_t  enc1_state;               // placeholder 2bit value for encoder A and B state 
static volatile uint8_t enc1_sw_level;    // switch level (deboucned) active low (pressed = 0)
static uint8_t  enc1_sw_count;            // debounce counter

static volatile uint8_t enc2_position;    
static uint8_t  enc2_state;                
static volatile uint8_t enc2_sw_level;    
static uint8_t  enc2_sw_count;

static volatile uint8_t enc3_position;    
static uint8_t  enc3_state;                
static volatile uint8_t enc3_sw_level;    
static uint8_t  enc3_sw_count;

static volatile uint8_t enc4_position;    
static uint8_t  enc4_state;                
static volatile uint8_t enc4_sw_level;
static uint8_t  enc4_sw_count;

static volatile uint8_t tact1_level;      // switch level (debounced) active low (pressed = 0)
static uint8_t  tact1_count;              // debounce counter

static volatile uint8_t tact2_level;      
static uint8_t  tact2_count;               

static volatile uint8_t tact3_level;      
static uint8_t  tact3_count;              

static volatile uint8_t tact4_level;      
static uint8_t  tact4_count;              

static void inputs_init(void)
{
  enc1_state    = (((GPIOC->IDR >> 0U) & 1U) << 1) | ((GPIOC->IDR >> 1U) & 1U); // initial state of encoder C0 and C1
  enc1_position = 128;  // initial position of encoder (midpoint)
  enc1_sw_level = 1U;  // active low so at rest value is high 
  enc1_sw_count   = 0U;
  
  enc2_state    = (((GPIOC->IDR >> 4U) & 1U) << 1) | ((GPIOC->IDR >> 5U) & 1U); // initial state of encoder C4 and C5
  enc2_position = 128;  // initial position of encoder (midpoint)
  enc2_sw_level = 1U;  // active low so at rest value is high 
  enc2_sw_count   = 0U;

  enc3_state    = (((GPIOB->IDR >> 12U) & 1U) << 1) | ((GPIOB->IDR >> 13U) & 1U); // initial state of encoder B12 and B13
  enc3_position = 128;  // initial position of encoder (midpoint)
  enc3_sw_level = 1U;  // active low so at rest value is high 
  enc3_sw_count   = 0U;

  enc4_state    = (((GPIOB->IDR >> 15U) & 1U) << 1) | ((GPIOC->IDR >> 6U) & 1U); // initial state of encoder B15 and C6
  enc4_position = 128;  // initial position of encoder (midpoint)
  enc4_sw_level = 1U;  // active low so at rest value is high 
  enc4_sw_count   = 0U;

  tact1_level = 1U;  // active low so at rest value is high
  tact1_count = 0U;

  tact2_level = 1U;  // active low so at rest value is high
  tact2_count = 0U;

  tact3_level = 1U;  // active low so at rest value is high
  tact3_count = 0U;

  tact4_level = 1U;  // active low so at rest value is high
  tact4_count = 0U;
}

// encoder 1: A PC0, B PC1, switch PC2
static void enc1_poll(void)
{
  uint8_t A = ((GPIOC->IDR >> 0U) & 1U);  // polls the input on C0
  uint8_t B = ((GPIOC->IDR >> 1U) & 1U);  // polls the input on C1
  uint8_t current_state = ((A << 1) | B); // encodes state into 2 bits 

  // enc table magic. store previous state in bits 2 and 3 and new state in bits 0 and 1 
  // this maps to the enc_table and whether the encoder is incrementing, decrementing, or remaining the same
  enc1_position   = enc1_position + enc_table[(enc1_state << 2) | current_state];

  enc1_state = current_state;  // store current state into old state
}

// buttons have bounce so basically we want to make sure that the levels we read remain steady so 
// basically we measure if the levels remain consistant over a debounce period. in our case it's about 3ms 
// if it does then we register that and update the ouput value (debounced)
static void enc1_sw_poll(void)
{
  uint8_t raw_input = (GPIOC->IDR >> 2U) & 1U; // polls input on C2
  
  if (raw_input == enc1_sw_level) 
  {
    enc1_sw_count = 0U;
    return;
  }

  if (enc1_sw_count++ < DEBOUNCE_TIME) return;  

  enc1_sw_count   = 0U;
  enc1_sw_level = raw_input;
}

// encoder 2: A PC4, B PC5, switch PB0
static void enc2_poll(void)
{
  uint8_t A = ((GPIOC->IDR >> 4U) & 1U);  // polls the input on C4
  uint8_t B = ((GPIOC->IDR >> 5U) & 1U);  // polls the input on C5
  uint8_t current_state = ((A << 1) | B); // encodes state into 2 bits 

  enc2_position   = enc2_position + enc_table[(enc2_state << 2) | current_state];

  enc2_state = current_state;
}
static void enc2_sw_poll(void)
{
  uint8_t raw_input = (GPIOB->IDR >> 0U) & 1U; // polls input on B0
  
  if (raw_input == enc2_sw_level) 
  {
    enc2_sw_count = 0U;
    return;
  }

  if (enc2_sw_count++ < DEBOUNCE_TIME) return;  

  enc2_sw_count   = 0U;
  enc2_sw_level = raw_input;
}

// encoder 3: A PB12, PB13, switch PB14
static void enc3_poll(void)
{
  uint8_t A = ((GPIOB->IDR >> 12U) & 1U);  // polls the input on B12
  uint8_t B = ((GPIOB->IDR >> 13U) & 1U);  // polls the input on B13
  uint8_t current_state = ((A << 1) | B); // encodes state into 2 bits 

  enc3_position   = enc3_position + enc_table[(enc3_state << 2) | current_state];

  enc3_state = current_state;
}
static void enc3_sw_poll(void)
{
  uint8_t raw_input = (GPIOB->IDR >> 14U) & 1U; // polls input on B14
  
  if (raw_input == enc3_sw_level) 
  {
    enc3_sw_count = 0U;
    return;
  }

  if (enc3_sw_count++ < DEBOUNCE_TIME) return;  

  enc3_sw_count   = 0U;
  enc3_sw_level = raw_input;
}

// encoder 4: A PB14, B PC6, switch PC7
static void enc4_poll(void)
{
  uint8_t A = ((GPIOB->IDR >> 15U) & 1U);  // polls the input on B15
  uint8_t B = ((GPIOC->IDR >> 6U) & 1U);  // polls the input on C6
  uint8_t current_state = ((A << 1) | B); // encodes state into 2 bits 

  enc4_position   = enc4_position + enc_table[(enc4_state << 2) | current_state];

  enc4_state = current_state;
}
static void enc4_sw_poll(void)
{
  uint8_t raw_input = (GPIOC->IDR >> 7U) & 1U; // polls input on C7
  
  if (raw_input == enc4_sw_level) 
  {
    enc4_sw_count = 0U;
    return;
  }

  if (enc4_sw_count++ < DEBOUNCE_TIME) return;  

  enc4_sw_count   = 0U;
  enc4_sw_level = raw_input;
}

// tact 1 PC9
static void tact1_poll(void)
{
  uint8_t raw_input = (GPIOC->IDR >> 9U) & 1U; // polls input on C9
  
  if (raw_input == tact1_level)
  {
    tact1_count = 0U;
    return;
  }

  if (tact1_count++ < DEBOUNCE_TIME) return;

  tact1_count   = 0U;
  tact1_level = raw_input;
}
// tact 2 PCA8
static void tact2_poll(void)
{
  uint8_t raw_input = (GPIOA->IDR >> 8U) & 1U; // polls input on A8
  
  if (raw_input == tact2_level)
  {
    tact2_count = 0U;
    return;
  }

  if (tact2_count++ < DEBOUNCE_TIME) return;

  tact2_count   = 0U;
  tact2_level = raw_input;
}
// tact 3 PCA9
static void tact3_poll(void)
{
  uint8_t raw_input = (GPIOA->IDR >> 9U) & 1U; // polls input on A9

  if (raw_input == tact3_level)
  {
    tact3_count = 0U;
    return;
  }

  if (tact3_count++ < DEBOUNCE_TIME) return;

  tact3_count   = 0U;
  tact3_level = raw_input;
}
// tact 4 PCA10
static void tact4_poll(void)
{
  uint8_t raw_input = (GPIOA->IDR >> 10U) & 1U; // polls input on A10
  
  if (raw_input == tact4_level)
  {
    tact4_count = 0U;
    return;
  }

  if (tact4_count++ < DEBOUNCE_TIME) return;

  tact4_count   = 0U;
  tact4_level = raw_input;
}

// input poll 
static void inputs_poll(void)
{
  enc1_poll(); enc1_sw_poll();
  enc2_poll(); enc2_sw_poll();
  enc3_poll(); enc3_sw_poll();
  enc4_poll(); enc4_sw_poll();
  tact1_poll();
  tact2_poll();
  tact3_poll();
  tact4_poll();
}