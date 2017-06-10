/*
  DebugWiFi
*/

// Use primary serial over USB interface for logging output
SerialLogHandler logHandler;
TCPClient client;
IPAddress ip;
int reading_content = 0;
int connected = 0;
String msg = "";

void setup() {

  // The WiFi module saves credentials!
  //WiFi.clearCredentials();

  delay(10000);

  waitUntil(WiFi.ready);
  
  byte mac[6];
  Particle.process();
  
  Log.info("System version: %s", System.version().c_str());
  Log.info("Wifi SSID: %s", WiFi.SSID());
  WiFi.BSSID(mac);
  Log.info("Mac address SSID: %02x:%02x:%02x:%02x:%02x:%02x", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
  WiFi.macAddress(mac);
  Log.info("Mac address: %02x:%02x:%02x:%02x:%02x:%02x", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
  ip = WiFi.dhcpServerIP();
  Log.info("DHCP Server IP: %u.%u.%u.%u", ip[0],ip[1],ip[2],ip[3]);
  ip = WiFi.localIP();
  Log.info("Assigned IP: %u.%u.%u.%u", ip[0],ip[1],ip[2],ip[3]);
  ip = WiFi.subnetMask();
  Log.info("Subnet Mask: %u.%u.%u.%u", ip[0],ip[1],ip[2],ip[3]);
  ip = WiFi.gatewayIP();
  Log.info("Gateway IP: %u.%u.%u.%u", ip[0],ip[1],ip[2],ip[3]);
  ip = WiFi.dnsServerIP();
  Log.info("DNS Server IP: %u.%u.%u.%u", ip[0],ip[1],ip[2],ip[3]);
  
  // Do a simple port 80 test to www.google.com to test for a firewall
  // Port 80 is generally open
  ip = WiFi.resolve("www.google.com");
  Log.info("Resolved IP for www.google.com: %u.%u.%u.%u", ip[0],ip[1],ip[2],ip[3]);
  
  if (client.connect(ip, 80))
  {
      Log.info("Connected...");
      client.println("GET /search?q=unicorn HTTP/1.0");
      client.println("Host: www.google.com");
      client.println("Content-Length: 0");
      client.println();
      connected = 1;
  } 
  else
  {
      Log.info("Connection failed.");
  }
}

void loop() 
{
    if (client.available()) {
        if (reading_content == 0) {
            Log.info("Reading content...");
        }
        reading_content++;
        char c = client.read();
        if (c > 0) {
            if (c == 10 || c == 13) {
                if (msg != "") {
                    Log.info("Web>%s", (const char *) msg);
                }
                msg = "";
            } else {
                msg.concat(c);
            }
        }
    }

    if (connected && !client.connected())
    {
        if (msg != "") {
           Log.info("Web>%s", (const char *) msg); 
        }
        Log.info("Disconnecting.");
        client.stop();
        connected = 0;
    }
}
