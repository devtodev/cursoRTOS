#define TaskC 0
#define TaskB 0
#define ActivateTaskA 0
#define ActivateTaskB 0
#define AppMode1 0

/*==================[inclusions]=============================================*/
#include "os.h"               /* <= operating system header */
#include "ciaaPOSIX_stdio.h"  /* <= device handler header */
#include "ciaaPOSIX_string.h" /* <= string header */
#include "ciaak.h"            /* <= ciaa kernel header */
#include "blinking.h"         /* <= own header */


/** \brief File descriptor for digital output ports
 *
 * Device path /dev/dio/out/0
 */
static int32_t leds;


/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
/** \brief Main function
 *
 * This is the main entry point of the software.
 *
 * \returns 0
 *
 * \remarks This function never returns. Return value is only to avoid compiler
 *          warnings or errors.
 */
int main(void)
{
   /* Starts the operating system in the Application Mode 1 */
   /* This example has only one Application Mode */
   StartOS(AppMode1);

   /* StartOs shall never returns, but to avoid compiler warnings or errors
    * 0 is returned */
   return 0;
}

/** \brief Error Hook function
 *
 * This fucntion is called from the os if an os interface (API) returns an
 * error. Is for debugging proposes. If called this function triggers a
 * ShutdownOs which ends in a while(1).
 *
 * The values:
 *    OSErrorGetServiceId
 *    OSErrorGetParam1
 *    OSErrorGetParam2
 *    OSErrorGetParam3
 *    OSErrorGetRet
 *
 * will provide you the interface, the input parameters and the returned value.
 * For more details see the OSEK specification:
 * http://portal.osek-vdx.org/files/pdf/specs/os223.pdf
 *
 */
void ErrorHook(void)
{
   ciaaPOSIX_printf("ErrorHook was called\n");
   ciaaPOSIX_printf("Service: %d, P1: %d, P2: %d, P3: %d, RET: %d\n", OSErrorGetServiceId(), OSErrorGetParam1(), OSErrorGetParam2(), OSErrorGetParam3(), OSErrorGetRet());
   ShutdownOS(0);
}

/** \brief Initial task
 *
 * This task is started automatically in the application mode 1.
 */
TASK(InitTask)
{
   /* init CIAA kernel and devices */
   ciaak_start();

   /* print message (only on x86) */
   ciaaPOSIX_printf("Init Task...\n\0");

   /* open CIAA digital outputs */
   leds = ciaaPOSIX_open("/dev/dio/out/0", ciaaPOSIX_O_RDWR);
   /* activate periodic task:
    *  - for the first time after 350 ticks (350 ms)
    *  - and then every 250 ticks (250 ms)
    */
   SetRelAlarm(ActivateTaskA, 1000, 1000);

   /* terminate task */
   TerminateTask();
}

/** \brief Task A
 *
 * This task is started automatically every time that the alarm
 * ActivatePeriodicTask expires.
 *
 */
int32_t cantidad = 5, contador = 0;

#define RGB_R 	0x01
#define RGB_G 	0x02
#define RGB_B 	0x04
#define LED1 	0x08
#define LED2 	0x10
#define LED3	0x20

TASK(TaskA)
{
	uint8_t outputs;
	contador++;
	if (cantidad <= contador)
	{
		ActivateTask(TaskC);
		CancelAlarm(ActivateTaskA);

	}
	ciaaPOSIX_read(leds, &outputs, 1);
	outputs ^= LED1;
	ciaaPOSIX_write(leds, &outputs, 1);

	ChainTask(TaskB);
}

TASK(TaskB)
{
   uint8_t outputs;

   /* blink output */
   ciaaPOSIX_read(leds, &outputs, 1);
   outputs ^= LED2;
   ciaaPOSIX_write(leds, &outputs, 1);

   /* terminate task */
   TerminateTask();
}

TASK(TaskC)
{
   uint8_t outputs;

   /* blink output */
   ciaaPOSIX_read(leds, &outputs, 1);
   outputs ^= LED3;
   ciaaPOSIX_write(leds, &outputs, 1);

   /* terminate task */
   TerminateTask();
}


/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
/*==================[end of file]============================================*/

