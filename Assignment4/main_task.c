#include "common.h"
int i, a;
int ingredient_index1 = -1;
int ingredient_index2 = -1;
int switch_var = 0;
unsigned char hash_in_progress = 0;

int ingredient_need_check(unsigned char ingredient){
    if(ingredient == 'B' || ingredient == 'M' || ingredient == 'P'){
        for(a = 0; a < 3; a++){
            if(customer_ingredient1[a] == ingredient){
                return 1;
            }
        }
        for(a = 0; a < 3; a++){
            if(customer_ingredient2[a] == ingredient){
                return 1;
            }
        }
        return 0;
    }
    return 1;

}

int ingredient_exists_at_index(unsigned char ingredient){
    int x;
    for(x = 0; x < 4; x++){
        if(ingredient_status[x] == ingredient){
            return x;
        }
    }
    return -1;
}



TASK(MAIN_TASK)
{
	SetRelAlarm(ALARM_TSK0, 50, 50);
	while (1) {
		WaitEvent(SEND_COMMAND_EVENT); // Wait for timing
		ClearEvent(SEND_COMMAND_EVENT);

		if (command_in == END){
			break;
        }
        
		WaitEvent(DATA_SEND_DONE_EVENT); // Wait for the ongoing transmission to be done
		ClearEvent(DATA_SEND_DONE_EVENT);
            
        switch(switch_var){
            
            case 0:
                //Check if the customer is being served.
                if(hash_in_progress == 0 && customer_in_process[0] == 0 && customer_ids[0] != 0){
                    if(customer_ingredient1[0] == 'F'){
                        //If food judge, select the first ingredient at hand and send slow cook command.
                        if(ingredient_status[0] == 'P' || ingredient_status[0] == 'B' || ingredient_status[0] == 'M'){
                            command_out = SLOW_COOK;
                            command_list[SLOW_COOK][1] = customer_ids[0];
                            command_list[SLOW_COOK][2] = 0;
                            customer_in_process[0] = 1;
                            break;
                        }
                        else if(ingredient_status[1] == 'P' || ingredient_status[1] == 'B' || ingredient_status[1] == 'M'){
                            command_out = SLOW_COOK;
                            command_list[SLOW_COOK][1] = customer_ids[0];
                            command_list[SLOW_COOK][2] = 1;
                            customer_in_process[0] = 1;
                            break;
                        }
                        else if(ingredient_status[2] == 'P' || ingredient_status[2] == 'B' || ingredient_status[2] == 'M'){
                            command_out = SLOW_COOK;
                            command_list[SLOW_COOK][1] = customer_ids[0];
                            command_list[SLOW_COOK][2] = 2;
                            customer_in_process[0] = 1;
                            break;
                        }
                        else if(ingredient_status[3] == 'P' || ingredient_status[3] == 'B' || ingredient_status[3] == 'M'){
                            command_out = SLOW_COOK;
                            command_list[SLOW_COOK][1] = customer_ids[0];
                            command_list[SLOW_COOK][2] = 3;
                            customer_in_process[0] = 1;
                            break;
                        }
                        
                    }
                    else{
                        if(customer_ingredient2[0] == 'N'){
                            //Single ingredient food command.
                            ingredient_index1 = ingredient_exists_at_index(customer_ingredient1[0]);
                            if(ingredient_index1 != -1){
                                command_out = COOK;
                                command_list[COOK][1] = customer_ids[0];
                                command_list[COOK][2] = ingredient_index1;
                                command_list[COOK][3] = 'N';
                                ingredient_index1 = -1;
                                customer_in_process[0] = 1;
                                break;
                            }
                        }
                        else{
                            ingredient_index1 = ingredient_exists_at_index(customer_ingredient1[0]);
                            ingredient_index2 = ingredient_exists_at_index(customer_ingredient2[0]);
                            if(ingredient_index1 != -1 && ingredient_index2 != -1){
                                command_out = COOK;
                                command_list[COOK][1] = customer_ids[0];
                                command_list[COOK][2] = ingredient_index1;
                                command_list[COOK][3] = ingredient_index2;
                                ingredient_index1 = -1;
                                ingredient_index2 = -1;
                                customer_in_process[0] = 1;
                                break;
                            }
                        }
                        
                    }
                }
                
            case 1:
                //Check if the customer is being served.
                if(hash_in_progress == 0 && customer_in_process[1] == 0 && customer_ids[1] != 0){
                    if(customer_ingredient1[1] == 'F'){
                        //If food judge, select the first ingredient at hand and send slow cook command.
                        if(ingredient_status[0] == 'P' || ingredient_status[0] == 'B' || ingredient_status[0] == 'M'){
                            command_out = SLOW_COOK;
                            command_list[SLOW_COOK][1] = customer_ids[1];
                            command_list[SLOW_COOK][2] = 0;
                            customer_in_process[1] = 1;
                            break;
                        }
                        else if(ingredient_status[1] == 'P' || ingredient_status[1] == 'B' || ingredient_status[1] == 'M'){
                            command_out = SLOW_COOK;
                            command_list[SLOW_COOK][1] = customer_ids[1];
                            command_list[SLOW_COOK][2] = 1;
                            customer_in_process[1] = 1;
                            break;
                        }
                        else if(ingredient_status[2] == 'P' || ingredient_status[2] == 'B' || ingredient_status[2] == 'M'){
                            command_out = SLOW_COOK;
                            command_list[SLOW_COOK][1] = customer_ids[1];
                            command_list[SLOW_COOK][2] = 2;
                            customer_in_process[1] = 1;
                            break;
                        }
                        else if(ingredient_status[3] == 'P' || ingredient_status[3] == 'B' || ingredient_status[3] == 'M'){
                            command_out = SLOW_COOK;
                            command_list[SLOW_COOK][1] = customer_ids[1];
                            command_list[SLOW_COOK][2] = 3;
                            customer_in_process[1] = 1;
                            break;
                        }
                    }
                    else{
                        if(customer_ingredient2[1] == 'N'){
                            //Single ingredient food command.
                            ingredient_index1 = ingredient_exists_at_index(customer_ingredient1[1]);
                            if(ingredient_index1 != -1){
                                command_out = COOK;
                                command_list[COOK][1] = customer_ids[1];
                                command_list[COOK][2] = ingredient_index1;
                                command_list[COOK][3] = 'N';
                                ingredient_index1 = -1;
                                customer_in_process[1] = 1;
                                break;
                            }
                        }
                        else{
                            ingredient_index1 = ingredient_exists_at_index(customer_ingredient1[1]);
                            ingredient_index2 = ingredient_exists_at_index(customer_ingredient2[1]);
                            if(ingredient_index1 != -1 && ingredient_index2 != -1){
                                command_out = COOK;
                                command_list[COOK][1] = customer_ids[1];
                                command_list[COOK][2] = ingredient_index1;
                                command_list[COOK][3] = ingredient_index2;
                                ingredient_index1 = -1;
                                ingredient_index2 = -1;
                                customer_in_process[1] = 1;
                                break;
                            }
                        }
                        
                    }
                }
            case 2:
                //Check if the customer is being served.
                if(hash_in_progress == 0 && customer_in_process[2] == 0 && customer_ids[2] != 0){
                    if(customer_ingredient1[2] == 'F'){
                        //If food judge, select the first ingredient at hand and send slow cook command.
                        if(ingredient_status[0] == 'P' || ingredient_status[0] == 'B' || ingredient_status[0] == 'M'){
                            command_out = SLOW_COOK;
                            command_list[SLOW_COOK][1] = customer_ids[2];
                            command_list[SLOW_COOK][2] = 0;
                            customer_in_process[2] = 1;
                            break;
                        }
                        else if(ingredient_status[1] == 'P' || ingredient_status[1] == 'B' || ingredient_status[1] == 'M'){
                            command_out = SLOW_COOK;
                            command_list[SLOW_COOK][1] = customer_ids[2];
                            command_list[SLOW_COOK][2] = 1;
                            customer_in_process[2] = 1;
                            break;
                        }
                        else if(ingredient_status[2] == 'P' || ingredient_status[2] == 'B' || ingredient_status[2] == 'M'){
                            command_out = SLOW_COOK;
                            command_list[SLOW_COOK][1] = customer_ids[2];
                            command_list[SLOW_COOK][2] = 2;
                            customer_in_process[2] = 1;
                            break;
                        }
                        else if(ingredient_status[3] == 'P' || ingredient_status[3] == 'B' || ingredient_status[3] == 'M'){
                            command_out = SLOW_COOK;
                            command_list[SLOW_COOK][1] = customer_ids[2];
                            command_list[SLOW_COOK][2] = 3;
                            customer_in_process[2] = 1;
                            break;
                        }
                    }
                    else{
                        if(customer_ingredient2[2] == 'N'){
                            //Single ingredient food command.
                            ingredient_index1 = ingredient_exists_at_index(customer_ingredient1[2]);
                            if(ingredient_index1 != -1){
                                command_out = COOK;
                                command_list[COOK][1] = customer_ids[2];
                                command_list[COOK][2] = ingredient_index1;
                                command_list[COOK][3] = 'N';
                                ingredient_index1 = -1;
                                customer_in_process[2] = 1;
                                break;
                            }
                        }
                        else{
                            ingredient_index1 = ingredient_exists_at_index(customer_ingredient1[2]);
                            ingredient_index2 = ingredient_exists_at_index(customer_ingredient2[2]);
                            if(ingredient_index1 != -1 && ingredient_index2 != -1){
                                command_out = COOK;
                                command_list[COOK][1] = customer_ids[2];
                                command_list[COOK][2] = ingredient_index1;
                                command_list[COOK][3] = ingredient_index2;
                                ingredient_index1 = -1;
                                ingredient_index2 = -1;
                                customer_in_process[2] = 1;
                                break;
                            }
                        }
                        
                    }
                }

            case 3://Toss command.
                if(ingredient_status[0] != 'N' && ingredient_status[1] != 'N' && ingredient_status[2] != 'N' && ingredient_status[3] != 'N'){
                    
                    if(hash_in_progress == 0 && ingredient_need_check(ingredient_status[0]) == 0){
                        command_out = TOSS;
                        command_list[TOSS][1] = 0;
                        break;
                    }
                    else if(hash_in_progress == 0 && ingredient_need_check(ingredient_status[1]) == 0){
                        command_out = TOSS;
                        command_list[TOSS][1] = 1;
                        break;
                    }
                    else if(hash_in_progress == 0 && ingredient_need_check(ingredient_status[2]) == 0){
                        command_out = TOSS;
                        command_list[TOSS][1] = 2;
                        break;
                    }
                    else if(hash_in_progress == 0 && ingredient_need_check(ingredient_status[3]) == 0){
                        command_out = TOSS;
                        command_list[TOSS][1] = 3;
                        break;
                    }
                    
                }
                
            default:
                command_out = WAIT;
                break;
        }
            
        if(hash_done){
            command_out = HASH_REPLY;
            hash_done = 0;
                
            //Load the command_list with the hash.
            for(i = 1; i < 17; i++){
                command_list[HASH_REPLY][i] = hash_to_send[i - 1];
                hash_to_send[i - 1] = 0;
                hash_in_progress = 0;
            }
        }

		startSendData(command_out);
	}
	TerminateTask();
}