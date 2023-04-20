/* 
 * File:   postlab8.c
 * Author: Luis Pedro Gonzalez 21513
 *
 * Created on 19 de abril de 2023, 03:34 PM
 */
#define _XTAL_FREQ 1000000

//variables
int ascii[10] = {0, 1, 2, 3, 4, 5, 7, 8, 9}; 
//unsigned int word; //variable para guardar caracter ascii
unsigned char bandera; //bandera para manetener loop
unsigned char opcion; //selector para entrar en algun modo
unsigned char bandera; //bandera para mantener en loop
unsigned int centena; //variable para centenas en conversión ascii
unsigned int decena; //variable para decenas en conversión ascii
unsigned int unidad; //variable para unidades en conversión ascii
unsigned int pot; //variable para guardar valor del potenciómetro

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
#include <stdio.h>
#include <pic16f887.h>


// --------------- Rutina de  interrupciones --------------- 
void __interrupt() isr(void){
    // ---- INTERRUPCION DEL ADC --------
    if (PIR1bits.ADIF == 1){ // Chequear bandera del conversor ADC
        pot = ADRESH; // Pasar valor del ADRESH a variable pot
        centena = (pot/100); //operación para obtener centenas
        decena = ((pot/10)%10); //operación para obtener decenas
        unidad = (pot%10); //operación para obtener unidades
        PIR1bits.ADIF = 0; // Limpiar bandera de conversor
        }
}

// --------------- Prototipos --------------- 
void setup(void);
void cadena(char *cursor);

// --------------- main --------------- 
void main(void){
    setup();
    
    
    
    // Envio de datos
    while (1){
        //cadena("\n\rSELECCIONE UNA OPCION\n\r) Leer Potenciometro\n\r2) Enviar ASCII\n\r"); // mostrar el menu como string
        cadena("SELECCIONE UNA OPCION:\n\r"); //enter
        cadena("1. VALOR DEL POTENCIOMETRO\n\r"); //enter
        cadena("2. VALOR EN ASCII\n\r"); //enter

        bandera = 1;
        while (bandera == 1){
                if (PIR1bits.RCIF == 1){ //revisar bandera de si el recibidor esta lleno 
                opcion = RCREG; //pasar a dato a varible
                PIR1bits.RCIF = 0; //limpiar bandera
                __delay_ms(10);} //delay de 10ms
                
            // --------------- Leer POT ---------------    
            if (opcion == '1'){ //revisar si se selecciono 1
                cadena("\n\r"); //enter
                cadena("EL VALOR DEL POTENCIOMETRO ES\n\r"); //enter
                ADCON0bits.GO = 1; //iniciar conversor ADC
                __delay_ms(5); //delay de 5ms
                TXREG = ascii[centena]+48; //mostrar valor de centena
                __delay_ms(5); //delay de 5ms
                TXREG = ascii[decena]+48; //mostrar valor de decena
                __delay_ms(5); //delay de 5ms
                TXREG = ascii[unidad]+48; //mostrar valor de unidades
                cadena("\n\r\n\r"); //enter
                bandera = 0; //regresar al menu
                opcion = 0; //limpiar selector
            }
            
            // --------------- Enviar ASCII --------------- 
            if (opcion == '2'){ //revisar si se selecciono 2
                cadena("\n\r"); //enter
                cadena("INGRESE UN CARACTER PARA MOSTRAR EN ASCII\n\r"); //mostrar informacion
                cadena("\n\r"); //enter
                bandera = 1; //encender bandera para esperar que se introduzca tecla
                PIR1bits.RCIF = 0; //limpiar bandera 
                while (bandera == 1){ //loop
                if (PIR1bits.RCIF == 1 && RCREG != 0){ //esperar que se presione tecla  
                PORTB = RCREG; //mostrar valor en el puerto B
                PIR1bits.RCIF = 0; //limpiar bandera
                bandera = 0; //apagar bandera
                opcion = 0;}}} //apagar bandera
        
            }
        }  
        __delay_ms(500);                
    }


// --------------- Setup General --------------- 
void setup(void){
    // --------------- Definir analogicas --------------- 
    ANSEL = 0b00000001; // Habilitar AN0 como analogica
    ANSELH = 0;
    
    TRISB = 0; // PORTB como salida
    PORTB = 0; // Iniciar PORTB
    
// --------------- Oscilador --------------- 
    OSCCONbits.IRCF = 0b100; // 8 MHz
    OSCCONbits.SCS = 1; // Seleccionar oscilador interno

    // --------------- Banderas e interrupciones ---------------
 
    PIE1bits.ADIE = 1; // Habiliar interrupcion del conversor ADC
    PIR1bits.ADIF = 0; // Limpiar bandera de interrupción del ADC
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    
//---------------SETUP TX Y RX------------------
        TXSTAbits.SYNC = 0;//asincrono
    TXSTAbits.BRGH = 1;//high baud rate select bit
    
    BAUDCTLbits.BRG16 = 1;//utilizar q6 bits baud rate
    
    SPBRG = 25; //configurar a 9615
    SPBRGH = 0;    

    
    RCSTAbits.SPEN = 1;//habilitar la comunicacion serial
    RCSTAbits.RX9 = 0;//deshabiliamos bit de direccion
    RCSTAbits.CREN = 1;//habilitar recepcion 
    TXSTAbits.TXEN = 1;//habiliar la transmision
    
    
//--------------------ADC-------------------------
        // --------------- Configura el canal --------------- 
    ADCON0bits.CHS = 0b0000; // seleccionar AN0
    
            
    // --------------- Seleccion voltaje referencia --------------- 
    ADCON1bits.VCFG1 = 0; // Voltaje de referencia de 0V
    ADCON1bits.VCFG0 = 0; // Voltaje de referencia de 5V
            
    // --------------- Seleccion de reloj ---------------
    ADCON0bits.ADCS = 0b10; // Fosc/32
            
    // --------------- Habilitar interrupciones del ADC ---------------
    
            
    // --------------- Asignar 8 bits, justificado izquierda ---------------
    ADCON1bits.ADFM = 0;        
            
    //--------------- Iniciar el ADC ---------------
    ADCON0bits.ADON = 1;  
    __delay_ms(1);
}
    


//Funcion para mostrar texto
void cadena(char *cursor){
    while (*cursor != '\0'){ // Mientras el cursor sea diferente a nulo
        while (PIR1bits.TXIF == 0); // Mientras que se este enviando no hacer nada
            TXREG = *cursor; // Asignar el valor del cursor para enviar
            *cursor++; // Aumentar posicion del cursor
    }
}

