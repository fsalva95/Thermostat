#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/sfr_defs.h>
#include <avr/eeprom.h>
#include <string.h>
#include <stdlib.h>

//avrdude -v -p atmega328p -c arduino -P /dev/ttyACM0 -b 115200 -D -U flash:w:termostato.hex

//MACRO GENERALI
#ifdef	F_CPU
#undef	F_CPU
#define F_CPU	16000000UL
#endif

//MACRO PER SERIALE
#define BUAD	9600
#define BRC		(((F_CPU / (BUAD * 16UL))) - 1)
#define TX_BUFFER_SIZE	128

//MACRO DELAY
#define PIN PORTB1
#define DELAY_MS 250
#define DELAY_MS_MIN 20
#define DELAY_MS_RESPONSE 40
#define MIN_INTERVAL 2000

//DICHIARAZIONE MACRO PER FUNZIONI AVR
#define PB 2
#define PC 3
#define PD 4

#define NOT_A_PIN 0
#define NOT_A_PORT 0
#define NOT_ON_TIMER 0


#define LOW 0
#define HIGH 1

#define digitalPinToBitMask(P) ( pgm_read_byte( digital_pin_to_bit_mask_PGM + (P) ) )
#define portInputRegister(P)( (volatile uint8_t *)( pgm_read_word( port_to_input_PGM + (P))) )
#define digitalPinToPort(P)  (pgm_read_byte( digital_pin_to_port_PGM + (P) ) )

#define portModeRegister(P) ( (volatile uint8_t *)( pgm_read_word( port_to_mode_PGM + (P))) )
#define portOutputRegister(P) ( (volatile uint8_t *)( pgm_read_word( port_to_output_PGM + (P))) )

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() ) //ciclate di clock a microsecondo

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
        _BV(0), /* 0, port D */
        _BV(1),
        _BV(2),
        _BV(3),
        _BV(4),
        _BV(5),
        _BV(6),
        _BV(7),
        _BV(0), /* 8, port B */
        _BV(1),
        _BV(2),
        _BV(3),
        _BV(4),
        _BV(5),
        _BV(0), /* 14, port C */
        _BV(1),
        _BV(2),
        _BV(3),
        _BV(4),
        _BV(5),
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
	PD, //0
	PD,
	PD,
	PD,
	PD,
	PD,
	PD,
	PD,
	PB, //8
	PB,
	PB,
	PB,
	PB,
	PB,
	PC, //14
	PC,
	PC,
	PC,
	PC,
	PC,
};

const uint16_t PROGMEM port_to_input_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t) &PINB,
	(uint16_t) &PINC,
	(uint16_t) &PIND,
};

const uint16_t PROGMEM port_to_mode_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t) &DDRB,
	(uint16_t) &DDRC,
	(uint16_t) &DDRD,
};

const uint16_t PROGMEM port_to_output_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t) &PORTB,
	(uint16_t) &PORTC,
	(uint16_t) &PORTD,
};

//variabili che uso per la comunicazione seriale
uint8_t serialReadPos = 0;
uint8_t serialWritePos = 0;

//variabili che uso per il sensore
uint32_t count = 0;
uint8_t StatoPorta;
uint8_t bit;
uint8_t porta;
uint32_t maxCycles;

volatile uint8_t *reg;
volatile uint8_t *out;

//eeprom
uint16_t delay_serial;
uint16_t min_value;
uint16_t max_value;
uint8_t sens_to_read;
unsigned int addr;
int length = 0;

// buffer circolare
unsigned int addr_data = 0;
unsigned int init_addr_data = 0;
int count_circular = 0;
int circular_buffer_length = 0;

int flag;
int k=0;
char ReceivedChar;
int cambiamentoStringaInput=0;

int data[5]={0,0,0,0,0};
uint32_t cycles[80];
char str[10];

int _lastresult;//variabile booelana
float temp=0;


//DICHIARAZIONE FUNZIONI
void USART_init(uint8_t ubrr);
void appendSerial(char c);
void serialWrite(char  c[]);
uint32_t impulso(int level);
int readT();
void tx();
int digitalRead(uint8_t pin);
void EEPROM_read(void* dest_, const uint16_t src, uint16_t size);
int EEPROM_write(uint16_t dest, const void* src_,  uint16_t size);
void eeprom_data();
void eeprom_circular();
void eeprom_circular_print();
unsigned char USART_receive();
void riceviValoriDaSoftware();
void aspetta_e_spera(int n);




