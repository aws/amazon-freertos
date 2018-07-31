/*
 * PEM-encoded client certificate
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----"
 * "...base64 data..."
 * "-----END CERTIFICATE-----";
 */
static const char clientcredentialCLIENT_CERTIFICATE_PEM[] = "Paste client certificate here.";

/*
 * PEM-encoded issuer certificate for AWS IoT Just In Time Registration (JITR).
 * This is required if you're using JITR, since the issuer (Certificate
 * Authority) of the client certificate is used by the server for routing the
 * device's initial request. (The device client certificate must always be
 * sent as well.) For more information about JITR, see:
 *  https://docs.aws.amazon.com/iot/latest/developerguide/jit-provisioning.html,
 *  https://aws.amazon.com/blogs/iot/just-in-time-registration-of-device-certificates-on-aws-iot/.
 *
 * If you're not using JITR, set the below pointer to NULL.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----"
 * "...base64 data..."
 * "-----END CERTIFICATE-----";
 */
static const char * clientcredentialJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM = NULL;

/*
 * PEM-encoded client private key.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----"
 * "...base64 data..."
 * "-----END RSA PRIVATE KEY-----";
 */
static const char clientcredentialCLIENT_PRIVATE_KEY_PEM[] = "Paste client private key here.";

/* The root certificate used for the encrypted echo server.
 * This certificate is self-signed, and not in the trusted catalog. */
static const char tcptestECHO_HOST_ROOT_CA[] = "Paste root certificate here";

/*
 * PEM-encoded client certificate.
 *
 * Certificate for P-256 elliptic curve key.
 */
static const char tlstestCLIENT_CERTIFICATE_PEM_EC[] = "Paste client certificate here.";

/*
 * PEM-encoded client private key.
 *
 * This is a P-256 elliptic curve key.
 */
static const char tlstestCLIENT_PRIVATE_KEY_PEM_EC[] = "Paste client private key here.";


/* One character of this certificate has been changed in the issuer
 * name from Amazon Web Services to Amazon Web Cervices. */
static const char tlstestCLIENT_CERTIFICATE_PEM_MALFORMED[] = "Paste client certificate here.";

/* Certificate which is not trusted by the broker. */
static const char tlstestCLIENT_UNTRUSTED_CERTIFICATE_PEM[] = "Paste client certificate here.";

/* Private key corresponding to the untrusted certificate. */
static const char tlstestCLIENT_UNTRUSTED_PRIVATE_KEY_PEM[] = "Paste client private key here.";

/* Device certificate created using BYOC instructions. */
static const char tlstestCLIENT_BYOC_CERTIFICATE_PEM[] = "Paste client certificate here.";

/* Device private key created using BYOC instructions. */
static const char tlstestCLIENT_BYOC_PRIVATE_KEY_PEM[] = "Paste client private key here.";
