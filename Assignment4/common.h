#ifndef COMMON_H
#define COMMON_H

#include "device.h"
#include <string.h>

/***********************************************************************
 * ------------------------ Timer settings -----------------------------
 **********************************************************************/
#define _10MHZ	63320
#define _16MHZ	61768
#define _20MHZ	60768
#define _32MHZ	57768
#define _40MHZ 	55768

/***********************************************************************
 * ----------------------------- Events --------------------------------
 **********************************************************************/
#define SEND_COMMAND_EVENT      0x01 //Set every 50 ms to send a command
#define HASH_START_EVENT        0x02 //To start the hash computation.
#define DATA_SEND_DONE_EVENT    0x04 //Signals that the data sending is done.
#define LCD_ALARM_EVENT         0x08 //Used for timing the LCD.
#define LCD_READY_EVENT         0x10 //LCD is ready.
#define LCD_DONE_EVENT          0x80 //LCD is done.

/***********************************************************************
 * ----------------------------- Task ID -------------------------------
 **********************************************************************/
/* Info about the tasks:
 * MAIN_TASK: Basically sends all the commands depending on the status of the simulator.
 * INIT_TASK: After taking GO, activates main task.
 */
#define MAIN_TASK_ID             1
#define HASH_TASK_ID             2
#define INIT_TASK_ID             3
#define LCD_TASK_ID              4

/* Priorities of the tasks */
#define MAIN_TASK_PRIO           5
#define HASH_TASK_PRIO           5
#define INIT_TASK_PRIO           2
#define LCD_TASK_PRIO            6

#define ALARM_TSK0               0
#define LCD_ALARM_ID             1

/***********************************************************************
 * -------------------------- Command enums ----------------------------
 **********************************************************************/

typedef enum {
    GO, END, STATUS_RESPONSE,
    HASH_RESPONSE, WAIT,
    COOK, SLOW_COOK, HASH_REPLY,
    TOSS
} command;

extern char command_list[9][21];

extern command command_in;
extern command command_out;

extern unsigned int total_money;

extern unsigned char hash_received[8];
extern unsigned char hash_to_send[16];
extern unsigned char hash_done;

extern unsigned int customer_in_process[3];
extern unsigned int customer_ids[3];
extern unsigned int old_ids[3];
extern unsigned int customer_patiences[3];
extern unsigned char customer_ingredient1[3];
extern unsigned char customer_ingredient2[3];
extern unsigned char ingredient_status[4];
extern unsigned char hash_in_progress;


void sendDataISR();

/* Called in the interrupt, receives the data */
void dataReceiveISR();

// Parses the string read from the serial port. Updates command_in.
void parseBuffer();

// Starts the sending of a command
void startSendData(command cmd);


#endif

/* End of File : common.h */
