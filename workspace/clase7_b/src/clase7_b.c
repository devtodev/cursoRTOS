/*==================[inclusions]=============================================*/
#include "../../clase7_b/inc/blinking_echo.h"         /* <= own header */

#include "os.h"               /* <= operating system header */
#include "ciaaPOSIX_stdio.h"  /* <= device handler header */
#include "ciaaPOSIX_string.h" /* <= string header */
#include "ciaak.h"            /* <= ciaa kernel header */

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/
/** \brief File descriptor for digital input ports
 *
 * Device path /dev/dio/in/0
 */
static int32_t fd_in;

static int titilar = 200;
/** \brief File descriptor for digital output ports
 *
 * Device path /dev/dio/out/0
 */
static int32_t fd_out;

/** \brief File descriptor of the USB uart
 *
 * Device path /dev/serial/uart/1
 */
static int32_t fd_uart1;

/** \brief File descriptor of the RS232 uart
 *
 * Device path /dev/serial/uart/2
 */
static int32_t fd_uart2;

/** \brief Periodic Task Counter
 *
 */
static uint32_t Periodic_Task_Counter;

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

   ciaaPOSIX_printf("Init Task...\n");
   /* open CIAA digital inputs */
   fd_in = ciaaPOSIX_open("/dev/dio/in/0", ciaaPOSIX_O_RDONLY);

   /* open CIAA digital outputs */
   fd_out = ciaaPOSIX_open("/dev/dio/out/0", ciaaPOSIX_O_RDWR);

   /* open UART connected to USB bridge (FT2232) */
   fd_uart1 = ciaaPOSIX_open("/dev/serial/uart/1", ciaaPOSIX_O_RDWR);

   /* open UART connected to RS232 connector */
   fd_uart2 = ciaaPOSIX_open("/dev/serial/uart/2", ciaaPOSIX_O_RDWR);

   /* change baud rate for uart usb */
   ciaaPOSIX_ioctl(fd_uart1, ciaaPOSIX_IOCTL_SET_BAUDRATE, (void *)ciaaBAUDRATE_115200);

   /* change FIFO TRIGGER LEVEL for uart usb */
   ciaaPOSIX_ioctl(fd_uart1, ciaaPOSIX_IOCTL_SET_FIFO_TRIGGER_LEVEL, (void *)ciaaFIFO_TRIGGER_LEVEL3);

   /* activate example tasks */
   Periodic_Task_Counter = 0;
   titilar = 20;
   SetRelAlarm(ActivateTaskLED, titilar, titilar);

   /* Activates the SerialEchoTask task */
   ActivateTask(taskUART);

   /* end InitTask */
   TerminateTask();
}

TASK(taskUART)
{
   int8_t buf[20];   /* buffer for uart operation              */
   uint8_t outputs;  /* to store outputs status                */
   int32_t ret;      /* return value variable for posix calls  */

   char mensaje[] = "Ejercicio 7B Curso RTOS I CAPSE \n\r Carlos Miguens\n\r";
   ciaaPOSIX_write(fd_uart1, mensaje, ciaaPOSIX_strlen(mensaje));

   while(1)
   {
      /* wait for any character ... */
      ret = ciaaPOSIX_read(fd_uart1, buf, 20);

      if(ret > 0)
      {

    	  	ciaaPOSIX_write(fd_uart1, buf, ret);
    	  	if (buf[0] == 'a')
    	  	{
    	  		ciaaPOSIX_write(fd_uart1, '*', ret);
    	  	}

    	  	switch (buf[0]) {
				 case '+':
					 titilar++;
					 CancelAlarm(ActivateTaskLED);
					 SetRelAlarm(ActivateTaskLED, titilar, titilar);
					 break;
				 case '-':
					 if (titilar > 1) titilar--;

					 CancelAlarm(ActivateTaskLED);
					 SetRelAlarm(ActivateTaskLED, titilar, titilar);
					 break;
        	 }

    	  	if ((buf[0] <= '9') && (buf[0] >= '0'))
    	  	{
    	  		titilar = (buf[0] - '0') * 100;
    	  		if (titilar == 0) titilar++;
    			CancelAlarm(ActivateTaskLED);
    			SetRelAlarm(ActivateTaskLED, titilar, titilar);
    	  	}
      }
   }
}

int inputChar;

TASK(taskLED)
{
	   uint8_t outputs;

	   /* blink output */
	   ciaaPOSIX_read(fd_out, &outputs, 1);
	   outputs ^= 0x20;
	   ciaaPOSIX_write(fd_out, &outputs, 1);

	   /* terminate task */
	   TerminateTask();
}

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
/*==================[end of file]============================================*/

