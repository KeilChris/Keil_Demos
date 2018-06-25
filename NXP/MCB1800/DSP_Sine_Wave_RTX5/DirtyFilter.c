/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    BLinky.c
 *      Purpose: RTX example program
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2014 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include "cmsis_os2.h"                  // ARM::CMSIS:RTOS2:Keil RTX5
#include "LPC18xx.h"
#include <stdlib.h>
#include "arm_math.h"
#include "sine_generator.h"
#include "low_pass_filter.h"
#include <stdio.h>
#include "EventRecorder.h"              // Keil.ARM Compiler::Compiler:Event Recorder

osThreadId_t tid_sine_gen;
osThreadId_t tid_noise_gen;
osThreadId_t tid_disturb_gen;
osThreadId_t tid_filter_thread;
osThreadId_t tid_sync_tsk;

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <e>Oscillator Configuration
// <i>This is a demonstration on how to use the Configuration Wizard to create an easy configuration menu.

#define ENABLE_CONFIG 1
// =============================
//   <o>Oscillator Sampling Frequency [Hz] <1000-10000>
//   <i> Set the oscillator sampling frequency.
//   <i> Default: 5000  (5 KHz)
#define SAMPLING_FREQ 10000  // generating task (5 KHz)

//   <o>Noise Frequency [Hz] <500-10000>
//   <i> Set the noise signal frequency.
//   <i> Default: 1500 Hz
#define NOISE_FREQ    500  // noise (2 KHz)

//   <o>Signal Frequency [Hz] <100-1000>
//   <i> Set the signal frequency.
//   <i> Default: 330 Hz
#define SIGNAL_FREQ    100  // disturbed signal (250 Hz)

// </e>
//------------- <<< end of configuration section >>> -----------------------

void SystemInit (void);

sine_generator_q15_t Signal_set;
sine_generator_q15_t Noise_set;

q15_t sine;
q15_t noise;
q15_t disturbed;
q15_t filtered;

/*
*********************************************************************
*
* Sine Generator thread
*
*********************************************************************
*/

void sine_gen(void *argument)
{
  for (;;)
  {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		osDelay(1);
    sine = sine_calc_sample_q15(&Signal_set) / 2;
		osThreadFlagsSet(tid_noise_gen, 0x0001);                 /* set signal to thread 'noise_gen' */
  }
}

/*
*********************************************************************
*
* Noise Sine Generator thread
*
*********************************************************************
*/

void noise_gen(void *argument)
{
  for (;;)
  {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, 0);
		osDelay(1);
    noise = sine_calc_sample_q15(&Noise_set) / 6;
		osThreadFlagsSet(tid_disturb_gen, 0x0001);                 /* set signal to thread 'disturb_gen' */
  }
}

/*
*********************************************************************
*
* 1st Disturb Generator thread
*
* Adds Noise to Signal
*
*********************************************************************
*/

void disturb_gen(void *argument)
{
  for (;;)
  {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, 0);
		osDelay(1);
    //disturbed = sine * noise;
    disturbed = sine + noise;
		osThreadFlagsSet(tid_filter_thread, 0x0001);                 /* set signal to thread 'filter_thread' */
  }
}

/*
*********************************************************************
*
* Filter thread
*
* Extract Signal from disturbed signal by filtering out Noise
*
*********************************************************************
*/

void filter_thread(void *argument)
{
  for (;;)
  {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, 0);
		osDelay(1);
    filtered = low_pass_filter(&disturbed); 
		osThreadFlagsSet(tid_sine_gen, 0x0001);                 /* set signal to thread 'sine_gen' */
  }
}


/*----------------------------------------------------------------------------
 *      Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
void app_main (void *argument) {

	// compute coefficients for IIR sine generators
	sine_generator_init_q15(&Noise_set, NOISE_FREQ, SAMPLING_FREQ);
  sine_generator_init_q15(&Signal_set, SIGNAL_FREQ, SAMPLING_FREQ);
  
	// initialize low pass filter
  low_pass_filter_init();
	
  tid_sine_gen  = osThreadNew(sine_gen, NULL, NULL);
  tid_noise_gen = osThreadNew(noise_gen, NULL, NULL);
  tid_disturb_gen = osThreadNew(disturb_gen, NULL, NULL);
  tid_filter_thread = osThreadNew(filter_thread, NULL, NULL);

  osThreadFlagsSet(tid_sine_gen, 0x0001);                          /* set signal to phaseA thread   */

  osDelay(osWaitForever);
  while(1);
}

int main(void) {
	
  // System Initialization
  SystemCoreClockUpdate();
  EventRecorderInitialize(EventRecordAll, 1); // Initialize the Event Recorder
  osKernelInitialize();                       // Initialize CMSIS-RTOS
  osThreadNew(app_main, NULL, NULL);          // Create application main thread
  if (osKernelGetState() == osKernelReady) {
    osKernelStart();                          // Start thread execution
  }
  for(;;);
}
