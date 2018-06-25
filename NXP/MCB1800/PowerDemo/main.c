/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "Board_ADC.h"                  // ::Board Support:A/D Converter
#include "Board_Buttons.h"              // ::Board Support:Buttons
#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCB1800::Board Support:Graphic LCD
 
#ifdef RTE_Compiler_EventRecorder
#include "EventRecorder.h"
#endif
 
#ifndef RTE_CMSIS_RTOS2_RTX5
  #error  This example works only with CMSIS-RTOS2 RTX5 implementation!
#endif

extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

osThreadId_t tid_thread_led;                /* Thread id of thread: thread_led      */
osThreadId_t tid_thread_adc;                /* Thread id of thread: thread_adc      */

volatile uint32_t delay_val = 100;


/*-----------------------------------------------------------------------------
 *        Initialize On Board LCD Module
 *----------------------------------------------------------------------------*/
static void init_display () {

  GLCD_Initialize         ();
  GLCD_SetBackgroundColor (GLCD_COLOR_BLUE);
  GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
  GLCD_ClearScreen        ();
  GLCD_SetFont            (&GLCD_Font_16x24);
  GLCD_DrawString         (0, 0*24, " ");
  GLCD_DrawString         (0, 1*24, " ");
  GLCD_DrawString         (0, 2*24, "      MCB1800");
  GLCD_DrawString         (0, 3*24, "  Evaluation Board");
  GLCD_DrawString         (0, 4*24, "    Arm Keil MDK");
  GLCD_DrawString         (0, 5*24, "     ULINKplus");
  GLCD_DrawString         (0, 6*24, "     Power Demo");
}


/*-----------------------------------------------------------------------------
 *        LED thread
 *----------------------------------------------------------------------------*/
void thread_led (void *argument) {
  uint32_t led_cnt = LED_GetCount();
  uint32_t led_num = 0;

  while (1) {
#ifdef RTE_Compiler_EventRecorder
	EventStartA(0);
#endif
    LED_On(led_num);                    // Turn specified LED on
    osDelay(delay_val);                 // Wait for delay_val period of time
    LED_Off(led_num);                   // Turn specified LED off
    led_num++;                          // Change LED number
    if (led_num >= led_cnt) {
      led_num = 0;                      // Restart with first LED
    }
#ifdef RTE_Compiler_EventRecorder
	EventStopA(0);
#endif
  }
}


/*-----------------------------------------------------------------------------
 *        ADC thread
 *----------------------------------------------------------------------------*/
void thread_adc (void *argument) {

  while (1) {
#ifdef RTE_Compiler_EventRecorder
	EventStartB(0);
#endif
    ADC_StartConversion();                              // start ADC conversion
    osDelay(20);                                        // wait 20ms

    if (ADC_ConversionDone() == 0) {
      delay_val = ((uint32_t)ADC_GetValue() >> 4) + 10U;// scale delay value
    } 
#ifdef RTE_Compiler_EventRecorder
	EventStopB(0);
#endif
  }
}


/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void app_main (void *argument) {
 
  tid_thread_led = osThreadNew(thread_led, NULL, NULL);
  tid_thread_adc = osThreadNew(thread_adc, NULL, NULL);
  
  osDelay(delay_val);                                            // put thread into blocked state
}
 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	
	init_display();
	
#ifdef RTE_Compiler_EventRecorder
  // Initialize and start Event Recorder
  EventRecorderInitialize(EventRecordAll, 1U);
#endif
  LED_Initialize();                         /* Initialize LED                 */
  Buttons_Initialize();                     /* Initialize Buttons             */
  ADC_Initialize();                         /* Initialize ADC                 */
 
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(app_main, NULL, NULL);    // Create application main thread
  osKernelStart();                      // Start thread execution
  osDelay(osWaitForever);               // put thread into blocked state forever
}
