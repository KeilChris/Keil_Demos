/*----------------------------------------------------------------------------
 * Name:    sine generator
 * Purpose: Freescale MKL25Z128xxx4 / Freedom Board
 *----------------------------------------------------------------------------
 *   IIR Oscillator
 *
 *   This module implements a 2nd-order IIR filter for generating
 *   sinusoid signal.
 *   There are several ways to implement a sine wave generator,
 *   such as a lookup table, interpolation, polynomials, etc.
 *   One elegant technique is using an IIR filter, making it oscillating
 *   by locating its poles on the unit circle of the Argand diagram.
 *   This implementation is based on the CMSIS-DSP Library function
 *   of the IIR filter for the q15 data format.
 * 
 *   This software is supplied "AS IS" without warranties of any kind.
 *
 *   Copyright (c) 2012 Keil - An ARM Company.
 *----------------------------------------------------------------------------*/

#include "LPC18xx.h"                    // Device header
#include "arm_math.h"
#include "sine_generator.h"

/*
*********************************************************************
*
*   Compute filter coefficients
*
*   The slides explain the theory behind the formulas...
*
*********************************************************************
*/

void sine_generator_init_q15(sine_generator_q15_t *sine_desc, q15_t sine_frequency, q15_t sampling_frequency)
{
  float32_t y[4], coeff4, coeff5;

  y[0] = 0;
  y[1] = arm_sin_f32(2 * PI * sine_frequency / sampling_frequency);
  y[2] = arm_sin_f32(2 * 2 * PI * sine_frequency / sampling_frequency);
  y[3] = arm_sin_f32(3 * 2 * PI * sine_frequency / sampling_frequency);

  coeff4 = (y[2] / y[1]) / 2 ;
  coeff5 = ((y[1] * y[3] - y[2] * y[2]) / (y[1] * y[1])) / 2;

  arm_float_to_q15(&coeff4, &(sine_desc->coeff[4]), 1);
  arm_float_to_q15(&coeff5, &(sine_desc->coeff[5]), 1);

  arm_biquad_cascade_df1_init_q15(&(sine_desc->iir_sine_generator_instance), 1, sine_desc->coeff, sine_desc->state, 1);

  arm_float_to_q15(&y[1], &(sine_desc->state[3]), 1);
}

/*
*********************************************************************
*
*   Sine sample generator
*
*********************************************************************
*/

q15_t sine_calc_sample_q15(sine_generator_q15_t *sine_desc)
{
  q15_t input, output, result;

  input = 0;
  result = 0;
  arm_biquad_cascade_df1_q15(&(sine_desc->iir_sine_generator_instance), &input, &output, 1);
  if (input >= 10000) {
    result = input * output;
  } else {
    result = result + 1;
  }
  return (output);
}
