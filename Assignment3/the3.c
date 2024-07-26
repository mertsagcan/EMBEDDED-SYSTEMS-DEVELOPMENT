/*
 * File: Main.c
 */


#include "Pragmas.h"
#include "ADC.h"
#include "LCD.h"
#include "the3.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>


typedef enum{
    ACTIVE_MODE,
    INACTIVE_MODE
} game_state;

typedef enum{
    TEAM_A,
    TEAM_B
} team;

typedef enum{
    Player_A,
    Player_AC,
    Player_AFC,
    Player_B,
    Player_BC,
    Player_BFC,
    Frisbee,
    Frisbee_Target,
} LCD_add_character;


typedef struct{
    int x;
    int y;
    team team_of_player;
    bool is_selected;
} player;

typedef struct{
    int x;
    int y;
} frisbee_pos;

typedef struct{
    int x;
    int y;
    bool target_on;
} frisbee_target_pos;

//variables
player player_a1, player_a2, player_b1, player_b2;
player *selected_player;

frisbee_pos frisbee1;
frisbee_target_pos frisbee_target1;

game_state g_state;
team last_thrower_team;

int team_a_score, team_b_score;
byte portB_previous;
int timer1_counter;
int game_speed;
int num_of_steps;
int frisbee_step_counter;

//literals
int numbers[10] = {NUM_0, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9};


//Init functions.

init(){
    //Initialise ports and game items.

    //Clear RA and RD ports and set them as output.
    TRISA = 0b00000000;
    TRISD = 0b00000000;

    PORTA = 0b00000000;
    PORTD = 0b00000000;

    //Set RB port input.
    TRISB = 0b11111111;

    portB_previous = PORTB;

    
    //Init all the players and the frisbee to the initial config.
    player_a1.x = 3;
    player_a1.y = 2;
    player_a1.team_of_player = TEAM_A;
    player_a1.is_selected = true;

    player_a2.x = 3;
    player_a2.y = 3;
    player_a2.team_of_player = TEAM_A;
    player_a2.is_selected = false;

    player_b1.x = 14;
    player_b1.y = 2;
    player_b1.team_of_player = TEAM_B;
    player_b1.is_selected = false;

    player_b2.x = 14;
    player_b2.y = 3;
    player_b2.team_of_player = TEAM_B;
    player_b2.is_selected = false;

    frisbee1.x = 9;
    frisbee1.y = 2;

    //Set the selected player to player_a1.
    selected_player = &player_a1;

    //Set the frisbee_targets position (0, 0) as it is invalid.
    frisbee_target1.x = 0;
    frisbee_target1.y = 0;
    frisbee_target1.target_on = false;

    //Set the game to inactive mode.
    g_state = INACTIVE_MODE;

    //Set the scores to 0.
    team_a_score = 0;
    team_b_score = 0;

    game_speed = 4;
}

init_button_interrupts(){
    //Set INTCON, INTCON3 bits for button interrupts.

    INTCONbits.GIE = 0; //Global interrupt disable.
    INTCONbits.PEIE = 1; //Peripheral interrupt enable.
    INTCONbits.INT0IE = 1; //RB0 external interrupt enable.
    INTCONbits.RBIE = 1; //RB port change interrupt enable.
    INTCONbits.INT0IF = 0; //RB0 external interrupt flag 0.

    byte portB = PORTB; //This row is just to read PORTB.
    INTCONbits.RBIF = 0; //RB interrupt flag 0.

    INTCON3bits.INT1IE = 1; //RB1 external interrupt enable.
    INTCON3bits.INT1IF = 0; //RB1 external interrupt flag.

}

init_timers(){
    //Timer0 init.(Used for debouncing).
    T0CON = 0b00000011; //Will be used with 1/16 prescaler.
    INTCONbits.TMR0IE = 1;
    INTCONbits.TMR0IF = 0;


    //Timer1 init(Used for blink and speed).
    timer1_counter = 0;
    T1CON = 0b01101000; // Will be used with 1/4 prescaler.
    PIE1bits.TMR1IE = 1;
    PIR1bits.TMR1IF = 0;

    //Timer2 init.(For random function) Just needed to be started.
    T2CON = 0b00000100;
}


//LCD functions.
void add_game_items_to_LCD(){

    //Add the game items to the LCD.
    LCDAddSpecialCharacter(Player_A, teamA_player);
    LCDAddSpecialCharacter(Player_AC, selected_teamA_player);
    LCDAddSpecialCharacter(Player_AFC, selected_teamA_player_with_frisbee);
    LCDAddSpecialCharacter(Player_B, teamB_player);
    LCDAddSpecialCharacter(Player_BC, selected_teamB_player);
    LCDAddSpecialCharacter(Player_BFC, selected_teamB_player_with_frisbee);
    LCDAddSpecialCharacter(Frisbee, frisbee);
    LCDAddSpecialCharacter(Frisbee_Target, frisbee_target);

    LCDGoto(player_a1.x, player_a1.y);
    LCDDat(Player_AC);
    LCDGoto(player_a2.x, player_a2.y);
    LCDDat(Player_A);
    LCDGoto(player_b1.x, player_b1.y);
    LCDDat(Player_B);
    LCDGoto(player_b2.x, player_b2.y);
    LCDDat(Player_B);
    LCDGoto(frisbee1.x, frisbee1.y);
    LCDDat(Frisbee);

}

void update_LCD(){
    //Update LCD all the time.
    //Disable interrupts.
    INTCONbits.GIE = 0;

    //Set RA 3 4 5 to 0.
    PORTAbits.RA3 = 0;
    PORTAbits.RA4 = 0;
    PORTAbits.RA5 = 0;

    //Update the LCD with the current game state.

    //Clear the display.
    LCDCmd(LCD_CLEAR);

    //Display the current game state.
    LCDGoto(frisbee1.x, frisbee1.y);
    LCDDat(Frisbee);
    
    if(frisbee_target1.target_on){
        LCDGoto(frisbee_target1.x, frisbee_target1.y);
        LCDDat(Frisbee_Target);
    }
    if(player_a1.is_selected){
        LCDGoto(player_a1.x, player_a1.y);
        if(player_a1.x == frisbee1.x && player_a1.y == frisbee1.y){
            LCDDat(Player_AFC);
        }
        else{
            LCDDat(Player_AC);
        }
    }
    else{
        LCDGoto(player_a1.x, player_a1.y);
        LCDDat(Player_A);
    }

    if(player_a2.is_selected){
        LCDGoto(player_a2.x, player_a2.y);
        if(player_a2.x == frisbee1.x && player_a2.y == frisbee1.y){
            LCDDat(Player_AFC);
        }
        else{
            LCDDat(Player_AC);
        }
    }
    else{
        LCDGoto(player_a2.x, player_a2.y);
        LCDDat(Player_A);
    }

    if(player_b1.is_selected){
        LCDGoto(player_b1.x, player_b1.y);
        if(player_b1.x == frisbee1.x && player_b1.y == frisbee1.y){
            LCDDat(Player_BFC);
        }
        else{
            LCDDat(Player_BC);
        }
    }
    else{
        LCDGoto(player_b1.x, player_b1.y);
        LCDDat(Player_B);
    }

    if(player_b2.is_selected){
        LCDGoto(player_b2.x, player_b2.y);
        if(player_b2.x == frisbee1.x && player_b2.y == frisbee1.y){
            LCDDat(Player_BFC);
        }
        else{
            LCDDat(Player_BC);
        }
    }
    else{
        LCDGoto(player_b2.x, player_b2.y);
        LCDDat(Player_B);
    }
    //Enable interrupts.
    INTCONbits.GIE = 1;
}

//Helper functions.

int max(int a, int b){
    if(a > b){
        return a;
    }
    else{
        return b;
    }
}

int min(int a, int b){
    if(a < b){
        return a;
    }
    else{
        return b;
    }
}

bool is_occupied(int x, int y){
    //Check if the given position is occupied by a player.
    if(x < MIN_X || x > MAX_X || y < MIN_Y || y > MAX_Y){
        return true;
    }
    else if(player_a1.x == x && player_a1.y == y){
        return true;
    }
    else if(player_a2.x == x && player_a2.y == y){
        return true;
    }
    else if(player_b1.x == x && player_b1.y == y){
        return true;
    }
    else if(player_b2.x == x && player_b2.y == y){
        return true;
    }
    else{
        return false;
    }
}

bool is_frisbee_or_target(int x, int y){
    //Check if the given position is occupied by the frisbee or the target.
    if(frisbee1.x == x && frisbee1.y == y){
        return true;
    }
    else if(frisbee_target1.x == x && frisbee_target1.y == y){
        return true;
    }
    else{
        return false;
    }
}

void set_player_pos(player *player_temp){
    //If a player is not selected, assign a random direction
    if(!player_temp->is_selected){
        int rand_number = random_generator(9);
        //Switch case for random direction.
        switch(rand_number){
            case 0:
                if(!is_occupied(player_temp->x, player_temp->y - 1) && !is_frisbee_or_target(player_temp->x, player_temp->y - 1)){
                    player_temp->y--;
                    break;
                }
            case 1:
                if(!is_occupied(player_temp->x + 1, player_temp->y - 1) && !is_frisbee_or_target(player_temp->x + 1, player_temp->y - 1)){
                    player_temp->x++;
                    player_temp->y--;
                    break;
                }
            case 2:
                if(!is_occupied(player_temp->x + 1, player_temp->y) && !is_frisbee_or_target(player_temp->x + 1, player_temp->y)){
                    player_temp->x++;
                    break;
                }
            case 3:
                if(!is_occupied(player_temp->x + 1, player_temp->y + 1) && !is_frisbee_or_target(player_temp->x + 1, player_temp->y + 1)){
                    player_temp->x++;
                    player_temp->y++;
                    break;
                }
            case 4:
                if(!is_occupied(player_temp->x, player_temp->y + 1) && !is_frisbee_or_target(player_temp->x, player_temp->y + 1)){
                    player_temp->y++;
                    break;
                }
            case 5:
                if(!is_occupied(player_temp->x - 1, player_temp->y + 1) && !is_frisbee_or_target(player_temp->x - 1, player_temp->y + 1)){
                    player_temp->x--;
                    player_temp->y++;
                    break;
                }
            case 6:
                if(!is_occupied(player_temp->x - 1, player_temp->y) && !is_frisbee_or_target(player_temp->x - 1, player_temp->y)){
                    player_temp->x--;
                    break;
                }
            case 7:
                if(!is_occupied(player_temp->x - 1, player_temp->y - 1) && !is_frisbee_or_target(player_temp->x - 1, player_temp->y - 1)){
                    player_temp->x--;
                    player_temp->y--;
                    break;
                }
            case 8:
                if(!is_occupied(player_temp->x, player_temp->y) && !is_frisbee_or_target(player_temp->x, player_temp->y)){
                    break;
                }
                else{
                    set_player_pos(player_temp);
                }
            
        }
    }
}

void debouncing_init(){
    //Disable RB port interrupts.
    INTCON3bits.INT1IE = 0;
    INTCONbits.RBIE = 0;
    

    //Load and start timer0 for debouncing. (For 400 ms)
    T0CONbits.T0PS = 011; //Set for 1/16 preascaler.
    TMR0H = 0b00001011;
    TMR0L = 0b11101010;
    T0CONbits.TMR0ON = 1;
}

void update_frisbee(){

    //If the frisbee is not at the target position, move it.
    if(frisbee1.x != frisbee_target1.x || frisbee1.y != frisbee_target1.y){
        //Move the frisbee according to the route provided by compute_frisbee_target_and_route function.
        if(frisbee_step_counter < num_of_steps){
            frisbee1.x = frisbee_steps[frisbee_step_counter][0];
            frisbee1.y = frisbee_steps[frisbee_step_counter][1];
            frisbee_step_counter++;
        }
    }
}

void update_players(){
    set_player_pos(&player_a1);
    set_player_pos(&player_a2);
    set_player_pos(&player_b1);
    set_player_pos(&player_b2);
}


//Interrupt functions.

void RB0_interupt_routine(){
    //Throw frisbee button.
    //Change the game state to active.
    //Activate game speed timer.
    //Change last thrower team.
    if(g_state != ACTIVE_MODE && selected_player->x == frisbee1.x && selected_player->y == frisbee1.y && PORTBbits.RB0 == 0){
        g_state = ACTIVE_MODE;
        last_thrower_team = selected_player->team_of_player;

        //Compute the frisbee target and route.
        num_of_steps = compute_frisbee_target_and_route(frisbee1.x, frisbee1.y);
        frisbee_step_counter = 0;
        frisbee_target1.x = frisbee_steps[num_of_steps - 1][0];
        frisbee_target1.y = frisbee_steps[num_of_steps - 1][1];

        //Make the first move of the gameplay.
        update_frisbee();
        update_players();

        //Start timer1.
        TMR1H = 11011111;
        TMR1L = 10000101;
        timer1_counter = 0;
        T1CONbits.TMR1ON = 1;
    }
    INTCONbits.INT0IF = 0;
}

void RB1_interrupt_routine(){
    //Change player if selected player doesnt have a frisbee in the inactive state.

    if(g_state == INACTIVE_MODE && selected_player->x == frisbee1.x && selected_player->y == frisbee1.y){
        //Do nothing
    }
    else if(PORTBbits.RB1 == 0){
        if(selected_player == &player_a1){
            selected_player = &player_a2;
            player_a1.is_selected = false;
            player_a2.is_selected = true;
        }
        else if(selected_player == &player_a2){
            selected_player = &player_b1;
            player_a2.is_selected = false;
            player_b1.is_selected = true;
        }
        else if(selected_player == &player_b1){
            selected_player = &player_b2;
            player_b1.is_selected = false;
            player_b2.is_selected = true;
        }
        else if(selected_player == &player_b2){
            selected_player = &player_a1;
            player_b2.is_selected = false;
            player_a1.is_selected = true;
        }
    }
    INTCON3bits.INT1IF = 0;
    debouncing_init();
}

void gamepad_interrupt_routine(){
    byte temp = portB_previous ^ PORTB;
    portB_previous = PORTB;

    //If RB4 is pressed and released, go up.
    if(temp == 0b00010000 && PORTBbits.RB4 == 0){
        if(!is_occupied(selected_player->x, selected_player->y-1)){
                selected_player->y--;
                selected_player->y = max(selected_player->y, MIN_Y);
                update_LCD();
                debouncing_init();
            }
    }
    //If RB5 is pressed and released, go right.
    else if(temp == 0b00100000 && PORTBbits.RB5 == 0){
        if(!is_occupied(selected_player->x+1, selected_player->y)){
                selected_player->x++;
                selected_player->x = min(selected_player->x, MAX_X);
                update_LCD();
                debouncing_init();
            }
    }
    //If RB6 is pressed and released, go down.
    else if(temp == 0b01000000 && PORTBbits.RB6 == 0){
        if(!is_occupied(selected_player->x, selected_player->y+1)){
                selected_player->y++;
                selected_player->y = min(selected_player->y, MAX_Y);
                update_LCD();
                debouncing_init();
            }
    }
    //If RB7 is pressed and released, go left.
    else if(temp == 0b10000000 && PORTBbits.RB7 == 0){
        if(!is_occupied(selected_player->x-1, selected_player->y)){
                selected_player->x--;
                selected_player->x = max(selected_player->x, MIN_X);
                update_LCD();
                debouncing_init();
            }
    }

    //Set the RB interrupt flag to 0.
    INTCONbits.RBIF = 0;
}

void debouncing_interrupt_routine(){
    INTCONbits.TMR0IF = 0;
    T0CONbits.TMR0ON = 0;
    INTCONbits.RBIE = 1;
    INTCON3bits.INT1IE = 1;
}

void blink_gamespeed_interrupt_routine(){
    //Default game speed is 400 ms.
    timer1_counter++;
    if(timer1_counter >= game_speed){
        //Update the positions of the players
        timer1_counter = 0;
        update_frisbee();
        update_players();
    }
    
    //When the frisbee reaches the destination.
    if(frisbee1.x == frisbee_target1.x && frisbee1.y == frisbee_target1.y){
        
        frisbee_target1.target_on = false;

        //If a player cathces the frisbee.
        if(selected_player->x == frisbee1.x && selected_player->y == frisbee1.y){
            if(selected_player->team_of_player == TEAM_A){
                team_a_score++;
                last_thrower_team = TEAM_A;
            }
            else{
                team_b_score++;
                last_thrower_team = TEAM_B;
            }
        }

        //If it falls to the ground.
        else{
            if(last_thrower_team == TEAM_A){
                selected_player->is_selected = false;
                selected_player = &player_b1;
                selected_player->is_selected = true;
            }
            else{
                selected_player->is_selected = false;
                selected_player = &player_a1;
                selected_player->is_selected = true;
            }
        }

        //Deactivate the timer1 and go into the INACTIVE_MODE
        //Make the frisbee target dissapear.
        T1CONbits.TMR1ON = 0;
        g_state = INACTIVE_MODE;
        frisbee_target1.x = 0;
        frisbee_target1.y = 0;
    }

    if(g_state == ACTIVE_MODE){
        //Blink target.
        frisbee_target1.target_on = !frisbee_target1.target_on;
    }
    PIR1bits.TMR1IF = 0;
}

void __interrupt() isr(void){
    if(INTCONbits.INT0IF){
        //RB0 interrupt routine.
        RB0_interupt_routine();
    }
    if(INTCON3bits.INT1IF){
        //RB1 interrupt routine.
        RB1_interrupt_routine();
    }
    if(INTCONbits.RBIF){
        //Gamepad interrupt routine.
        gamepad_interrupt_routine();
    }
    if(INTCONbits.TMR0IF){
        //Debouncing interrupt routine.
        debouncing_interrupt_routine();
    }
    if(PIR1bits.TMR1IF){
        //Blink and game speed interrupt routine.
        blink_gamespeed_interrupt_routine();
    }
    update_LCD();
}


//Seven segment display functions.

void seven_segment_display(int score_a, int score_b){
    //Display the score of team A.
    PORTA = 0b00001000;
    PORTD = numbers[score_a];
    __delay_ms(5);

    PORTA = 0b00010000;
    PORTD = DASH;
    __delay_ms(5);

    //Display the score of team B.
    PORTA = 0b00100000;
    PORTD = numbers[score_b];
    __delay_ms(5);    
}



void main(void){
    init();
    init_button_interrupts();
    init_timers();
    InitLCD();
    add_game_items_to_LCD();
    initADC();
    
    INTCONbits.GIE = 1;

    while(1){
        
        seven_segment_display(team_a_score, team_b_score);

        //ADCconversion.
        unsigned int adc_value = readADCChannel(0);
        if(1){
            //AD Conversion calculations and interrupts.
            if(adc_value < 256 && adc_value > 0){
                game_speed = 4;                  
            }
            else if(adc_value < 512 && adc_value >= 256){
                game_speed = 8;
            }
            else if(adc_value < 768 && adc_value >= 512){
                game_speed = 12;
            }
            else if(adc_value < 1024 && adc_value >= 768){
                game_speed = 16;
            }
        }
    }


    return;
}