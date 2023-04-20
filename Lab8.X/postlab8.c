/* 
 * File:   postlab8.c
 * Author: Luis Pedro Gonzalez 21513
 *
 * Created on 19 de abril de 2023, 03:34 PM
 */
#define _XTAL_FREQ 1000000

//variables
unsigned int centena; // Almacena las centenas en  ASCII
unsigned int decena; // Almacena las decenas en  ASCII
unsigned int unidad; // Almacena las unidades en  ASCII
unsigned int pot; // Guarda el valor leído del potenciómetro
unsigned char opcion; // Variable para elegir una opción
unsigned char bandera; // Indicador para controlar bucles
int ASCII[10] = {0, 1, 2, 3, 4, 5, 7, 8, 9}; // Array con los valores ASCII de los números del 0 al 9

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
        
        pot = ADRESH; // Transferir el valor de ADRESH a la variable pot
        centena = (pot/100); // Calcular las centenas del valor
        decena = ((pot/10)%10); // Calcular las decenas del valor
        unidad = (pot%10); // Calcular las unidades del valor
        
        PIR1bits.ADIF = 0; // Borrar el indicador del conversor ADC
        
        }
}

// --------------- Prototipos --------------- 
void setup(void);
void cadena(char *cursor);

// --------------- main --------------- 
void main(void){
    setup();
    
    //menu de opciones
        while (1){
        cadena("\n\r"); 
        cadena("SELECCIONE UNA OPCION:\n\r"); 
        cadena("1. VALOR DEL POTENCIOMETRO\n\r"); 
        cadena("2. VALOR EN ASCII\n\r"); 

        bandera = 1;
        while (bandera == 1){
                if (PIR1bits.RCIF == 1){ //revisar bandera de si el recibidor esta lleno 
                opcion = RCREG; //pasar a dato a varible
                PIR1bits.RCIF = 0; //limpiar bandera
                __delay_ms(10);} //delay de 10ms
                
        //opciones de menu y valore de potenciometro
            if (opcion == '1'){ //revisar si se selecciono 1
                
                cadena("\n\r"); // Salto de línea
                cadena("\n\r EL VALOR DEL POTENCIOMETRO ES:\n\r"); // Mostrar mensaje
                //cadena("\n\r"); //salto

                ADCON0bits.GO = 1; // Comenzar la conversión ADC
                __delay_ms(5); // Esperar 5 milisegundos
                
                TXREG = ASCII[centena]+48; // Enviar el valor de las centenas
                __delay_ms(5); // Esperar 5 milisegundos
                
                TXREG = ASCII[decena]+48; // Enviar el valor de las decenas
                __delay_ms(5); // Esperar 5 milisegundos
                
                TXREG = ASCII[unidad]+48; // Enviar el valor de las unidades
                
                //cadena("\n\r\n\r"); // Salto de línea
                bandera = 0; // Reiniciar al menú
                opcion = 0; // Borrar el selector
            }
            
            // --------------- Enviar ASCII --------------- 
            if (opcion == '2'){ //revisar si se selecciono 2
                cadena("\n\r"); // Salto de línea
                cadena("\n\rINGRESE UN CARACTER PARA MOSTRAR EN ASCII:\n\r"); // Solicitar entrada
                cadena("\n\r"); // Salto de línea
                bandera = 1; // Activar la bandera para esperar entrada
                PIR1bits.RCIF = 0; // Borrar el indicador
                
                
                while (bandera == 1){ // Ciclo
                    
                    if (PIR1bits.RCIF == 1 && RCREG != 0){ // Esperar a que se presione una tecla
                        PORTB = RCREG; // Mostrar el valor en el puerto B
                        PIR1bits.RCIF = 0; // Borrar el indicador
                        bandera = 0; // Desactivar la bandera
                        opcion = 0;
                            }
                       }
                } //apagar bandera
        
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
            
    
 
    // ---------------justificado izquierda ---------------
    ADCON1bits.ADFM = 0;        
            
    //--------------- Iniciar el ADC ---------------
    ADCON0bits.ADON = 1;  
    __delay_ms(1);
}
    


//Funcion para mostrar texto
void cadena(char *cursor){
    while (*cursor != '\0'){ //verificar si llega al nulo
        while (PIR1bits.TXIF == 0); // no haces nada cuando envia
            TXREG = *cursor; // asigna el valor a enviae
            *cursor++; // incremeta posicion del cursor
    }
}

