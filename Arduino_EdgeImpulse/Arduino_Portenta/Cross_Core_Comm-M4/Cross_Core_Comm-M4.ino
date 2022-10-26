// Sketch uses  154,496 bytes (14%) of program storage space.
// Maximum is 1,048,576 bytes.
#include "RPC.h"              // Comes with the mbed board installation.

void BlinkR()
{
  delay(500);
  delay(500);
  
  digitalWrite(LEDG, LOW);  // Turn the red LED on (LOW is the voltage level).
  delay(250);               // Wait for 250 milliseconds.
  digitalWrite(LEDG, HIGH); // Turn the LED off by setting the voltage HIGH.
  delay(250);               // Wait for 250 milliseconds.
  digitalWrite(LEDG, LOW);  // Turn the red LED on (LOW is the voltage level).
  delay(250);               // Wait for 250 milliseconds.
  digitalWrite(LEDG, HIGH); // Turn the LED off by setting the voltage HIGH.
  delay(250);               // Wait for 250 milliseconds.

  delay(500);
  delay(500);

}

int m4IntGlobalVar1 = 4444;
/**
 * @brief      When the other core, M7, does RPC with M4 it is bounded to land here.
 */
int setOneM4Var(int var1_FromM7)
{
    m4IntGlobalVar1 = (int)var1_FromM7;                            // Save M7 value into an M4 global variable.

    String toM7 = "M4 says: m4IntGlobalVar1=" + String(m4IntGlobalVar1); 
    RPC.println(toM7);                                             // Show what M7 value got stored in M4.
                                                                   // TODO: Create a wrapper function so we can use a statement here that looks like:
                                                                   //       RPC_println("My integer = %i", myInt);

    return m4IntGlobalVar1;                                        // Return what M7 value got stored in M4.
                                                                   // Hopefully, this is the same as what got sent in.
}


// The setup() function runs once when you press reset or power the board.
void setup()
{
  RPC.begin();
  RPC.println("M4 Says: Entering setup()...");
  RPC.println("M4 Says: Entering setup()...");
  RPC.println("M4 Says: Entering setup()...");

  RPC.bind("setOneVarInM4", setOneM4Var);                        // These arguments do not have to be the same.
                                                                 // M7 calls "setOneVarInM4()".
                                                                 // M4 has a routine called "setOneM4Var()".
                                                                 // Thus, bind() is a bounden between the name and function.    

 
  // initialize digital pin LED_BUILTIN as an output.
  //bootM4(); // Maximum is 1,048,576 bytes. <-- REM in when programming M7 core!!!
  //bootM7(); // Maximum is   786,432 bytes.
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  //delay(10000);
  RPC.println("M4 Says: Exiting setup()...");
}

///#define Serial RPC  // So the M4 regular serial prints go to RPC
///Serial.println("Printing from M4...");

// The loop() function runs over and over again forever.
void loop()
{ 
  BlinkR();
  
  auto resReady = RPC.println("M4 Says: Blinking fast RED from M4 Core.");
  // resReady=42
  // RPC.println( "resReady=" + String(resReady) );
  //          1         2         3         4
  // 1234567890123456789012345678901234567890
  // M4 Says: Blinking fast RED from M4 Core.

  int myRand1 = rand() % 100;  // A value from 0 to 99.
  int myRand2 = rand() % 100;  // A value from 0 to 99.
  auto res = RPC.call("setOneVar", myRand1).as<int>();
  RPC.println("M4 Says: M4 core set M7 variable #1 to: " + String(res));

  RPC.call("setTwoVar", myRand1, myRand2); //.as<string>();
  String sentToM7Value = String(myRand1) + " and " + String(myRand2);
  RPC.println("M4 Says: M4 core is setting M7 variables to: " + sentToM7Value);
////  RPC.println("M4 core set M7 variables to:        " + res);
}
