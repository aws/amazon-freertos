#define ensure_memory_is_valid( px ) (px != NULL)

/* Implementation of safe malloc */
void *safeMalloc(size_t xWantedSize) {
	if(xWantedSize == 0) {
		return NULL;
	}
	uint8_t byte;
	return byte ? malloc(xWantedSize) : NULL;
}

/* Memory assignment for FreeRTOS_Socket_t */
FreeRTOS_Socket_t * ensure_FreeRTOS_Socket_t_is_allocated () {
	return safeMalloc(sizeof(FreeRTOS_Socket_t));
}

/* Memory assignment for FreeRTOS_Network_Buffer */
NetworkBufferDescriptor_t * ensure_FreeRTOS_NetworkBuffer_is_allocated () {
	return safeMalloc(sizeof(NetworkBufferDescriptor_t));
}

/* Memory assignment for FreeRTOS_StreamBuffer_t */
StreamBuffer_t * ensure_FreeRTOS_StreamBuffer_t_is_allocated() {
	return safeMalloc(sizeof(StreamBuffer_t));
}
