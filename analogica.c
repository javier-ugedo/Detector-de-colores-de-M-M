#include <p33FJ32MC202.h>
#include "analogica.h"

#define ROJO 1
#define VERDE 2
#define AZUL 3

static unsigned int colormm = 0;
static unsigned int i = 0;
static unsigned int medida = 0;
static unsigned int medida_rojo_d = 0;
static unsigned int medida_verde_d = 0;
static unsigned int medida_azul_d = 0;
static unsigned int ticksmedir = 0;

static unsigned int portb = 0;

unsigned int medir_color(void) {
    unsigned int result = 0;
    unsigned int roj,ver,az;
    if (i == 0) {
        ticksmedir = 0;
        i++;
    }

    if (ticksmedir < 10) {
        medida = ROJO;
        portb = PORTB;
        TRISB &= ~(1 << 9); //activar medida color rojo
        TRISB |= (1 << 10); //desactivar medida color verde
        TRISB |= (1 << 11); //desactivar medida color azul
 
        portb &= ~(1 << 9); //activar medida color rojo 
        PORTB = portb;
    }
    else if (ticksmedir < 20) {
        medida = VERDE;
        
        TRISB |= (1 << 9); // desactivar medida color rojo
        TRISB &= ~(1 << 10); //activar medida color verde
        TRISB |= (1 << 11); //desactivar medida color azul

        portb = PORTB;
        portb &= ~(1 << 10); //activar medida color verde
        PORTB = portb;
    }
    else if (ticksmedir < 30) {
        medida = AZUL;
        TRISB |= (1 << 9); // desactivar medida color rojo
        TRISB |= (1 << 10); //desactivar medida color verde
        TRISB &= ~(1 << 11); //activar medida color azul
        
        portb = PORTB;
        portb &= ~(1 << 11); //activar medida color azul
        PORTB = portb;
    } else {

        medida = 0;
        roj = medida_rojo_d;
        ver = medida_verde_d;
        az = medida_azul_d;
        portb = PORTB;
        portb |= (0x0E00); //NO MIDE NINGÚN COLOR
        if (medida_rojo_d >= medida_verde_d && medida_rojo_d >= medida_azul_d) {
            //ES ROJO
            portb &= ~(1 << 13);
            portb |= (1 << 14); //14 ROJO 100%
            portb &= ~(1 << 15);
            colormm = ROJO;
        } else if (medida_verde_d >= medida_rojo_d && medida_verde_d >= medida_azul_d) {
            //ES VERDE
            portb &= ~(1 << 13);
            portb &= ~(1 << 14);
            portb |= (1 << 15);
            colormm = VERDE;
        } else if (medida_azul_d >= medida_rojo_d && medida_azul_d >= medida_verde_d) {
            //ES AZUL
            portb |= (1 << 13); // 13 azul
            portb &= ~(1 << 14); // 14 rojo
            portb &= ~(1 << 15); // 15 verde
            colormm = AZUL;
        }
        PORTB = portb;
        result = colormm;
    }
    return result;
}

unsigned int get_ticksmedir(void) {
    return ticksmedir;
}

void reset_ticksmedir(void) {
    ticksmedir = 0;
}

void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void) {
    IFS0bits.T1IF = 0;
    ticksmedir++;
    if (ticksmedir >= 40) {
        ticksmedir = 0;
    }
}

void __attribute__((interrupt, no_auto_psv))_ADC1Interrupt(void) {
    IFS0bits.AD1IF = 0; // Borrar la bandera
    //medida color
    if (medida == ROJO) {
        medida_rojo_d = ADC1BUF0;
    } else if (medida == VERDE) {
        medida_verde_d = (ADC1BUF0/3 )*2;
    } else if (medida == AZUL) {
        medida_azul_d = (ADC1BUF0);
    }
      
    //medida infrarrojo
    AD1CON1 |= 1 << 1;
}

//