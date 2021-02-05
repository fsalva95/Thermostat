#include "termostato.h"
#include "eeprom.h"
#include "serial.h"

void main(){
	USART_init(BRC); // INIZIALIZZAZIONE SERIALE (TX)
	sei();
	DDRB |= _BV(DDB1); //set pin 9 output
	bit=digitalPinToBitMask(9); //risultato digitalPinToBitMask(pin);
	maxCycles = microsecondsToClockCycles(1000);
	porta = digitalPinToPort(9);
	_delay_ms(100);

	while(1){
		//la lettura va fatta ogni 2 secondi altrimenti il sensore si incazza
		//ogni volta che finisce un ciclo di lettura va fatto un delay di 2000ms
		aspetta_e_spera((int)delay_serial);
		
		flag=1;
        riceviValoriDaSoftware();

        if(cambiamentoStringaInput){
			eeprom_data();
		}
		
		_delay_ms(100);
		int ret=readT();
		
        if (sens_to_read == 1 || sens_to_read == 3){
			int f = data[2];
			strcpy(str,"T:");
			itoa(f,str+2,10);
			strcat(str,",\n\0");
			serialWrite(str);
        }
        
        if (sens_to_read == 2 || sens_to_read == 3){
            int u=data[0];
            strcpy(str,"H:");
            itoa(u,str+2,10);
            strcat(str,",\n\0");
            serialWrite(str);
        }
        eeprom_circular();
        if (count_circular == circular_buffer_length-init_addr_data){
			eeprom_circular_print();
			eeprom_data();
			//serialWrite("%");
		}
        memset(str, 0, strlen(str));
	}
}

//FUNZIONE CHE BUFFERIZZA TUTTI I CARATTERI RICEVUTI FINO A RITORNO A CAPO
void riceviValoriDaSoftware(){
    k=0;
    int res;
    int i=1;  //indice che indica la posizione del carattere corrente dell'interval vect
    int j=0; //indice che indica la posizione del carattere corrente dell'aux vect
    int index=0; //indice che indica la zona della stringa in input(suddivisa in 5 zone separate da ;)
    char aux_Vect[40]= { 0 }; //vettore ausiliario che utilizzo per memorizzare i dati
    memset(aux_Vect, 0, strlen(aux_Vect));
    char Interval_Vect[30];
    memset(Interval_Vect, 0, strlen(Interval_Vect));
    serialWrite("%");
    while(flag){
        Interval_Vect[k]=USART_receive();
        if(Interval_Vect[k]=='\n'){ //&& Interval_Vect[k-1]=='!'){
            flag=0;
        }
        k++;
    }
    if(Interval_Vect[1] == '1'){
		cambiamentoStringaInput = 1;
		while (Interval_Vect[i]!='!'){
			if (Interval_Vect[i]!=';'){
				aux_Vect[j]=Interval_Vect[i];
				i++;
				j++;
			}
			else {
				aux_Vect[j]='\0';
				j=0;
				res=atoi(aux_Vect);
				if (index==1){
					sens_to_read=res;
				}
				else if (index==2){
					min_value=res;
				}
				else if (index==3){
					max_value=res;
				}
				else{
					delay_serial=res;
				}
				res=0;
				memset(aux_Vect, 0, strlen(aux_Vect));
				index++;
				i++;
			}
			
		}
	}
	else{
		cambiamentoStringaInput = 0;
	}
}

int readT(){
    int i;
	for(i=0;i<5;i++){
		data[i]=0; //azzero vettore in cui memorizzo i dati
	}

	PORTB |= (1 << PIN); //mi metto in uno stato di alta impedenza per iniziare la lettura del sensore
	_delay_ms(DELAY_MS); //delay(250);

	DDRB |= (1 << PIN); //set pin 9 output -- pinMode(_pin, OUTPUT);
	PORTB &= (0 << PIN); //digitalWrite(_pin, LOW);
	_delay_ms(DELAY_MS_MIN); //delay(20);


	/* va disattivato interrupt perchè questa parte di lettura è fortemente legata al tempo
	 * il microcontrollore aspetta un tempo che va da 20 a 40 uS
	 * tempo in cui il sensore di temperatura può fornire una risposta
	 */
	 cli(); //disabilito interrupt

	
	PORTB |= (1 << PIN); 			//termino il segnale di avvio impostando un segnale logico alto per 40 microsecondi e attendo 
									//la risposta
    _delay_us(DELAY_MS_RESPONSE);

    //Setto il pin come INPUT_PULLUP e attendo circa 10 microsecondi per far stabilizzare la linea
    reg = portModeRegister(porta);		
	out = portOutputRegister(porta);

    uint8_t oldSREG = SREG;
    *reg &= ~bit;
	*out |= bit;
	SREG = oldSREG;
    _delay_us(10);  


	//se impulso(LOW) ritorna 0, sono passati più di 80 microsecondi
	//tempo entro il quale mi aspetto un segnale basso
    if (impulso(LOW) == 0){
      return 0;
    }

	//se impulso(LOW) ritorna 0, sono passati più di 80 microsecondi
	//tempo entro il quale mi aspetto un segnale alto
    if (impulso(HIGH) == 0) {
      return 0;
    }

	
	/* leggo i 40 bit che mi invia il sensore
	 * ogni trasmissione comincia con un livello logico di tensione basso che dura circa ~50microsecondi
	 * Il segnale che segue, a seconda della durata dice se è a 1 o 0 il bit
	 * 0 -> segnale di durata variabile da 26 a 28 us
	 * 1 -> segnale di durata variabile fino a 70 us
	 */
	 
    for (i=0; i<80; i+=2) {
      cycles[i]   = impulso(LOW);
      cycles[i+1] = impulso(HIGH);
    }

    for (i=0; i<40; ++i) {
		uint32_t lowCycles  = cycles[2*i];
		uint32_t highCycles = cycles[2*i+1];
		if ((lowCycles == 0) || (highCycles == 0)) {
		  return 0;
		}
		data[i/8] <<= 1;
		if (highCycles > lowCycles) {
		  data[i/8] |= 1;
		}
	}
    sei(); //riabilito interrupt
    return 1;
}

uint32_t impulso(int level){
	/*
	 * level=1 -> HIGH
	 * level=0 -> LOW
	 */
	 
	count = 0;
	
    while (digitalRead(9) == level) {
      if (count++ >= maxCycles) {
        return 0; // Exceeded timeout, fail.
      }
    }

    return count;
}

int digitalRead(uint8_t pin){
	uint8_t bit = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);

	if (port == NOT_A_PIN) return LOW;
	
	// Per PWM
	if (*portInputRegister(port) & bit) return HIGH;
	return LOW;
}

void aspetta_e_spera(int n) {
	while(n--) {
		_delay_ms(1);
	}
}
