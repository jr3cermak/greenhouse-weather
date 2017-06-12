// Alternative to Strings
//
// Inspired by
//   https://community.particle.io/t/need-help-sorting-asset-tracker-coordinates-from-serial-on-arduino/33420

/*  
 Many many posts about heap fragmentation with String use. 
 REF:
   https://community.particle.io/t/photon-softap-inputs/31289/30
   https://community.particle.io/t/hard-fault-on-electron-after-about-an-hour/33141/8
   https://community.particle.io/t/rest-api-arguments-is-there-a-better-way/960/22
   https://community.particle.io/t/problems-with-mqtt-and-ubidots/31579/12
   https://community.particle.io/t/photon-system-panic-hard-fault-task-stack-size/30758/4
   https://community.particle.io/t/solved-storing-strings-into-an-array-basic-programming-question/30563/2
   https://community.particle.io/t/2nd-string-missing-after-concat-2nd-3rd-string-to-1st-string/30006/2
   https://community.particle.io/t/string-concat-const-char-cstr-unsigned-int-length-is-protected-why/29259/7
   https://community.particle.io/t/particle-cloud-cheat-sheet/28698/31
   https://community.particle.io/t/char-arrays-or-string-objects/29037/8
   https://community.particle.io/t/electron-particle-subscribe-sometimes-works/27571/13
   https://community.particle.io/t/time-timestr-sometimes-returns-empty/27546/2
   .... and on and on ....

 snprintf is your friend
   https://community.particle.io/t/encoding-json-in-firmware/33131/4
   https://community.particle.io/t/using-the-attachinterrupt-function/32950/5
   https://community.particle.io/t/trying-to-get-particlepublish-to-publish-data-contained-in-a-float-type-variable/32591/4
   https://community.particle.io/t/running-out-of-heap-memory/32078/2
   https://community.particle.io/t/truncate-round-double-variables-to-2-decimals-in-particle-publish/30723/2

 What what? Deep sleep does a reset?  Helps heap fragmentation.   This also 
 turns of cellular which will use data for each reconnect.  Use wisely.
   https://community.particle.io/t/code-not-working-after-15-days-in-the-field/32767/13

 In this example, we use a String to emulate a stream of text that we have to parse into
 latitude and longitude.  Doesn't matter if you a reading characters over Serial or Wire
 or whatever.   We can use a fixed character buffer for temporary storage.   I've added
 some garbage to the stream so you can see what other pitfalls may befall you.

 We will mark the beginning of a transmission block with a '$'.   The should generally
 mean that anything we have in the buffer currently should be dumped regardless of
 their contents.  If we see a '#', then we have a completed transmission.  We can
 do some sanity checks on the input and pass it along to the application.   We have
 an agreed on pattern of input that following the $ we will get a latitude and upon
 seeing a , we should get a longitude and finish with a #.   Any exceptions to this
 pattern should be ignored.  How about that last patterns?
 
 The 2nd to last pattern, we didn't sent a '$' so that part should be ignored completely.
 This may be a result of a buffer overrun.
 
 Since we are going to use atof() to convert the passed data to floats/doubles, any
 bad data will end up being zero (0).  Unfortunately, any poor soul at 
 0.0000 Lat and 0.0000 Long will be out of luck.

 This is not a definitive solution!  Feel free to adopt your own solution(s)!
 
 Descriptions of the variables and what they do:
 
 msgLen : we will initialize this in setup() to the test string we will send.  If you are reading
 from serial, you will not know the length, you will just be read()ing one character at a time.
 
 msgToParse : test string
 
 msgPtr : tells us where in the test message we are at.
 
 msgPart :
   0 -> we are not currently parsing a message
   1 -> we think we are reading latitude
   2 -> we think we are reading longitude
   
 completeMessage:
   0 -> we don't have a complete message yet
   1 -> we have a complete message, go display it if the data looks right
   
 latitude : storage location for a latitude we just read
 
 longitude : storage location for a longitude we just read

 Why so many little functions?
 
   For microcontroller work, we want routines to run quickly with
   as little effort as possible since we are sharing essentially
   one processor and there could be a thousand other things
   trying to use the processor too like timers or inbound Particle
   events.
 
*/

// Globals

int msgLen = 0;
String msgToParse = "$13.123812,-123.148240#$13.123812,-123.148240#$13.12garbage$13.123812,-123.148240#$more,crud#.235,-150.0#";
int msgPtr = 0;
int msgPart = 0;
int completeMessage = 0;
double latitude = 0.0;
double longitude = 0.0;

// Here is our message buffer, make it the desired size.
char comBuf[32] = { 0 };
// We have to keep track of what is in the buffer.
char *comBufPtr = comBuf;
char const *comBufEnd = comBuf + sizeof(comBuf);

// I like using the Log command for logging; no need for Serial.begin(9600) 
// unless you are using for other purposes.   If you comment the line out.
// All the logging vanishes.
// Allows for use of Particle-CLI: particle serial monitor
SerialLogHandler logHandler;

// Functions

// Two helper functions for the communcation buffer

// Append to current communications buffer, check to
// make sure we don't write past available memory.
void writeComBuf(char c) {
  if (comBufPtr < comBufEnd) {
    comBufPtr += snprintf(comBufPtr, comBufEnd-comBufPtr, "%c", c);
  }
}

// Clear out the contents of the communications buffer and
// reset the pointer
void clearComBuf() {
  memset(comBuf, 0, sizeof(comBuf));
  comBufPtr = comBuf;
}

// Simulate Serial.available()
int available() {
    int n = msgLen - msgPtr;
    
    return n;
}

// Simulate Serial.read()
// Read a character from our String
char readChar() {
    char c = NULL;
    if (msgPtr < msgLen) {
        c = msgToParse.charAt(msgPtr);
        Log.info("Read char %d %c", c, c);
        msgPtr++;
    }
    return c;
}

// Serial processing
// This is the workhorse, but let us keep it as compact as possible.
// If the message is complete, let someone else handle it! 
void processSerial(char c) {

    // This means we have a NEW message! Reset 
    // all our storage.
    if (c == '$') {
        Log.info("Start of message seen");
        clearComBuf();
        msgPart = 1;
        latitude = 0.0;
        longitude = 0.0;
        completeMessage = 0;
        return;
    }
    
    // This means we are done reading latitude
    // Store the value of latitude and assume
    // we will read longitude next!
    if (msgPart == 1 && c == ',') {
        latitude = atof(comBuf);
        Log.info("Got lat (%f), now reading lon",latitude);
        clearComBuf();
        msgPart = 2;
        return;
    }
    
    // This means we are done reading longitude
    // We might have a complete message?
    if (msgPart == 2 && c == '#') {
        Log.info("End of message seen");
        longitude = atof(comBuf);
        clearComBuf();
        msgPart = 0;
        completeMessage = 1;
        return;
    }
    
    // If we are actively reading data, we have to add
    // it to the communcations buffer
    if (msgPart) {
        writeComBuf(c);
        return;
    }
    
    // If we get here, then we apparently are
    // ignoring bad data
    return;
}

void setup() {
    // Things boot up to fast to start the serial monitor, let us wait a bit.
    delay(10000);
    msgLen = msgToParse.length();
    Log.info("Message to parse: LEN=%d", msgLen);
}

void loop() {
    // Iterate through the string until we are done
    if (available()) {
        char c = readChar();
        processSerial(c);
    }
    
    // Wait for a signal that we have a complete message, then display it
    if (completeMessage) {
        if (latitude != 0.0 && longitude != 0.0) {
          // This could easily call another function or Particle.publish() or whatever
          Log.info("New latitude and longitude: %.2f LAT %.2f LON",latitude,longitude);
        }
        latitude = 0.0;
        longitude = 0.0;
        completeMessage = 0;
    }
}

/*  OUTPUT

0000013006 [app] INFO: Message to parse: LEN=105
0000013006 [app] INFO: Read char 36 $
0000013006 [app] INFO: Start of message seen
0000013007 [app] INFO: Read char 49 1
0000013008 [app] INFO: Read char 51 3
0000013009 [app] INFO: Read char 46 .
0000013010 [app] INFO: Read char 49 1
0000013011 [app] INFO: Read char 50 2
0000013012 [app] INFO: Read char 51 3
0000013013 [app] INFO: Read char 56 8
0000013014 [app] INFO: Read char 49 1
0000013015 [app] INFO: Read char 50 2
0000013016 [app] INFO: Read char 44 ,
0000013016 [app] INFO: Got lat (13.123812), now reading lon
0000013017 [app] INFO: Read char 45 -
0000013018 [app] INFO: Read char 49 1
0000013019 [app] INFO: Read char 50 2
0000013020 [app] INFO: Read char 51 3
0000013021 [app] INFO: Read char 46 .
0000013022 [app] INFO: Read char 49 1
0000013023 [app] INFO: Read char 52 4
0000013024 [app] INFO: Read char 56 8
0000013025 [app] INFO: Read char 50 2
0000013026 [app] INFO: Read char 52 4
0000013027 [app] INFO: Read char 48 0
0000013028 [app] INFO: Read char 35 #
0000013028 [app] INFO: End of message seen
0000013028 [app] INFO: New latitude and longitude: 13.12 LAT -123.15 LON
0000013029 [app] INFO: Read char 36 $
0000013029 [app] INFO: Start of message seen
0000013030 [app] INFO: Read char 49 1
0000013031 [app] INFO: Read char 51 3
0000013032 [app] INFO: Read char 46 .
0000013033 [app] INFO: Read char 49 1
0000013034 [app] INFO: Read char 50 2
0000013035 [app] INFO: Read char 51 3
0000013036 [app] INFO: Read char 56 8
0000013037 [app] INFO: Read char 49 1
0000013038 [app] INFO: Read char 50 2
0000013039 [app] INFO: Read char 44 ,
0000013039 [app] INFO: Got lat (13.123812), now reading lon
0000013040 [app] INFO: Read char 45 -
0000013041 [app] INFO: Read char 49 1
0000013042 [app] INFO: Read char 50 2
0000013043 [app] INFO: Read char 51 3
0000013044 [app] INFO: Read char 46 .
0000013045 [app] INFO: Read char 49 1
0000013046 [app] INFO: Read char 52 4
0000013047 [app] INFO: Read char 56 8
0000013048 [app] INFO: Read char 50 2
0000013049 [app] INFO: Read char 52 4
0000013050 [app] INFO: Read char 48 0
0000013051 [app] INFO: Read char 35 #
0000013051 [app] INFO: End of message seen
0000013051 [app] INFO: New latitude and longitude: 13.12 LAT -123.15 LON
0000013052 [app] INFO: Read char 36 $
0000013052 [app] INFO: Start of message seen
0000013053 [app] INFO: Read char 49 1
0000013054 [app] INFO: Read char 51 3
0000013055 [app] INFO: Read char 46 .
0000013056 [app] INFO: Read char 49 1
0000013057 [app] INFO: Read char 50 2
0000013058 [app] INFO: Read char 103 g
0000013059 [app] INFO: Read char 97 a
0000013060 [app] INFO: Read char 114 r
0000013061 [app] INFO: Read char 98 b
0000013062 [app] INFO: Read char 97 a
0000013063 [app] INFO: Read char 103 g
0000013064 [app] INFO: Read char 101 e
0000013065 [app] INFO: Read char 36 $
0000013065 [app] INFO: Start of message seen
0000013066 [app] INFO: Read char 49 1
0000013067 [app] INFO: Read char 51 3
0000013068 [app] INFO: Read char 46 .
0000013069 [app] INFO: Read char 49 1
0000013070 [app] INFO: Read char 50 2
0000013071 [app] INFO: Read char 51 3
0000013072 [app] INFO: Read char 56 8
0000013073 [app] INFO: Read char 49 1
0000013074 [app] INFO: Read char 50 2
0000013075 [app] INFO: Read char 44 ,
0000013075 [app] INFO: Got lat (13.123812), now reading lon
0000013076 [app] INFO: Read char 45 -
0000013077 [app] INFO: Read char 49 1
0000013078 [app] INFO: Read char 50 2
0000013079 [app] INFO: Read char 51 3
0000013080 [app] INFO: Read char 46 .
0000013081 [app] INFO: Read char 49 1
0000013082 [app] INFO: Read char 52 4
0000013083 [app] INFO: Read char 56 8
0000013084 [app] INFO: Read char 50 2
0000013085 [app] INFO: Read char 52 4
0000013086 [app] INFO: Read char 48 0
0000013087 [app] INFO: Read char 35 #
0000013087 [app] INFO: End of message seen
0000013087 [app] INFO: New latitude and longitude: 13.12 LAT -123.15 LON
0000013088 [app] INFO: Read char 36 $
0000013088 [app] INFO: Start of message seen
0000013089 [app] INFO: Read char 109 m
0000013090 [app] INFO: Read char 111 o
0000013091 [app] INFO: Read char 114 r
0000013092 [app] INFO: Read char 101 e
0000013093 [app] INFO: Read char 44 ,
0000013093 [app] INFO: Got lat (0.000000), now reading lon
0000013094 [app] INFO: Read char 99 c
0000013095 [app] INFO: Read char 114 r
0000013096 [app] INFO: Read char 117 u
0000013097 [app] INFO: Read char 100 d
0000013098 [app] INFO: Read char 35 #
0000013098 [app] INFO: End of message seen
0000013099 [app] INFO: Read char 46 .
0000013100 [app] INFO: Read char 50 2
0000013101 [app] INFO: Read char 51 3
0000013102 [app] INFO: Read char 53 5
0000013103 [app] INFO: Read char 44 ,
0000013104 [app] INFO: Read char 45 -
0000013105 [app] INFO: Read char 49 1
0000013106 [app] INFO: Read char 53 5
0000013107 [app] INFO: Read char 48 0
0000013108 [app] INFO: Read char 46 .
0000013109 [app] INFO: Read char 48 0
0000013110 [app] INFO: Read char 35 #

*/

