/*
 * Project ColorEventQueue
 * Description: Event color queue example.  This uses the Arduino QueueList library to store
 *   a queue of items that handled in a first in, first out basis.
 * REF: https://community.particle.io/t/how-to-store-color-values-in-a-dynamic-2d-array/34029
 * REF: https://www.codingunit.com/cplusplus-tutorial-structures-typedef-and-unions
 * REF: https://playground.arduino.cc/Code/QueueList
 * Author: Rob Cermak
 * Date: 2017-06-26
 */

#include "application.h"
#include <QueueList.h>
SYSTEM_THREAD(ENABLED);

// Use primary serial over USB interface for logging output
// You can watch logging of the Particle device via the Particle CLI
// This will enable basic diagnostic output.  See "axtls_config.h"
// for other options.
// $ particle serial monitor --follow
SerialLogHandler logHandler(LOG_LEVEL_ALL);

typedef struct _eventColor {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} eventColor;

// Define our queue with our structure
QueueList <eventColor> eventQueue;

// We want the loop to run some commands once
int run_once = 0;

// FUNCTIONS
void doEvent() {
  eventColor myEvent;

  myEvent = eventQueue.pop();

  Log.info("Event seen R(%d) G(%d) B(%d)", myEvent.red, myEvent.green, myEvent.blue);
}

// setup() runs once, when the device is first turned on.
void setup() {
  String devId;
  // Do not need this, this helps group program output at the end
  delay(10000);

  // Display device ID just for grins
  devId = System.deviceID();
  Log.info("Device ID(%s)",(const char *) devId);

  // Create a variable with the defined structure
  eventColor event;

  // Put initialization like pinMode and begin functions here.
  Log.trace("creating eventColor queue");

  Log.trace("adding some sample events:RG");
  event.red = 200;
  event.green = 200;
  event.blue = 0;
  eventQueue.push(event);

  Log.trace("adding some sample events:R");
  event.green = 0;
  eventQueue.push(event);

  Log.trace("Event count? %d", eventQueue.count());
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  eventColor eventLoop;
  // The core of your code will likely live here.

  if (run_once == 0) {
    // Simulate adding an event here, could be via IFTTT or MQTT or Particle.function()
    Log.trace("adding some sample events:B");
    eventLoop.red = 0;
    eventLoop.green = 0;
    eventLoop.blue = 200;
    eventQueue.push(eventLoop);

    // We only want to do this once
    run_once = 1;
  }

  // Event handler
  if (eventQueue.count() > 0) {
    // We have an event to process, let us do one at a time
    doEvent();
  }
}
