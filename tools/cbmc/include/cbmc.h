// CBMC pointer uses BITS bits for object id, remaining bits for offset
// ProcessDHCPReplies needs 7 bits, most need at most 4
#define CBMC_BITS 7
#define CBMC_MAX_OBJECT_SIZE (0xFFFFFFFF >> CBMC_BITS)

enum CBMC_LOOP_CONDITION { CBMC_LOOP_BREAK, CBMC_LOOP_CONTINUE, CBMC_LOOP_RETURN };

// CBMC specification: capture old value for precondition/postcondition checking

#define OLDVAL(var) _old_ ## var
#define SAVE_OLDVAL(var,typ) const typ OLDVAL(var) = var

// CBMC specification: capture old value for values passed by reference in function abstractions

#define OBJ(var) (*var)
#define OLDOBJ(var) _oldobj_ ## var
#define SAVE_OLDOBJ(var,typ) const typ OLDOBJ(var) = OBJ(var)

// CBMC debugging: printfs for expressions

#define __CPROVER_printf(var) { uint32_t ValueOf_ ## var = (uint32_t) var; }
#define __CPROVER_printf2(str,exp) { uint32_t ValueOf_ ## str = (uint32_t) (exp); }

// CBMC debugging: printfs for pointer expressions

#define __CPROVER_printf_ptr(var) { uint8_t *ValueOf_ ## var = (uint8_t *) var; }
#define __CPROVER_printf2_ptr(str,exp) { uint8_t *ValueOf_ ## str = (uint8_t *) (exp); }

/* CBMC assert to test pvPortMalloc result when xWantedSize is 0. Mostly used to report
 * full coverage on pvPortMalloc, but use with caution as it might complicate debugging
 */
#define __CPROVER_assert_zero_allocation() __CPROVER_assert( pvPortMalloc(0) == NULL, "pvPortMalloc allows zero-allocated memory.")

/* xWantedSize is not bounded in this function, but there might be a need to bound it in the future.
 * In theory, CBMC malloc allows to allocate an arbitrary amount of data. This will not be true for
 * embedded devices.
 */
void *pvPortMalloc( size_t xWantedSize )
{
	if ( xWantedSize == 0 )
	{
		return NULL;
	}
	return nondet_bool() ? malloc( xWantedSize ) : NULL;
}

void vPortFree( void *pv )
{
	free(pv);
}
