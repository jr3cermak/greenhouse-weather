/*
 * Project axtls
 * Description:
 * Author:
 * Date:
 */

#include <TlsTcpClient.h>
#include "axtls_logging.h"

// Use primary serial over USB interface for logging output
// You can watch logging of the Particle device via CLI:
// $ particle serial monitor --follow
SerialLogHandler logHandler;
Logger appLog("axtls");

TlsTcpClient testMe;

int sentCmd(String command) {
  // try to connect
  if (command == "c") {
    debug_tls("Trying to connect");
    testMe.connect("jupyter.lccllc.info", 4443);
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
