#ifndef _SSL_SERVER_H_
#define _SSL_SERVER_H_

#include "wm_config.h"
#include "HTTPClientWrapper.h"

#if TLS_CONFIG_SERVER_SIDE_SSL

#if TLS_CONFIG_USE_POLARSSL
#include "polarssl/config.h"
#include "polarssl/ssl.h"

#error "PolaSSL does not support ssl server now!"
#else
#include "matrixsslApi.h"
typedef ssl_t   tls_ssl_t;
typedef sslKeys_t   tls_ssl_key_t;
#endif
//key type for tls_ssl_server_init
#define KEY_RSA	1
#define	KEY_ECC	2
#define KEY_DH	3

int tls_ssl_server_init(void * arg);

int tls_ssl_server_load_keys(tls_ssl_key_t **keys, unsigned char *certBuf,
			int32 certLen, unsigned char *privBuf, int32 privLen,
			unsigned char *CAbuf, int32 CAlen, int keyType);
int tls_ssl_server_handshake(tls_ssl_t **ssl_p, int fd, tls_ssl_key_t *keys);
int tls_ssl_server_send(tls_ssl_t *ssl, int s,char *sndbuf, int len,int flags);

int tls_ssl_server_recv(tls_ssl_t *ssl,int s,char *buf, int len,int flags);
void tls_ssl_server_close_conn(tls_ssl_t *ssl, int s);
int tls_ssl_server_close(tls_ssl_key_t * keys);


#endif /*TLS_CONFIG_SERVER_SIDE_SSL*/
#endif /*_SSL_SERVER_H_*/

