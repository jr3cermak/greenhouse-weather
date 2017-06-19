#include "TlsTcpClient.h"


// Constructor
TlsTcpClient::TlsTcpClient() {
  _connected = false;
}

// Destructor
TlsTcpClient::~TlsTcpClient() {
  this->close();
}

// Close connection
int TlsTcpClient::close() {
  if (_connected) {
    ssl_free(ssl);
    ssl_ctx_free(ssl_ctx);
    _client.stop();
    _connected = false;
    debug_tls("close()");
    return 0;
  }
  debug_tls("close() failed");
  return -1;
}

// Connnect to remote server and begin negotiation
int TlsTcpClient::connect(const char* hn, uint16_t port) {
  int cert_index = 0, ca_cert_index = 0;
  int cert_size, ca_cert_size;
  char **ca_cert, **cert;
  SSL_EXTENSIONS *extensions = NULL;
  int quiet = _quiet;
  int reconnect = 0;
  int res = 0;
  int retry = 0;
  uint8_t session_id[SSL_SESSION_ID_SIZE];
  // Depricate (we don't have real file descripters here)
  int client_fd = 0;

  debug_tls("begin connect()");
  debug_tls("host:%s port:%d", hn, port);

  cert_size = ssl_get_config(SSL_MAX_CERT_CFG_OFFSET);
  ca_cert_size = ssl_get_config(SSL_MAX_CA_CERT_CFG_OFFSET);
  ca_cert = (char **)calloc(1, sizeof(char *)*ca_cert_size);
  cert = (char **)calloc(1, sizeof(char *)*cert_size);

  if ((ssl_ctx = ssl_ctx_new(_options, SSL_DEFAULT_CLNT_SESS)) == NULL) {
    debug_tls("Error: Client context is invalid");
    return 1;
  }

  // Always perform an init here to update ssl_ctx pointer on
  // i/o pathway.  TODO: fix later
  this->init();

  // Assign ssl_ctx->_client here
  ssl_ctx->_client = &_client;

  free(cert);
  free(ca_cert);

  // Ready to make the connection, but via the TcpClient layer
  while (!_connected && retry < CONFIG_SSL_CLIENT_MAX_RETRY) {
    retry++;
    debug_tls("connect() try %d", retry);
    if (_client.connect(hn, port)) {
      _connected = true;
    } else {
      delay(CONFIG_SSL_CLIENT_RETRY_TIMEOUT);
    }
  }
  if (!_connected) {
    debug_tls("Connection failed");
    return 1;
  }

    /* Try session resumption? */
    if (reconnect)
    {
        while (reconnect--)
        {
            ssl = ssl_client_new(ssl_ctx, client_fd, session_id,
                    sizeof(session_id), extensions);
            if ((res = ssl_handshake_status(ssl)) != SSL_OK)
            {
                if (!quiet)
                {
                    ssl_display_error(res);
                }
                ssl_free(ssl);
                ssl_ctx_free(ssl_ctx);
                _client.stop();
                _connected = false;
                return 1;
            }

            display_session_id(ssl);
            memcpy(session_id, ssl_get_session_id(ssl), SSL_SESSION_ID_SIZE);

            if (reconnect)
            {
                ssl_free(ssl);
                //SOCKET_CLOSE(client_fd);
                _client.stop();

                //client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                //connect(client_fd, (struct sockaddr *)&client_addr, 
                //        sizeof(client_addr));
                _connected = _client.connect(hn, port);
                // Should bail here if _connect returns false
            }
        }
    }
    else
    {
        debug_tls("ssl_client_new()");
        ssl = ssl_client_new(ssl_ctx, client_fd, NULL, 0, extensions);
    }

    /* check the return status */
    if ((res = ssl_handshake_status(ssl)) != SSL_OK)
    {
        if (!quiet)
        {
            ssl_display_error(res);
        }
        debug_tls("Handshake error: %d",res);
        ssl_free(ssl);
        ssl_ctx_free(ssl_ctx);
        _client.stop();
        _connected = false;
        return 1;
    }

    if (!quiet)
    {
        display_session_id(ssl);
        display_cipher(ssl);
    }

    // At this point we are connected and verified over SSL
    // or not connected. 
    // res = 0 (success)
    // res = !0 (failed)
    // We can also check the state of !_connected
    debug_tls("connect():%d",res);
    return res;
}

// Return the connection status
int TlsTcpClient::connected() {
  return _connected;
}

// Initialize the IO between this app and
// axtls library
int TlsTcpClient::init() {
  int ret = 0;

  if (_connected) {
    this->close();
  }
  _connected = false;
  _options = SSL_SERVER_VERIFY_LATER|SSL_DISPLAY_CERTS;
  //DEBUGGING
  //_options = SSL_SERVER_VERIFY_LATER|SSL_DISPLAY_CERTS|SSL_DISPLAY_BYTES;

  if (ssl_ctx == NULL) {
    debug_tls("init()");
  } else {
    debug_tls("init() ssl_ctx(%p) send_Tls(%p) recv_Tls(%p)",
      ssl_ctx, &TlsTcpClient::send_Tls, &TlsTcpClient::recv_Tls);
    axtls_io_pathways(ssl_ctx, &TlsTcpClient::send_Tls, &TlsTcpClient::recv_Tls);
  }
  
  return ret;
}

// Read from the server until the timeout is seen
int TlsTcpClient::read(int timeout) {
  int ctimeout = 0;
  int res = 0;
  unsigned char *readbuffer;
  char buf[512] = { 0 };
  int bptr = 0;
  unsigned int i;
  unsigned char ch;
  int nb;

  debug_tls("read:begin()");

  // Wait a timeout or a valid response
  while (ctimeout < timeout and res == 0) {
    if (_connected) {
      if (_client.available() > 0) {
        nb = ssl_read(ssl, &readbuffer);
        if (nb > 0) {
          debug_tls("read(%d)",strlen((char*)readbuffer));
          for (i = 0; i < strlen((char*)readbuffer); i++) {
            ch = readbuffer[i];
            if (ch == 10 || ch == 13) {
              if (bptr > 0) {
                debug_tls("web>%s",(const char*)buf);
                bptr = 0;
                memset(buf, 0, sizeof(buf));
              }
            } else {
              snprintf(buf+bptr,512-bptr,"%c",ch);
              bptr++;
            }
          }
          if (bptr > 0) {
            debug_tls("web>%s",(const char*)buf);
            bptr = 0;
            memset(buf, 0, sizeof(buf));
          }
        } else {
          if (nb < 0) {
            res = nb;
          }
        }
        ctimeout = 0;
      } else {
        Particle.process();
        delay(100);
      }
    } else {
      res = -1;
    }
    ctimeout++;
  }

  // The connection closed or died
  if (res < 0) {
      ssl_free(ssl);
      ssl_ctx_free(ssl_ctx);
      _client.stop();
      _connected = false;
  }
  debug_tls("read:end(%d)",res);
  return res;
}

// Send items to the server
int TlsTcpClient::write(const char *authToken, const char *url) {

  unsigned char* data = (unsigned char *) malloc(sizeof(unsigned char) * 400);
  int res = 0;
  int len = 0;

  if (_connected) {
    sprintf((char *)data, "GET %s HTTP/1.1\r\n",url);
    //sprintf(data, "%sHost: things.ubidots.com\r\nUser-Agent: %s/%s\r\n", data, USER_AGENT, VERSION);
    sprintf((char *)data, "%sHost: things.ubidots.com\r\nUser-Agent: %s/%s\r\n", data, "axTLS", "2.3.1a");
    //len = sprintf((char *)data, "%sX-Auth-Token: %s\r\nConnection: close\r\n\r\n", data, authToken);
    len = sprintf((char *)data, "%sX-Auth-Token: %s\r\n\r\n", data, authToken);

    Particle.process();
    res = ssl_write(ssl, data, strlen((char *)data));

    if (res < 0) {
      ssl_free(ssl);
      ssl_ctx_free(ssl_ctx);
      _client.stop();
      _connected = false;
      res = -1;
    } else {
      debug_tls("successfully wrote to ssl_write()");
    }
  }
  return res;
}

// This is the SOCKET_READ we need
// We pass this into axtls library as
// a function callback. 
// We need to introduce a timer to wait for
// the desired number of bytes.
// 
int TlsTcpClient::recv_Tls(void *ssl, uint8_t *in_data, int in_len) {
  int ret = 0;
  int timeout = 0;

  // We have to extract the _client pointer
  SSL *real_ssl = (SSL *)ssl;
  TCPClient *sock = (TCPClient *) real_ssl->ssl_ctx->_client;

  debug_tls("Want %d byte(s)",in_len);
  Particle.process();

  debug_tls("ssl(%p) ssl_ctx(%p) sock(%p)",real_ssl,real_ssl->ssl_ctx,sock);
  debug_tls("sock->connected():%d sock->available():%d", sock->connected(), sock->available());

  // Loop here for a bit until we get data or we timeout
  while (ret == 0 && timeout < CONFIG_HTTP_TIMEOUT) {
    if (sock->connected()) {
      if (sock->available() > 0) {
        ret = sock->read(in_data, in_len);
      } else {
        delay(10); // 0.01s (300 ~ 3s)
        Particle.process();
        timeout++;
      }
    } else {
      // No longer connected, return a bad result
      ret = -1;
    }
  }
  // Check for timeout
  if (timeout >= CONFIG_HTTP_TIMEOUT) {
    debug_tls("TCP read() timeout");
    ret = -1;
  }

  debug_tls("Got %d byte(s)",ret);
  return ret;
}

// This is the SOCKET_WRITE we need
// We pass this into axtls library as
// a function callback.
int TlsTcpClient::send_Tls(void *ssl, uint8_t *out_data, int out_len) {
  int ret = 0;

  // We have to extract the _client pointer
  SSL *real_ssl = (SSL *)ssl;
  TCPClient *sock = (TCPClient *) real_ssl->ssl_ctx->_client;

  debug_tls("ssl(%p) ssl_ctx(%p) sock(%p)",real_ssl,real_ssl->ssl_ctx,sock);
  debug_tls("sock->connected():%d sock->available():%d", sock->connected(), sock->available());
  if (sock->connected()) {
    ret = sock->write(out_data, out_len);
    sock->flush();
  } else {
    return -1;
  }

  // Allow the WiFi module to catch up
  Particle.process();
  debug_tls("Wanted to send %d bytes, sent %d bytes", out_len, ret);

  return ret;
}

