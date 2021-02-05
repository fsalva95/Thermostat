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
#include <sys/types.h> // mkfifo()
#include <sys/stat.h>  // mkfifo()


#include "common.h"
#define LEN 200
int ret=0;
int irec=-1;
///////////////////////////77////////////////////////7
int ret5;
    int client_fifo;
    int bytes_left, bytes_sent, bytes_read;
    char buf[1024];
////////////////////////////////////////////////////////

int ret2=0;

char rec[LEN];
void stampa_arr(char c[],int car);
int scrivi(int fd,char* c);
int leggi(int fd);
void childFunction();
void* thread_work(void *arg);

char* str_base="#0!\n";
pid_t pid;
int sens=0;
int max_t=0;
int min_t=0;
int del=0;
char ter;

char* str="#1;1;0;40;2000;!\n";
char* str2="#0!\n";

int fd;
int ap;



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

void main(){



    printf("fatto");


    //ret5 = mkfifo(CLNT_FIFO_NAME, 0666);
    //ERROR_HELPER(ret5, "Cannot create Client FIFO");

    printf("fatto");

    client_fifo = open(CLNT_FIFO_NAME, O_RDONLY);
    ERROR_HELPER(client_fifo, "Cannot open Client FIFO for reading");




	pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t));
		if (pthread_create(threads, NULL, thread_work, NULL) != 0) {
			fprintf(stderr, "Can't create a new thread, error %d\n", errno);
			exit(EXIT_FAILURE);
			return ;
		}
		pthread_join(*threads, NULL);

	printf("ok\n");
	//fd=serial_open("/dev/ttyACM0");
	//ap=serial_set_interface_attribs(fd,9600,0);

	//const char *cmd = "gnome-terminal -x sh -c \"/home/ivb/first\"";
	//system("gnome-terminal");
	/*
	while(ter!='x'){


	}
	*/


	/*
	pid_t fork_result=fork(); // here we store the result of the fork;

	 if(fork_result==0){ // we are in the child process
		pid=getpid();
		printf(" started, pid: %d\n", pid);
		childFunction();
	  } else {
		printf("%s SONO IL PADRE, pid: %d\n", pid);
		//funzione babbo
		//wait(0);
	  }
	  printf(" terminating, pid: %d\n", pid);
	*/

	/*
	//char rec[LEN]; da modificare la dichiarazuone di leggi e passare il puntatore al vettore
	int fd=serial_open("/dev/ttyACM0");
	int ap=serial_set_interface_attribs(fd,9600,0);
	printf("FD: %d\n",fd);
	printf("AP: %d\n",ap);
	delay(1000);
	*/



	/*
	ret2=leggi(fd);
	printf("LETTI %d\n",ret2);
	//printf("LUNGHEZZA ricevuta: %d\n",strlen(rec));
	stampa_arr(rec,ret2);
	ret2=0;


	//printf("LUNGHEZZA STRINGA DA INVIARE: %ld\n",strlen(str));
	delay(1000);
	ret=write(fd,str,strlen(str));
	printf("SCRITTI: %d\n",ret);
	delay(1000);


	pulisci(rec);
	printf("pulito: %s\n",rec);
	ret2=leggi(fd);

	printf("LETTI %d\n",ret2);
	stampa_arr(rec,ret2);

	if(rec[ret2-2]==','){
		printf("COMPLETO\n");
		ret=write(fd,str2,strlen(str2));
	}

	pulisci(rec);
	ret2=leggi(fd);
	printf("LETTI %d\n",ret2);
	printf("ho letto2: \n%s\n",rec);

	*/
    // close the descriptors and destroy the two FIFOs
    ret = close(client_fifo);
    ERROR_HELPER(ret, "Cannot close Client FIFO");

    // destroy the two FIFOs
    ret = unlink(CLNT_FIFO_NAME);
    ERROR_HELPER(ret, "Cannot unlink Client FIFO");

    exit(EXIT_SUCCESS);

	return;
}

void* thread_work(void *arg) {
    ////////////////////////////////////////
    char* quit_command = QUIT_COMMAND;
    size_t quit_command_len = strlen(quit_command);
    ////////////////////////////////////////
	int fp;
	//system("gnome-terminal");
	printf("STREDDO");
	fd=serial_open("/dev/ttyACM0");
	ap=serial_set_interface_attribs(fd,9600,0);

	ret2=leggi(fd);
	delay(200);
	ret=write(fd,str,strlen(str));
	printf("SCRITTI: %d\n",ret);

	while(1){
		pulisci(rec);
		//printf("pulito: %s\n",rec);
		ret2=leggi(fd);
		//printf("LETTI %d\n",ret2);
		stampa_arr(rec,ret2);

		/*
		if(rec[ret2-2]==','){
			printf("COMPLETO\n");
			//ret=write(fd,str2,strlen(str2));
		}
		* */


		pulisci(rec);
		ret2=leggi(fd);
		//printf("LETTI %d\n",ret2);

		if(rec[0]=='%'){
			printf("ok");
			pulisci(rec);
            /////////////////////////////////////////////////////////////////
            while ( (bytes_read = read(client_fifo, buf, sizeof(buf) - 1)) <= 0 ) {
            if (bytes_read == 0) {
                fprintf(stderr, "Client process has closed the Client FIFO unexpectedly! Exiting...\n");
                ret5 = close(client_fifo);
                ERROR_HELPER(ret5, "Cannot close Client FIFO");
                ret5 = unlink(CLNT_FIFO_NAME);
                ERROR_HELPER(ret5, "Cannot unlink Client FIFO");
                exit(EXIT_FAILURE);
            }
            if (errno == EINTR) continue;
            ERROR_HELPER(-1, "Cannot read from Client FIFO");
            }

            if (DEBUG) {
                buf[bytes_read] = '\0';
                printf("Message received: %s\n", buf);
            }

            // check whether I have just been told to quit...
            if (bytes_read == quit_command_len && !memcmp(buf, quit_command, quit_command_len)) break;




            /////////////////////////////////////////////////////////////////
            if (strlen(buf)!=0){
                ret=write(fd,buf,strlen(buf));
            }
            else ret=write(fd,str2,strlen(str2));
		}
		delay(500);
	}
    /*while (1) {  //lettura da altra roxterm

        while ( (bytes_read = read(client_fifo, buf, sizeof(buf) - 1)) <= 0 ) {
            if (bytes_read == 0) {
                fprintf(stderr, "Client process has closed the Client FIFO unexpectedly! Exiting...\n");
                cleanFIFOs(echo_fifo, client_fifo);
                exit(EXIT_FAILURE);
            }
            if (errno == EINTR) continue;
            ERROR_HELPER(-1, "Cannot read from Client FIFO");
        }

        if (DEBUG) {
            buf[bytes_read] = '\0';
            printf("Message received: %s\n", buf);
        }

        // check whether I have just been told to quit...
        if (bytes_read == quit_command_len && !memcmp(buf, quit_command, quit_command_len)) break;
    } */
	/*
	while(1){
		printf("cane\n");
		printf("INSERISCI SENSORE DA LEGGERE");
		scanf("%d",&sens);
		printf("INSERISCI TEMPERATURA MASSIMA");
		scanf("%d",&max_t);
		printf("INSERISCI TEMPERATURA MINIMA");
		scanf("%d",&min_t);
		printf("INSERISCI INTERVALLO DI TEMPO:");
		scanf("%d",&del);

		printf("%d,%d,%d,%d\n",sens,max_t,min_t,del);
	}
	*/


	return NULL;
}

void stampa_arr(char c[],int car){
	printf("ARRAY:  ");
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
