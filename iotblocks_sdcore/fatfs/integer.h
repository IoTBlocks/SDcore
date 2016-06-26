/*-------------------------------------------*/
/* Integer type definitions for FatFs module */
/*-------------------------------------------*/
/*
 * File	: integer.h
 * This file has been modified as part of the IoTBLocks project
 * Find out more:
 * http://www.pratikpanda.com/iotblocks/SDcore
 */

#ifndef _FF_INTEGER
#define _FF_INTEGER

#ifdef _WIN32	/* FatFs development platform */

#include <windows.h>
#include <tchar.h>

#else			/* Embedded platform */

/* This type MUST be 8 bit */
typedef unsigned char	BYTE;

/* These types MUST be 16 bit */
typedef short			SHORT;
typedef unsigned short	WORD;
typedef unsigned short	WCHAR;

/* These types MUST be 16 bit or 32 bit */
typedef int				INT;
typedef unsigned int	UINT;

/* These types MUST be 32 bit */
typedef int			LONG;
typedef unsigned int	DWORD;

#endif

#endif
