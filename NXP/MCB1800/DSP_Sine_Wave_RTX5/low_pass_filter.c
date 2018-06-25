/*----------------------------------------------------------------------------
 * Name:    low_pass_filter.c
 * Purpose: Freescale MKL25Z128xxx4 / Freedom Board
 *----------------------------------------------------------------------------
 *
 * 
 * This module defines the filter coefficients for the low pass filter.
 * It makes use of the CMSIS-DSP Library for running the FIR filter based
 * on q15 data format.
 * 
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2012 Keil - An ARM Company.
 *----------------------------------------------------------------------------*/


#include "LPC18xx.h"                    // Device header
#include "arm_math.h"

arm_fir_instance_q15 low_pass_filter_set;

#define FILTER_TAPS      (32u)
#define FILTER_BLOCK_LEN (1u)

q15_t low_pass_filter_coeff[FILTER_TAPS] = 
{
0xFB5C,
0x021C,
0x0219, 
0x024E,  

0x02AC,   
0x0325,    
0x03B0,     
0x0445,      

0x04DC,        
0x056E,         
0x05F7,          
0x0675,           

0x06DB,            
0x072D,             
0x0766,              
0x0782,               

0x0782,                
0x0766,                 
0x072D,                  
0x06DB,                   

0x0675,                    
0x05F7,                     
0x056E,                      
0x04DC,                       

0x0445,
0x03B0,
0x0325,
0x02AC,

0x024E,
0x0219,
0x021C,
0xFB5C,
};

q15_t low_pass_filter_state[FILTER_TAPS + FILTER_BLOCK_LEN];

//

void low_pass_filter_init(void)
{
  arm_fir_init_q15(&low_pass_filter_set, FILTER_TAPS, low_pass_filter_coeff, low_pass_filter_state, FILTER_BLOCK_LEN);
}

q15_t low_pass_filter(q15_t *input)
{
  q15_t out;

  arm_fir_q15(&low_pass_filter_set, input, &out, FILTER_BLOCK_LEN);

  return out;
}
