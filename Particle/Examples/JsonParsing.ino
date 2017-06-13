/*
 * Project: JsonParsing
 * Description: Demonstrate the use of the JsonStreamingParser
 * Author: Rob Cermak
 * Date: 2017-06-12
 *
 * REF:
 *  https://community.particle.io/t/is-this-the-best-way-to-handle-simple-ubidots-webhook-responses/33691
*/

#include <JsonStreamingParser.h>

// Globals
int hourly_status_code = 0;
int isHourly = 0;
int run_once = 0;

// CLASSES
class ExampleListener: public JsonListener {

  public:
    void whitespace(char c) {
        Log.info("whitespace");
    }

    void value(String key, String value) {
        Log.info("key: %s value: %s", (const char *) key, (const char *) value);
        
        if (isHourly && key == "status_code") {
          hourly_status_code = atoi(value);
        }
    }

    void startDocument() {
        Log.info("start document");
    }

    void endDocument() {
        Log.info("end document");
    }

    void startArray(String key) {
        if (key == "hourly") {
          isHourly = 1;
        } else {
          isHourly = 0;
        }
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

// Create a parser and listener.   The parser is a
// very generic parser.   You can construct different
// listener classes to parse different JSON responses.
JsonStreamingParser parser;
ExampleListener listener;

// Allows use of $ particle serial monitor --follow
// to watch Log messages.
SerialLogHandler logHandler;

// Establish logging via serial
// Initialization steps
void setup() {
  // Delay starting the program for about 10 seconds to 
  // allow us to start up the serial monitor
  delay(10000);
  // Format text message
  Log.info("System version: %s", (const char*)System.version());

  // We need a listener to be connected to the parser
  parser.setListener(&listener);
}

// Continuously runs after setup()
void loop() {

  if (run_once == 0) {
    char c;
    String msgToParse;

    Log.info("** TEST PARSE GOOD MESSAGE #1");
    msgToParse = "\
      {\
        \"battery\": [{\"status_code\":201}],\
        \"hourly\": [{\"status_code\":201}],\
        \"temp\": [{\"status_code\":201}],\
        \"daily\": [{\"status_code\":201}],\
      }";
    // Parse the input
    for (unsigned int i = 0; i < msgToParse.length(); i++) {
      c = msgToParse.charAt(i);
      parser.parse(c);
    }
    Log.info("Hourly status code: %d",hourly_status_code);

    Log.info("** TEST PARSE GOOD MESSAGE #2");
    msgToParse = "\
      {\
        \"battery\": [{\"status_code\":201}],\
        \"hourly\": [{\"status_code\":500}],\
        \"temp\": [{\"status_code\":201}],\
        \"daily\": [{\"status_code\":201}],\
      }";
    // Parse the input
    for (unsigned int i = 0; i < msgToParse.length(); i++) {
      c = msgToParse.charAt(i);
      parser.parse(c);
    }
    Log.info("Hourly status code: %d",hourly_status_code);

    Log.info("** TEST PARSE BAD MESSSAGE");
    msgToParse = "\
      {\
        \"battery\": [{\"status_code\":201}]\
        \"hourly\": [{\"status_code\":201}]\
        \"temp\": [{\"status_code\":201}]\
        \"daily\": [{\"status_code\":201}]\
      }";
    // Parse the input
    for (unsigned int i = 0; i < msgToParse.length(); i++) {
      c = msgToParse.charAt(i);
      parser.parse(c);
    }
    Log.info("Hourly status code: %d",hourly_status_code);

    run_once = 1;
  }
}
