/*
 * dolly-os.h
 *
 *  Created on: Sep 28, 2016
 *      Author: koneca
 */

#ifndef DOLLY_OS_H_
#define DOLLY_OS_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "proto.h"

#undef __try
#undef __finally
#undef __leave
#define __try
#define __finally			goto try_exit; try_exit:
#define __leave				goto try_exit;

//#define CHECK(__e)					if(0 != __e) { \
//	TraceCheck(__FILE__, __LINE__,  #__e); */__leave; }

typedef enum _DOLLY_STATUS{
	DOLLY_SUCCESS				= 0,
	DOLLY_E_ABORT				= 1,
	DOLLY_E_CONNECT_ERROR		= 2,
	DOLLY_E_SEND_ERROR			= 3,
	DOLLY_E_RECEIVE_ERROR		= 4,
	DOLLY_E_MSG_UNKNOWN			= 5,
	DOLLY_E_DISCONNECTED		= 6,
	DOLLY_E_INVALIT_DATA		= 7,
	DOLLY_E_SOCKET_ERROR		= 8,
	DOLLY_DISCONNECT			= 9,
}DOLLY_STATUS;

/* ---------------------------------------------------------------------------*/

void
HexDump (
	char 					* Desc,
	void 					* Addr,
	int 					Len
);

#endif /* DOLLY_OS_H_ */
