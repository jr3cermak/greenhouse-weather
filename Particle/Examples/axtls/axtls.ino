/*
 * Project axtls
 * Description:
 * Author:
 * Date:
 */

/* You need to create your own Private.h file with
the needed information

---Private.h---
  Include private definitions, tokens, etc

#ifndef PRIVATE_H
#define PRIVATE_H

// Authentication token
#ifndef AUTHTOKEN
#define AUTHTOKEN "w............................A"
#endif

#endif
---------------
*/

#include <TlsTcpClient.h>
#include "axtls_logging.h"
#include "Private.h"

// Use primary serial over USB interface for logging output
// You can watch logging of the Particle device via CLI:
// $ particle serial monitor --follow
SerialLogHandler logHandler;
Logger appLog("axtls");

TlsTcpClient testMe;

int sentCmd(String command) {
  if (command == "c") {
    testMe.close();
  }
  // try to connect
  if (command == "j") {
    debug_tls("Trying to connect");
    if (testMe.connect("jupyter.lccllc.info", 4443)) {
      debug_tls("Connection failed");
    } else {
      debug_tls("Connected");
    }
    return 1;
  }
  if (command == "t") {
    debug_tls("Trying to connect");
    if (testMe.connect("things.ubidots.com", 443)) {
      debug_tls("Connection failed");
    } else {
      debug_tls("Connected");
    }
    return 1;
  }
  if (command == "q") {
    if (testMe.connected()) {
      testMe.write(AUTHTOKEN,"/api/v1.6/datasources/");
      testMe.read(1000); 
    } else {
      debug_tls("Not connected");
    }
    return 1;
  }
  if (command == "i") {
    debug_tls("Reinitialize");
    testMe.init();
    return 1;
  }
  return 0;
}

void setup() {

  // Allow us to send commands to the Particle
  Particle.function("cmd",sentCmd);

  testMe.init();
  debug_tls("setup() done.");
}

void loop() {
}
