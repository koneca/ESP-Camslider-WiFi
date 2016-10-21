
/* dbg.c */


/* -------------------------------------------------------------------------- */
/*!	\ingroup test-client
	\file
	\brief generic dbg functions and definitions
	*/ 
/* -------------------------------------------------------------------------- */

#include "test-client.h"

/* -------------------------------------------------------------------------- */

/*!	\ingroup test-client
 * \brief dunp one Buffer to stdout
 * @Param [IN]Desc Text to be displazed before the dumped context
 * @Param [IN]Addr The Buffer, to be dumped
 * @Param {IN}Len  Length of the Buffer which has to be parsed
 */
void
HexDump (
	char 					* Desc,
	void 					* Addr,
	int 					Len
	)
{
    int 					i;
    unsigned char 			Buff[17];
    unsigned char 			*pc = (unsigned char*)Addr;

    // Output description if given.
    if (Desc != NULL)
    {
        printf ("%s:\n", Desc);
    }

    if (Len == 0)
    {
        printf("  ZERO LENGTH\n");
        return;
    }
    if (Len < 0)
    {
        printf("  NEGATIVE LENGTH: %i\n",Len);
        return;
    }

    // Process every byte in the data.
    for (i = 0; i < Len; i++)
    {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0)
        {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf ("  %s\n", Buff);

            // Output the offset.
            printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
        {
            Buff[i % 16] = '.';
        }
        else
        {
            Buff[i % 16] = pc[i];
        }
        
        Buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0)
    {
        printf ("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf ("  %s\n", Buff);
}


/* ---------------------------------------------------------------------------*/

void
TraceCheck(
	const char * File,
	int LineNumber,
	int Status
	)
{
	printf("%s(%d): %d\n", File, LineNumber, Status);
}


/* ---------------------------------------------------------------------------*/
	
void
error(
	const char *msg
	)
{
    perror(msg);
    exit(0);
}


