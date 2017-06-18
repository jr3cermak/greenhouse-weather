/*
 * Project UbidotsDataFill
 * Description: Demonstration of backfilling apparently
 *   dropped data.
 * Author: Rob Cermak
 * Date: 2017-06-13
 */

// Stash private stuff in Private.h and be sure not to
// post it anywhere useful.
#include <Private.h>

// Cloud service for data, etc.
#include <Ubidots.h>

// Parsing JSON responses
#include <JsonStreamingParser.h>

// Use primary serial over USB interface for logging output
// You can watch logging of the Particle device via CLI:
// $ particle serial monitor --follow
SerialLogHandler logHandler;

// This will give seconds, we have to *1000 or add
// 000 as Ubicode does to make it milliseconds.
// tm = Time.now();

unsigned long tm = 0;
int runOnce = 0;

// Setup a Ubidots object with our 
// authentication token
Ubidots ubidots(AUTHTOKEN);

// JSON PARSING CLASSES

class DatasourcesListener: public JsonListener {

  public:
    void whitespace(char c) {
        Log.info("whitespace");
    }

    void value(String key, String value) {
        Log.info("key: %s value: %s", (const char *) key, (const char *) value);
    }

    void startDocument() {
        Log.info("start document");
    }

    void endDocument() {
        Log.info("end document");
    }

    void startArray(String key) {
        Log.info("start array: %s", (const char *) key);
    }

    void endArray() {
        Log.info("end array");
    }

    void startObject(String key) {
        if (key != "") {
            Log.info("start object: %s", (const char *) key);
        } else {
            Log.info("start object");
        }
    }

    void endObject() {
        Log.info("end object");
    }
};

// Create parser and listeners.   We will see if the 
// parser is fairly generic and that we can swap
// listeners on the fly?
JsonStreamingParser parser;

// Parsing http://things.ubidots.com/api/v1.6/datasources/
DatasourcesListener datasourcesListener;

// MAIN SETUP

void setup() {
  // Wait for the network to become ready
  waitFor(WiFi.ready, 60000);
  // Wait for the time sync with the Particle cloud to complete.
  // This should guarantee the local device time to be correct.
  waitFor(Particle.syncTimeDone, 60000);
  // Wait another 5 seconds for full setup on Particle cloud
  delay(5000);
}

// MAIN PROGRAM

void loop() {
  if (runOnce == 0) {
    tm = Time.now();
    Log.info("Timestamp: %lu", tm);

    // Step 1: Load a listing of devices
    parser.setListener(&datasourcesListener);
    ubidots.setParser(&parser);
    if (ubidots.getDevices()) {
      Log.info("%d devices are loaded.",ubidots.numDevices());
    } else {
      Log.info("Device loading failed.");
    }
    
    runOnce = 1;
  }
}

/***************************************************************************
FORCE COMPILER FAILURE TO SEE WARNINGS FROM PARTICLE BUILD SERVER
***************************************************************************/

void forcefail()
{
    Time.now();
}
