#include "main.h"

// https://www.w3.org/TR/audio-eq-cookbook/

void set_coefficients(float hz);
void update_coefficients(void);
void audio_process_init(void);
void audio_process(float frame[2]);

#define Q     0.707f    // buttersworth response
#define Fs    96000.0f  // sample rate
#define PI    3.14159265f
#define TWOPI 6.2831853f

float w0;
float cosw;
float sinw;
float alpha;

float a0, a1, a2, b0, b1, b2;
float x_1[2], x_2[2], y_1[2], y_2[2];
float x, y;

float fc = 1000.0f;   
uint8_t last_enc1; 

// right from the website
void set_coefficients(float hz)
{
  fc = hz;
  w0    = TWOPI * (fc / Fs);
  cosw  = cosf(w0);
  sinw  = sinf(w0);
  alpha = sinw / (2.0f * Q);

  // lpf coefficient formulas 
  a0 =   1.0f + alpha;
  b0 = ((1.0f - cosw) / 2.0f) / a0;
  b1 =  (1.0f - cosw)         / a0;
  b2 = ((1.0f - cosw) / 2.0f) / a0;
  a1 = (-2.0f * cosw)         / a0;
  a2 =  (1.0f - alpha)        / a0;
  //

  /* hpf coefficient formulas 
  a0 =   1.0f + alpha;
  b0 = ((1.0f + cosw) / 2.0f) / a0;
  b1 =(-(1.0f + cosw))        / a0;
  b2 = ((1.0f + cosw) / 2.0f) / a0;
  a1 = (-2.0f * cosw)         / a0;
  a2 =  (1.0f - alpha)        / a0;
  */

  /* bpf coefficient formulas
  a0 =   1.0f + alpha;
  b0 =   alpha       / a0;
  b1 =   0.0f;
  b2 =  -alpha      / a0;
  a1 = (-2.0f * cosw)  / a0;
  a2 =  (1.0f - alpha) / a0;
  */
}

// the equation on the The most straight forward implementation would be the "Direct Form 1" line
void audio_process(float frame[2])
{
  // L sample
  x = frame[0];
  y = b0*x + b1*x_1[0] + b2*x_2[0] - a1*y_1[0] - a2*y_2[0];

  x_2[0] = x_1[0];  
  x_1[0] = x;

  y_2[0] = y_1[0]; 
  y_1[0] = y;

  frame[0] = y;

  // R sample
  x = frame[1];
  y = b0*x + b1*x_1[1] + b2*x_2[1] - a1*y_1[1] - a2*y_2[1];

  x_2[1] = x_1[1];  
  x_1[1] = x;

  y_2[1] = y_1[1]; 
  y_1[1] = y;

  frame[1] = y;
}

// fc controlled by encoder 1
void update_coefficients(void)
{
  if (enc1_position == last_enc1) return;

  float k = (float)enc1_position / 255.0f;
  set_coefficients(20 * powf(20000 / 20, k));

  last_enc1 = enc1_position;
}

void audio_process_init(void){ set_coefficients(fc); }