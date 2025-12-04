#include <p33FJ32MC202.h>
#include "config.h"
#include "pwm.h"

int main(void){
    inicializarReloj();
    inicializarClasificador();
    //inicializarPWM(1 << 6);

//    AD1PCFGL = 0xFFFF; 
//    TRISA |= (1 << 3);
//    TRISA &= ~(1 << 4);

    while(1){
        //periodoPWM(6,200);
        //dcPWM(6,25);
        tareaClasificador();
//        unsigned int pulsador = ((PORTA >> 3) & 0x1);
//        if (pulsador == 1) PORTA &= ~(1<<4);
//        else PORTA |= (1<<4);
    }
    return 0;
}

