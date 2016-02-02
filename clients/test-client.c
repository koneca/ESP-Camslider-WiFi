/* test-client.c */

/* -------------------------------------------------------------------------- */
/*!	\ingroup test-client
	\file
	\brief Client for Linux based Systems. For debugging the Target
	*/ 
/* -------------------------------------------------------------------------- */

#include "test-client.h"


/* -------------------------------------------------------------------------- */

ErrorCode 
ClearBuffer(
	char					*Buffer,
	unsigned int 			Length
)
{
	ErrorCode				Status = ERR_SUCCESS;
	unsigned int 			I = 0;
	__try
	{
	
		if(0 == *Buffer || 1 > Length)
		{
			CHECK(ERR_INSUFFICIENT_RESOURCES);
		}
	
		for(I = 0; I < Length; I++)
		{
			Buffer[I] = '\0';
		}
	
	}
	__finally
	{
	
	}
	return Status;
}

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
	PSLIDER_LIST_ENTRY		RequestsList,
	const char 				* Buffer,
    unsigned int            CurrPos,
    const unsigned int      BufferLen,
    const unsigned short	Id,
    const uint8_t			Type,
    const unsigned char *	Value,
    const uint8_t			Length
    )
{
    unsigned int            LocPos = CurrPos;
	PSLIDER_TLV_DATA		CurrTLV = 0;
	unsigned int 			I = 0;
	    
    if(LocPos >= BufferLen)
    {
        printf("Enqueue Tupel: Error: buffer overflow Pos: %d Len:%d\n", LocPos, BufferLen);
        return 0;
    }  
    if((LocPos + sizeof(SLIDER_TLV_DATA)) > BufferLen)
    {
        printf("Enqueue Tupel: Error: buffer full\n");
        return 0;
    }
    if(0 == Buffer || 0 == Value )
    {
        printf("Enqueue Tupel: Error: Missing input data\n");
        return 0;
    }
    if(Length > MAX_VALUE_LENGTH)
    {
    	printf("Enqueue Tupel: Error: Missing input data\n");
        return 0;
    }
 
 	printf("locpos %d\n", LocPos);
    CurrTLV = (PSLIDER_TLV_DATA) &Buffer[LocPos];
    CurrTLV->Id = Id;
    CurrTLV->Type = Type;
    CurrTLV->Length = Length;
    
    for(I = 0; I < Length; I++)
    {
    	CurrTLV->Value[I] = Value[I];
    }
    
    

    LocPos += sizeof (SLIDER_TLV_DATA) - (MAX_VALUE_LENGTH-Length);
    
    return LocPos;
}

ErrorCode
BuildMessage(
	PSLIDER_APP				Slider,
	char					* Buffer,
	const unsigned int		CurrPos,
	const unsigned int      BufferLen
	)
{
    unsigned int            LocPos = CurrPos;
    
	ClearBuffer(Buffer, BufferLen);
    
    printf("pos %d 0x%04x\n", LocPos, Slider->CurrId);
    LocPos = EnqueueTupel(&Slider->RequestsList, Buffer, LocPos, MAX_PACKET_LENGTH, htons(Slider->CurrId), TLV_Move, (unsigned char *)"left", (uint8_t)sizeof("left"));
	
	Slider->CurrId++;
    
    printf("pos %d 0x%04x\n", LocPos, Slider->CurrId);
    LocPos = EnqueueTupel(&Slider->RequestsList, Buffer, LocPos, MAX_PACKET_LENGTH, htons(Slider->CurrId), TLV_Move, (unsigned char *)"right", (uint8_t)sizeof("right"));
	
	Slider->CurrId++;
	
    printf("pos %d \n", LocPos);
    
	return LocPos;
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
    char         			Buffer[MAX_PACKET_LENGTH];
    unsigned int            CurrPos = 0;
    //char                    ValueBuffer[MAX_VALUE_LENGTH];
   	SLIDER_APP				Slider;
    
    TargetInit(&Serv_addr);
    
    Slider.ServerAdress = Serv_addr;
        
    CurrPos = BuildMessage(&Slider, Buffer, 0, MAX_PACKET_LENGTH);
      
    HexDump("after", Buffer, CurrPos);
    
    Sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (Sockfd < 0) 
        error("ERROR opening socket");

       
    if (connect(Sockfd,(struct sockaddr *) &Serv_addr,sizeof(Serv_addr)) < 0)
    {
        error("ERROR connecting");
    }
        
    printf("Please enter the message: ");
    bzero(Buffer, 256);
   // fgets(Buffer, 255, stdin);
    n = write(Sockfd, Buffer, strlen(Buffer));
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
