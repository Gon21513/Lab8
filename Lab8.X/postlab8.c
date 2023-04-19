/* 
 * File:   postlab8.c
 * Author: Luis Pedro Gonzalez 21513
 *
 * Created on 19 de abril de 2023, 03:34 PM
 */


#define _XTAL_FREQ 1000000

//variables
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include <xc.h>
#include <stdint.h>
#include <pic16f887.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//----------variables-----------
uint8_t bandera;
char selection;
char RX;
uint8_t pot;
char pot_char[];



//-----------------prototipos----------
void setup(void);
void desplegar(char selection);
void Print (unsigned char *word);
//void cadena(char *cursor);

//--------------INTERRUPCION-----------
void __interrupt() isr(void){
    //recepcion de datos
    if(PIR1bits.RCIF)//bandera de interrucpcion de eusart
    {
        RX = RCREG;//revisamos rcreg y lo guardamos
    }
    
    if (PIR1bits.ADIF){//revisar la interrupcion del adc
       if (ADCON0bits.CHS == 0b0000){//revisar el canal 0
           pot = ADRESH;
       }
       PIR1bits.ADIF = 0;// reiniciar la interrucipcn del adc
    }
}


void main(void){
    setup();

    while (1){
        
        if (ADCON0bits.GO == 0){
            ADCON0bits.GO = 1; 
        }
    
        
        Print("\r --------------BIENVENIDO----------------\r");
        Print("\r ESCOJA UNA DE LAS OPCIONES: \r");
        Print("\r 1.LEER POTENCIOETRO \r");
        Print("\r 2.ENVIAR ASCII \r");

        bandera = 1;
        
        while (bandera){//chequear la bandera
            
            while (PIR1bits.RCIF == 0); //recibimos datos
            selection = RX;
            
            switch(selection){
                
                case ('1'):
                    Print("\r VALOR DEL POTENCIOMETRO: \r");
                    itoa(pot_char, pot,10);
                    desplegar(pot_char);
                    bandera = 0;
                    break;
            
            
            case ('2'):
                Print("\r INGRESE UN CARACTER PARA MOSTRAR EN ASCII: \r");
                
                while (PIR1bits.RCIF == 0);
                    PORTB = RX; 
                    desplegar(RX);
                    bandera = 0;
                    break;
                }
            
        }
    
    }
    return;
}



//-----------set up general--------
void setup(void){
    ANSEL = 0b00000001; //ans0 como entrda analogivs
    ANSELH = 0;
    
    TRISB = 0;//configurar portd como salids
    PORTB = 0;//iniciar el puerto d
    
//--------------oscilador----------------

    

    OSCCONbits.IRCF = 0b100; //1mhz
    OSCCONbits.SCS = 1;//oscilador interno

    
//-------configuracion de tx y rx-----------
    TXSTAbits.SYNC = 0;//asincrono
    TXSTAbits.BRGH = 1;//high baud rate select bit
    
    BAUDCTLbits.BRG16 = 1;//utilizar q6 bits baud rate
    

    SPBRG = 25; //configurar a 9615
    SPBRGH = 0;
   
    RCSTAbits.SPEN = 1;//habilitar la comunicacion serial
    TXSTAbits.TX9 = 0; //USAR SOLO 8 BITS    
    RCSTAbits.CREN = 1;//habilitar recepcion 
    TXSTAbits.TXEN = 1;//habiliar la transmision
    
    
//------------configuracion de interrupciones-------
    PIE1bits.RCIE = 1;//interrupcion de la recepcion 
    INTCONbits.PEIE = 1;
    PIE1bits.ADIE = 1; //interrupcion del adc
    PIR1bits.ADIF = 0; //bandera del adc en 0
    INTCONbits.GIE = 1;
    
    
//------------configuracion del adc-----------    
    ADCON0bits.ADCS = 0b01; ; //forc8
    
    // --------------- Seleccion voltaje referencia --------------- 
    ADCON1bits.VCFG1 = 0; // Voltaje de referencia de 0V
    ADCON1bits.VCFG0 = 0; // Voltaje de referencia de 5V
    
            
    // --------------- justificado izquierda ---------------
    ADCON1bits.ADFM = 0; 
    
    // --------------- canales --------------- 
    ADCON0bits.CHS = 0b0000; // seleccionar AN0
           
            
    //--------------- Iniciar el ADC ---------------
    ADCON0bits.ADON = 1;  //INICIA EL ADC
    __delay_us(50);
}

//funcion para enviar caracteres
void Print(unsigned char *word){
    while(*word != '\0'){
        while (TXIF != 1);
        TXREG = *word;
        *word++;
    }
    return;
}

void desplegar (char selection){
    while (TXSTAbits.TRMT == 0);
    TXREG = selection;
}