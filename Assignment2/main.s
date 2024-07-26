PROCESSOR    18F4620

#include <xc.inc>

; CONFIGURATION (DO NOT EDIT)
CONFIG OSC = HSPLL      ; Oscillator Selection bits (HS oscillator, PLL enabled (Clock Frequency = 4 x FOSC1))
CONFIG FCMEN = OFF      ; Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
CONFIG IESO = OFF       ; Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)
; CONFIG2L
CONFIG PWRT = ON        ; Power-up Timer Enable bit (PWRT enabled)
CONFIG BOREN = OFF      ; Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
CONFIG BORV = 3         ; Brown Out Reset Voltage bits (Minimum setting)
; CONFIG2H
CONFIG WDT = OFF        ; Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
; CONFIG3H
CONFIG PBADEN = OFF     ; PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
CONFIG LPT1OSC = OFF    ; Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
CONFIG MCLRE = ON       ; MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)
; CONFIG4L
CONFIG LVP = OFF        ; Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
CONFIG XINST = OFF      ; Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

; GLOBAL SYMBOLS
; You need to add your variables here if you want to debug them.
GLOBAL state_RB4, state_RB5, state_RB6, state_RB7 ;button state variables
GLOBAL bar_length, speed_level, is_paused, beat_count, digit_counter
GLOBAL num_0, num_1, num_2, num_3, num_4, num_5, num_6, num_7, num_8, num_9, segment_dash, segment_P
GLOBAL speed_1_load_high, speed_1_load_low, speed_2_load_high, speed_2_load_low, speed_3_load_high, speed_3_load_low
GLOBAL speed_4_load_high, speed_4_load_low, speed_5_load_high, speed_5_load_low, speed_6_load_high, speed_6_load_low
GLOBAL speed_7_load_high, speed_7_load_low, speed_8_load_high, speed_8_load_low, speed_9_load_high, speed_9_load_low
GLOBAL temp

; Define space for the variables in RAM
PSECT udata_acs
state_RB4:
  DS 1 
state_RB5:
  DS 1
state_RB6:
  DS 1
state_RB7:
  DS 1
bar_length:
    DS 1
speed_level:
    DS 1
is_paused:
    DS 1
num_0:
    DS 1
num_1:
    DS 1
num_2:
    DS 1
num_3:
    DS 1
num_4:
    DS 1
num_5:
    DS 1
num_6:
    DS 1
num_7:
    DS 1
num_8:
    DS 1
num_9:
    DS 1
segment_dash:
    DS 1
segment_P:
    DS 1
digit_counter:
    DS 1
beat_count:
    DS 1
speed_1_load_high:
    DS 1
speed_1_load_low:
    DS 1
speed_2_load_high:
    DS 1
speed_2_load_low:
    DS 1
speed_3_load_high:
    DS 1
speed_3_load_low:
    DS 1
speed_4_load_high:
    DS 1
speed_4_load_low:
    DS 1
speed_5_load_high:
    DS 1
speed_5_load_low:
    DS 1
speed_6_load_high:
    DS 1
speed_6_load_low:
    DS 1
speed_7_load_high:
    DS 1
speed_7_load_low:
    DS 1
speed_8_load_high:
    DS 1
speed_8_load_low:
    DS 1
speed_9_load_high:
    DS 1
speed_9_load_low:
    DS 1
temp:
    DS 1
  
PSECT CODE
org 0x0000
  goto main

org 0x0008
interrupt_service_routine:
  
  ;Check the interrupt flacgs and go to the appropriate interrupt routine

  btfsc INTCON, 0
    goto RB_interrupt
  
  btfsc INTCON, 2
    goto timer0_interrupt
  
  btfsc PIR1, 0
    goto timer_1_interrupt

  //timer 2 interrupt routine
  btfsc PIR1, 1
    goto timer_2_interrupt
  
  retfie  1                 ; No match


RB_interrupt:
  ;This routine checks if a button is pressed and released and then calls the appropriate subroutine
  movlw 0x01
  btfsc PORTB, 4
    movwf state_RB4
  btfsc PORTB, 5
    movwf state_RB5
  btfsc PORTB, 6
    movwf state_RB6
  btfsc PORTB, 7
    movwf state_RB7
  
  bcf INTCON, 0 ;clear interrupt flag

  btfss PORTB, 4
    call RB4_release_check
  
  btfss PORTB, 5
    call RB5_release_check
  
  btfss PORTB, 6
    call RB6_release_check
  
  btfss PORTB ,7
    call RB7_release_check

  retfie  1                 

RB4_release_check:
  btfsc state_RB4, 0
    goto RB4_interrupt
  return             

RB5_release_check:
  btfsc state_RB5, 0
    goto RB5_interrupt
  return

RB6_release_check:
  btfsc state_RB6, 0
    goto RB6_interrupt
  return

RB7_release_check:
  btfsc state_RB7, 0
    goto RB7_interrupt
  return   
  
RB4_interrupt:
    ;This routine is called when the RB4 button is pressed and released
    ;It updates is_paused and calls the appropriate init routine
    clrf state_RB4
    movlw 0x00
    btfss is_paused, 0
    movlw 0x01
    movwf is_paused

    btfsc is_paused, 0
        call paused_mode_init
    btfss is_paused, 0
        call running_mode_init
    retfie 1

paused_mode_init:
  ;Clear the beat count
  movlw 0x01
  movwf beat_count

  ;Stop timer 0
  bcf T0CON, 7

  ;Stop timer 1
  bcf T1CON, 0

  ;Clear TMR0H, TMR0L
  movlw 0x00
  movwf TMR0H
  movwf TMR0L

  ;Clear TMR1H, TMR1L
  movlw 0x00
  movwf TMR1H
  movwf TMR1L

  return

running_mode_init:
  ;Initialise timer0 according to the speed level
  call timer0_speed_init

  ;Start timer 0
  bsf T0CON, 7

  ;Start timer 1
  bsf T1CON, 0
  
  ;Light RC LED's according to the beat count
  movff beat_count, temp
  movlw 0x01
  dcfsnz temp
  movlw 0x03
  
  movwf LATC
  
  ;After lighting the LED's load timer 1 with the appropriate value
  call tmr1_load 
  
  ;Start timer 1
  bsf T1CON, 0

  return

tmr1_load:
  ;Load timer 1 with the appropriate value
    movlw 0xCF
    movwf TMR1H
    
    movlw 0x2C
    movwf TMR1L
    
    return

RB5_interrupt:
    ;If paused, increase speed level
    clrf state_RB5
    btfsc is_paused, 0
        call inc_speed_level
    ;If running, increase bar length
    btfss is_paused, 0
        call inc_bar_length

    retfie 1

inc_bar_length:
    ;If the bar length is smaller or equal to 7 increase it
    movlw 0x07
    cpfsgt bar_length
    incf bar_length

    return

inc_speed_level:
    ;If the speed level is smaller or equal to 8 increase it
    movlw 0x08
    cpfsgt speed_level
    incf speed_level

    return

RB6_interrupt:
    ; If paused, decrease speed level
    clrf state_RB6
    btfsc is_paused, 0
        call dec_speed_level
    
    ; If running, decrease bar length
    btfss is_paused, 0
        call dec_bar_length
    retfie 1


dec_bar_length:
    ;If the bar length is bigger or equal to 3 decrease it
    movlw 0x03
    cpfslt bar_length
    decf bar_length

    return

dec_speed_level:
    ;If the speed level is bigger or equal to 2 decrease it
    movlw 0x02
    cpfslt speed_level
    decf speed_level

    return

RB7_interrupt:
    ; If paused, reset speed level
    ; If running, reset bar length
    clrf state_RB7
   
    movlw 0x04
    btfsc is_paused, 0
        movlw 0x06
    btfsc is_paused, 0
        movwf speed_level
    btfss is_paused, 0
        movwf bar_length

    retfie 1

timer0_interrupt:

  ;Clear timer 0 interrupt flag
  bcf INTCON, 2

  ;Turn of timer 0
  bcf T0CON,7 

  ;Load timer 0 according to the speed level
  call timer0_speed_init
  movf beat_count, 0
  ;If beat count is equal or greater than bar length reset beat count
  cpfsgt bar_length
    call reset_beat_count
  
  ;Increase beat count
  incf beat_count

  ;Turn on timer 0
  bsf T0CON,7
  
  ;light RC LED's according to the beat count
  movff beat_count, temp
  movlw 0x01
  dcfsnz temp
  movlw 0x03
  
  movwf LATC
  
  ;After lighting the LED's load timer 1 with the appropriate value
  call tmr1_load 
  ;Start timer 1
  bsf T1CON, 0
  
  retfie 1

reset_beat_count:
  movlw 0x00
  movwf beat_count
  return

timer0_speed_init:
  ;Subroutine to initialise timer 0 according to the speed level
  movff speed_level, temp

  dcfsnz temp
    call speed_level_1_init

  dcfsnz temp
    call speed_level_2_init
  
  dcfsnz temp
    call speed_level_3_init

  dcfsnz temp
    call speed_level_4_init
  
  dcfsnz temp
    call speed_level_5_init

  dcfsnz temp
    call speed_level_6_init

  dcfsnz temp
    call speed_level_7_init

  dcfsnz temp
    call speed_level_8_init

  dcfsnz temp
    call speed_level_9_init 

  return

;These following functions are used to set the timer0 load values according to the speed level

speed_level_1_init:
  movff speed_1_load_high, TMR0H
  movff speed_1_load_low, TMR0L
  return

speed_level_2_init:
  movff speed_2_load_high, TMR0H
  movff speed_2_load_low, TMR0L
  return

speed_level_3_init:
  movff speed_3_load_high, TMR0H
  movff speed_3_load_low, TMR0L
  return

speed_level_4_init:
  movff speed_4_load_high, TMR0H
  movff speed_4_load_low, TMR0L
  return

speed_level_5_init:
  movff speed_5_load_high, TMR0H
  movff speed_5_load_low, TMR0L
  return

speed_level_6_init:
  movff speed_6_load_high, TMR0H
  movff speed_6_load_low, TMR0L
  return

speed_level_7_init:
  movff speed_7_load_high, TMR0H
  movff speed_7_load_low, TMR0L
  return

speed_level_8_init: 
  movff speed_8_load_high, TMR0H
  movff speed_8_load_low, TMR0L
  return

speed_level_9_init:
  movff speed_9_load_high, TMR0H
  movff speed_9_load_low, TMR0L
  return

;Timer 1 interrupt fuction turns off the LED's that timer 0 turns on and turns off itself untill timer 0 turns it on again
timer_1_interrupt:
    bcf PIR1, 0
    clrf LATC
    bcf T1CON, 0

    retfie 1

timer_2_interrupt:
    ;Clear timer 2 interrupt flag and clear timer 2
    bcf PIR1, 1
    clrf TMR2
    ;Go to the subroutine according to the digit counter
    movff digit_counter, temp

    dcfsnz temp
      call RA3_Digit_Controller

    dcfsnz temp
      call RA2_Digit_Controller

    dcfsnz temp
      call RA1_Digit_Controller

    dcfsnz temp
      call RA0_Digit_Controller
      
    retfie 1




RA3_Digit_Controller:
    ; Light the rightmost digit to show the bar length if its running and the speed level if its paused
    clrf LATD
    movlw 0x08
    movwf LATA
    btfss is_paused, 0
    call bar_length_check    
    
    btfsc is_paused, 0
    call speed_level_check
    
    ;If its paused set the digit counter for it to go to the leftmost digit
    ;If its running set the digit counter for it to go to the next digit
    movlw 0x02
    btfsc is_paused, 0
    movlw 0x04
    movwf digit_counter
    ;Start timer 2
    bsf T2CON, 2
    retfie 1
    
RA2_Digit_Controller:
    ;Since this function wont be called when its paused we dont need to check for that
    ;Light the correct digit to show a dash
    clrf LATD
    movlw 0x04
    movwf LATA
    
    movff segment_dash, LATD

    ;Set the digit counter to go to the next digit
    movlw 0x03
    movwf digit_counter
    ;Start timer 2
    bsf T2CON, 2
    retfie 1
    
RA1_Digit_Controller:
    ;Since this function wont be called when its paused we dont need to check for that
    ;Light the correct digit to show current beat
    clrf LATD
    movlw 0x02
    movwf LATA
    
    call current_beat_check

    ;Set the digit counter to go to the rightmost digit because the leftmost digit wont be used in running mode
    movlw 0x01
    movwf digit_counter

    ;Start timer 2
    bsf T2CON, 2
    retfie 1
    
RA0_Digit_Controller:
    ;Since this function wont be called when its running we dont need to check for that
    clrf LATD
    movlw 0x01
    movwf LATA
    ;Set PORTD to show P
    movff segment_P, LATD

    movlw 0x01
    movwf digit_counter
    ;Start timer 2
    bsf T2CON, 2
    retfie 1

speed_level_check:
  ;Set LATD to show the speed level according to the speed level variable
  movff speed_level, temp

  dcfsnz temp
    movff num_1, LATD

  dcfsnz temp
    movff num_2, LATD
  
  dcfsnz temp
    movff num_3, LATD

  dcfsnz temp
    movff num_4, LATD
  
  dcfsnz temp
    movff num_5, LATD

  dcfsnz temp
    movff num_6, LATD

  dcfsnz temp
    movff num_7, LATD

  dcfsnz temp
    movff num_8, LATD

  dcfsnz temp
    movff num_9, LATD

  return  


bar_length_check:
;Set LATD to show the bar length according to the bar length variable
  movff bar_length, temp

  decf temp

  dcfsnz temp
    movff num_2, LATD

  dcfsnz temp
    movff num_3, LATD

  dcfsnz temp
    movff num_4, LATD

  dcfsnz temp
    movff num_5, LATD

  dcfsnz temp
    movff num_6, LATD

  dcfsnz temp
    movff num_7, LATD

  dcfsnz temp
    movff num_8, LATD

  return

current_beat_check:
;Set LATD to show the current beat according to the beat count variable
  movff beat_count, temp

  dcfsnz temp
    movff num_1, LATD

  dcfsnz temp
    movff num_2, LATD

  dcfsnz temp
    movff num_3, LATD

  dcfsnz temp
    movff num_4, LATD

  dcfsnz temp
    movff num_5, LATD

  dcfsnz temp
    movff num_6, LATD

  dcfsnz temp
    movff num_7, LATD

  dcfsnz temp
    movff num_8, LATD

  return

init:
    ;Clear all ports
    clrf PORTA
    clrf PORTB
    clrf PORTC
    clrf PORTD
    
    ;Makde PORTD output (Seven segment display)
    movlw 0x00
    movwf TRISD
    
    ;MAke RA0-RA3 output(Multiplexer for seven segment display)
    movlw 0xF0
    movwf TRISA
    
    ;Make RB4-RB7 input (buttons)
    movlw 0xF0
    movwf TRISB
    
    ;Make port C output
    movlw 0x00
    movwf TRISC
    
    clrf state_RB4
    clrf state_RB5
    clrf state_RB6
    clrf state_RB7
    
    ;Set default bar length (between 2-8)
    movlw 0x04
    movwf bar_length
    
    ;Set default speed level (between 1-9)
    movlw 0x06
    movwf speed_level
    
    ;Program start in the paused state, set it to 1
    movlw 0x01
    movwf is_paused
    
    ;Set beat count to 1
    movlw 0x01
    movwf beat_count

    ;set digit_count to 4
    movlw 0x04
    movwf digit_counter
    
    ;TODO: Variables for seven segment display
    
    movlw 0x3F
    movwf num_0
    
    movlw 0x06
    movwf num_1
    
    movlw 0x5B
    movwf num_2
    
    movlw 0x4F
    movwf num_3
    
    movlw 0x66
    movwf num_4
    
    movlw 0x6D
    movwf num_5
    
    movlw 0x7D
    movwf num_6
    
    movlw 0x07
    movwf num_7
    
    movlw 0x7F
    movwf num_8
    
    movlw 0x6F
    movwf num_9
    
    movlw 0x40
    movwf segment_dash
    
    movlw 0x73
    movwf segment_P
    
    ;TODO: Enable Interrupts, Initialise timer 0, timer 2, and RB 4-7 interrupts
    ;INTCON = 0b11101000
    movlw 0xE8
    movwf INTCON

    ;PIR1 = 0b0000000000
    movlw 0x00
    movwf PIR1

    ;PIE1 = 0b0000000011
    movlw 0x03
    movwf PIE1

    ;RCON = 0b00000000
    movlw 0x00
    movwf RCON

    ;T0CON = 0b00000011
    ;set prescaler to 1:16
    movlw 0x03
    movwf T0CON

    ;TMR0H = 0b00000000
    movlw 0x00
    movwf TMR0H

    ;TMR0L = 0b00000000
    movlw 0x00
    movwf TMR0L

    ;T1CON = 0b11101100
    movlw 0xEC
    movwf T1CON
    

    ;TMR1H = 0b00000000
    movlw 0x00
    movwf TMR1H

    ;TMR1L = 0b00000000
    movlw 0x00
    movwf TMR1L

    ;T2CON = 0b00000111
    movlw 0x07
    movwf T2CON

    ;TMR2 = 0b00000000
    movlw 0x00
    movwf TMR2

    ;PR2 = 0b10000000
    movlw 0x80
    movwf PR2

    ;set timer0 load values for 9 different speed levels
    ;speed_1_load_high = 0x0B
    movlw 0x0B
    movwf speed_1_load_high

    ;speed_1_load_low = 0xDC
    movlw 0xDC
    movwf speed_1_load_low

    ;speed_2_load_high = 0x24
    movlw 0x24
    movwf speed_2_load_high

    ;speed_2_load_low = 0x46
    movlw 0x46
    movwf speed_2_load_low

    ;speed_3_load_high = 0x3C
    movlw 0x3C
    movwf speed_3_load_high

    ;speed_3_load_low = 0xB0
    movlw 0xB0
    movwf speed_3_load_low

    ;speed_4_load_high = 0x55
    movlw 0x55
    movwf speed_4_load_high

    ;speed_4_load_low = 0x1A
    movlw 0x1A
    movwf speed_4_load_low

    ;speed_5_load_high = 0x6D
    movlw 0x6D
    movwf speed_5_load_high

    ;speed_5_load_low = 0x84
    movlw 0x84
    movwf speed_5_load_low

    ;speed_6_load_high = 0x85
    movlw 0x85
    movwf speed_6_load_high

    ;speed_6_load_low = 0xEE
    movlw 0xEE
    movwf speed_6_load_low

    ;speed_7_load_high = 0x9E
    movlw 0x9E
    movwf speed_7_load_high

    ;speed_7_load_low = 0x58
    movlw 0x58
    movwf speed_7_load_low

    ;speed_8_load_high = 0xB6
    movlw 0xB6
    movwf speed_8_load_high

    ;speed_8_load_low = 0xC2
    movlw 0xC2
    movwf speed_8_load_low

    ;speed_9_load_high = 0xCF
    movlw 0xCF
    movwf speed_9_load_high

    ;speed_9_load_low = 0x2C
    movlw 0x2C
    movwf speed_9_load_low

    ;set timer0 load to inital speed level = 6
    ;TMR0H = speed_6_load_high
    movff speed_6_load_high, TMR0H
    ;TMR0L = speed_6_load_low
    movff speed_6_load_low, TMR0L

    return

main:
  
  call init
main_loop:
  goto main_loop

;All operations are done in the interrupt service routine
;Main loop just waits for an interruppt to occur and then jumps to the ISR

    
    
    
