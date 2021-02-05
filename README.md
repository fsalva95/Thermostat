PROGETTO: TERMOSTATO REALIZZATO CON MICROCONTROLLORE

(Partecipanti: Gianluca Capozzi 1693255, Federico Fiorini 1709036, Salvatore Frontera 1710456).

• WHAT

Questo progetto si propone di realizzare un termostato in grado di misurare i valori in real-time di umidità e temperatura tramite l’utilizzo di un microcontrollore (in questo caso atmega 328p, lo stesso installato sulla scheda arduino UNO) e il sensore DHT11. I dati vengono raccolti tramite l’utilizzo del sensore DHT11 in grado di misurare i valori necessari. L’utente è in grado di settare, tramite un software appositamente creato, i valori di interesse, come il delay (tempo che trascorre tra due misurazioni successive), la temperatura massima e quella minima in grado di essere lette dal sensore (tenendo però presente che il sensore ha un ristretto range di misurazione e un tempo minimo che deve trascorrere tra una misurazione e l’altra) e il tipo di dato che si vuole ottenere (cioè misurazione di temperatura, di umidità o di entrambe). Dopo aver settato i parametri, questi verranno inviati tramite comunicazione seriale dal software al microcontrollore che li salverà sulla eeprom e li processerà. Una volta ottenute le misurazioni, queste verranno inviate, sempre tramite comunicazione seriale, al software che provvederà a visualizzarle sulla gui dopo averle elaborate. Ad ogni iterazione l’utente potrà decidere se cambiare i dati inseriti oppure no.

• HOW

Questo progetto è diviso in due sezioni: la gui, realizzata in c++, con l’utilizzo della libreria QT, e la parte di comunicazione tra la gui, il microcontrollore e il sensore.
Per quanto riguarda la gui questa è composta essenzialmente da tre sezioni, l’area per il settaggio dei dati, l’area per la visualizzazione delle varie misurazioni e un’area che mostra l’interazione software-microcontrollore. Una volta avviata la gui, l’utente dovrà premere il bottone “Connect” che, richiamando l’apposito metodo (on_pushButton_2_clicked()), andrà a verificare se il microcontrollore è collegato al PC. Una volta effettuato il collegamento, il microcontrollore invierà al software un carattere (‘%’) che sta a significare la possibilità di inviare dati. A questo punto sarà possibile settare i dati e inviarli, tramite il pulsante “Set” (che richiama il metodo on_pushButton_clicked()), al microcontrollore. I dati appena inseriti vengono formattati in una particolare tipologia di stringa del tipo: #a;b;c;d;e;! dove il carattere ‘#’ sta ad indicare l’inizio della stringa, l'intero 'a' (che ha sempre valore pari a 1) sta ad indicare che la stringa appena inviata è diversa da quella precedente, l’intero 'b' sta ad indicare la tipologia di dato che si vuole leggere (1 indica temperatura, 2 indica umidità e 3 entrambi), 'c' sta ad indicare la temperatura minima, 'd' la temperatura massima ed 'e' il delay. A questo punto la stringa verrà inviata dalla gui al microcontrollore.
Nel codice caricato sul microcontrollore, la prima operazione che viene eseguita è l’inizializzazione della porta seriale. Il pin usato per comunicare con il sensore è il 9.
Dopo queste operazioni preliminari, il microcontrollore entrerà nella sezione di codice corrispondente al loop. Come prima cosa si va a verificare se è stato ricevuto qualche dato. Dopo aver scompattato la stringa ricevuta si procede al rilevamento dei dati (tramite la funzione readT()) che verranno poi inviati tramite la seriale al software ogni volta che scade il delay impostato. Oltre che a regolamentare la frequenza di invio dei dati, tale delay serve anche per andare a verificare la presenza di una nuova stringa infatti, ogni volta che il delay scade, il microcontrollore invia il carattere ‘%’ alla gui. Se l’utente non ha impostato dei nuovi parametri allora la gui invierà la stringa “#0!” che sta a significare che i dati inviati in precedenza sono ancora validi altrimenti l’utente avrà inserito dei nuovi dati che verranno inviati come in precedenza.
La readT() rappresenta la funzione principale per la lettura del dato trasmesso dal sensore. In essa viene utilizzato un array di interi (data) in cui vengono memorizzati i dati trasmessi : in data[0] si trova il valore dell’umidità misurato, in data[2] quello della temperatura. Dopo l’assegnamento di registri, viene utilizzata la funzione impulso(), che esegue una sequenza di istruzioni tra cui la lettura digitale dal sensore, differenziando il segnale alto con il segnale basso ottenuto per poi assegnare il valore ottenuto in data. 

• HOW-TO-RUN

Per quanto riguarda la parte sul microcontrollore è stato scritto un makefile attraverso il quale si può compilare il programma scritto:
il compilatore utilizzato è avr-gcc. Per caricare il file sul microcontrollore sarà sufficiente lanciare da terminale il seguente comando:
    avrdude -v -p atmega328p -c arduino -P #porta_seriale_utilizzata -b 57600 -D -U flash:w:termostato.hex
Questo comando carica il file IHEX (generato in precedenza dal compilatore) nel chip Atmega, le opzioni informano avrdude di comunicare con 
il chip usando il protocollo seriale di Arduino, attraverso una particolare porta seriale, inoltre è specificato anche di usare un data rate di 115200 bps.
Per lanciare la gui è necessario aver installato sul proprio pc il software QTCreator. Una volta aperto il suddetto software sarà sufficiente importare il file con
estensione .pro contenuto nella directory temperature_monitor e lanciarlo.