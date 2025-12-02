#include <p33FJ32MC202.h>
#include "config.h"
#include "timer.h"
#include "analogica.h"

#define ROJO 1
#define VERDE 2
#define AZUL 3

static unsigned int colormmd=0;
static unsigned int ii=0;
static unsigned int medidad = 0;
static unsigned int medida_rojo_d=0;
static unsigned int medida_verde_d=0;
static unsigned int medida_azul_d=0;
static unsigned int ticksmedird=0;

static unsigned int portb=0;

unsigned int medir_color(void) {
    if(ii == 0){
        ticksmedird = 0;
        ii++;
    }
    
    if(ticksmedird<10){
        medidad = ROJO;
        portb = PORTB;
        portb &= ~(1 << 9); //activar medidad color rojo
        portb |= (1 << 10); //desactivar medidad color verde
        portb |= (1 << 11); //desactivar medidad color azul
        PORTB = portb;
    }

     else if(ticksmedird<20){
        medidad = VERDE;
        portb = PORTB;
        portb |= (1 << 9); //desactivar medidad color rojo
        portb &= ~(1 << 10); //activar medidad color verde
        portb |= (1 << 11); //desactivar medidad color azul
        PORTB = portb;
    }

     else if(ticksmedird<30){
        medidad = AZUL;
        portb = PORTB;
        portb |= (1 << 9); //desactivar medidad color rojo
        portb |= (1 << 10); //desactivar medidad color verde
        portb &= ~(1 << 11); //activar medidad color azul
        PORTB = portb;
    }
        else {

         medidad = 0;
            AD1CON1 &= ~(1 << 1); //dejar de medir
            PORTB |= (0x0E00); //NO MIDE NINGÚN COLOR
            if(medida_rojo_d >= medida_verde_d && medida_rojo_d >= medida_azul_d){
                //ES ROJO
                PORTB |= (1 << 13);
                PORTB &= ~(1 << 14);
                PORTB &= ~(1 << 15);
                colormmd = ROJO;
            } else if(medida_verde_d >= medida_rojo_d && medida_verde_d >= medida_azul_d){
                //ES VERDE
                PORTB &= ~(1 << 13);
                PORTB |= (1 << 14);
                PORTB &= ~(1 << 15);
                colormmd = VERDE;
            } else if(medida_azul_d >= medida_rojo_d && medida_azul_d >= medida_verde_d){
                //ES AZUL
                PORTB &= ~(1 << 13);
                PORTB &= ~(1 << 14);
                PORTB |= (1 << 15);
                colormmd = AZUL;
            }
            return colormmd;
        }
}

void __attribute__((interrupt, no_auto_psv))_ADC1Interrupt(void) {
    IFS0bits.AD1IF = 0; // Borrar la bandera
    //medidad color
    if(medidad == ROJO){
        medida_rojo_d = ADC1BUF0;
    } else if(medidad == VERDE){
        medida_verde_d = ADC1BUF0;
    } else if(medidad == AZUL){
        medida_azul_d = ADC1BUF0;
    }
    
    //medidad infrarrojo
    AD1CON1 |= 1 << 1;
}