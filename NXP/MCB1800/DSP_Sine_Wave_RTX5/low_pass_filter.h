/*----------------------------------------------------------------------------
 * Name:    low_pass_filter.h
 * Purpose: Freescale MKL25Z128xxx4 / Freedom Board
 *----------------------------------------------------------------------------
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2012 Keil - An ARM Company.
 *----------------------------------------------------------------------------*/

#ifndef LOW_PASS_FILTER_H

  #define LOW_PASS_FILTER_H
	
  void low_pass_filter_init(void);
  q15_t low_pass_filter(q15_t *input);

#endif
