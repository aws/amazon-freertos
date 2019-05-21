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
