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
GLOBAL _t1, _t2, _t3, duration, bar_length, paused
GLOBAL button_pressedB0, button_pressedB1, button_pressedB2, button_pressedB3, button_pressedB4
GLOBAL button_releasedB0, button_releasedB1, button_releasedB2, button_releasedB3, button_releasedB4
GLOBAL bar_counter

; Define space for the variables in RAM
PSECT udata_acs
_t1:
    DS 1
_t2:
    DS 1
_t3:
    DS 1
duration:
    DS 1
bar_length:
    DS 1
paused:
    DS 1
button_pressedB0:
    DS 1
button_pressedB1:
    DS 1
button_pressedB2:
    DS 1
button_pressedB3:
    DS 1
button_pressedB4:
    DS 1
button_releasedB0:
    DS 1
button_releasedB1:
    DS 1
button_releasedB2:
    DS 1
button_releasedB3:
    DS 1
button_releasedB4:
    DS 1
bar_counter:
    DS 1

PSECT resetVec,class=CODE,reloc=2
resetVec:
    goto       main

PSECT CODE
 
init:
    clrf TRISA ; Make PORTA output
    setf TRISB ; Make PORTB input
    
    clrf paused
    clrf duration
    
    clrf button_pressedB0
    clrf button_pressedB1
    clrf button_pressedB2
    clrf button_pressedB3
    clrf button_pressedB4    
    
    clrf button_releasedB0
    clrf button_releasedB1
    clrf button_releasedB2
    clrf button_releasedB3
    clrf button_releasedB4   
    
    movlw 0x04
    movwf bar_length
    movwf bar_counter
  
    movlw 00000111B
    movwf LATA ; Light RA0, RA1, RA2
  
    call busy_wait_1sec
    
    
    goto main
    
main:
    call check_buttons
    call metronome
  
    call not_busy_wait_250msec
    call check_buttons
    btfss duration, 0
    call not_busy_wait_250msec
    
    goto main

  

busy_wait_1sec:
    movlw 0x016      ; copy desired value to W
    movwf _t3       ; copy W into t3
    _loop3:
        movlw 0x70      ; copy desired value to W
        movwf _t2       ; copy W into t2
        _loop2:
            movlw 0x86      ; copy desired value to W
            movwf _t1       ; copy W into t1
            _loop1:
                decfsz _t1, 1   ; decrement t1, if 0 skip next 
                goto _loop1     ; else keep counting down
                decfsz _t2, 1   ; decrement t2, if 0 skip next 
                goto _loop2     ; else keep counting down
                decfsz _t3, 1   ; decrement t3, if 0 skip next 
                goto _loop3     ; else keep counting down
                return
		
		
not_busy_wait_250msec:
    movlw 0x04      ; copy desired value to W
    movwf _t3       ; copy W into t3
    _loop6:
        movlw 0x74      ; copy desired value to W
        movwf _t2       ; copy W into t2
        _loop5:
            movlw 0x08      ; copy desired value to W
            movwf _t1       ; copy W into t1
            _loop4:
		call check_buttons
                decfsz _t1, 1   ; decrement t1, if 0 skip next 
                goto _loop4     ; else keep counting down
                decfsz _t2, 1   ; decrement t2, if 0 skip next 
                goto _loop5     ; else keep counting down
                decfsz _t3, 1   ; decrement t3, if 0 skip next 
                goto _loop6     ; else keep counting down
                return
		

check_buttons:
    movlw 0x00
    btfsc PORTB, 0
    movlw 0x01

    btfss PORTB, 0
    movff button_pressedB0, button_releasedB0
    movwf button_pressedB0
    
    btfsc button_releasedB0, 0
    call check_RB0

    movlw 0x00
    movwf button_releasedB0
    
    
    movlw 0x00
    btfsc PORTB, 1
    movlw 0x01

    btfss PORTB, 1
    movff button_pressedB1, button_releasedB1
    movwf button_pressedB1
    
    btfsc button_releasedB1, 0
    call check_RB1

    movlw 0x00
    movwf button_releasedB1
    
    
    movlw 0x00
    btfsc PORTB, 2
    movlw 0x01

    btfss PORTB, 2
    movff button_pressedB2, button_releasedB2
    movwf button_pressedB2
    
    btfsc button_releasedB2, 0
    call check_RB2

    movlw 0x00
    movwf button_releasedB2
    
    
    movlw 0x00
    btfsc PORTB, 3
    movlw 0x01

    btfss PORTB, 3
    movff button_pressedB3, button_releasedB3
    movwf button_pressedB3
    
    btfsc button_releasedB3, 0
    call check_RB3

    movlw 0x00
    movwf button_releasedB3
    
    
    movlw 0x00
    btfsc PORTB, 4
    movlw 0x01

    btfss PORTB, 4
    movff button_pressedB4, button_releasedB4
    movwf button_pressedB4
    
    btfsc button_releasedB4, 0
    call check_RB4

    movlw 0x00
    movwf button_releasedB4
    
    return
    
    
check_RB0:
    movlw 0x00
    btfss paused, 0
    movlw 0x01
    
    movwf paused
    
    btfsc paused, 0
    goto pause
    
    return
    
check_RB1:
    movlw 0x00
    btfss duration, 0
    movlw 0x01
    
    movwf duration
    
    return

check_RB2:
    movlw 0x04    
    movwf bar_length
    movwf bar_counter
    return
    
check_RB3:
    decf bar_length
    
    movff bar_length, bar_counter
    return

check_RB4:
    incf bar_length
    
    movff bar_length, bar_counter
    return
    

    
metronome:
    
    btfsc paused, 0
    return
    
    
    movf bar_length, 0
    cpfslt bar_counter
    call light_2leds
    
    
    movf bar_length, 0
    cpfseq bar_counter
    call light_1led
    
    btfsc LATA, 0
    call bar_handler    
    
    return
    
bar_handler:
    dcfsnz bar_counter
    movff bar_length, bar_counter
    
    return
    

light_2leds:
    movlw 0x03
    btfsc LATA, 0
    movlw 0x00
    
    movwf LATA
    
    return
    
light_1led:
    movlw 0x01
    btfsc LATA, 0
    movlw 0x00
    
    movwf LATA
    
    return
    
    
pause:
    movlw 0x04
    btfss LATA, 2
    movwf LATA
    
    call check_buttons
    btfss paused, 0
    goto main
    goto pause


end resetVec