
#include "common.h"
#include "lcd.h"

/**
 *
 * Initializer Task
 * Initializes LCD and enables serial receive interrupt.
 * Starts the program after GO command then terminates.
 *
 */

unsigned char init_str[2][16] = {"    COOKING     ", "      SIM       "};

TASK(INIT_TASK)
{
	//WaitEvent(LCD_DONE_EVENT);
	//ClearEvent(LCD_DONE_EVENT);;
	PIE1bits.RCIE = 1;
	while (1) {
		if (command_in == GO) {
			ActivateTask(MAIN_TASK_ID);
			SetEvent(MAIN_TASK_ID, DATA_SEND_DONE_EVENT);
			break;
		}
	}
	TerminateTask();
}

/* End of File : init_task.c */
