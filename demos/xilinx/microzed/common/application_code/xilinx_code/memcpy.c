/*
 * An memcpy() in C which does not use the FPU registers, as the default does.
 */

#include <string.h>
#include <stdint.h>

#define SIMPLE_MEMCPY	1
#define SIMPLE_MEMSET	1

#ifndef SIMPLE_MEMCPY
	#define SIMPLE_MEMCPY	( 0 )
#endif

#ifndef SIMPLE_MEMSET
	#define SIMPLE_MEMSET	( 0 )
#endif

#if( SIMPLE_MEMCPY != 0 )
void *memcpy( void *pvDest, const void *pvSource, size_t ulBytes )
{
unsigned char *pcDest = ( unsigned char * ) pvDest, *pcSource = ( unsigned char * ) pvSource;
size_t x;

	for( x = 0; x < ulBytes; x++ )
	{
		*pcDest = *pcSource;
		pcDest++;
		pcSource++;
	}

	return pvDest;
}
/*-----------------------------------------------------------*/
#endif /* SIMPLE_MEMCPY */

union xPointer {
	uint8_t *u8;
	uint16_t *u16;
	uint32_t *u32;
	uint32_t uint32;
};


#if( SIMPLE_MEMCPY == 0 )
void *memcpy( void *pvDest, const void *pvSource, size_t ulBytes )
{
union xPointer pxDestination;
union xPointer pxSource;
union xPointer pxLastSource;
uint32_t ulAlignBits;

	pxDestination.u8 = ( uint8_t * ) pvDest;
	pxSource.u8 = ( uint8_t * ) pvSource;
	pxLastSource.u8 = pxSource.u8 + ulBytes;

	ulAlignBits = ( pxDestination.uint32 & 0x03 ) ^ ( pxSource.uint32 & 0x03 );

	if( ( ulAlignBits & 0x01 ) == 0 )
	{
		if( ( ( pxSource.uint32 & 1 ) != 0 ) && ( pxSource.u8 < pxLastSource.u8 ) )
		{
			*( pxDestination.u8++ ) = *( pxSource.u8++) ;
		}
		/* 16-bit aligned here */
		if( ( ulAlignBits & 0x02 ) != 0 )
		{
			uint32_t extra = pxLastSource.uint32 & 0x01ul;

			pxLastSource.uint32 &= ~0x01ul;

			while( pxSource.u16 < pxLastSource.u16 )
			{
				*( pxDestination.u16++ ) = *( pxSource.u16++) ;
			}

			pxLastSource.uint32 |= extra;
		}
		else
		{
			int iCount;
			uint32_t extra;

			if( ( ( pxSource.uint32 & 2 ) != 0 ) && ( pxSource.u8 < pxLastSource.u8 - 1 ) )
			{
				*( pxDestination.u16++ ) = *( pxSource.u16++) ;
			}
			// 32-bit aligned
			extra = pxLastSource.uint32 & 0x03ul;

			pxLastSource.uint32 &= ~0x03ul;
			iCount = pxLastSource.u32 - pxSource.u32;
			while( iCount > 8 )
			{
				/* Copy 32 bytes */
				/* Normally it doesn't make sense to make this list much longer because
				the indexes will get too big, and therefore longer instructions are needed. */
				pxDestination.u32[ 0 ] = pxSource.u32[ 0 ];
				pxDestination.u32[ 1 ] = pxSource.u32[ 1 ];
				pxDestination.u32[ 2 ] = pxSource.u32[ 2 ];
				pxDestination.u32[ 3 ] = pxSource.u32[ 3 ];
				pxDestination.u32[ 4 ] = pxSource.u32[ 4 ];
				pxDestination.u32[ 5 ] = pxSource.u32[ 5 ];
				pxDestination.u32[ 6 ] = pxSource.u32[ 6 ];
				pxDestination.u32[ 7 ] = pxSource.u32[ 7 ];
				pxDestination.u32 += 8;
				pxSource.u32 += 8;
				iCount -= 8;
			}

			while( pxSource.u32 < pxLastSource.u32 )
			{
				*( pxDestination.u32++ ) = *( pxSource.u32++) ;
			}

			pxLastSource.uint32 |= extra;
		}
	}
	else
	{
		/* This it the worst alignment, e.g. 0x80000 and 0xA0001,
		only 8-bits copying is possible. */
		int iCount = pxLastSource.u8 - pxSource.u8;
		while( iCount > 8 )
		{
			/* Copy 8 bytes the hard way */
			pxDestination.u8[ 0 ] = pxSource.u8[ 0 ];
			pxDestination.u8[ 1 ] = pxSource.u8[ 1 ];
			pxDestination.u8[ 2 ] = pxSource.u8[ 2 ];
			pxDestination.u8[ 3 ] = pxSource.u8[ 3 ];
			pxDestination.u8[ 4 ] = pxSource.u8[ 4 ];
			pxDestination.u8[ 5 ] = pxSource.u8[ 5 ];
			pxDestination.u8[ 6 ] = pxSource.u8[ 6 ];
			pxDestination.u8[ 7 ] = pxSource.u8[ 7 ];
			pxDestination.u8 += 8;
			pxSource.u8 += 8;
			iCount -= 8;
		}
	}
	while( pxSource.u8 < pxLastSource.u8 )
	{
		*( pxDestination.u8++ ) = *( pxSource.u8++ );
	}
	return pvDest;
}
#endif /* SIMPLE_MEMCPY == 0 */
/*-----------------------------------------------------------*/

#if( SIMPLE_MEMSET != 0 )
void *memset( void *pvDest, int iValue, size_t ulBytes )
{
unsigned char *pcDest = ( unsigned char * ) pvDest;
size_t x;

	for( x = 0; x < ulBytes; x++ )
	{
		*pcDest = ( unsigned char ) iValue;
		pcDest++;
	}

	return pvDest;
}
#endif /* SIMPLE_MEMSET != 0 */
/*-----------------------------------------------------------*/


#if( SIMPLE_MEMSET == 0 )
void *memset(void *pvDest, int iValue, size_t ulBytes)
{
union xPointer pxDestination;
union xPointer pxLast;
uint32_t ulPattern;

	pxDestination.u8 = ( unsigned char * ) pvDest;
	pxLast.u8 = pxDestination.u8 + ulBytes;

	if( ulBytes >= 8 )
	{
	uint32_t ulAlignBits;
	uint32_t ulExtra;
	int iCount;

		ulPattern = iValue & 0xff;
		ulPattern |= ( ulPattern << 8 ) | ( ulPattern << 16 ) | ( ulPattern << 24 );
		ulAlignBits = ( pxDestination.uint32 & 0x03 );
		if( ulAlignBits != 0 )
		{
			ulAlignBits = 4 - ulAlignBits;
			while( ulAlignBits-- > 0 )
			{
				pxDestination.u8[ 0 ] = ( unsigned char )iValue;
				pxDestination.u8++;
			}
		}
		/* Strip-off the last 1 up-to 3 bytes because they can not be set in a 32-bit
		memory instruction. */
		ulExtra = pxLast.uint32 & 0x03ul;

		pxLast.uint32 &= ~0x03ul;
		iCount = ( int ) ( pxLast.u32 - pxDestination.u32 );
		while( iCount > 8 )
		{
			/* Set 8 * 4 bytes and save some increments */
			/* It won't make sense to make this list much longer than 8 assignments. */
			pxDestination.u32[ 0 ] = ulPattern;
			pxDestination.u32[ 1 ] = ulPattern;
			pxDestination.u32[ 2 ] = ulPattern;
			pxDestination.u32[ 3 ] = ulPattern;
			pxDestination.u32[ 4 ] = ulPattern;
			pxDestination.u32[ 5 ] = ulPattern;
			pxDestination.u32[ 6 ] = ulPattern;
			pxDestination.u32[ 7 ] = ulPattern;
			pxDestination.u32 += 8;
			iCount -= 8;
		}

		while( pxDestination.u32 < pxLast.u32 )
		{
			*( pxDestination.u32++ ) = ulPattern;
		}

		pxLast.uint32 |= ulExtra;
	}

	while( pxDestination.u8 < pxLast.u8 )
	{
		pxDestination.u8[ 0 ] = ( unsigned char ) iValue;
		pxDestination.u8++;
	}

	return pvDest;
}
#endif /* SIMPLE_MEMSET -= 0 */
/*-----------------------------------------------------------*/
