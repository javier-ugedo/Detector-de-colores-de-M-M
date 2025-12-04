#include <p33FJ32MC202.h>
#include "fsm.h"
#include "timer.h"
#include "analogica.h"
#include "pwm.h"

// -------------------------
// Definición de estados
// -------------------------
#define REPOSO 1
#define MEDIR_COLOR 2
#define MOVER_TAMBOR 3
#define ABRIR_COMPUERTA 4
#define CERRAR_COMPUERTA 5
#define ERROR 99        

// -------------------------
// Variables internas
// -------------------------
static unsigned int estado = REPOSO;

static unsigned int colormm = 0;

static unsigned int portb = 0;


// -------------------------
// Inicialización global
// -------------------------
void inicializarClasificador(void) {

    //pines digitales
    AD1PCFGL = 0xFFFF; 
    AD1PCFGL &= ~(1 << 0); //medida color
    //servos
    TRISB &= ~(1 << 6); // RB6 salida servo compuerta
    TRISB &= ~(1 << 12); // RB12 salida servo tambor
    //led RGB
    TRISB &= ~(1 << 9); //RB9 salida MEDIDA COLOR ROJO
    TRISB &= ~(1 << 10); //RB10 salida MEDIDA COLOR VERDE
    TRISB &= ~(1 << 11); //RB11 salida MEDIDA COLOR AZUL
    //pulsador <-> sensor IR
    TRISA |= (1 << 3); // RA3 entrada pulsador
    // led de estado
    TRISA &= ~(1 << 4); // RA4 salida led estado
    PORTA &= ~(1 << 4);
    // apagar leds de medida de color
    PORTB |= (0x0E00); //NO MIDE NINGÚN COLOR

    TRISA |= 0x1; //entrada DEL ADC PARA MEDIR COLOR
    //pulsador RB2
    //led rgb RB13,RB14,RB15
    TRISB &= ~(0xE000);
    PORTB &= ~(0xE000);
    //inicializar ADC y timer1
    inicializarADC(0);
    inicializarTimer1(1000);

   //inicializar servos
   inicializarPWM(1 << 6); //pines 12 y 6
   inicializarPWM(1 << 12); //pines 12 y 6
   periodoPWM(6,200);
   periodoPWM(12,200);
   dcPWM(6,11);
   dcPWM(12,15);

    // estado inicial
    estado = REPOSO;
    colormm = 0;
}  

void tareaClasificador(void){

    //unsigned char mm_detectado = leerSensorIR();
    unsigned int pulsador = 0;
    switch(estado) {

        // ============================================================
        case REPOSO:
        // ============================================================
            
            pulsador = (PORTA >> 3) & 0x1;
            colormm = 0;
            PORTA |= (1 << 4); //led estado encendido
            if(pulsador == 1) {
                colormm = 0;
                PORTA &= ~(1 << 4);
                estado = MEDIR_COLOR;
                reset_ticksmedir();
            }
            dcPWM(6,11);
            //dcPWM(12,15);
            break;

        // ============================================================
        case MEDIR_COLOR:
        // ============================================================
            TRISB |= (1 << 6);
            TRISB |= (1 << 12);
            if (colormm == 0) {
                    colormm = medir_color();
                } else {
                    estado = MOVER_TAMBOR;
                    reset_ticksmedir();
                    TRISB &= ~(1 << 6);
                    TRISB &= ~(1 << 12);
                }
            break;

        // ============================================================
        case MOVER_TAMBOR:
        // ============================================================
            //PWM del tambor es rb12
            if(colormm == AZUL){
                dcPWM(12,15); //pwm a 90º
            }
            else if(colormm == VERDE){
                dcPWM(12,19);//pwm a 150º 22
            }
            else if(colormm == ROJO) {
                dcPWM(12,11);//pwm  30º 8
            }
            if(get_ticksmedir() > 20){
                estado =ABRIR_COMPUERTA;
                reset_ticksmedir();
            }
            break;
            
        // ============================================================
        case ABRIR_COMPUERTA:
        // ============================================================
            dcPWM(6,20); // abrir compuerta dc 5 cerrado
            if(get_ticksmedir() > 20){
                estado = CERRAR_COMPUERTA;
                reset_ticksmedir();
            }
            break;
        // ============================================================
        case CERRAR_COMPUERTA:
        // ============================================================
            dcPWM(6,11); // cerrar compuerta en estado reposo
            //dcPWM(12,15); // poner tambor en posición neutra
            if(get_ticksmedir() > 20){
                estado = REPOSO;
                reset_ticksmedir();
                portb = PORTB;
                portb &= ~(1 << 13); // desactivar led RGB
                portb &= ~(1 << 14); 
                portb &= ~(1 << 15); 
                PORTB = portb;
            }
            break;

        // ============================================================
        case ERROR:
        // ============================================================
//            apagarLEDRGB();
//            parpadearLEDReposo();
//            moverServoCompuerta(0);
//            moverServoDisco(90);
            // aquí solo sales pulsando reset
            break;
    }
}
