/*----------------------------------------------------------------------------
 * Name:    sine_generator.h
 * Purpose: Freescale MKL25Z128xxx4 / Freedom Board
 *----------------------------------------------------------------------------
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2012 Keil - An ARM Company.
 *----------------------------------------------------------------------------*/

#ifndef SINE_GENERATOR_H

  #define SINE_GENERATOR_H
	
	typedef struct 
	{
	  arm_biquad_casd_df1_inst_q15 iir_sine_generator_instance;
	  q15_t coeff[6];
	  q15_t state[4];
	} sine_generator_q15_t;
	
  void sine_generator_init_q15(sine_generator_q15_t *sine_desc, q15_t sine_frequency, q15_t sampling_frequency);
  q15_t sine_calc_sample_q15(sine_generator_q15_t *sine_desc);

#endif
