/****************************************************************
 *								*
 *	Pad.c							*
 *								*
 *								*
 *	Set of pad routines to present a simple interface 	*
 ****************************************************************/




/****************************************************************
Use:

call PadInit();	as part of initialisation in main

each frame (VSync loop), 
call PadRead(): puts pad status into an unsigned long variable

e.g. padStatus = PadRead();

then, test individual buttons by masking the pad status variable
with the #defined constants in pad.h

e.g. if (padStatus & PADLleft)	 // Pad L left button is pressed
		{
		// do PADLleft actions here
		}


****************************************************************/



#include <libps.h>

