#include "TlsTcpClient.h"


TlsTcpClient::TlsTcpClient() {
  _connected = false;
}

TlsTcpClient::~TlsTcpClient() {
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
      _connected = 1;
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
                //exit(1);
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
                _client.connect(hn, port);
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
        //exit(1);
        return 1;
    }

    if (!quiet)
    {
        display_session_id(ssl);
        display_cipher(ssl);
    }

    // At this point we are connected and verified over SSL
    // or not connected
    return res;
}

// Initialize the IO between this app and
// axtls library
int TlsTcpClient::init() {
  int ret = 0;

  _connected = false;
  _options = SSL_SERVER_VERIFY_LATER|SSL_DISPLAY_CERTS;

  if (ssl_ctx == NULL) {
    debug_tls("init()");
  } else {
    debug_tls("init() ssl_ctx(%p) send_Tls(%p) recv_Tls(%p)",
      ssl_ctx, &TlsTcpClient::send_Tls, &TlsTcpClient::recv_Tls);
    axtls_io_pathways(ssl_ctx, &TlsTcpClient::send_Tls, &TlsTcpClient::recv_Tls);
  }
  
  return ret;
}

// This is the SOCKET_READ we need
// We pass this into axtls library as
// a function callback.
int TlsTcpClient::recv_Tls(void *ssl, uint8_t *in_data, int in_len) {
  int ret = 0;

  // We have to extract the _client pointer
  SSL *real_ssl = (SSL *)ssl;
  TCPClient *sock = (TCPClient *) real_ssl->ssl_ctx->_client;

  debug_tls("ssl(%p) ssl_ctx(%p) sock(%p)",real_ssl,real_ssl->ssl_ctx,sock);
  debug_tls("sock->connected():%d sock->available():%d", sock->connected(), sock->available());
  if (sock->connected()) {
    if (sock->available() > 0) {
      ret = sock->read(in_data, in_len);
    } else {
      ret = -1;
    }
    ret = -1;
  }

  debug_tls("Want %d bytes, got %d bytes",in_len,ret);
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
  } else {
    return -1;
  }
  sock->flush();

  debug_tls("Wanted to send %d bytes, sent %d bytes", out_len, ret);

  return ret;
}

