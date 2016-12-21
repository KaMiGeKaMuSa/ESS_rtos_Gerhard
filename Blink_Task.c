/*
 *  ======== Blink_Task.c ========
 *  Author: Michael Kramer / Matthias Wenzl
 */
#include <stdbool.h>
#include <inc/hw_memmap.h>


/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>


/*Board Header files */
#include <Blink_Task.h>
#include <Board.h>
#include <EK_TM4C1294XL.h>


#include <ctype.h>
#include <string.h>

/*
 *  ======== Blink  ========
 *  Perform Blink Operation on led given at arg0
 */
void BlinkFxn(UArg arg0 , UArg arg1)
{
	led_descriptor_t *led_d = (led_descriptor_t *)arg0;
	/*gpio driverlib api uses same bit pattern for gpio mask and value*/
	uint8_t ui8val = (uint8_t)led_d->led;

	while(1) {

	ui8val ^= (uint8_t)led_d->led;//initially off
	GPIOPinWrite (led_d->port_base, led_d->led, ui8val);
	//event_pend
	Event_pend((Event_Handle)arg1, Event_Id_00, Event_Id_NONE, BIOS_WAIT_FOREVER);
	}
}


/*
 *  setup task function
 */
int setup_Blink_Task(led_descriptor_t *led_d, Event_Handle myEvent)
{
	Task_Params taskLedParams;
	Task_Handle taskLed;
	Error_Block eb;

    /*configure gpio port_base according to led*/
	GPIOPinTypeGPIOOutput(led_d->port_base, led_d->led);
	
    /* Create Blink task with priority 15*/
    Error_init(&eb);
    Task_Params_init(&taskLedParams);
    taskLedParams.stackSize = 1024;/*stack in bytes*/
    taskLedParams.priority = 15;/*0-15 (15 is highest priority on default -> see RTOS Task configuration)*/
    taskLedParams.arg0 = (UArg) led_d;/*pass led descriptor to arg0*/
    taskLedParams.arg1 = (UArg) myEvent;
    taskLed = Task_create((Task_FuncPtr)BlinkFxn, &taskLedParams, &eb);
    if (taskLed == NULL) {
    	System_abort("TaskLed create failed");
    }

    return (0);
}


void ClockFxn(Event_Handle myEvent)
{
	Event_post(myEvent, Event_Id_00);
}


int setup_Clock_Task(uint32_t wait_ticks, Event_Handle myEvent)
{
	Clock_Params clockParams;
	Clock_Handle myClock;
	Error_Block eb;
	Error_init(&eb);
	Clock_Params_init(&clockParams);
	clockParams.period = wait_ticks;
	clockParams.startFlag = TRUE;
	clockParams.arg = (UArg) myEvent;
	myClock = Clock_create((Clock_FuncPtr)ClockFxn, 1, &clockParams, &eb);
	if (myClock == NULL) {
	System_abort("Clock create failed");
	}
    return (0);
}

