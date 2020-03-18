/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *  Prototypes of functions for enterprise security commands
 */

#ifdef __cplusplus
extern "C" {
#endif
void ent_utility_init();
int join_ent( int argc, char *argv[], tlv_buffer_t** data );
int leave_ent( int argc, char *argv[], tlv_buffer_t** data );
int mallinfo( int argc, char *argv[], tlv_buffer_t** data );
#ifdef __cplusplus
}
#endif
