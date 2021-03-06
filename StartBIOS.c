/*
 * CCSv6 project using TI-RTOS
 *
 */

/*
 *  ======== StartBIOS.c ========
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/Timer.h>
#include <ti/sysbios/knl/Event.h>

/* TI-RTOS Header files */
#include <ti/drivers/UART.h>


/*Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

/*application header files*/
#include <ctype.h>
#include <string.h>


#include <Blink_Task.h>
#include <UART_Task.h>



int main(void)
{
    uint32_t ui32SysClock;
    static led_descriptor_t led_d[1];
	/* Call board init functions. */
	ui32SysClock = Board_initGeneral(120*1000*1000);
	(void)ui32SysClock; // We don't really need this (yet)

	Event_Handle myEvent;
	Error_Block eb;
	Error_init(&eb);
	/* Default instance configuration params */
	myEvent = Event_create(NULL, &eb);
	if (myEvent == NULL) {
		System_abort("Event create failed");
	}
	System_printf("Created Event\n");
	System_flush();

	led_d[0].port_base = GPIO_PORTF_BASE;
	led_d[0].led = GPIO_PIN_0;


	/*Initialize+start Blink Task*/
	(void) setup_Blink_Task(&led_d[0], myEvent); //1
	System_printf("Created Blink Task\n");
	System_flush();

	(void) setup_Clock_Task(500, myEvent); //1
	System_printf("Created Clock Task\n");
	System_flush();


	/*Initialize+start UART Task*/
	(void) setup_UART_Task();
	System_printf("Created UART Task\n");

    /* SysMin will only print to the console upon calling flush or exit */

    System_printf("Start BIOS\n");
    System_flush();

    /* Start BIOS */
    BIOS_start();

}
