#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

#define DEFAULT_PORT        2390
#define MAX_PACKET_LENGTH   256
#define MAX_VALUE_LENGTH    20

enum TYPES
{
    Move,
    Duration,
}TupelType;

typedef struct _slider_tlv
{
	TupelType               Type;
	uint8_t                 Length;
	uint8_t                 * Value;
	
} SLIDER_TLV, * PSLIDER_TLV;


/* -------------------------------------------------------------------------- */

void
TargetInit(
    struct sockaddr_in      * serv_addr
    )
{
    bzero((char *) serv_addr, sizeof(serv_addr));
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(DEFAULT_PORT);
     
    inet_pton(serv_addr->sin_family, "192.168.4.1", &serv_addr->sin_addr);
    
    printf("The Server:\n  IP: %s\n", "192.168.4.1");    
    return;
}


/* -------------------------------------------------------------------------- */

unsigned int
EnqueueTupel(
    unsigned int            CurrPos,
    const PSLIDER_TLV       * CurrTLV,
    unsigned int            BufferLen
    )
{
    unsigned int            LocPos = CurrPos;
    
    if(CurrPos >= BufferLen)
    {
        return 0;
    }
    LocPos += sizeof (SLIDER_TLV);
    
    return 0;
}


/* -------------------------------------------------------------------------- */

int
main(
    int argc,
    char *argv[]
    )
{
    int                     Sockfd, n;
    struct sockaddr_in      Serv_addr;
    char                    Buffer[MAX_PACKET_LENGTH];
    PSLIDER_TLV             CurrTLV = 0;
    unsigned int            CurrPos = 0;
    char                    ValueBuffer[MAX_VALUE_LENGTH];
    
    TargetInit(&Serv_addr);
      
    CurrPos = 0;
    CurrTLV = (PSLIDER_TLV) &Buffer;
    
    CurrTLV->Type = Move;
    CurrTLV->Length = sizeof(uint8_t);
    CurrTLV->Value = n = 2;
    
    printf("pos: %d\n", CurrPos);
    CurrPos = EnqueueTupel(CurrPos, &CurrTLV, MAX_PACKET_LENGTH);
    printf("pos: %d\n", CurrPos);
    
    
    Sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (Sockfd < 0) 
        error("ERROR opening socket");

       
    if (connect(Sockfd,(struct sockaddr *) &Serv_addr,sizeof(Serv_addr)) < 0)
    {
        error("ERROR connecting");
    }
        
    printf("Please enter the message: ");
    bzero(Buffer, 256);
    fgets(Buffer, 255, stdin);
    n = write(Sockfd,Buffer,strlen(Buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(Buffer,256);
    n = read(Sockfd, Buffer, 255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n", Buffer);
    close(Sockfd);
    return 0;
}
