#include <p33FJ32MC202.h>
#include "config.h"
#include "timer.h"
#include "pwm.h"
#include "analogica.h"
void inicializarPuertos(void);

#define ROJO 1
#define VERDE 2
#define AZUL 3

#define REPOSO 1
#define MEDIR_COLOR 2
#define MOVER_TAMBOR 3
#define ABRIR_COMPUERTA 4
#define ESPERAR_CAIDA 5
#define CERRAR_COMPUERTA 6
//variables medir color
static unsigned int medida_rojo=0;
static unsigned int medida_verde=0;
static unsigned int medida_azul=0;
static unsigned int estado = 0;
//variables elegir contenedor

//interrupciones ADC
static unsigned int medida=0;
static unsigned int ticksmedir=0;

int main(void){
    inicializarReloj();
    inicializarPuertos();
    inicializarADC(0); //EMPIEZA MIDIENDO AN0
    inicializarTimer1(1000); 
    //inicializar pwm servos 0,1ms
    inicializarPWM(0x1080); //pines 12 y 6
    periodoPWM(6,200);
    periodoPWM(12,200);
    dcPWM(6,15);
    dcPWM(12,15);

    //servo 1 rb6
    //servo 2 rb12
    unsigned int portb=0;
    unsigned int i=0;
    unsigned int colormm = 0;
    estado = MEDIR_COLOR;
    colormm = 0;
    while(1){
        
        switch(estado){
            
            case REPOSO:
                
            break;
            
            case MEDIR_COLOR:
                colormm = medir_color();
                if(colormm != 0){
                    estado +=1;
                    break;
                }
                /*
                if(i == 0){
                    AD1CON1 |= 1 << 1;
                    ticksmedir = 0;
                    i++;
            }
            if(ticksmedir<10){
                medida = ROJO;
                portb = PORTB;
                portb &= ~(1 << 9); //activar medida color rojo
                portb |= (1 << 10); //desactivar medida color verde
                portb |= (1 << 11); //desactivar medida color azul
                PORTB = portb;
            }

             else if(ticksmedir<20){
                medida = VERDE;
                portb = PORTB;
                portb |= (1 << 9); //desactivar medida color rojo
                portb &= ~(1 << 10); //activar medida color verde
                portb |= (1 << 11); //desactivar medida color azul
                PORTB = portb;
            }

             else if(ticksmedir<30){
                medida = AZUL;
                portb = PORTB;
                portb |= (1 << 9); //desactivar medida color rojo
                portb |= (1 << 10); //desactivar medida color verde
                portb &= ~(1 << 11); //activar medida color azul
                PORTB = portb;
            }
                else {

                 medida = 0;
                    AD1CON1 &= ~(1 << 1); //dejar de medir
                    PORTB |= (0x0E00); //NO MIDE NINGÚN COLOR
                    if(medida_rojo >= medida_verde && medida_rojo >= medida_azul){
                        //ES ROJO
                        PORTB |= (1 << 13);
                        PORTB &= ~(1 << 14);
                        PORTB &= ~(1 << 15);
                        colormm = ROJO;
                    } else if(medida_verde >= medida_rojo && medida_verde >= medida_azul){
                        //ES VERDE
                        PORTB &= ~(1 << 13);
                        PORTB |= (1 << 14);
                        PORTB &= ~(1 << 15);
                        colormm = VERDE;
                    } else if(medida_azul >= medida_rojo && medida_azul >= medida_verde){
                        //ES AZUL
                        PORTB &= ~(1 << 13);
                        PORTB &= ~(1 << 14);
                        PORTB |= (1 << 15);
                        colormm = AZUL;
                    }
                    estado = MOVER_TAMBOR;
                    break;
                 
                }*/
            case MOVER_TAMBOR: //PWM del tambor es rb12
                ticksmedir = 0;
                if(colormm == AZUL){
                    dcPWM(12,15); //pwm a 90º
                }
                else if(colormm == VERDE){
                    dcPWM(12,22);//pwm a 150º
                }
                else if(colormm == ROJO) {
                    dcPWM(12,8);//pwm a 30º
                }
                if(ticksmedir > 10){
                    estado += 1;
                    break;
                }
            
            case ABRIR_COMPUERTA:
                estado = ABRIR_COMPUERTA;
                dcPWM(6,15);
                if(1){ //!!!
                    estado +=1;//pwm a 90º
                    break;
                }
                
            case ESPERAR_CAIDA:
                break;
                
  
            
        //ESTADO ABRIR COMPUERTA
        //pwm para el segundo servo para abrir puerta
        //esperar periodo corto para asergurar que se ha abierto
        
        //ESTADO CERRAR COMPUERTA
        //pwm para el segundo servo para cerrar puerta
        //confirmar que no hay mm en el sensor de infrarrojos
        }
    }
        
    return 0;
}

void inicializarPuertos(void){
    AD1PCFGL = 0xFFFF; //pines digitales
    AD1PCFGL &= ~(1 << 0); 
    TRISB &= ~(1 << 6); // RB6 salida servo compuerta
    TRISB &= ~(1 << 12); // RB12 salida servo tambor
    TRISB &= ~(1 << 9); //RB9 salida MEDIDA COLOR ROJO
    TRISB &= ~(1 << 10); //RB10 salida MEDIDA COLOR VERDE
    TRISB &= ~(1 << 11); //RB11 salida MEDIDA COLOR AZUL
    PORTB |= (0x0E00); //NO MIDE NINGÚN COLOR

    TRISA |= 0x1; //entrada DEL ADC PARA MEDIR COLOR

    TRISB &= ~(0xF000);
    PORTB &= ~(0xF000);
    
    //InicializarPuertos
}
 //Sacar al driver
void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void){
    IFS0bits.T1IF = 0;
    ticksmedir++;
    if(ticksmedir>=40){
        ticksmedir=0;
    }
}
// Sacar al driver
/*
void __attribute__((interrupt, no_auto_psv))_ADC1Interrupt(void) {
    IFS0bits.AD1IF = 0; // Borrar la bandera
    //medida color
    if(medida == ROJO){
        medida_rojo = ADC1BUF0;
    } else if(medida == VERDE){
        medida_verde = ADC1BUF0;
    } else if(medida == AZUL){
        medida_azul = ADC1BUF0;
    }
    
    //medida infrarrojo
    AD1CON1 |= 1 << 1;
}
*/