#include "common.h"
#include "lcd.h"
#include "stdlib.h"

command cmd_out_ = -1;

unsigned char send_ind = 0;

unsigned char receive_buf[21];
unsigned char receive_ind = 0;
unsigned char receive_f = 0;

command command_in = -1;
command command_out = WAIT;

unsigned int total_money = 0;


unsigned char hash_received[8] = {0};
unsigned char hash_to_send[16] = {0};
unsigned char hash_done = 0;


unsigned int customer_in_process[3] = {0};
unsigned int old_ids[3] = {0};
unsigned int customer_ids[] = {0};
unsigned int customer_patiences[] = {0};
unsigned char customer_ingredient1[] = {0};
unsigned char customer_ingredient2[] = {0};
unsigned char ingredient_status[] = {0};

char command_list[9][22] = {
    "GO:X", "END:X", "R##################:X",
    "H########:X", "W:X", "C###:X", "S##:X",
    "H################:X", "T#:X"
};


void startSendData(command cmd)
{   
    cmd_out_ = cmd;
    send_ind = 0;
    TXREG = '$';
}

void sendDataISR()
{
    if(command_list[cmd_out_][send_ind] == 'X'){
        cmd_out_ = WAIT;
        
        SetEvent(MAIN_TASK_ID, DATA_SEND_DONE_EVENT);
        return;
    }
    TXREG = command_list[cmd_out_][send_ind++];
}

void parseBuffer()
{
    int index, i;

	switch (receive_buf[0]) {
	case 'G':
		//GO response
        command_in = GO;
        
		break;
        
	case 'E':
		//END response
        command_in = END;
        
		break;
        
	case 'R':
		//Status response
        command_in = STATUS_RESPONSE;
        
        customer_ids[0] = receive_buf[1];
        customer_ingredient1[0] = receive_buf[2];
        customer_ingredient2[0] = receive_buf[3];
        customer_patiences[0] = receive_buf[4];
        
        customer_ids[1] = receive_buf[5];
        customer_ingredient1[1] = receive_buf[6];
        customer_ingredient2[1] = receive_buf[7];
        customer_patiences[1] = receive_buf[8];
        
        customer_ids[2] = receive_buf[9];
        customer_ingredient1[2] = receive_buf[10];
        customer_ingredient2[2] = receive_buf[11];
        customer_patiences[2] = receive_buf[12];
        
        ingredient_status[0] = receive_buf[13];
        ingredient_status[1] = receive_buf[14];
        ingredient_status[2] = receive_buf[15];
        ingredient_status[3] = receive_buf[16];
        
        
        total_money = receive_buf[17];
        total_money << 8;
        total_money += receive_buf[18];
        
        for(i = 0; i < 4; i++){
            if(old_ids[i] != customer_ids[i]){
                old_ids[i] = customer_ids[i];
                customer_in_process[i] = 0;              
            }
        }
        
		break;
        
	case 'H':
		//Hash response
        command_in = HASH_RESPONSE;
        
        for(index = 1; index < 9; index++){
            hash_received[index - 1] = receive_buf[index];
        }
        hash_in_progress = 1;
        SetEvent(HASH_TASK_ID, HASH_START_EVENT);
		break;
	}
}



void dataReceiveISR()
{
	char tmp = RCREG;

	if (tmp == '$') {
		receive_ind = 0;
		receive_f = 1;
		return;
	}

	if (!receive_f)
		return;

	if (tmp == ':') {
		receive_ind = 0;
		receive_f = 0;
		parseBuffer();
		return;
	}

	receive_buf[receive_ind++] = tmp;
}


/* End of File : common.c */


/* End of File : common.c */
