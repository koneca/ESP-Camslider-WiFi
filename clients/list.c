
/* dbg.c */


/* -------------------------------------------------------------------------- */
/*!	\ingroup test-client
	\file
	\brief generic dbg functions and definitions
	*/ 
/* -------------------------------------------------------------------------- */

#include "test-client.h"

void
InitializeListHead(
	PLIST_ENTRY 		ListHead
	)
{
    ListHead->Flink = ListHead->Blink = ListHead;
}

void
InsertTailList(
	PLIST_ENTRY 		ListHead,
	PLIST_ENTRY 		Entry
	)
{
    PLIST_ENTRY Blink;

    Blink = ListHead->Blink;
    Entry->Flink = ListHead;
    Entry->Blink = Blink;
    Blink->Flink = Entry;
    ListHead->Blink = Entry;
}

void
InsertHeadList(
	PLIST_ENTRY 		ListHead,
	PLIST_ENTRY 		Entry
	)
{
    PLIST_ENTRY Flink;

    Flink = ListHead->Flink;
    Entry->Flink = Flink;
    Entry->Blink = ListHead;
    Flink->Blink = Entry;
    ListHead->Flink = Entry;
}

int RemoveEntryList(PLIST_ENTRY Entry)
{
    PLIST_ENTRY Blink;
    PLIST_ENTRY Flink;

    Flink = Entry->Flink;
    Blink = Entry->Blink;
    Blink->Flink = Flink;
    Flink->Blink = Blink;
    return (int)(Flink == Blink);
}

PLIST_ENTRY 
RemoveHeadList(
	PLIST_ENTRY 		ListHead
	)
{
    PLIST_ENTRY Flink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Flink;
    Flink = Entry->Flink;
    ListHead->Flink = Flink;
    Flink->Blink = ListHead;
    return Entry;
}

PLIST_ENTRY 
RemoveTailList(PLIST_ENTRY ListHead)
{
    PLIST_ENTRY Blink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Blink;
    Blink = Entry->Blink;
    ListHead->Blink = Blink;
    Blink->Flink = ListHead;
    return Entry;
}

int
CountListItems(
	PLIST_ENTRY 		ListHead
	)
{
	PLIST_ENTRY pe = ListHead->Flink;
	int i = 0;
	while(pe != ListHead) 
	{
		i++;
		pe = pe->Flink;
	}
	return i;
}

