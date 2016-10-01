/* test-client.c */

/* -------------------------------------------------------------------------- */
/*!	\ingroup test-client
	\file
	\brief Client for Linux based Systems. For debugging the Target
	*/ 
/* -------------------------------------------------------------------------- */

#include "test-client.h"

/* -------------------------------------------------------------------------- */

ERROR_CODE
ClearBuffer(
	char					*Buffer,
	unsigned int 			Length
)
{
	ERROR_CODE				Status = ERR_SUCCESS;
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
/*! \brief euqueue one TLV tupel to the send string. For now there will be sent only one tupel at a time (no queue on slider side)
 *  @param [IN/OUT]Buffer The send buffer, where the the Tupel has to be enqueued. This have to be allocated bevore invocation
 *  @param [IN]CurrPos Position in the buffer
 *  @param [IN]BufferLen The length of the send buffer
 *  @param [IN]Id The ID of the command. Because the Slider answers to every Command. Store the command in an queue according to this ID 
 *  @param [IN]Type The type of the command
 *  @param [Length] The length of the command
 *  @param [IN]Value The value of the command
 */
unsigned int
EnqueueTupel(
	PLIST_ENTRY				RequestsList,
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
	PSLIDER_LIST_ENTRY 		Entry = 0;
	    
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
 
 	//printf("locpos %d\n", LocPos);
    CurrTLV = (PSLIDER_TLV_DATA) &Buffer[LocPos];
    CurrTLV->Id = Id;
    CurrTLV->Type = Type;
    CurrTLV->Length = Length;
    
    for(I = 0; I < Length; I++)
    {
    	CurrTLV->Value[I] = Value[I];
    }
   
    LocPos += sizeof (SLIDER_TLV_DATA) - (MAX_VALUE_LENGTH-Length);
    
    Entry = (PSLIDER_LIST_ENTRY)malloc(sizeof(SLIDER_LIST_ENTRY));
    memcpy(&Entry->Data, CurrTLV, LocPos - CurrPos);

    InsertTailList(RequestsList, Entry->Entry);
    
    return LocPos;
}

/*unsigned int
BuildMessage(
	PSLIDER_APP				Slider,
	char					* Buffer,
	const unsigned int		CurrPos,
	const unsigned int      BufferLen
	)
{
    unsigned int            LocPos = CurrPos;
    
	ClearBuffer(Buffer, BufferLen);

    LocPos = EnqueueTupel(&Slider->RequestsList, Buffer, LocPos, MAX_PACKET_LENGTH, htons(Slider->CurrId), TLV_Move, (unsigned char *)"left", (uint8_t)sizeof("left"));
	
	Slider->CurrId++;
    
	return LocPos;
}*/

unsigned int
SliderConnect(
	PSLIDER_APP Slider
	)
{
	unsigned int 			Status = 0;

	Slider->SockFd = 0;

    Slider->SockFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (Slider->SockFd < 0)
    {
        TRACE("ERROR opening socket");
	}

    if (connect(Slider->SockFd, (struct sockaddr *)&Slider->ServerAdress, sizeof(Slider->ServerAdress)) < 0)
    {
        error("ERROR connecting");
    }
    return Status;
}

unsigned int
SliderDisconnect(
	PSLIDER_APP				Slider
	)
{
	return close(Slider->SockFd);
}

ERROR_CODE
ReadCommandFromStdin(
		char 				* Buffer,
		size_t				* Length
		)
{
	ERROR_CODE				Status = 0;

	if(0 == Buffer)
	{
		return ERR_WRONG_INPUT;
	}

	TRACE("Please enter command:\t");

	if(-1 == getline(&Buffer, Length, stdin))
	{
		TRACE("Error reading line \n");
		free(Buffer);
		return ERR_WRONG_INPUT;
	}


	return Status;
}

unsigned int
ComputeCommand(
	PSLIDER_APP	 			Slider,
	char					** Buffer,
	const unsigned int 		Length,
	char 					* Command,
	const unsigned int	 	CmdLength
	)
{
	ERROR_CODE				Status = ERR_SUCCESS;
	//unsigned int			LocPos = 0;
	char 					delimiter[] = " ";
	char 					*ptr = 0;
	unsigned int			Tag = 0;
	char 					* Value;
	unsigned int 			ValueLen = 0;

	if(0 == *Buffer)
	{
		return ERR_INSUFFICIENT_RESOURCES;
	}
	if(0 == Command)
	{
		return ERR_INSUFFICIENT_RESOURCES;
	}
	if(1 > CmdLength)
	{
		return ERR_INSUFFICIENT_RESOURCES;
	}

	ptr = strtok(Command, delimiter);

	while (0 != ptr)
	{
		// move one step in VALUE direction
		if(0 == strcmp("move", ptr))
		{
			Tag = TLV_Move;
			ptr = strok(NULL, delimiter);
			if(0 == strlen(ptr))
			{
				TRACE("no value given\n");
				Status = ERR_WRONG_INPUT;
			}
			else if(0 == strcmp("left", Value))
			{
				Value = 1; CmdLength = sizeof(int);
			}
			else if(0 == strcmp("right", Value))
			{
				Value = 2; CmdLength = sizeof(int);
			}
			else
			{
				TRACE("no valid value given: move [left, right]\n");
				Status = ERR_WRONG_INPUT;
			}
		}
		// set the driving speed
		else if (0 == strcmp("speed", ptr))
		{
			Tag = TLV_Speed;
			ptr = strok(NULL, delimiter);
			if(0 == strlen(ptr))
			{
				TRACE("no value given\n");
				Status = ERR_WRONG_INPUT;
			}
			else if(1 < atoi(Value) || 200 > atoi(Value))
			{
				Value = atoi(Value); CmdLength = sizeof(int);
			}
			else
			{
				TRACE("no valid value given: speed [1, ..., 200] \n");
				Status = ERR_WRONG_INPUT;
			}
		}
		// drive to one end of the rail
		else if (0 == strcmp("home", ptr))
		{
			Tag = TLV_Home;
			ptr = strok(NULL, delimiter);
			if(0 == strlen(ptr))
			{
				TRACE("no value given\n");
				Status = ERR_WRONG_INPUT;
			}
			else if(0 != strcmp("up", Value))
			{
				Value = 1; CmdLength = sizeof(int);
			}
			else if(0 != strcmp("down", Value))
			{
				Value = 2; CmdLength = sizeof(int);
			}
			else
			{
				TRACE("no valid value given: home [up, down] \n");
				Status = ERR_WRONG_INPUT;
			}
		}
		// do one capture. FUTURE: if value is given, do multiple captures
		else if (0 == strcmp("capt", ptr))
		{
			Tag = TLV_Capture;
			Value = 1;
			CmdLength = sizeof(int);
		}
		else if (0 == strcmp("start", ptr))
		{
			Tag = TLV_Start;
			ptr = strok(NULL, delimiter);
			if(0 == strlen(ptr))
			{
				TRACE("no value given\n");
				Status = ERR_WRONG_INPUT;
			}
			else if(0 != strcmp("up", Value))
			{
				Value = 1; CmdLength = sizeof(int);
			}
			else if(0 != strcmp("down", Value))
			{
				Value = 2; CmdLength = sizeof(int);
			}
			else
			{
				TRACE("no valid value given: start [up, down] \n");
				Status = ERR_WRONG_INPUT;
			}
		}

		// drive VALUE steps to the left. Safe direction for capture even if zero
		else if (0 == strcmp("left", ptr))
		{
			Tag = TLV_Drive_left;
			ptr = strok(NULL, delimiter);
			if(0 == strlen(ptr))
			{
				TRACE("no value given\n");
				Status = ERR_WRONG_INPUT;
			}
			else if(1 < atoi(Value) || 200 > atoi(Value))
			{
				Value = atoi(Value); CmdLength = sizeof(int);
			}
			else
			{
				TRACE("no valid value given: left [1, ..., 200] \n");
				Status = ERR_WRONG_INPUT;
			}
		}
		// drive VALUE steps to the right. Safe direction for capture even if zero
		else if (0 == strcmp("right", ptr))
		{
			Tag = TLV_Drive_right;
			ptr = strok(NULL, delimiter);
			if(0 == strlen(ptr))
			{
				TRACE("no value given\n");
				Status = ERR_WRONG_INPUT;
			}
			else if(1 < atoi(Value) || 200 > atoi(Value))
			{
				Value = atoi(Value); CmdLength = sizeof(int);
			}
			else
			{
				TRACE("no valid value given: right [1, ..., 200] \n");
				Status = ERR_WRONG_INPUT;
			}

		}
	}

	if(ERR_SUCCESS == Status)
	{
		//LocPos = EnqueueTupel(&Slider->RequestsList, Buffer, LocPos, MAX_PACKET_LENGTH, htons(Slider->CurrId), TLV_Move, (unsigned char *)"left", (uint8_t)sizeof("left"));
	}


	return Status;
}

/* -------------------------------------------------------------------------- */

int
main(
    int 					argc,
    char 					*argv[]
    )
{
    int                     n;
    char         			* Buffer;
    unsigned int            CurrPos = 0;
    size_t					CommandLength = MAX_VALUE_LENGTH;
    char                    * CommandBuffer = 0;
    static SLIDER_APP 		Slider;
    ERROR_CODE				Status = ERR_SUCCESS;
    

    __try
	{
		TargetInit(&Slider.ServerAdress);

		InitializeListHead(&Slider.RequestsList);
		SliderConnect(&Slider);

		CommandBuffer = (char *)malloc(sizeof(char) * CommandLength);
		Buffer = (char *)malloc(sizeof(char) * MAX_PACKET_LENGTH);
		TRACE("Slider initialized. Starting input\n");


		while(1)
		{

			if(0 != ReadCommandFromStdin(CommandBuffer, &CommandLength))
			{
				bzero(Buffer,MAX_PACKET_LENGTH);
				bzero(CommandBuffer,CommandLength);
				continue;
			}
			if(0 != ComputeCommand(&Slider, &Buffer, MAX_PACKET_LENGTH, CommandBuffer, CommandLength))
			{
				bzero(Buffer,MAX_PACKET_LENGTH);
				bzero(CommandBuffer,CommandLength);
				continue;
			}

			Slider.CurrId++;

		//	CurrPos = BuildMessage(&Slider, Buffer, 0, MAX_PACKET_LENGTH);
			HexDump("Sending UDP message", Buffer, CurrPos);

			if(0 > (n = write(Slider.SockFd, &Buffer, CurrPos)))
			{
				 error("ERROR writing to socket");
			}
			bzero(Buffer,MAX_PACKET_LENGTH);
			n = read(Slider.SockFd, Buffer, 255);
			if (n < 0)
				 error("ERROR reading from socket");
			printf("%s\n", Buffer);
		}
	}

    __finally
	{
    	free(CommandBuffer);
    	SliderDisconnect(&Slider);
	}
    return Status;
}
