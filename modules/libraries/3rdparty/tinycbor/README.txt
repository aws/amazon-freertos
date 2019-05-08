# Deviation from tinycbor master

If it is needed to upgrade tinycbor, the deviation code we already made needs extreme caution.

master: https://github.com/intel/tinycbor

## compilersupport_p.h

The below list may be NOT exhaustive.

1. Disable optimization for TI

```
#if !defined(__TI_COMPILER_VERSION__) || __TI_COMPILER_VERSION__ < 18000000
```

2. Comment out function mappint to standard ntohs or htons

```
//#  include <netinet/in.h>
//#  define cbor_ntohs        ntohs
//#  define cbor_htons        htons
```

3. Implement default cbor_ntohll with cbor_ntohl, instead of undefined ntohl

```
define ntohll(x)       ((cbor_ntohl((uint32_t)(x)) * UINT64_C(0x100000000)) + (cbor_ntohl((x) >> 32)))
```