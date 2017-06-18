# Changes since 2.1.13

# 2017-06-18

Next step is to tear apart the packets as the send_client_hello()
is not working with the server.

Enabled the SOCKET_READ driver.   We can now read and write.

Solicited help on the Particle #Library forum.

# 2017-06-17

We are able to build and begin testing [44244]

i/o wiring complete, now to do the actual send/recv parts. [52788]

BYTE ORDER CODE:

https://developer.mbed.org/users/vshymanskyy/code/Blynk/docs/b942afadf9be/BlynkProtocolDefs_8h_source.html

https://fossies.org/linux/misc/mosh-1.3.0.tar.gz/mosh-1.3.0/src/crypto/byteorder.h?m=t

Next: wire up the i/o functions [52596]

do_clnt_handshake() [52516]

* process_server_hello()
  * ssl_session_update() [50340] 
* send_client_key_xchg()
  * DISPLAY_RSA() -> RSA_print() -> putc in bigint.ccp (TODO FIX) [51252]

Next area: do_clnt_handshake() and do_srv_handshake().   Firmware size: 49780

TODO: Add ability to disable SERVER code.

Next area: do_handshake().   Firmware size: 49716

Slowly getting the connection established with axtls.

I/O symbols Traced to basic_read().  Firmware size: 46580.

Now to define the ssl_read and ssl_write functions so it
does not use standard I/O.

Avoid C language references to:

* printf
* time
* gettimeofday

We need proper c++ libraries for byte shifting.

* htonl
* be64toh

# 2017-06-16

Trying a different open source project, axTLS

REF:

* https://sourceforge.net/projects/axtls/files/
* http://axtls.sourceforge.net/

# 2017-06-15

## got a pared down version

openssl tools

* openssl s_client -showcerts -cipher DHE-RSA-AES256-SHA -connect
* openssl ciphers

Test hosts

* things.ubidots.com:443
* jupyter.lccllc.info:4443
* www.google.com:443

Mapping openssl to mbed ciphersuites

* https://tls.mbed.org/supported-ssl-ciphersuites

Testing (worked on the Mac openssl)


Failed

* AES128-SHA -> TLS-RSA-WITH-AES-128-CBC-SHA -> MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA
* AES256-SHA -> TLS-RSA-WITH-AES-256-CBC-SHA -> MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA
* ECDHE-RSA-AES256-GCM-SHA384 -> TLS-ECDHE-RSA-WITH-AES-256-GCM-SHA384 -> MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384

Works with jupyter

* AES256-SHA256 -> TLS-RSA-WITH-AES-256-CBC-SHA256 -> MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA256

## initial build but too big

Ok.  Got an intial build.  The final size is too large by 87392

* Original - 87392
* Disable MBEDTLS_SSL_PROTO_TLS1
* Disable MBEDTLS_SSL_PROTO_TLS1_1
* Disable MBEDTLS_SSL_CBC_RECORD_SPLITTING
* Test - 86496
* Disable all but MBEDTLS_ECP_DP_SECP256R1_ENABLED
* Test - 78024
* Disable MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
* Disable MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED
* Disable MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED
* Disable MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
* Disable MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED
* Disable MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED
* Test - 70248
* Disable MBEDTLS_FS_IO
* Disable MBEDTLS_SELF_TEST
* Disable MBEDTLS_SSL_PROTO_DTLS
* Disable MBEDTLS_SSL_DTLS_ANTI_REPLAY
* Disable MBEDTLS_SSL_DTLS_HELLO_VERIFY
* Disable MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE
* Disable MBEDTLS_SSL_DTLS_BADMAC_LIMIT
* Test - 59240
* Disable MBEDTLS_VERSION_FEATURES
* Disable MBEDTLS_ARC4_C
* Test - 58136
* Disable MBEDTLS_ASN1_WRITE_C
* Disable MBEDTLS_BLOWFISH_C
* Disable MBEDTLS_CAMELLIA_C
* Disable MBEDTLS_CCM_C
* Disable MBEDTLS_CERTS_C
* Disable MBEDTLS_DHM_C
* Disable MBEDTLS_ECDH_C
* Disable MBEDTLS_ECDSA_C
* Disable MBEDTLS_X509_CREATE_C
* Disable MBEDTLS_X509_CRT_WRITE_C
* Disable MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED
* Disable MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
* Disable MBEDTLS_X509_CSR_WRITE_C
* Disable MBEDTLS_XTEA_C
* Test - 30248
* Disable MBEDTLS_SHA512_C
* Disable MBEDTLS_SSL_COOKIE_C
* Disable MBEDTLS_SSL_TICKET_C
* Disable MBEDTLS_SSL_SRV_C
* Disable MBEDTLS_CIPHER_MODE_CFB
* Test - 11376
* Disable MBEDTLS_REMOVE_ARC4_CIPHERSUITES
* Disable MBEDTLS_AESNI_C
* Disable MBEDTLS_DES_C
* Test - 5024
* Disable MBEDTLS_PEM_WRITE_C
* Disable MBEDTLS_PK_WRITE_C
* Disable MBEDTLS_PADLOCK_C
* Test - 4096
* Disable MBEDTLS_PK_PARSE_EC_EXTENDED
* Test - 4288
* Disable MBEDTLS_PK_RSA_ALT_SUPPORT
* Disable MBEDTLS_SSL_FALLBACK_SCSV
* Disable MBEDTLS_PK_PARSE_EC_EXTENDED
* Disable MBEDTLS_PK_RSA_ALT_SUPPORT

x509_crt.cpp: There is directory access that is deliberately commented out which might
  be contributing to Ubidots failure.

We are going to start with the development tree from the mbedtls
Github site and see if this fixes a few bugs and allows us to
connect with the https://things.ubidots.com site.

We are developing a bash script that will do a mass conversion of
source code so we can eliminate a lot of typing.

* Conversion going fairly well.  Strict compiler errors require
some standard casting to be added to some source code.

* There are a lot of missing casts for (void *)

# 2017-06-14

## config.h

Playing with config.h broke its communication with other sites!  Restored config.h
and I can talk to other sites now.

## No longer talks to my original site

* jupyter fails with -28928 = -0x7100 : MBEDTLS_ERR_SSL_BAD_INPUT_DATA
* -26880 = -0x6900 : MBEDTLS_ERR_SSL_WANT_READ

## Getting closer

* We know it dies in the handShake() and the connection is not live

```
0000008243 [app] INFO: begin connect()
0000008305 [app] INFO: begin handShake()
0000008700 [app] INFO: Verify requested for (Depth 1):
....
0000009636 [app] INFO: Client connected = 0
0000009636 [app] INFO: Connect res = -16
0000009636 [app] INFO: ERROR FFFFFFF0
0000009639 [app] INFO: Device loading failed.
```

* What is error -16?

```
0000003949 [app] INFO:   This certificate has no flags
0000004883 [app] INFO: Connect res = -16
0000004883 [app] INFO: ERROR FFFFFFF0
0000004883 [app] INFO: Device loading failed.
0000004884 [comm.sparkprotocol] INFO: Sending A describe message
0000004884 [comm.sparkprotocol] WARN: received ERROR CoAPMessage
0000004884 [system] WARN: Communication loop error, closing cloud socket
0000004885 [system] INFO: Cloud: connecting
```
## Take 2

Firmware size getting large!
```
downloading binary from: /v1/binaries/59418143b34e65370745f363
saving to: tmp.bin
Memory use: 
 text    data     bss     dec     hex filename
 106356   156   13148  119660   1d36c /workspace/target/workspace.elf
```

This is what we need:

* RSA with Elliptic Curve Ephemeral Diffie Hellman (ECDHE-RSA) key exchange
* mbed TLS Name / NIST Name: TLS-ECDHE-RSA-WITH-AES-256-GCM-SHA384
* OpenSSL equivalent: ECDHE-RSA-AES256-GCM-SHA384
* Value: {0xC0,0x30}

## Current problem

The TlsTcpClient program dies with this error in the SSL validation.   Why does this fail but
curl continue to work?

* 0000009880 [app] INFO: Connect res = -30592
* http://www.rapidtables.com/convert/number/decimal-to-hex.htm
* -30592 -> -0x7780
* MBEDTLS_ERR_SSL_FATAL_ALERT_MESSAGE via ssl.h
* Possible solution?
* https://tls.mbed.org/discussions/platform-specific/shakehand-failed-and-return-mbedtls_err_ssl_fatal_alert_message
* We have to enable more cypher suites!
* config.h
* #define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
* Requires: MBEDTLS_ECDH_C, MBEDTLS_RSA_C, MBEDTLS_PKCS1_V15, MBEDTLS_X509_CRT_PARSE_C

## Starting out

* The TlsTcpClient works for other SSL websites, it seems the Ubidots server is
using a self-signed certificate that breaks the client.
* There is a lot of code to support SSL and bloats the final firmware binary.  If you
are consistently using a particular server, then we can tune the software to only use the
parts of encryption we really need.   The downside to this is when the certificate or
encryption change, then you have to re-send your firmware to the devices that need it.
* Now to the breakage!
* https://things.ubidots.com:443 uses the following encryption
* TLS 1.2 connection using TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384
* It offers the following certificates
* Server certificate: things.ubidots.com
* Server certificate: Lets Encrypt Authority X3
* Server certificate: DST Root CA X3
* You can discover all this using curl -v https://things.ubidots.com/
* We should write a program that tells the user which parts of the code
* There is something we can do later that may allow us to dynamically pull in the
encryption chunks as we need them. [dynamic libraries](https://community.particle.io/t/dynamic-linking/30593)


# 2017-06-13

## Working towards 3.0.0

* Fix more spelling mistakes
* Define a structure based on the Value structure.
* Start building the getDevices() routine.
* Version bump 3.0.0 as we switch things from an apparently depricated API
to the currently documented http/https REST API.

### TODO

* See if we can add to a dummy variable the old way
* See if we can add to a dummy variable the new way
* See if we can retrieve data in a date range
* Compare to data we have and send un-sent data

## General clean up and experimentation

* Not much documentation on the translate.ubidots.com API, so
we will begin to phase that out.   
* Allow switching between http and https modes and allow selecting of
a different port number.
* Fix some typos
* Spark.process() -> Particle.process();
* Eliminate all compile warnings

These warnings can not be fixed with simple updates.   These will be
rewritten and we will just bump the version to 3.X.X.

```
Ubidots.cpp: In member function 'bool Ubidots::sendAll()':
Ubidots.cpp:573:24: warning: passing NULL to non-pointer argument 1 of 'bool Ubidots::sendAll(long unsigned int)' [-Wconversion-null]
     return sendAll(NULL);
                        ^
Ubidots.cpp: In member function 'bool Ubidots::sendAll(long unsigned int)':
Ubidots.cpp:579:30: warning: NULL used in arithmetic [-Wpointer-arith]
     if ( timestamp_global != NULL) {
                              ^
Ubidots.cpp:594:41: warning: NULL used in arithmetic [-Wpointer-arith]
         if ((val + i)->timestamp_val != NULL) {
```
