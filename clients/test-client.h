/* test-client.h */

#ifndef _TEST_CLIENT
#define _TEST_CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 

#include <time.h>
#include <stdlib.h>


/* ---------------------------------------------------------------------------*/

/*
	Compiler Specific Definitions
*/ 

#ifdef GCC
#define DATA_PACKED					__attribute__ ((packed))
#else
#define DATA_PACKED
#endif

#define DEFAULT_PORT        2390
#define MAX_PACKET_LENGTH   255
#define MAX_VALUE_LENGTH    20

#define SLIDER_START_TAG	0x7C

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define __try
#define __finally					goto try_exit; try_exit:
#define __leave						goto try_exit;

#define FALSE						0
#define TRUE						1

#define CHECK(__e)					if(0 != (Status = __e)) { \
	TraceCheck(__FILE__, __LINE__, Status); __leave; }
	
#define TRACE(n)					printf(n);
	
#define for_list(List, Entry)		for(Entry = (List)->Flink; Entry != (List); Entry = Entry->Flink)
#define lst_cnt(List, Entry, Count) {Count = 0; for_list(List, Entry){Count ++}}

typedef	unsigned char		BYTE;
typedef BYTE				BOOLEAN;

typedef struct _LIST_ENTRY
{
	struct _LIST_ENTRY		*Flink;
	struct _LIST_ENTRY		*Blink;
} DATA_PACKED
 LIST_ENTRY, *PLIST_ENTRY;

typedef enum _Tag_Types
{
    TLV_Move				= 1,
    TLV_Duration,
} DATA_PACKED
 TupelType;

typedef enum _ErrorCodes
{
	ERR_SUCCESS				= 0,
	ERR_INSUFFICIENT_RESOURCES,
    ERR_DEFAULT,
} DATA_PACKED
ErrorCode;

typedef struct _slider_tlv_data
{
	unsigned short 			Id;
	uint8_t               	Type;
	uint8_t                 Length;
	uint8_t					Value[MAX_VALUE_LENGTH];
	
} DATA_PACKED
SLIDER_TLV_DATA, * PSLIDER_TLV_DATA;

typedef struct _SLIDER_LIST_ENTRY {
	PLIST_ENTRY				Entry;		/* Link				*/
	unsigned int 			Id;
	SLIDER_TLV_DATA			Data;
	
} DATA_PACKED
SLIDER_LIST_ENTRY, *PSLIDER_LIST_ENTRY;

typedef struct _SLIDER_APP
{
	LIST_ENTRY				RequestsList;
	BOOLEAN					Connected;
	unsigned int 			LastPosition;
	struct sockaddr_in      ServerAdress;
	unsigned short			CurrId;

} DATA_PACKED
SLIDER_APP, * PSLIDER_APP;

/* ---------------------------------------------------------------------------*/
	
void
HexDump (
	char 					* Desc,
	void 					* Addr,
	int 					Len
	);
	
/* ---------------------------------------------------------------------------*/

void
TraceCheck(
	const char * File,
	int LineNumber,
	int Status
	);

/* ---------------------------------------------------------------------------*/
	
void
error(
	const char *msg
	);
	
/* ------------------------------- list --------------------------------------*/

void
InitializeListHead(
	PLIST_ENTRY 		ListHead
	);

void
InsertTailList(
	PLIST_ENTRY 		ListHead,
	PLIST_ENTRY 		Entry
	);
	
void
InsertHeadList(
	PLIST_ENTRY 		ListHead,
	PLIST_ENTRY 		Entry
	);
	
PLIST_ENTRY 
RemoveHeadList(
	PLIST_ENTRY 		ListHead
	);
	
int
CountListItems(
	PLIST_ENTRY 		ListHead
	);
	
#endif
