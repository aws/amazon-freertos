# Overview

This is just a copy of [intel/tinycbor](https://github.com/intel/tinycbor), except some deviations listed below.

**Current version is 0.5.2. If it is needed to upgrade tinycbor, the deviation code we already made needs extreme caution.**

## Excluded "Cbor To Json"

Three files are excluded: cborjson.h, cbortojson.c, open_memstream.c. 

- They are not useful in Amazon FreeRTOS.
- They don't compile with "make" on ESP.
- Not every compiler defines "FILE" data type.

## Modified "compilersupport_p.h"

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

## Modified "cborvalidation.c"


Initialize local variable in function validate_floating_point; otherwise it is an may-not-initialized error when compiling with "make" on ESP.
```
// In function: validate_floating_point(CborValue *it, CborType type, uint32_t flags)
float valf = 0.0;
```