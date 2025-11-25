#include <p33FJ32MC202.h>
#include "config.h"
#include "timer.h"
#include "pwm.h"

void inicializarPuertos(void);

#define ROJO 1
#define VERDE 2
#define AZUL 3

static unsigned int medida_rojo=0;
static unsigned int medida_verde=0;
static unsigned int medida_azul=0;

static unsigned int medida=0;

static unsigned int ticksmedir=0;

int main(void){
    inicializarReloj();
    inicializarPuertos();
    inicializarADC(0); //EMPIEZA MIDIENDO AN0
    inicializarTimer1(1000); 

    unsigned int portb=0;
    unsigned int i=0;
    while(1){
        if(i == 0){
            AD1CON1 |= 1 << 1;
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
                } else if(medida_verde >= medida_rojo && medida_verde >= medida_azul){
                    //ES VERDE
                    PORTB &= ~(1 << 13);
                    PORTB |= (1 << 14);
                    PORTB &= ~(1 << 15);
                } else if(medida_azul >= medida_rojo && medida_azul >= medida_verde){
                    //ES AZUL
                    PORTB &= ~(1 << 13);
                    PORTB &= ~(1 << 14);
                    PORTB |= (1 << 15);
                }
                //ESTADO_NEXT
        
        }
    }
    return 0;
}

void inicializarPuertos(void){
    AD1PCFGL = 0xFFFF; //pines digitales
    AD1PCFGL &= ~(1 << 0); 
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
void __attribute__((interrupt, no_auto_psv))_ADC1Interrupt(void) {
    IFS0bits.AD1IF = 0; // Borrar la bandera

    if(medida == ROJO){
        medida_rojo = ADC1BUF0;
    } else if(medida == VERDE){
        medida_verde = ADC1BUF0;
    } else if(medida == AZUL){
        medida_azul = ADC1BUF0;
    }
    AD1CON1 |= 1 << 1;
}