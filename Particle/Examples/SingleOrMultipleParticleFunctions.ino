/* 
 * Project SingleOrMultipleParticleFunctions
 * Description: Demonstrate the utility of Particle.function() to supply
     one argument or separate arguments.
 * Author: Rob Cermak
 * Date: 2017-06-11

A Google search for "hypotenuse calculator" was performed
to obtain the formulas for this code.  The code assumes
a right sided triangle.

      |\
      | \
  a   |  \ c
      |   \
      |_   \
      |_L___\
         b

Hypotenuse:   c = sqrt (a^2 + b^2)
Area      :   A = a * b / 2
Perimeter :   P = a + b + c

We setup 3 Particle.functions() to change the input of a
and b.  You can update a or b individually or send a
JSON string in to update a and b at the same time.

Example JSON string as input:
  { "a": 3, "b": 4 }

There are advantages and disadvantages to both methods.

Using a and b separately:
  * Use more cellular data is used by performing individual calls.
  * This method may be considered spammier due to the extra hit
    needed to the services.
  * Have to tell the program when to do the computations.
    
Sending a string with all your arguments at once:
  * Need to parse the string.
  * Input validation is problematic.
  * We can use a string parser library that already exists.
    You inherit all the good and/or bad parts about it as well.
  * Have to tell the program when valid input is present
    and do the computations.

Program assumptions:
  * atof() is fairly bullet proof.   Further input validation
    is not performed.
  * No calculation is done if a or b is less than or equal to
    zero.
  * A calculation is performed if a and b are greater than
    zero on any change of a or b.
  * If input becomes invalid, we invalidate "c" the hypotenuse,
    area and perimeter.

*/

// This #include statement was automatically added by the Particle IDE.
// This is using a modified Particle IDE library JsonStreamingParser v1.0.5
// Once working, we will release v1.0.6
#include <JsonStreamingParser.h>

// Use primary serial over USB interface for logging output
// You can watch logging of the Particle device via CLI:
// $ particle serial monitor
SerialLogHandler logHandler;

// Required for sqrt function
#include <math.h>

// Define basic storage
double a = 0.0;
double b = 0.0;
double c = 0.0;
double area = 0.0;
double perimeter = 0.0;

// TIMERS

// We toss a timer in here just to be sure things are
// working.
Timer tickTimer(10000, print_timer_tick);

// CLASSES
class ExampleListener: public JsonListener {

  public:
    void whitespace(char c) {
        Log.info("whitespace");
    }

    void value(String key, String value) {
        Log.info("key: %s value: %s", (const char *) key, (const char *) value);
        
        // Parse the JSON response and assign key(input) and values 
        // appropriately.
        if (key == "a") {
            a = atof(value);
        }
        if (key == "b") {
            b = atof(value);
        }
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

// We have to create a parser and listener each time
// as the library does not end the document correctly.
// IE: We can't define this globally and re-use it.
JsonStreamingParser parser;
ExampleListener listener;

// FUNCTIONS

// This just echo's a tick out to the log
// to show things are working.
void print_timer_tick() 
{
    Log.info("Timer tick");
}

void computePerimeter() 
{
    if (c > 0.0) {
        perimeter = a + b + c;
    }
}

void computeHypotenuse() 
{
  if (a > 0.0 and b > 0.0) {
      c = sqrt((a*a) + (b*b));
  } else {
      c = 0.0;
      area = 0.0;
      perimeter = 0.0;
  }
}

void computeArea()
{
  if (a > 0.0 and b > 0.0) {
    area = ( a * b ) / 2.0;
  }
}

void computeAll()
{
    computeHypotenuse();
    computeArea();
    computePerimeter();
}

// This is our Particle.function() input for
// a supplied JSON string with a and b arguments.
// The JsonStreamingParser is fairly bullet proof
// to avoid most string parsing problems.
int inputArgs(String inArgs)
{
    char c;
    
    Log.info("inArgs:%s", (const char *) inArgs);

    // Parse the input argument
    for (unsigned int i = 0; i < inArgs.length(); i++) {
        c = inArgs.charAt(i);
        parser.parse(c);
    }
    computeAll();
    return 1;
}

// This is a single parameter "a" passed by
// a Particle.function()
int inputA(String inA)
{
    a = atof(inA);
    computeAll();
    if (a > 0.0) {
        return 1;
    }
    return 0;
}

// This is a single parameter "b" passed by
// a Particle.function()
int inputB(String inB)
{
    b = atof(inB);
    computeAll();
    if (b > 0.0) {
        return 1;
    }
    return 0;
}

// MAIN PROGRAM
void setup() {
    
    Particle.variable("a",a);
    Particle.variable("b",b);
    Particle.variable("c",c);
    Particle.variable("area",area);
    Particle.variable("perimeter",perimeter);
    Particle.function("inputA",inputA);
    Particle.function("inputB",inputB);
    Particle.function("inputArgs",inputArgs);

    // We need a listener to be connected to the parser
    parser.setListener(&listener);

    // Start our tick timer
    tickTimer.start();
}

void loop() {

}
