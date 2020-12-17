// Trace definitions for script application. 
//
#ifdef __cplusplus
extern "C" {
#endif
    extern void BTU_trace_debug(const char* p_fmt, ...);
    extern void BTU_trace_error(const char* p_fmt, ...);
#ifdef __cplusplus
}
#endif

/**
 *  @ingroup     gentypes
 *
 *  @{
 */
/** Debug trace macro */
#define WICED_BT_TRACE          BTU_trace_debug
/** Debug trace array macro */
#define WICED_BT_TRACE_ARRAY(ptr,len,string)     WICED_BT_TRACE("%s %A",string,ptr,len);
/** Error trace array macro */
#define WICED_BT_TRACE_CRIT     BTU_trace_error

