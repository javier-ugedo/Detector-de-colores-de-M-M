#include <p33FJ32MC202.h>
#include "config.h"
#include "timer.h"

void inicializarTimer1(unsigned int dms){
    unsigned int TCKPS;
    unsigned int preesc;
    
        if( dms > 4235){
            return;
        }
        if (dms < 17){
            TCKPS = 0;
            preesc=1;
            }else if(dms < 133){
                TCKPS = 0x1;
                preesc=8;
                    }else if(dms < 1059){
                        TCKPS = 0x2;
                        preesc=64;
                    } else {
                        TCKPS=0x3;
                        preesc=256;
                    }

    T1CON = (0x0000 | (TCKPS << 4)); //Off, TCKPSalado 1/1
    PR1 = ((FCY)/10000)*(dms)/preesc; //(8000000/256); //numero de ciclos para el contador
    TMR1 = 0; //Inicializar cuenta
    IFS0bits.T1IF = 0 ; //borrar la flag
    IEC0bits.T1IE = 1; //Habilitar interrupción del timer 1
    IPC0bits.T1IP = 5; //Prioridad 3
    T1CON |= (1 << 15); //Contador ON
}

void inicializarADC(unsigned int pinAN){
    AD1CON3 = 0x0105; //Muestreo 1 ciclo, ADCS = 5(5 ciclos de reloj en convertir)
    IFS0bits.AD1IF = 0; //borrar flag
    IEC0bits.AD1IE = 1; //Habilitar interrupciones del adc
    IPC3bits.AD1IP = 4; //Prioridad 3
    AD1CON1 = 0x80E0; //ADC ON, conversion automatica
    AD1CHS0 = pinAN;
    AD1CON1 |= 1 << 1;
}