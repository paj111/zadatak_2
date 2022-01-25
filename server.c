#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr
	

/*ovo je jezgro serverske funkcionalnosti-ova funkcija se poziva
nakon uspostavljanja veze sa klijentom kako bi klijentu bilo
omoguceno da komunicira sa serverom. */
void doprocessing (int sock)
{
    char buff[MAX];
    int n;
    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);
   	
        // read the message from client and copy it in buffer
        read(sock, buff, sizeof(buff));
        // print buffer which contains the client contents
        printf("Klijent -> Server %s\t To client : ", buff);
        bzero(buff, MAX);
        n = 0;
        // copy server message in the buffer
        while ((buff[n++] = getchar()) != '\n')
            ;
   
        //and send that buffer to client
        write(sock, buff, sizeof(buff));
   
        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("ENDE", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
        
    }
}

/* glavni program serverske aplikacije */
int main( int argc, char *argv[] )
{
    int sockfd, newsockfd, portno, clilen,ch;
     int key = 3, done = 0;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int  n;

    /* najpre se poziva uvek socket() funkcija da se registruje socket:
	AF_INET je neophodan kada se zahteva komunikacija bilo koja
	dva host-a na Internetu;
	Drugi argument definise tip socket-a i moze biti SOCK_STREAM ili SOCK_DGRAM:
	SOCK_STREAM odgovara npr. TCP komunikaciji, dok SOCK_DGRAM kreira npr. UDP kanal
	Treci argument je zapravo protokol koji se koristi: najcesce se stavlja 0 sto znaci da
	OS sam odabere podrazumevane protokole za dati tip socket-a (TCP za SOCK_STREAM
	ili UDP za SOCK_DGRAM)  */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    /* Inicijalizacija strukture socket-a */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 5001;
    serv_addr.sin_family = AF_INET; //mora biti AF_INET
    /* ip adresa host-a. INADDR_ANY vraca ip adresu masine na kojoj se startovao server */
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    /* broj porta-ne sme se staviti kao broj vec se mora konvertovati u
	tzv. network byte order funkcijom htons*/
    serv_addr.sin_port = htons(portno);
 


    /* Sada bind-ujemo adresu sa prethodno kreiranim socket-om */
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
                          sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(1);
    }
    printf("Server started.. waiting for clients ...\n");

    /* postavi prethodno kreirani socket kao pasivan socket
	koji ce prihvatati zahteve za konekcijom od klijenata
	koriscenjem accept funkcije */
    listen(sockfd,5); //maksimalno 5 klijenata moze da koristi moje usluge
    clilen = sizeof(cli_addr);
	
    while (1)
    { 	
    
	/*ovde ce cekati sve dok ne stigne zahtev za konekcijom od prvog klijenta*/
        newsockfd = accept(sockfd,
                (struct sockaddr *) &cli_addr, &clilen);
	printf("Client connected...\n");
        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            exit(1);
        }

        /* Kreiraj child proces sa ciljem da mozes istovremeno da
	komuniciras sa vise klijenata */
	int  pid = fork();
        if (pid < 0)
        {
            perror("ERROR on fork");
	    exit(1);
        }
        if (pid == 0)  
        {
            /* child proces ima pid 0 te tako mozemo znati da li
		se ovaj deo koda izvrsava u child ili parent procesu */
            close(sockfd);
            doprocessing(newsockfd);
            exit(0);
        }
        else
        {
		/*ovo je parent proces koji je samo zaduzen da
		delegira poslove child procesima-stoga ne moras
		da radis nista vec samo nastavi da osluskujes
		nove klijente koji salju zahtev za konekcijom*/		
            close(newsockfd);
        }
    } /* end of while */
    //ave cezar desifrovanje
    for (int i = 0; buffer[i] != '\0'; ++i)
        {
            ch = buffer[i];
            if (ch >= 'a' && ch <= 'z')
            {
                ch = ch - key;
                if (ch < 'a')
                {
                    ch = ch + 'z' - 'a' + 1;
                }
                buffer[i] = ch;
            }
            else if (ch >= 'A' && ch <= 'Z')
            {
                ch = ch - key;
                if (ch < 'A')
                {
                    ch = ch + 'Z' - 'A' + 1;
                }
                buffer[i] = ch;
            }
        }
    
    
}


