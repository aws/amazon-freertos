# Setting up cryptoauthlib as a PKCS11 Provider for your system (LINUX)

These instructions are for building, installing and configuring cryptoauthlib as a pkcs11
provider. These instructions are for commonly available Linux systems with package managers.


## Update libp11 on the system. The version should be at minimum 0.4.10

* Install the build dependendencies for the system:

    ```bash
    # Debian like systems
    $ sudo apt-get build-dep libengine-pkcs11-openssl1.1
    ```
    
    ```bash
    # RPM based systems
    $ yum-builddep engine-pkcs11
    ```

* Change to a sane directory

    ```bash
    cd ~
    ```

* Get the latest version of libp11

    ```bash
    $ git clone https://github.com/OpenSC/libp11.git
    ```

* Rerun the build configuration tools:

    ```
    $ cd libp11
    $ ./bootstrap
    $ ./configure
    ```

* Build the library:

    ```bash
    $ make
    ```

* Install the library:

    ```bash
    $ sudo make install
    ```

## Build and Install cryptoauthlib with PKCS11 support

* Install the build dependendencies for the system:

    ```bash
    # Debian like systems
    $ sudo apt-get install cmake libudev-dev
    ```

    ```bash
    # RPM based systems
    $ yum install cmake
    $ yum install libudev-devel
    ```

* Change to a sane directory

    ```bash
    cd ~
    ```

* Get the latest version of cryptoauthlib with PKCS11 support 

    ```bash
    $ git clone --single-branch -b pkcs11 https://github.com/MicrochipTech/cryptoauthlib
    ```

* Rerun the build configuration tools:

    ```bash
    $ cd cryptoauthlib
    $ cmake .
    ```

* Build the library:

    ```bash
    $ make
    ```

* Install the library:

    ```bash
    $ sudo make install
    ```

## Configuring the cryptoauthlib PKCS11 library

By default the following files will be created.

* /etc/cryptoauthlib/cryptoauthlib.conf

    ```text
    # Cryptoauthlib Configuration File
    filestore = /var/lib/cryptoauthlib
    ```

* /var/lib/cryptoauthlib/slot.conf.tmpl

    ```text
    # Reserved Configuration for a device
    # The objects in this file will be created and marked as undeletable
    # These are processed in order. Configuration parameters must be comma
    # delimited and may not contain spaces

    interface = i2c,0xB0
    freeslots = 1,2,3

    # Slot 0 is the primary private key
    object = private,device,0

    # Slot 10 is the certificate data for the device's public key
    #object = certificate,device,10

    # Slot 12 is the intermedate/signer certificate data
    #object = certificate,signer,12

    # Slot 15 is a public key
    object = public,root,15
    ```

### cryptoauthlib.conf
This file provides the basic configuation information for the library. The only variable is
"filestore" which is where cryptoauthlib will find device specific configuration and where
it will store object files from pkcs11 operations.

### slot.conf.tmpl
This is a template for device configuration files that cryptoauthlib will use to map devices
and their resources into pkcs11 tokens and objects.

A device file must be named <pkcs11_slot_number>.conf

For a single device:

   ```bash
   $ cd /var/lib/cryptoauthlib
   $ cp slot.conf.tmpl 0.conf
   ```

Then edit 0.conf to match the device configuration being used.


#### interface
Allows values: 'hid', 'i2c'
If using i2c specify the address in hex for the device. This is in the device format (upper
7 bits define the address) so will not appear the same as the i2cdetect address (lower 7 bits)

#### freeslots
This is a list of slots that may be used by the library when a pkcs11 operation that creates
new objects is used. When the library is initialized it will scan for files of the form
<pkcs11_slot_num>.<device_slot_num>.conf which defines the object using that device resource.


## Using p11-kit-proxy

This is an optional step but is very helpful for using multiple pkcs11 libraries in a system.
Detailed setup can be found at [p11-glue](https://p11-glue.github.io/p11-glue/p11-kit/manual/)

```bash
# Debian like systems
$ sudo apt-get install p11-kit
```

```bash
# RPM based systems
$ yum install p11-kit
```

* Create or edit the global configuration file ```/etc/pkcs11/pkcs11.conf```.  The directory ```/etc/pkcs11``` may require creation first.

    ```
    # This setting controls whether to load user configuration from the
    # ~/.config/pkcs11 directory. Possible values:
    #    none: No user configuration
    #    merge: Merge the user config over the system configuration (default)
    #    only: Only user configuration, ignore system configuration
    user-config: merge
    ```

* Create a module configuration file.

  - User module name (only available for a single user): ```~/.config/pkcs11/modules/cryptoauthlib.module```
  - Global module name (available to the whole system): ```/usr/share/p11-kit/modules/cryptoauthlib.module```

    ```
    module: /usr/lib/libcryptoauth.so
    critical: yes
    trust-policy: yes
    managed: yes
    log-calls: no
    ```

For more details on the configuration files see the [configuration documentation](https://p11-glue.github.io/p11-glue/p11-kit/manual/pkcs11-conf.html).

## Without using p11-kit-proxy

OpenSSL (via the libp11 project above) and p11tool support p11-kit-proxy natively so do not require
additional set up if it is being used. If p11-kit-proxy is not being used then OpenSSL will have to
be manually configured to use libp11 and cryptoauthlib

This requires editing the default openssl.cnf file. To locate the file being used by the system run
the following command:

```bash
$ openssl version -a | grep OPENSSLDIR:

OPENSSLDIR: "/usr/lib/ssl"
```

This gives the default path where openssl is compiled to find the openssl.cnf file

In this case the file to edit will be /usr/lib/ssl/openssl.cnf

This line must be placed at the top, before any sections are defined:

```text
openssl_conf = openssl_init
```

This should be added to the bottom of the file:

```text
[openssl_init]
engines=engine_section

[engine_section]
pkcs11 = pkcs11_section

[pkcs11_section]
engine_id = pkcs11
# Wherever the engine installed by libp11 is. For example it could be:
# /usr/lib/arm-linux-gnueabihf/engines-1.1/libpkcs11.so
dynamic_path = /usr/lib/ssl/engines/libpkcs11.so 
MODULE_PATH = /usr/lib/libcryptoauth.so
init = 0
```

## Testing

To use p11tool it has to be installed:

```bash
# Debian like systems
$ sudo apt-get install gnutls-bin
```

```bash
# RPM based systems
$ yum install gnutls-utils
```

__Note__: If not using p11-kit-proxy then the provider has to be specified in p11tool calls:

```bash
$ p11tool --provider=/usr/lib/libcryptoauth.so
```

* Get the public key for a private key (as defined by the 0.conf file cited above):

    ```bash
    $ p11tool --export-pubkey "pkcs11:token=0123EE;object=device;type=private"
    warning: --login was not specified and it may be required for this operation.
    warning: no --outfile was specified and the public key will be printed on screen.
    -----BEGIN PUBLIC KEY-----
    MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAE9wzUq1EUAoNrG01rXYjNd35mxKuA
    Ojw/klIrNEBciSLLOTLjs/gvFS7N8AFXDK18vpxxu6ykzF2LRd7RY8yEFw==
    -----END PUBLIC KEY-----
    ```

* Get the public key and decode it using OpenSSL

    ```bash
    $ p11tool --export-pubkey "pkcs11:token=0123EE;object=device;type=private" | openssl pkey -pubin -text -noout
    warning: --login was not specified and it may be required for this operation.
    warning: no --outfile was specified and the public key will be printed on screen.
    Public-Key: (256 bit)
    pub:
        04:f7:0c:d4:ab:51:14:02:83:6b:1b:4d:6b:5d:88:
        cd:77:7e:66:c4:ab:80:3a:3c:3f:92:52:2b:34:40:
        5c:89:22:cb:39:32:e3:b3:f8:2f:15:2e:cd:f0:01:
        57:0c:ad:7c:be:9c:71:bb:ac:a4:cc:5d:8b:45:de:
        d1:63:cc:84:17
    ASN1 OID: prime256v1
    NIST CURVE: P-256
    ```

* Create a CSR for the private key

    ```bash
    $ openssl req -engine pkcs11 -key "pkcs11:token=0123EE;object=device;type=private" -keyform engine -new -out new_device.csr -subj "/CN=NEW CSR EXAMPLE"
    engine "pkcs11" set.

    $ cat new_device.csr
    -----BEGIN CERTIFICATE REQUEST-----
    MIHVMHwCAQAwGjEYMBYGA1UEAwwPTkVXIENTUiBFWEFNUExFMFkwEwYHKoZIzj0C
    AQYIKoZIzj0DAQcDQgAE9wzUq1EUAoNrG01rXYjNd35mxKuAOjw/klIrNEBciSLL
    OTLjs/gvFS7N8AFXDK18vpxxu6ykzF2LRd7RY8yEF6AAMAoGCCqGSM49BAMCA0kA
    MEYCIQDUPeLfPcOwtZxYJDYXPdl2UhpReVn6kK2lKCCX6byM8QIhAIfqfnggtcCi
    W21xLAzabr8A4mHyfIIQ1ofYBg8QO9jZ
    -----END CERTIFICATE REQUEST-----
    ```

* Verify the newly created csr

    ```bash
    $ openssl req -in new_device.csr -verify -text -noout
    verify OK
    Certificate Request:
        Data:
            Version: 1 (0x0)
            Subject: CN = NEW CSR EXAMPLE
            Subject Public Key Info:
                Public Key Algorithm: id-ecPublicKey
                    Public-Key: (256 bit)
                    pub:
                        04:f7:0c:d4:ab:51:14:02:83:6b:1b:4d:6b:5d:88:
                        cd:77:7e:66:c4:ab:80:3a:3c:3f:92:52:2b:34:40:
                        5c:89:22:cb:39:32:e3:b3:f8:2f:15:2e:cd:f0:01:
                        57:0c:ad:7c:be:9c:71:bb:ac:a4:cc:5d:8b:45:de:
                        d1:63:cc:84:17
                    ASN1 OID: prime256v1
                    NIST CURVE: P-256
            Attributes:
                a0:00
        Signature Algorithm: ecdsa-with-SHA256
             30:46:02:21:00:d4:3d:e2:df:3d:c3:b0:b5:9c:58:24:36:17:
             3d:d9:76:52:1a:51:79:59:fa:90:ad:a5:28:20:97:e9:bc:8c:
             f1:02:21:00:87:ea:7e:78:20:b5:c0:a2:5b:6d:71:2c:0c:da:
             6e:bf:00:e2:61:f2:7c:82:10:d6:87:d8:06:0f:10:3b:d8:d9
    ```


