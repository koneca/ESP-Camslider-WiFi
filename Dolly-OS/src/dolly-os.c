/*
 ============================================================================
 Name        : Dolly-OS.c
 Author      : Konrad Grzeca
 Version     :
 Copyright   : MIT
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "dolly-os.h"

#define BUFSIZE 		1024
#define PORT			8081
#define	TRUE			1
#define	FALSE			0

#define TRACE(x ...)		printf(x)

typedef unsigned int 		BOOLEAN;

/*
 * error - wrapper for perror
 */
void error(char *msg) {
	perror(msg);
	exit(1);
}

char *
Dolly_MsgTypeName(DOLLY_TYPE Type)
{
	switch(Type)
	{
	case Dolly_Type_Fwd:
		return "Dolly_Type_Fwd";
	case Dolly_Type_Rwd:
		return "Dolly_Type_Rwd";
	case DollyType_Stop:
		return "DollyType_Stop";
	case Dolly_Type_Run:
		return "Dolly_Type_Run";
	case Dolly_Type_Shoot:
		return "Dolly_Type_Shoot";
	default:
		TRACE("Messagetype %d not found\n", Type);
		return (char *)Type + 48;
		break;
	}
}

DOLLY_STATUS
Dolly_EvaluateInput(char * data, uint8_t length)
{
	DOLLY_STATUS		Status = DOLLY_SUCCESS;
	Dolly_Message			msg;

	__try
	{
		if(0 == length)
		{
			Status = DOLLY_E_INVALIT_DATA;
			__leave;
		}

		bzero(&msg, sizeof(Dolly_Message));
		msg.type = data[0];
		msg.length = data[1];
		msg.value = &data[2];
		TRACE("Type:\t%s\nLenght:\t%d\nValue:\t%s\n", Dolly_MsgTypeName(msg.type), msg.length, msg.value);

		switch(msg.type)
		{
		case Dolly_Type_Fwd:
			break;
		case Dolly_Type_Rwd:
			break;
		case DollyType_Stop:
			break;
		case Dolly_Type_Run:
			break;
		case Dolly_Type_Shoot:
			break;
		default:
			TRACE("Messagetype %d not found\n", msg.type);
			Status = DOLLY_E_MSG_UNKNOWN;
			__leave;
			break;
		}

		//HexDump("buf: ", &buf, n);
	}
	__finally
	{
		if(0 != Status)
		{
			TRACE("Evaluate Input: error %d\n", Status);
		}

	}
	return Status;
}

DOLLY_STATUS
Dolly_Connect(socklen_t * ParentFd)
{
	DOLLY_STATUS		Status = DOLLY_SUCCESS;
	int 				optval; /* flag value for setsockopt */
	struct sockaddr_in 	serveraddr; /* server's addr */

	__try
	{
		if(0 > (*ParentFd = socket(AF_INET, SOCK_STREAM, 0)))
		{
			error("could not build socket\n");
			Status = DOLLY_E_SOCKET_ERROR;
			__leave;
		}
		/* setsockopt: Handy debugging trick that lets
		 * us rerun the server immediately after we kill it;
		 * otherwise we have to wait about 20 secs.
		 * Eliminates "ERROR on binding: Address already in use" error.
		 */
		optval = 1;
		setsockopt(*ParentFd, SOL_SOCKET, SO_REUSEADDR,
				(const void *)&optval , sizeof(int));

		bzero((char *) &serveraddr, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		serveraddr.sin_port = htons((unsigned short)PORT);

		if (0 > (Status = bind(*ParentFd, (struct sockaddr *) &serveraddr,
				sizeof(serveraddr))))
		{
			error("ERROR on binding");
			__leave;
		}

		if (0 > (Status = listen(*ParentFd, 2))) /* allow 2 requests to queue up */
		{
			error("ERROR on listen");
			__leave;
		}
	}
	__finally
	{
		if(0 != Status)
		{
			TRACE("parrr %d\n", Status);
		}
	}
	return Status;
}

DOLLY_STATUS
Dolly_WaitForClient(const socklen_t * ParentFd, socklen_t * ChildFd)
{
	DOLLY_STATUS			Status = DOLLY_SUCCESS;
	socklen_t				clientlen; /* byte size of client's address */
	struct sockaddr_in 		clientaddr; /* client addr */
	char 					* hostaddrp; /* dotted decimal host addr string */

	__try
	{
		clientlen = sizeof(clientaddr);
		/*
		 * accept: wait for a connection request
		 */
		if(0 > (*ChildFd = accept(*ParentFd, (struct sockaddr *) &clientaddr, &clientlen)))
		{
			error("ERROR on accept");
			Status = DOLLY_E_SOCKET_ERROR;
			__leave;
		}

		hostaddrp = inet_ntoa(clientaddr.sin_addr);
		TRACE("server established connection with %s\n", hostaddrp);
	}
	__finally
	{
		if(Status)
		{
			TRACE("Could not accept connection to %s\n", hostaddrp);
			ChildFd = 0;
		}
	}

	return Status;
}

DOLLY_STATUS
Dolly_MsgValueWrite(
	char					* InData,
	uint8_t					InLength,
	char					* OutData,
	uint8_t					* OutLen
	)
{
	DOLLY_STATUS			Status = DOLLY_SUCCESS;
	uint8_t					Len = 0;
	__try
	{
		if(0 == (Len = strlen(InData)))
		{
			Status = DOLLY_E_INVALIT_DATA;
			__leave;
		}

		TRACE("Answering to Client\n");

		OutData = malloc(sizeof(char)*Len);
		if(0 == InLength)
		{
			*OutLen = InLength;
		}
		else
		{
			*OutLen = Len;
		}
		memcpy(OutData, InData, *OutLen);

	}
	__finally
	{
		if(0 != Status)
		{
			TRACE("errooor: %d\n", Status);
			free(OutData);
	}

	return Status;
}


DOLLY_STATUS
Dolly_Reply(
		socklen_t			ChildFd,
		uint8_t				StatusCode
		)
{
	DOLLY_STATUS			Status = DOLLY_SUCCESS;
	Dolly_Message			* Msg = 0;

	__try
	{
		Msg = malloc(sizeof(Dolly_Message));
		Msg->type = Dolly_Type_Answer;


		switch(StatusCode)
		{
		case DOLLY_SUCCESS:
			Status = Dolly_MsgValueWrite("Success\0", 0 ,Msg->value, &Msg->length);
			break;
		case DOLLY_DISCONNECT:
			Status = Dolly_MsgValueWrite("Disconnected\0", 0 ,Msg->value, &Msg->length);
			break;
		default:
			Status = Dolly_MsgValueWrite("Error\0", 0 ,Msg->value, &Msg->length);
			break;
		}

		HexDump("ac", &Msg, sizeof(Dolly_Message)+Msg->length);
		Status = write(ChildFd, (char *)Msg, sizeof(Dolly_Message) + Msg->length);
	}
	__finally
	{
		if(0 != Msg->value)
			free(Msg->value);
		free(Msg);

	}
	return Status;
}

int
main(
		int argc,
		char **args
)
{

	socklen_t				ChildFd; /* child socket */

	socklen_t 				ParentFd;
	DOLLY_STATUS			Status = 0;

	char 					buf[BUFSIZE]; /* message buffer */
	uint8_t					Bytes; /* message byte size */

	__try
	{
		TRACE("Server: Starting server on Port: %d\n", PORT);

		if(0 != (Status = Dolly_Connect(&ParentFd)))
		{
			__leave;
		}

		/*
		 * main loop: wait for a connection request, echo input line,
		 * then close connection.
		 */

		while (1) {

			if(0 != (Status = Dolly_WaitForClient(&ParentFd, &ChildFd)))
			{
				__leave;
			}


			while(!Status)
			{
				/*
				 * read: read input string from the client
				 */
				bzero(buf, BUFSIZE);
				if(0 > (Bytes = read(ChildFd, buf, BUFSIZE)))
				{
					error("ERROR reading from socket");
					continue;
				}
				else if(0 == Bytes)
				{
					Status = DOLLY_E_RECEIVE_ERROR;
					continue;
				}
				TRACE("server received %d bytes: %s\n", Bytes, buf);

				Status = Dolly_EvaluateInput(buf, Bytes);

				Status = Dolly_Reply(ChildFd, Status);

			}

			close(ChildFd);

		}
	}
	__finally
	{
	}
}
