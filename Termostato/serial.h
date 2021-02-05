void USART_init(uint8_t ubrr){
	UBRR0H = ubrr >> 8;
	UBRR0L = ubrr;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (3 << UCSZ00);
}

void appendSerial(char c){
    while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = c;
}

void serialWrite(char c[]){
    uint8_t i;
	for(i = 0; i < strlen(c); i++){
		appendSerial(c[i]);
	}

	if(UCSR0A & (1 << UDRE0)){
		UDR0 = 0;
	}
}

ISR(USART_TX_vect){
	if(serialReadPos != serialWritePos)
	{
		UDR0 = str[serialReadPos];
		serialReadPos++;

		if(serialReadPos >= strlen(str))
		{
			serialReadPos++;
		}
	}
}

unsigned char USART_receive(void){
	while(!(UCSR0A & (1 << RXC0)));
	return UDR0;
}
