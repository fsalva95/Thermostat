#include "seriale.h"
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>


#define LEN 200
int ret=0;
int irec=-1;

int ret2=0;

char rec[LEN];
void stampa_arr(char c[],int car);
int scrivi(int fd,char* c);
int leggi(int fd);
void childFunction();
void delay(unsigned int milliseconds);
void pulisci(char s[]);
void intHandler(int dummy) ;
int contains(char s[], char c);

int serial_set_interface_attribs (int fd, int speed, int parity);
void serial_set_blocking (int fd, int should_block);
int serial_open(const char* name) ;


void gestisci_input();
void crea_stringhetta_vit();

char* str_base="#0!\n";
pid_t pid;
int sens=0;
int max_t=0;
int min_t=0;
int del=0;
char ter;

char param[22];
char* str="#1;1;0;40;2000;!\n";
char* str2="#0!\n";

int fd;
int ap;


int cane=0;


int entra = 0;

void main(){
	signal(SIGINT, intHandler);
	
	fd=serial_open("/dev/ttyACM0");
	ap=serial_set_interface_attribs(fd,9600,0);
	
	while(1){
		if(!entra){
			gestisci_input();
			entra = 1;
		}
		else{
			
			if(contains(rec,'%')){
				if(cane){
					cane=0;
					memset(rec,0,strlen(rec));
					ret=write(fd,param,strlen(param));
				}
				
				else{
					memset(rec,0,strlen(rec));
					ret=write(fd,str2,strlen(str2));
				}
			}
			ret2=leggi(fd);
			stampa_arr(rec,ret2);
		}
	}
	return;
}
 

void stampa_arr(char c[],int car){
	int i=0;
	while(i<car){
		printf("%c",c[i]);
		i++;
	}
	printf("\n");
}

int scrivi(int fd,char* c){
	int len=strlen(c);
	int scritti=0;
	while(scritti<len){
		scritti+=write(fd,c,strlen(c));
	}
	
	return scritti;
}

int leggi(int fd){
	int letti=-1;
	while(letti<0){
		letti=read(fd,rec,LEN-1);
	}
	
	return letti;
}

void pulisci(char s[]){
	int i = 0;
	while(i < strlen(s)){
		s[i] = 0;
		i++;
	}
}

void delay(unsigned int milliseconds){

    clock_t start = clock();

    while((clock() - start) * 1000 / CLOCKS_PER_SEC < milliseconds);
}

void intHandler(int dummy) {
	gestisci_input();
	ret2 = leggi(fd);
	if(contains(rec, '%')){
		ret=write(fd,param,strlen(param));
	}
}

void gestisci_input(){
	
		printf("cane\n");
		printf("INSERISCI SENSORE DA LEGGERE:  ");
		printf("1 -> Temperatura\n");
		printf("2 -> Humidita\n");
		printf("3 -> TUTTEDDUE\n");
		scanf("%d",&sens);
		
		while(sens>3){
			
			if(sens==4){ //USCITA
				printf("CIAO CORE!!!!\n");
				int ret=close(fd);
				exit(0);
			}
			else{
				printf("INSERISCI SENSORE DA LEGGERE:  ");
				printf("1 -> Temperatura\n");
				printf("2 -> Humidita\n");
				printf("3 -> TUTTEDDUE\n");
				scanf("%d",&sens);
			}
			
		}
		

		
		printf("INSERISCI TEMPERATURA MINIMA:  ");
		scanf("%d",&min_t);
		printf("INSERISCI TEMPERATURA MASSIMA:  ");
		scanf("%d",&max_t);
		printf("INSERISCI INTERVALLO DI TEMPO IN SECONDI (> 2 secondi):  ");
		scanf("%d",&del);
		
		while(min_t > max_t){
			printf("La temperatura minima deve essere minore id quella massima\n");
			printf("INSERISCI TEMPERATURA MINIMA (la massima che hai inserito è: %d)",max_t);
			scanf("%d",&min_t);
		}
		
		while(del<2){
			printf("Devi inserire un tempo di campionamento maggiore di 2 secondi\n");
			scanf("%d",&del);
		}
		
		crea_stringhetta_vit();
		cane=1;
}

void crea_stringhetta_vit(){
		printf("%d,%d,%d,%d\n",sens,max_t,min_t,del);
		int i=0;
		param[i]='#';
		i++;
		param[i]='1';
		i++;
		param[i]=';';
		i++;
		sprintf(param+i,"%d", sens);
		i++;
		param[i]=';';
		i++;
		sprintf(param+i,"%d", min_t);
		if(min_t<10){
			i++;
		}
		else{
			i=i+2;
		}
		param[i]=';';
		i++;
		sprintf(param+i,"%d", max_t);
		if(max_t<10){
			i++;
		}
		else{
			i=i+2;
		}
		param[i]=';';
		i++;
		sprintf(param+i,"%d", (del*1000));
		i=i+4;
		param[i]=';';
		i++;
		param[i]='!';
		i++;
		param[i]='\n';
		stampa_arr(param,strlen(param));
}

int contains(char s[], char c){
	int i;
	for(i = 0; i < strlen(s); i++){
		if(s[i]==c){
			return 1;
		}
	}
	return 0;
}

int serial_set_interface_attribs (int fd, int speed, int parity) {
  struct termios tty;
  memset (&tty, 0, sizeof tty);
  if (tcgetattr (fd, &tty) != 0) {
    printf ("error %d from tcgetattr", errno);
    return -1;
  }
  switch (speed){
  case 57600:
    speed=B57600;
    break;
  case 115200:
    speed=B115200;
    break;
   case 9600:
	speed=B9600;
	break;
  default:
    printf("cannot sed baudrate %d\n", speed);
    return -1;
  }
  cfsetospeed (&tty, speed);
  cfsetispeed (&tty, speed);
  cfmakeraw(&tty);
  // enable reading
  tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
  tty.c_cflag |= parity;
  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars

  if (tcsetattr (fd, TCSANOW, &tty) != 0) {
      printf ("error %d from tcsetattr", errno);
      return -1;
    }
  return 0;
}

void serial_set_blocking (int fd, int should_block) {
  struct termios tty;
  memset (&tty, 0, sizeof tty);
  if (tcgetattr (fd, &tty) != 0)
    {
      printf ("error %d from tggetattr", errno);
      return;
    }

  tty.c_cc[VMIN]  = should_block ? 1 : 0;
  tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

  if (tcsetattr (fd, TCSANOW, &tty) != 0)
    printf ("error %d setting term attributes", errno);
}

int serial_open(const char* name) {
  int fd = open (name, O_RDWR | O_NOCTTY | O_SYNC );
  if (fd < 0) {
    printf ("error %d opening serial, fd %d\n", errno, fd);
  }
  return fd;
}
