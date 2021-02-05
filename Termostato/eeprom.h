void EEPROM_read(void* dest_, const uint16_t src, uint16_t size){
	uint8_t * dest=(uint8_t*)dest_;
	uint16_t s=src;;
	uint16_t s_end=src+size;
	while(s<s_end){
		eeprom_busy_wait();
		*dest=eeprom_read_byte((uint8_t*)s);
		++s;
		++dest;
	}
}

int EEPROM_write(uint16_t dest, const void* src_,  uint16_t size){
	const uint8_t * s=(uint8_t*)src_;
	const uint8_t * s_end=s+size;
	int length = 0;
	while(s<s_end){
		eeprom_busy_wait();
		eeprom_write_byte((uint8_t*)dest, *s);
		++s;
		++dest;
		length++;
	}
	return length;
}

void eeprom_data(){
	
	char buf1[20];
	char conv_buf[10];
	unsigned int write_length = 0;
	char eeprom_buffer[20];
	
	addr = 0;
	count_circular = 0;
	
	sprintf(conv_buf, "%d\0", sens_to_read);
	write_length = EEPROM_write(addr, conv_buf, strlen(conv_buf)+1);
	memset(eeprom_buffer, 0, strlen(eeprom_buffer));
	EEPROM_read(eeprom_buffer, addr, strlen(conv_buf)+1);
	eeprom_buffer[strlen(conv_buf)-1]=0;
	sprintf(buf1, "sens_read %s at %d\r\n", eeprom_buffer, addr);
	serialWrite(buf1);
	
	_delay_ms(1);
	
	addr += write_length;

	sprintf(conv_buf, "%d\0", min_value);
	write_length = EEPROM_write(addr, conv_buf, strlen(conv_buf)+1);
	memset(eeprom_buffer, 0, strlen(eeprom_buffer));
	EEPROM_read(eeprom_buffer, addr, strlen(conv_buf)+1);
	eeprom_buffer[strlen(conv_buf)-1]=0;
	sprintf(buf1, "min_value %s at %d\r\n", eeprom_buffer, addr);
	serialWrite(buf1);
	
	_delay_ms(1);
	
	addr += write_length;
	
	sprintf(conv_buf, "%d\0", max_value);
	write_length = EEPROM_write(addr, conv_buf, strlen(conv_buf)+1);
	memset(eeprom_buffer, 0, strlen(eeprom_buffer));
	EEPROM_read(eeprom_buffer, addr, strlen(conv_buf)+1);
	eeprom_buffer[strlen(conv_buf)-1]=0;
	sprintf(buf1, "max_value %s at %d\r\n", eeprom_buffer, addr);
	serialWrite(buf1);
	
	_delay_ms(1);
	
	addr += write_length;
	
	sprintf(conv_buf, "%d\0", delay_serial);
	write_length = EEPROM_write(addr, conv_buf, strlen(conv_buf)+1);
	memset(eeprom_buffer, 0, strlen(eeprom_buffer));
	EEPROM_read(eeprom_buffer, addr, strlen(conv_buf)+1);
	eeprom_buffer[strlen(conv_buf)-1]=0;
	sprintf(buf1, "del_read %s at %d\r\n", eeprom_buffer, addr);
	serialWrite(buf1);
	
	addr += write_length;
	
	init_addr_data = addr; // Memorizzo dove si trova l'ultimo elemento memorizzato
						   // da qui parte buffer circolare
	addr_data = init_addr_data;
	
	_delay_ms(1);
	
}

void eeprom_circular(){
	char buf[50];
	char eeprom_buffer[20];
	int write_length = 0;
	
	circular_buffer_length = 5*strlen(str)+init_addr_data;
	
	if(addr_data < init_addr_data){
		addr_data = init_addr_data;
	}
	
	write_length = EEPROM_write(addr_data, str, strlen(str)+1);
	memset(eeprom_buffer, 0, strlen(eeprom_buffer));
	
    addr_data = (addr_data+strlen(str))%circular_buffer_length;
	
    count_circular += strlen(str);
    
    memset(buf, 0, strlen(buf));
    
	serialWrite(buf);
}

void eeprom_circular_print(){
	char buf[20];
	char eeprom_buffer[20];
	int write_length = 0;
	int addr = init_addr_data;
	int read;
	memset(eeprom_buffer, 0, strlen(eeprom_buffer));
	for(read = 0; read < circular_buffer_length-init_addr_data; read += strlen(str)){
		EEPROM_read(eeprom_buffer, addr, strlen(str)+1);
		eeprom_buffer[strlen(str)-1]=0;
		sprintf(buf, "circular_buffer %s at %d\r\n", eeprom_buffer, addr);
		serialWrite(buf);
		addr += strlen(eeprom_buffer);
	}
	count_circular = 0;
}
