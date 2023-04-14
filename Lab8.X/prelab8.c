/* 
 * File:   Prelab.c
 * Author: Luis Pedro 
 *
 * Created on 13 de abril de 2023, 06:47 PM
 */

#define _XTAL_FREQ 1000000

//variables
const char data = 'A';
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


//-----------------prototipos----------
void setup(void);

//--------------INTERRUPCION-----------
void __interrupt() isr(void){
    //recepcion de datos
    if(PIR1bits.RCIF)//bandera de interrucpcion de eusart
    {
        PORTB = RCREG;//revisamos rcreg y lo guardamos
    }
}


void main(void){
    setup();
    
    //envio de datos
    while (1){
        __delay_ms(500);
        
        if (PIR1bits.TXIF){//si txif esta disponible 
            TXREG = data;// enviar el valor de data
        }        
    }       
}


//-----------set up general--------
void setup(void){
    ANSEL = 0b00000000; //ans0 como entrda analogivs
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
    RCSTAbits.RX9 = 0;//deshabiliamos bit de direccion
    RCSTAbits.CREN = 1;//habilitar recepcion 
   
    TXSTAbits.TXEN = 1;//habiliar la transmision
    
    
//------------configuracion de interrupciones-------
    PIR1bits.RCIF = 0;
    PIE1bits.RCIE = 1;//interrupcion de la recepcion 
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
}