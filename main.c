// Gustabo Córdova - 21776
// Proyecto3_D2

//Librerías
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/adc.h"
#include "utils/ustdlib.h"

#define XTAL 16000000 //Reloj externo de 16MHz

//Definimos variables
int8_t spot1; //parqueo1
int8_t spot2; //Parqueo2
int8_t spot3; //Parqueo3
int8_t spot4; //Parqueo4
char disponibles; //disponoblies

uint32_t i; //Para delay

//Funciones
void setup(void); //Función para el setup
void setupUART0(void); //Función para el setup del UART0
void setupUART1(void); //Función para el setup del UART1
void UART0_SendString(const char *str); //Función para poder mandar cadenas de texto por UART0 (monitor serial)
void UART0_SendInteger(int8_t num); //Función para mostar enteros por UART0
void UART1_SendString(const char *str); //Función para poder mandar cadenas de texto por UART1
void UART1_SendInteger(int8_t num); //Función para mostar enteros por UART1
void delay1ms(void); //Función de delay de 1ms
void delay(uint32_t msec); //Función para delay en ms

int main(void){
    setup(); //Llamar al setup
    setupUART0(); //Setup del UART0
    setupUART1(); //Setup del UART1
    while (1){

        ///////////////////////////////////////////////////////////////// PARQUEO 1  /////////////////////////////////////////////////////////////////

        if (GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_7) == 0){                             // Si el sensor devuelve un 0, está ocupado
            spot1 = 1;                                                                  // Bandera en 1
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 0);                               // Apaga el led verde
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);                      // Enciende el led rojo
        }

        else {
            spot1 = 0;                                                                  // Bandera en 0
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_PIN_0);                      // Enciende el led verde
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0);                               // Apaga el led rojo
        }

        ///////////////////////////////////////////////////////////////// PARQUEO 2  /////////////////////////////////////////////////////////////////

        if (GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_6) == 0){                             // Si el sensor devuelve un 0, está ocupado
            spot2 = 1;                                                                  // Bandera en 1
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0);                               // Apaga el led verde
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_PIN_3);                      // Enciende el led rojo
        }

        else {
            spot2 = 0;                                                                  // Bandera en 0
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_PIN_2);                      // Enciende el led verde
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0);                               // Apaga el led rojo
        }

        ///////////////////////////////////////////////////////////////// PARQUEO 3  /////////////////////////////////////////////////////////////////

        if (GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_5) == 0){                             // Si el sensor devuelve un 0, está ocupado
            spot3 = 1;                                                                  //Variable en 1
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, 0);                               //Apagar led verde
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_PIN_2);                      //Encender led rojo
        }
        else {
            spot3 = 0;                                                                  //Variable en 0
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_PIN_1);                      //Encender led verde
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0);                               //Apagar led rojo
        }

        ///////////////////////////////////////////////////////////////// PARQUEO 4  /////////////////////////////////////////////////////////////////

        if (GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_4) == 0){                             //Si el sensor devuelve un 0 el parqueo est� ocupado
            spot4 = 1;                                                                  //Variable en 1
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, 0);                               //Apagar led verde
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);                      //Encender led rojo
        }

        else {
            spot4 = 0;                                                                  //Variable en 0
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, GPIO_PIN_3);                      //Encender led verde
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);                               //Apagar led rojo
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        spot2 = spot2 << 1; //Mover bits
        spot3 = spot3 << 2; //Mover bits
        spot4 = spot4 << 3; //Mover bits
        disponibles = spot1 | spot2 | spot3 | spot4; // Realizar un OR para que cada bit represente un parqueo
        UARTCharPutNonBlocking(UART1_BASE, disponibles); // Enviar por UART1 al esp32
    }
}

void setup(void){
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN); //Configuración del reloj a 40MHz
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD));
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); //Pines D0, D1, D2, D3 como salidas
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); //Pines E1, E2, E3 como salidas
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1); //Pines F1 como salida
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7); //Pines A4, A5, A6, A7 como salidas
    IntMasterEnable();
}

void setupUART0(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinConfigure(GPIO_PA0_U0RX); //Definir pinA0 como RX
    GPIOPinConfigure(GPIO_PA1_U0TX); //Definir pinA1 como TX
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1); //Habilitar los pines para el UART
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |UART_CONFIG_PAR_NONE));
    UARTEnable(UART0_BASE); //Iniciar UART0
}

void setupUART1(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    GPIOPinConfigure(GPIO_PB0_U1RX); //Definir pinB0 como RX
    GPIOPinConfigure(GPIO_PB1_U1TX); //Definir pinB1 como TX
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1); //Habilitar los pines para el UART
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |UART_CONFIG_PAR_NONE));

    UARTEnable(UART1_BASE); //Iniciar UART1
}

void UART0IntHandler(void){
    char cReceived;
    UARTIntClear(UART0_BASE, UART_INT_RX | UART_INT_RT); //Limpiar bandera de interrupci�n para recepci�n y transmisi�n
    while(UARTCharsAvail(UART0_BASE)) //Mientras haya algo disponible en el canal ejecutar
        {
            cReceived = UARTCharGetNonBlocking(UART0_BASE); //Guardar el dato en una variable
            UARTCharPutNonBlocking(UART0_BASE, cReceived); //Devuelve el dato recibido al transmisor
        }
}

void UART1IntHandler(void){
    UARTIntClear(UART1_BASE, UART_INT_RX | UART_INT_RT); //Limpiar bandera de interrupci�n para recepci�n y transmisi�n
    while(UARTCharsAvail(UART1_BASE)) //Mientras haya algo disponible en el canal ejecutar
        {
            UARTCharPutNonBlocking(UART1_BASE, disponibles); //Devuelve el dato recibido al transmisor
        }
}

void UART0_SendString(const char *str){
  while (*str) {
    while (!UARTCharPutNonBlocking(UART0_BASE, *str))
      ;
    str++;
  }
}

void UART0_SendInteger(int8_t num){
  char buf[20];
  sprintf(buf, "%ld", num);

  UART0_SendString(buf);
}

void UART1_SendString(const char *str){
  while (*str) {
    while (!UARTCharPutNonBlocking(UART1_BASE, *str))
      ;
    str++;
  }
}

void UART1_SendInteger(int8_t num){
  char buf[20];
  sprintf(buf, "%ld", num);

  UART1_SendString(buf);
}

void delay(uint32_t msec){
    for (i = 0; i < msec; i++){
        delay1ms();
    }
}

void delay1ms(void){
    SysTickDisable(); //Deshabilitar el Sistick
    SysTickPeriodSet(40000-1); //Por aproximadamente 1ms
    SysTickEnable(); //Iniciar el set del periodo

    while((NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT) == 0); // Mientras el bit de count sea 0, no hay funciones
}
