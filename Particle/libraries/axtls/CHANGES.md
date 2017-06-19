# Changes since 2.1.3 

REF: 

* http://axtls.sourceforge.net/

# 2017-06-18

99% there... I think we are now decrypting https sites.  We have to plug this into
the Ubidots REST API to see if we are doing things correctly.

```
Memory use: 
  text    data     bss     dec     hex filename
 54964     188    2108   57260    dfac /workspace/target/workspace.elf

0000021824 [axtls] INFO: Trying to connect
0000021825 [axtls] INFO: begin connect()
0000021825 [axtls] INFO: host:things.ubidots.com port:443
0000021831 [axtls] INFO: init() ssl_ctx(0x20007780) send_Tls(0x80a4049) recv_Tls(0x80a3f71)
0000021831 [axtls] INFO: Assigning i/o pathways ssl_ctx(0x20007780) f_send(0x80a4049) f_recv(0x80a3f71)
0000021832 [axtls] INFO: connect() try 1
0000022077 [axtls] INFO: ssl_client_new()
0000022078 [axtls] INFO: ssl->version:0x33
0000022078 [axtls] INFO: do_client_connect()
0000022078 [axtls] INFO: send_client_hello() ssl(0x20008c38)
0000022079 [axtls] INFO: send_packet()
0000022079 [axtls] INFO: send_raw_packet() length=67 msg_length=67
0000022079 [axtls] INFO: sending 72 bytes
0000022080 [axtls] INFO: 16 03 03 00 43 01 00 00 : 3f 03 03 00 00 00 16 24 
0000022080 [axtls] INFO: 79 98 ad b3 c3 b0 c3 75 : cd c7 60 49 71 05 e1 c7 
0000022081 [axtls] INFO: 0a 0a 08 48 05 00 20 d8 : 77 00 20 00 00 08 00 3c 
0000022081 [axtls] INFO: 00 3d 00 35 00 2f 01 00 : 00 0e 00 0d 00 0a 00 08 
0000022082 [axtls] INFO: 06 01 05 01 04 01 02 01 : 
0000022082 [axtls] INFO: SOCKET_WRITE ssl(0x20008c38) t:72 s:0
0000022082 [axtls] INFO: sendParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_send(0x80a4049)
0000022083 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022084 [axtls] INFO: sock->connected():1 sock->available():0
0000022085 [axtls] INFO: Wanted to send 72 bytes, sent 72 bytes
0000022085 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022085 [axtls] INFO: Want 5 byte(s)
0000022087 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022088 [axtls] INFO: sock->connected():1 sock->available():0
0000022256 [axtls] INFO: Got 5 byte(s)
0000022256 [axtls] INFO: received 5 bytes
0000022256 [axtls] INFO: 16 03 03 00 4a 
0000022256 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022257 [axtls] INFO: Want 74 byte(s)
0000022258 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022258 [axtls] INFO: sock->connected():1 sock->available():123
0000022258 [axtls] INFO: Got 74 byte(s)
0000022259 [axtls] INFO: received 74 bytes
0000022259 [axtls] INFO: 02 00 00 46 03 03 4d 61 : 51 8a 10 03 a8 2e e1 ca 
0000022259 [axtls] INFO: 1f 6e d3 5b af cb 77 08 : 9c 10 1e ff c9 53 92 10 
0000022260 [axtls] INFO: 0f fa c1 19 8d 26 20 94 : 3d 8a 07 85 c8 69 e3 59 
0000022261 [axtls] INFO: 3c fc 6d 93 35 91 7d 92 : 74 f2 3f cb 0f 5e b1 e0 
0000022261 [axtls] INFO: b5 d0 c4 2c 70 4f 7a 00 : 3d 00 
0000022261 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022262 [axtls] INFO: Want 5 byte(s)
0000022263 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022263 [axtls] INFO: sock->connected():1 sock->available():49
0000022263 [axtls] INFO: Got 5 byte(s)
0000022264 [axtls] INFO: received 5 bytes
0000022264 [axtls] INFO: 16 03 03 09 d4 
0000022264 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022264 [axtls] INFO: Want 2516 byte(s)
0000022266 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022266 [axtls] INFO: sock->connected():1 sock->available():44
0000022266 [axtls] INFO: Got 44 byte(s)
0000022267 [axtls] INFO: received 44 bytes
0000022267 [axtls] INFO: 0b 00 09 d0 00 09 cd 00 : 05 31 30 82 05 2d 30 82 
0000022267 [axtls] INFO: 04 15 a0 03 02 01 02 02 : 12 03 bb 43 2d d0 ea 49 
0000022268 [axtls] INFO: f4 78 b1 0e b9 55 bd 29 : 82 c1 48 30 
0000022268 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022269 [axtls] INFO: Want 2472 byte(s)
0000022270 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022270 [axtls] INFO: sock->connected():1 sock->available():128
0000022270 [axtls] INFO: Got 128 byte(s)
0000022271 [axtls] INFO: received 128 bytes
0000022271 [axtls] INFO: 0d 06 09 2a 86 48 86 f7 : 0d 01 01 0b 05 00 30 4a 
0000022271 [axtls] INFO: 31 0b 30 09 06 03 55 04 : 06 13 02 55 53 31 16 30 
0000022272 [axtls] INFO: 14 06 03 55 04 0a 13 0d : 4c 65 74 27 73 20 45 6e 
0000022273 [axtls] INFO: 63 72 79 70 74 31 23 30 : 21 06 03 55 04 03 13 1a 
0000022273 [axtls] INFO: 4c 65 74 27 73 20 45 6e : 63 72 79 70 74 20 41 75 
0000022274 [axtls] INFO: 74 68 6f 72 69 74 79 20 : 58 33 30 1e 17 0d 31 37 
0000022274 [axtls] INFO: 30 35 32 39 30 34 32 38 : 30 30 5a 17 0d 31 37 30 
0000022275 [axtls] INFO: 38 32 37 30 34 32 38 30 : 30 5a 30 1d 31 1b 30 19 
0000022275 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022275 [axtls] INFO: Want 2344 byte(s)
0000022277 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022277 [axtls] INFO: sock->connected():1 sock->available():128
0000022277 [axtls] INFO: Got 128 byte(s)
0000022278 [axtls] INFO: received 128 bytes
0000022278 [axtls] INFO: 06 03 55 04 03 13 12 74 : 68 69 6e 67 73 2e 75 62 
0000022278 [axtls] INFO: 69 64 6f 74 73 2e 63 6f : 6d 30 82 01 22 30 0d 06 
0000022279 [axtls] INFO: 09 2a 86 48 86 f7 0d 01 : 01 01 05 00 03 82 01 0f 
0000022280 [axtls] INFO: 00 30 82 01 0a 02 82 01 : 01 00 b4 73 c2 f6 38 e0 
0000022280 [axtls] INFO: 26 ac 31 5d 1b 28 87 27 : 78 47 06 f7 6a b9 3e 51 
0000022281 [axtls] INFO: 8a 52 9e 49 ea 44 ac 4e : ff 3f ca dd c1 26 8a 3e 
0000022281 [axtls] INFO: b8 cf a6 10 c4 e2 ae 20 : 39 f6 8c e6 40 54 1c 78 
0000022282 [axtls] INFO: b7 da 2a df 81 be cb 78 : c3 14 4a cf 07 10 b7 80 
0000022282 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022282 [axtls] INFO: Want 2216 byte(s)
0000022284 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022284 [axtls] INFO: sock->connected():1 sock->available():128
0000022284 [axtls] INFO: Got 128 byte(s)
0000022285 [axtls] INFO: received 128 bytes
0000022285 [axtls] INFO: d5 e5 94 e5 a5 dd 6f ed : d5 64 ac ba 1c 75 76 27 
0000022285 [axtls] INFO: 1b fc 40 ef 79 64 52 7d : 47 76 15 b2 1c c2 1f 38 
0000022286 [axtls] INFO: 63 01 19 da 43 47 54 6e : 86 18 e7 13 cd 41 1d 73 
0000022287 [axtls] INFO: 7f dc 50 92 fc c3 4a df : d3 cf 7f 55 61 40 ad 2c 
0000022287 [axtls] INFO: ed 8f 34 e8 c9 30 f1 fd : 6a 2d db 43 51 64 9e fb 
0000022288 [axtls] INFO: 55 61 6b 97 bf ce fe c3 : 79 dd c6 87 52 48 22 4b 
0000022288 [axtls] INFO: c8 a1 8a bf 86 02 11 05 : 91 fc 67 26 65 23 49 2d 
0000022289 [axtls] INFO: ab 8b b5 73 df bc 1b 73 : 30 79 59 5f 66 05 5a 3b 
0000022289 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022289 [axtls] INFO: Want 2088 byte(s)
0000022291 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022291 [axtls] INFO: sock->connected():1 sock->available():128
0000022291 [axtls] INFO: Got 128 byte(s)
0000022292 [axtls] INFO: received 128 bytes
0000022292 [axtls] INFO: 0e 7e 4f fe 07 e5 40 6a : de 5b 0f 1a 8e c1 32 74 
0000022292 [axtls] INFO: 7d 8d b9 6c f3 af 1b d0 : 98 8a f4 0e 5d e9 53 07 
0000022293 [axtls] INFO: 5d c4 72 48 39 f6 50 f7 : 19 a9 eb e9 6b e0 b0 51 
0000022294 [axtls] INFO: bc 1b 06 e7 b4 4f 2b db : 90 43 02 03 01 00 01 a3 
0000022294 [axtls] INFO: 82 02 38 30 82 02 34 30 : 0e 06 03 55 1d 0f 01 01 
0000022295 [axtls] INFO: ff 04 04 03 02 05 a0 30 : 1d 06 03 55 1d 25 04 16 
0000022295 [axtls] INFO: 30 14 06 08 2b 06 01 05 : 05 07 03 01 06 08 2b 06 
0000022296 [axtls] INFO: 01 05 05 07 03 02 30 0c : 06 03 55 1d 13 01 01 ff 
0000022296 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022296 [axtls] INFO: Want 1960 byte(s)
0000022298 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022298 [axtls] INFO: sock->connected():1 sock->available():128
0000022298 [axtls] INFO: Got 128 byte(s)
0000022299 [axtls] INFO: received 128 bytes
0000022299 [axtls] INFO: 04 02 30 00 30 1d 06 03 : 55 1d 0e 04 16 04 14 4d 
0000022299 [axtls] INFO: 05 ce e9 0a db dd 6b 0e : 05 dc 99 e5 79 e6 85 7a 
0000022300 [axtls] INFO: 03 23 7f 30 1f 06 03 55 : 1d 23 04 18 30 16 80 14 
0000022301 [axtls] INFO: a8 4a 6a 63 04 7d dd ba : e6 d1 39 b7 a6 45 65 ef 
0000022301 [axtls] INFO: f3 a8 ec a1 30 6f 06 08 : 2b 06 01 05 05 07 01 01 
0000022302 [axtls] INFO: 04 63 30 61 30 2e 06 08 : 2b 06 01 05 05 07 30 01 
0000022302 [axtls] INFO: 86 22 68 74 74 70 3a 2f : 2f 6f 63 73 70 2e 69 6e 
0000022303 [axtls] INFO: 74 2d 78 33 2e 6c 65 74 : 73 65 6e 63 72 79 70 74 
0000022303 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022303 [axtls] INFO: Want 1832 byte(s)
0000022305 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022305 [axtls] INFO: sock->connected():1 sock->available():128
0000022305 [axtls] INFO: Got 128 byte(s)
0000022306 [axtls] INFO: received 128 bytes
0000022306 [axtls] INFO: 2e 6f 72 67 30 2f 06 08 : 2b 06 01 05 05 07 30 02 
0000022306 [axtls] INFO: 86 23 68 74 74 70 3a 2f : 2f 63 65 72 74 2e 69 6e 
0000022307 [axtls] INFO: 74 2d 78 33 2e 6c 65 74 : 73 65 6e 63 72 79 70 74 
0000022308 [axtls] INFO: 2e 6f 72 67 2f 30 43 06 : 03 55 1d 11 04 3c 30 3a 
0000022308 [axtls] INFO: 82 0f 61 70 70 2e 75 62 : 69 64 6f 74 73 2e 63 6f 
0000022309 [axtls] INFO: 6d 82 12 74 68 69 6e 67 : 73 2e 75 62 69 64 6f 74 
0000022309 [axtls] INFO: 73 2e 63 6f 6d 82 13 74 : 68 69 6e 67 73 36 2e 75 
0000022310 [axtls] INFO: 62 69 64 6f 74 73 2e 63 : 6f 6d 30 81 fe 06 03 55 
0000022310 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022310 [axtls] INFO: Want 1704 byte(s)
0000022312 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022312 [axtls] INFO: sock->connected():1 sock->available():128
0000022312 [axtls] INFO: Got 128 byte(s)
0000022313 [axtls] INFO: received 128 bytes
0000022313 [axtls] INFO: 1d 20 04 81 f6 30 81 f3 : 30 08 06 06 67 81 0c 01 
0000022314 [axtls] INFO: 02 01 30 81 e6 06 0b 2b : 06 01 04 01 82 df 13 01 
0000022314 [axtls] INFO: 01 01 30 81 d6 30 26 06 : 08 2b 06 01 05 05 07 02 
0000022315 [axtls] INFO: 01 16 1a 68 74 74 70 3a : 2f 2f 63 70 73 2e 6c 65 
0000022315 [axtls] INFO: 74 73 65 6e 63 72 79 70 : 74 2e 6f 72 67 30 81 ab 
0000022316 [axtls] INFO: 06 08 2b 06 01 05 05 07 : 02 02 30 81 9e 0c 81 9b 
0000022316 [axtls] INFO: 54 68 69 73 20 43 65 72 : 74 69 66 69 63 61 74 65 
0000022317 [axtls] INFO: 20 6d 61 79 20 6f 6e 6c : 79 20 62 65 20 72 65 6c 
0000022317 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022317 [axtls] INFO: Want 1576 byte(s)
0000022319 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022319 [axtls] INFO: sock->connected():1 sock->available():128
0000022319 [axtls] INFO: Got 128 byte(s)
0000022320 [axtls] INFO: received 128 bytes
0000022320 [axtls] INFO: 69 65 64 20 75 70 6f 6e : 20 62 79 20 52 65 6c 79 
0000022320 [axtls] INFO: 69 6e 67 20 50 61 72 74 : 69 65 73 20 61 6e 64 20 
0000022321 [axtls] INFO: 6f 6e 6c 79 20 69 6e 20 : 61 63 63 6f 72 64 61 6e 
0000022322 [axtls] INFO: 63 65 20 77 69 74 68 20 : 74 68 65 20 43 65 72 74 
0000022322 [axtls] INFO: 69 66 69 63 61 74 65 20 : 50 6f 6c 69 63 79 20 66 
0000022323 [axtls] INFO: 6f 75 6e 64 20 61 74 20 : 68 74 74 70 73 3a 2f 2f 
0000022323 [axtls] INFO: 6c 65 74 73 65 6e 63 72 : 79 70 74 2e 6f 72 67 2f 
0000022324 [axtls] INFO: 72 65 70 6f 73 69 74 6f : 72 79 2f 30 0d 06 09 2a 
0000022324 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022324 [axtls] INFO: Want 1448 byte(s)
0000022326 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022326 [axtls] INFO: sock->connected():1 sock->available():128
0000022326 [axtls] INFO: Got 128 byte(s)
0000022327 [axtls] INFO: received 128 bytes
0000022327 [axtls] INFO: 86 48 86 f7 0d 01 01 0b : 05 00 03 82 01 01 00 69 
0000022327 [axtls] INFO: 2f 8a 4f af cb 4b f8 e9 : 0c 41 d7 85 d2 13 23 ea 
0000022328 [axtls] INFO: d2 85 61 6a ed e4 8d aa : f1 aa bc 03 21 88 c3 73 
0000022329 [axtls] INFO: 2a 03 3b 34 fb 52 5f 2a : 06 ec 91 46 b3 cd d2 23 
0000022329 [axtls] INFO: 60 25 91 83 a2 e3 af ea : 86 bb 2f f5 0d fa a2 e0 
0000022330 [axtls] INFO: 9c 45 99 92 2a 0d 50 d3 : f0 e8 f9 42 12 91 0b ca 
0000022330 [axtls] INFO: 4c 25 71 ed 21 c6 1c a6 : af 66 20 18 4d df 27 2c 
0000022331 [axtls] INFO: 1f 38 93 eb 68 50 d4 ba : 42 1a 5f 04 69 f5 97 45 
0000022331 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022331 [axtls] INFO: Want 1320 byte(s)
0000022333 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022333 [axtls] INFO: sock->connected():1 sock->available():128
0000022333 [axtls] INFO: Got 128 byte(s)
0000022334 [axtls] INFO: received 128 bytes
0000022334 [axtls] INFO: f8 4f a4 a5 58 f8 a5 58 : 20 f4 98 3d 96 f5 95 54 
0000022334 [axtls] INFO: e6 63 44 15 cb d5 4c e4 : 31 60 df 7a 27 4b da 6d 
0000022335 [axtls] INFO: 25 3d 0a 01 53 ef 6f 22 : 3e a3 b1 85 e4 2f 3b cd 
0000022336 [axtls] INFO: 47 37 ad f7 9e 64 55 ec : e1 d5 58 9c 92 c6 bc 37 
0000022336 [axtls] INFO: 4b 3e 0a a3 6c f4 0a 18 : 27 e7 be b7 c6 1c 2c d9 
0000022337 [axtls] INFO: 9e 0e 80 dc f8 39 10 4e : 12 fb ad 2f 63 4c 30 87 
0000022337 [axtls] INFO: 6e 2e 8d c2 51 0e 0b 5c : af 3e 86 3a 5f a9 ae c7 
0000022338 [axtls] INFO: bd 25 20 6a 0c e6 f7 af : 30 53 e2 31 da e9 c5 e6 
0000022338 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022338 [axtls] INFO: Want 1192 byte(s)
0000022340 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022340 [axtls] INFO: sock->connected():1 sock->available():128
0000022340 [axtls] INFO: Got 128 byte(s)
0000022341 [axtls] INFO: received 128 bytes
0000022341 [axtls] INFO: 7d 00 0f 20 7f e2 1f 5a : 5a 6f c5 4a b2 7f 20 00 
0000022341 [axtls] INFO: 04 96 30 82 04 92 30 82 : 03 7a a0 03 02 01 02 02 
0000022342 [axtls] INFO: 10 0a 01 41 42 00 00 01 : 53 85 73 6a 0b 85 ec a7 
0000022343 [axtls] INFO: 08 30 0d 06 09 2a 86 48 : 86 f7 0d 01 01 0b 05 00 
0000022343 [axtls] INFO: 30 3f 31 24 30 22 06 03 : 55 04 0a 13 1b 44 69 67 
0000022344 [axtls] INFO: 69 74 61 6c 20 53 69 67 : 6e 61 74 75 72 65 20 54 
0000022344 [axtls] INFO: 72 75 73 74 20 43 6f 2e : 31 17 30 15 06 03 55 04 
0000022345 [axtls] INFO: 03 13 0e 44 53 54 20 52 : 6f 6f 74 20 43 41 20 58 
0000022345 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022345 [axtls] INFO: Want 1064 byte(s)
0000022347 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022347 [axtls] INFO: sock->connected():1 sock->available():128
0000022347 [axtls] INFO: Got 128 byte(s)
0000022348 [axtls] INFO: received 128 bytes
0000022348 [axtls] INFO: 33 30 1e 17 0d 31 36 30 : 33 31 37 31 36 34 30 34 
0000022348 [axtls] INFO: 36 5a 17 0d 32 31 30 33 : 31 37 31 36 34 30 34 36 
0000022349 [axtls] INFO: 5a 30 4a 31 0b 30 09 06 : 03 55 04 06 13 02 55 53 
0000022350 [axtls] INFO: 31 16 30 14 06 03 55 04 : 0a 13 0d 4c 65 74 27 73 
0000022350 [axtls] INFO: 20 45 6e 63 72 79 70 74 : 31 23 30 21 06 03 55 04 
0000022351 [axtls] INFO: 03 13 1a 4c 65 74 27 73 : 20 45 6e 63 72 79 70 74 
0000022351 [axtls] INFO: 20 41 75 74 68 6f 72 69 : 74 79 20 58 33 30 82 01 
0000022352 [axtls] INFO: 22 30 0d 06 09 2a 86 48 : 86 f7 0d 01 01 01 05 00 
0000022352 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022352 [axtls] INFO: Want 936 byte(s)
0000022354 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022354 [axtls] INFO: sock->connected():1 sock->available():128
0000022354 [axtls] INFO: Got 128 byte(s)
0000022355 [axtls] INFO: received 128 bytes
0000022355 [axtls] INFO: 03 82 01 0f 00 30 82 01 : 0a 02 82 01 01 00 9c d3 
0000022355 [axtls] INFO: 0c f0 5a e5 2e 47 b7 72 : 5d 37 83 b3 68 63 30 ea 
0000022356 [axtls] INFO: d7 35 26 19 25 e1 bd be : 35 f1 70 92 2f b7 b8 4b 
0000022357 [axtls] INFO: 41 05 ab a9 9e 35 08 58 : ec b1 2a c4 68 87 0b a3 
0000022357 [axtls] INFO: e3 75 e4 e6 f3 a7 62 71 : ba 79 81 60 1f d7 91 9a 
0000022358 [axtls] INFO: 9f f3 d0 78 67 71 c8 69 : 0e 95 91 cf fe e6 99 e9 
0000022358 [axtls] INFO: 60 3c 48 cc 7e ca 4d 77 : 12 24 9d 47 1b 5a eb b9 
0000022359 [axtls] INFO: ec 1e 37 00 1c 9c ac 7b : a7 05 ea ce 4a eb bd 41 
0000022359 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022359 [axtls] INFO: Want 808 byte(s)
0000022361 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022361 [axtls] INFO: sock->connected():1 sock->available():128
0000022361 [axtls] INFO: Got 128 byte(s)
0000022362 [axtls] INFO: received 128 bytes
0000022362 [axtls] INFO: e5 36 98 b9 cb fd 6d 3c : 96 68 df 23 2a 42 90 0c 
0000022362 [axtls] INFO: 86 74 67 c8 7f a5 9a b8 : 52 61 14 13 3f 65 e9 82 
0000022363 [axtls] INFO: 87 cb db fa 0e 56 f6 86 : 89 f3 85 3f 97 86 af b0 
0000022364 [axtls] INFO: dc 1a ef 6b 0d 95 16 7d : c4 2b a0 65 b2 99 04 36 
0000022364 [axtls] INFO: 75 80 6b ac 4a f3 1b 90 : 49 78 2f a2 96 4f 2a 20 
0000022365 [axtls] INFO: 25 29 04 c6 74 c0 d0 31 : cd 8f 31 38 95 16 ba a8 
0000022365 [axtls] INFO: 33 b8 43 f1 b1 1f c3 30 : 7f a2 79 31 13 3d 2d 36 
0000022366 [axtls] INFO: f8 e3 fc f2 33 6a b9 39 : 31 c5 af c4 8d 0d 1d 64 
0000022366 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022366 [axtls] INFO: Want 680 byte(s)
0000022368 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022368 [axtls] INFO: sock->connected():1 sock->available():128
0000022368 [axtls] INFO: Got 128 byte(s)
0000022369 [axtls] INFO: received 128 bytes
0000022369 [axtls] INFO: 16 33 aa fa 84 29 b6 d4 : 0b c0 d8 7d c3 93 02 03 
0000022369 [axtls] INFO: 01 00 01 a3 82 01 7d 30 : 82 01 79 30 12 06 03 55 
0000022370 [axtls] INFO: 1d 13 01 01 ff 04 08 30 : 06 01 01 ff 02 01 00 30 
0000022371 [axtls] INFO: 0e 06 03 55 1d 0f 01 01 : ff 04 04 03 02 01 86 30 
0000022371 [axtls] INFO: 7f 06 08 2b 06 01 05 05 : 07 01 01 04 73 30 71 30 
0000022372 [axtls] INFO: 32 06 08 2b 06 01 05 05 : 07 30 01 86 26 68 74 74 
0000022372 [axtls] INFO: 70 3a 2f 2f 69 73 72 67 : 2e 74 72 75 73 74 69 64 
0000022373 [axtls] INFO: 2e 6f 63 73 70 2e 69 64 : 65 6e 74 72 75 73 74 2e 
0000022373 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022373 [axtls] INFO: Want 552 byte(s)
0000022375 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022375 [axtls] INFO: sock->connected():1 sock->available():128
0000022375 [axtls] INFO: Got 128 byte(s)
0000022376 [axtls] INFO: received 128 bytes
0000022376 [axtls] INFO: 63 6f 6d 30 3b 06 08 2b : 06 01 05 05 07 30 02 86 
0000022376 [axtls] INFO: 2f 68 74 74 70 3a 2f 2f : 61 70 70 73 2e 69 64 65 
0000022377 [axtls] INFO: 6e 74 72 75 73 74 2e 63 : 6f 6d 2f 72 6f 6f 74 73 
0000022377 [axtls] INFO: 2f 64 73 74 72 6f 6f 74 : 63 61 78 33 2e 70 37 63 
0000022378 [axtls] INFO: 30 1f 06 03 55 1d 23 04 : 18 30 16 80 14 c4 a7 b1 
0000022379 [axtls] INFO: a4 7b 2c 71 fa db e1 4b : 90 75 ff c4 15 60 85 89 
0000022379 [axtls] INFO: 10 30 54 06 03 55 1d 20 : 04 4d 30 4b 30 08 06 06 
0000022380 [axtls] INFO: 67 81 0c 01 02 01 30 3f : 06 0b 2b 06 01 04 01 82 
0000022380 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022380 [axtls] INFO: Want 424 byte(s)
0000022382 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022382 [axtls] INFO: sock->connected():1 sock->available():128
0000022382 [axtls] INFO: Got 128 byte(s)
0000022383 [axtls] INFO: received 128 bytes
0000022383 [axtls] INFO: df 13 01 01 01 30 30 30 : 2e 06 08 2b 06 01 05 05 
0000022383 [axtls] INFO: 07 02 01 16 22 68 74 74 : 70 3a 2f 2f 63 70 73 2e 
0000022384 [axtls] INFO: 72 6f 6f 74 2d 78 31 2e : 6c 65 74 73 65 6e 63 72 
0000022385 [axtls] INFO: 79 70 74 2e 6f 72 67 30 : 3c 06 03 55 1d 1f 04 35 
0000022385 [axtls] INFO: 30 33 30 31 a0 2f a0 2d : 86 2b 68 74 74 70 3a 2f 
0000022386 [axtls] INFO: 2f 63 72 6c 2e 69 64 65 : 6e 74 72 75 73 74 2e 63 
0000022386 [axtls] INFO: 6f 6d 2f 44 53 54 52 4f : 4f 54 43 41 58 33 43 52 
0000022387 [axtls] INFO: 4c 2e 63 72 6c 30 1d 06 : 03 55 1d 0e 04 16 04 14 
0000022387 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022387 [axtls] INFO: Want 296 byte(s)
0000022389 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022389 [axtls] INFO: sock->connected():1 sock->available():128
0000022389 [axtls] INFO: Got 128 byte(s)
0000022390 [axtls] INFO: received 128 bytes
0000022390 [axtls] INFO: a8 4a 6a 63 04 7d dd ba : e6 d1 39 b7 a6 45 65 ef 
0000022390 [axtls] INFO: f3 a8 ec a1 30 0d 06 09 : 2a 86 48 86 f7 0d 01 01 
0000022391 [axtls] INFO: 0b 05 00 03 82 01 01 00 : dd 33 d7 11 f3 63 58 38 
0000022392 [axtls] INFO: dd 18 15 fb 09 55 be 76 : 56 b9 70 48 a5 69 47 27 
0000022392 [axtls] INFO: 7b c2 24 08 92 f1 5a 1f : 4a 12 29 37 24 74 51 1c 
0000022393 [axtls] INFO: 62 68 b8 cd 95 70 67 e5 : f7 a4 bc 4e 28 51 cd 9b 
0000022393 [axtls] INFO: e8 ae 87 9d ea d8 ba 5a : a1 01 9a dc f0 dd 6a 1d 
0000022394 [axtls] INFO: 6a d8 3e 57 23 9e a6 1e : 04 62 9a ff d7 05 ca b7 
0000022394 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022394 [axtls] INFO: Want 168 byte(s)
0000022396 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022396 [axtls] INFO: sock->connected():1 sock->available():128
0000022396 [axtls] INFO: Got 128 byte(s)
0000022397 [axtls] INFO: received 128 bytes
0000022397 [axtls] INFO: 1f 3f c0 0a 48 bc 94 b0 : b6 65 62 e0 c1 54 e5 a3 
0000022397 [axtls] INFO: 2a ad 20 c4 e9 e6 bb dc : c8 f6 b5 c3 32 a3 98 cc 
0000022398 [axtls] INFO: 77 a8 e6 79 65 07 2b cb : 28 fe 3a 16 52 81 ce 52 
0000022399 [axtls] INFO: 0c 2e 5f 83 e8 d5 06 33 : fb 77 6c ce 40 ea 32 9e 
0000022399 [axtls] INFO: 1f 92 5c 41 c1 74 6c 5b : 5d 0a 5f 33 cc 4d 9f ac 
0000022400 [axtls] INFO: 38 f0 2f 7b 2c 62 9d d9 : a3 91 6f 25 1b 2f 90 b1 
0000022400 [axtls] INFO: 19 46 3d f6 7e 1b a6 7a : 87 b9 a3 7a 6d 18 fa 25 
0000022401 [axtls] INFO: a5 91 87 15 e0 f2 16 2f : 58 b0 06 2f 2c 68 26 c6 
0000022401 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022401 [axtls] INFO: Want 40 byte(s)
0000022403 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022403 [axtls] INFO: sock->connected():1 sock->available():49
0000022404 [axtls] INFO: Got 40 byte(s)
0000022404 [axtls] INFO: received 40 bytes
0000022404 [axtls] INFO: 4b 98 cd da 9f 0c f9 7f : 90 ed 43 4a 12 44 4e 6f 
0000022405 [axtls] INFO: 73 7a 28 ea a4 aa 6e 7b : 4c 7d 87 dd e0 c9 02 44 
0000022405 [axtls] INFO: a7 87 af c3 34 5b b4 42 : 
0000022412 [axtls] INFO: app.ubidots.com 
0000022412 [axtls] INFO: things.ubidots.com 
0000022412 [axtls] INFO: things6.ubidots.com 
0000022418 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022419 [axtls] INFO: Want 5 byte(s)
0000022420 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022421 [axtls] INFO: sock->connected():1 sock->available():9
0000022422 [axtls] INFO: Got 5 byte(s)
0000022422 [axtls] INFO: received 5 bytes
0000022422 [axtls] INFO: 16 03 03 00 04 
0000022422 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022423 [axtls] INFO: Want 4 byte(s)
0000022424 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022425 [axtls] INFO: sock->connected():1 sock->available():4
0000022426 [axtls] INFO: Got 4 byte(s)
0000022426 [axtls] INFO: received 4 bytes
0000022426 [axtls] INFO: 0e 00 00 00 
0000022482 [axtls] INFO: send_raw_packet() length=262 msg_length=262
0000022483 [axtls] INFO: sending 267 bytes
0000022483 [axtls] INFO: 16 03 03 01 06 10 00 01 : 02 01 00 af e2 ea c8 ae 
0000022484 [axtls] INFO: 8b 40 d1 bb ab 04 20 1a : 7b 4c 19 88 55 f1 cf 4c 
0000022484 [axtls] INFO: 85 a9 a9 7d 7a 50 ea d9 : 4d ed 44 4e 37 8d a5 26 
0000022485 [axtls] INFO: 97 b3 92 89 30 78 e3 a7 : 45 14 3a 9b d2 7a f8 6c 
0000022485 [axtls] INFO: 3c f6 26 b7 6d 1c 3d a4 : 5c 21 ec 9f a6 65 67 f6 
0000022486 [axtls] INFO: 8e df f6 b6 be c3 8b 08 : 50 f2 05 90 3f 7a 1e cf 
0000022486 [axtls] INFO: f5 9c ff 38 04 5e 5c 2c : c3 ac 93 3a 6d 58 53 0e 
0000022487 [axtls] INFO: d7 e2 83 f2 58 d3 72 97 : 5c d4 71 bd 76 d8 ea 34 
0000022487 [axtls] INFO: 62 0c 62 89 94 e7 2f e6 : 14 cb 6a 12 9a 92 b5 cf 
0000022488 [axtls] INFO: e0 60 14 6b b1 7f 51 ec : a1 e1 46 7c 4f 0d 62 b5 
0000022488 [axtls] INFO: bf 2b f6 cf 4a eb cd 0e : 40 c6 54 9a d6 e2 fe 4a 
0000022489 [axtls] INFO: 0f 5f 6f 3e 03 b9 2d 0a : 96 59 e4 b2 72 1f 6d 74 
0000022489 [axtls] INFO: 72 96 38 4b 7a e6 21 c4 : 00 2b 82 12 31 6b a2 a9 
0000022490 [axtls] INFO: 9f 29 85 2c 31 dc c3 f4 : 8f 2d fc 8f e6 6f 84 5e 
0000022490 [axtls] INFO: 3d 86 ef e0 5d 72 34 2f : 68 f0 50 94 0a 36 61 22 
0000022491 [axtls] INFO: 27 82 4b e0 6e 30 e7 f2 : 3f 9f 6a 2f 31 8e fe ec 
0000022491 [axtls] INFO: 88 24 92 ae 96 82 98 e8 : 18 91 b9 
0000022491 [axtls] INFO: SOCKET_WRITE ssl(0x20008c38) t:267 s:0
0000022492 [axtls] INFO: sendParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_send(0x80a4049)
0000022492 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022493 [axtls] INFO: sock->connected():1 sock->available():0
0000022494 [axtls] INFO: Wanted to send 267 bytes, sent 267 bytes
0000022494 [axtls] INFO: send_raw_packet() length=1 msg_length=1
0000022494 [axtls] INFO: sending 6 bytes
0000022495 [axtls] INFO: 14 03 03 00 01 01 
0000022495 [axtls] INFO: SOCKET_WRITE ssl(0x20008c38) t:6 s:0
0000022495 [axtls] INFO: sendParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_send(0x80a4049)
0000022496 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022497 [axtls] INFO: sock->connected():1 sock->available():0
0000022498 [axtls] INFO: Wanted to send 6 bytes, sent 6 bytes
0000022500 [axtls] INFO: unencrypted write
0000022500 [axtls] INFO: 14 00 00 0c 4c 9f 6e 03 : 96 92 dd 9a 77 a0 44 65 
0000022501 [axtls] INFO: 59 c7 7d 7c 7a 5e d3 e9 : 4e 13 ec 02 9d fd 1d 7c 
0000022502 [axtls] INFO: 26 41 fc 64 a6 b4 ba 7c : 17 8a 1f 50 5e 24 98 e1 
0000022502 [axtls] INFO: 0f 0f 0f 0f 0f 0f 0f 0f : 0f 0f 0f 0f 0f 0f 0f 0f 
0000022503 [axtls] INFO: send_raw_packet() length=16 msg_length=80
0000022503 [axtls] INFO: sending 85 bytes
0000022503 [axtls] INFO: 16 03 03 00 50 ea 9a 0c : 75 e1 df 46 06 5a b0 41 
0000022504 [axtls] INFO: 56 23 82 ad 77 9b 9d c0 : e8 40 6f 9f cf b2 8f af 
0000022505 [axtls] INFO: 75 c8 8b 54 cb 41 ba 18 : 54 b1 1b a7 a7 28 39 bd 
0000022505 [axtls] INFO: 27 7a da 32 1f 98 80 99 : 20 45 8b 31 60 b2 94 5e 
0000022506 [axtls] INFO: 16 eb 94 6e c2 a7 9a e6 : 73 89 e4 4a b2 13 cb e8 
0000022506 [axtls] INFO: 57 8c 6e dc d1 
0000022506 [axtls] INFO: SOCKET_WRITE ssl(0x20008c38) t:85 s:0
0000022506 [axtls] INFO: sendParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_send(0x80a4049)
0000022507 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022508 [axtls] INFO: sock->connected():1 sock->available():0
0000022509 [axtls] INFO: Wanted to send 85 bytes, sent 85 bytes
0000022509 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022509 [axtls] INFO: Want 5 byte(s)
0000022511 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022512 [axtls] INFO: sock->connected():1 sock->available():0
0000022680 [axtls] INFO: Got 5 byte(s)
0000022680 [axtls] INFO: received 5 bytes
0000022680 [axtls] INFO: 14 03 03 00 01 
0000022680 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022681 [axtls] INFO: Want 1 byte(s)
0000022682 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022683 [axtls] INFO: sock->connected():1 sock->available():86
0000022684 [axtls] INFO: Got 1 byte(s)
0000022684 [axtls] INFO: received 1 bytes
0000022684 [axtls] INFO: 01 
0000022684 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022685 [axtls] INFO: Want 5 byte(s)
0000022686 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022687 [axtls] INFO: sock->connected():1 sock->available():85
0000022688 [axtls] INFO: Got 5 byte(s)
0000022688 [axtls] INFO: received 5 bytes
0000022688 [axtls] INFO: 16 03 03 00 50 
0000022688 [axtls] INFO: recvParticle ssl(0x20008c38) ssl_ctx(0x20007780) f_recv(0x80a4049)
0000022689 [axtls] INFO: Want 80 byte(s)
0000022690 [axtls] INFO: ssl(0x20008c38) ssl_ctx(0x20007780) sock(0x20000548)
0000022691 [axtls] INFO: sock->connected():1 sock->available():80
0000022692 [axtls] INFO: Got 80 byte(s)
0000022692 [axtls] INFO: received 80 bytes
0000022692 [axtls] INFO: a3 8d 1f 55 96 39 7c ac : 6e 94 87 57 cf ed 9a da 
0000022693 [axtls] INFO: b7 35 19 4d 94 b8 e0 02 : 2e d5 b7 51 69 94 be a9 
0000022693 [axtls] INFO: 85 6d 89 bd 1f 23 83 be : a4 3e 7b 7f bf de 30 4c 
0000022694 [axtls] INFO: d5 85 ae 50 d1 ed 63 ae : 3a d8 f2 82 0c 66 e3 62 
0000022694 [axtls] INFO: 0d 7b 77 74 1a 70 29 bb : 71 42 f9 a5 76 b8 b1 7e 
0000022695 [axtls] INFO: decrypted
0000022695 [axtls] INFO: 14 00 00 0c 8e 8b 25 79 : 7b de 02 f7 ad 33 30 c3 
```

Run tcpdump for capture and then try to connect.  We may not be waiting long
enough for a reply?  No, reply is coming, but we do not see it, will try
Particle.process() as this is needed to allow updating by the Wifi module.
This might be solved by SYSTEM_THREAD(ENABLED);, but we still have to wait
for the return packets.

DISPLAY_BYTES now ported so it looks the same in the Log and in
printf statements.  Using tcpdump and hexdump to dignose communication
issues.

```
tcpdump -ttttnnvvS src port 4443 or dst port 4443 -w cap
```

```
hexdump -v -e '"%07.1_ax | "' -e '8/1 "0x%02x " " |"' -e '8/1 " %3_c"' -e '"\n"' cap
```

Porting DISPLAY_BYTES code block.

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
