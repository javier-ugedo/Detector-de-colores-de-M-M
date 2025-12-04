#include <p33FJ32MC202.h>
#include "pwm.h"
// ------------------------------------------------------
#define NUM_PINES_PWM 16 // Número de pines
// ------------------------------------------------------
// ------------ VARIABLES GLOBALES AL MÓDULO ------------
// ------------------------------------------------------
static unsigned int periodo[] = {0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0};
static unsigned int dc[] = {0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0};
static unsigned int ticks[] = {0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0};

void inicializarPWM(unsigned int bitmap) {
// Configurar los pines como salidas digitales
AD1PCFGL |= (bitmap & 0xF) << 2;
TRISB &= ~bitmap;
// Configurar el Timer 2 con interrupciones
T2CON = 0x0000; // OFF, preescalado: 1:1
PR2 = 3961; // 0,1 ms (PR2 aproximado = 4000)
TMR2 = 0; // Inicializar la cuenta
IFS0bits.T2IF = 0; // Borrar la bandera
IEC0bits.T2IE = 1; // Habilitar interrupciones
IPC1bits.T2IP = 5; // Prioridad interrupciones
T2CON |= 1 << 15; // Encender el timer
}

void periodoPWM(unsigned char pin, unsigned int dms) {
// Comprobar que el pin es válido
if (pin > 15)
return;
periodo[pin] = dms;
}

void dcPWM(unsigned char pin, unsigned int dms) {
// Comprobar que el pin es válido
if (pin > 15)
return;
dc[pin] = dms;
}

void __attribute__((interrupt, no_auto_psv))
_T2Interrupt(void) {
IFS0bits.T2IF = 0; // Borrar la bandera
unsigned char i;
for (i = 0; i < NUM_PINES_PWM; i++) {
// Generar solo las señales con periodo no nulo, es
// decir, las que se han configurado explícitamente,
// para dejar libres los demás pines para otros usos.
if (periodo[i] > 0) {
if (ticks[i] < dc[i])
PORTB |= 1 << i;
else
PORTB &= ~(1 << i);
ticks[i]++;
if (ticks[i] >= periodo[i])
ticks[i] = 0;
}
}
}
