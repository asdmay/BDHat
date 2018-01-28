
/*  Pulse Sensor Amped 1.5    by Joel Murphy and Yury Gitman   http://www.pulsesensor.com

----------------------  Notes ----------------------  ----------------------
This code:
1) Blinks an LED to User's Live Heartbeat   PIN 9
2) Fades an LED to User's Live HeartBeat    PIN 5
3) Determines BPM
4) Prints All of the Above to Serial

Read Me:
https://github.com/WorldFamousElectronics/PulseSensor_Amped_Arduino/blob/master/README.md
 ----------------------       ----------------------  ----------------------
*/

#define PROCESSING_VISUALIZER 1
#define SERIAL_PLOTTER  2
#include <Servo.h>
Servo servo;
//  Variables
int pulsePin = 0;                 // Pulse Sensor purple wire connected to analog pin 0
//int blinkPin = 9;                // pin to blink led at each beat
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin
int pikonPin = 5;
int pulse_normal = 80;
int servoPin = 6;
int deg = 0; //サーボ初期角

// Volatile Variables, used in the interrupt service routine!
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded!
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat".
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.
volatile int count = 0;
// SET THE SERIAL OUTPUT TYPE TO YOUR NEEDS
// PROCESSING_VISUALIZER works with Pulse Sensor Processing Visualizer
//      https://github.com/WorldFamousElectronics/PulseSensor_Amped_Processing_Visualizer
// SERIAL_PLOTTER outputs sensor data for viewing with the Arduino Serial Plotter
//      run the Serial Plotter at 115200 baud: Tools/Serial Plotter or Command+L
static int outputType = SERIAL_PLOTTER;


void setup(){
//  pinMode(blinkPin,OUTPUT);         // pin that will blink to your heartbeat!
  pinMode(pikonPin,OUTPUT);
  Serial.begin(115200);             // we agree to talk fast!
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS
   // IF YOU ARE POWERING The Pulse Sensor AT VOLTAGE LESS THAN THE BOARD VOLTAGE,
   // UN-COMMENT THE NEXT LINE AND APPLY THAT VOLTAGE TO THE A-REF PIN
//   analogReference(EXTERNAL);
  servo.attach(servoPin);
}


//  Where the Magic Happens
void loop(){
  serialOutput();
  servo.write(deg);
  if (QS == true){     // A Heartbeat Was Found
                       // BPM and IBI have been Determined
                       // Quantified Self "QS" true when arduino finds a heartbeat
    serialOutputWhenBeatHappens();   // A Beat Happened, Output that to serial.
    deg = 0;
    if ( BPM < 75 ) {
       deg = 90;
       count = 0;
       digitalWrite(pikonPin, LOW); 
    }else{
       digitalWrite(pikonPin, HIGH);
       deg = 180;
       count++;
       if (count > 30 ) {
         digitalWrite(pikonPin, HIGH);
         delay(10);
         digitalWrite(pikonPin, LOW);
       }
    }
    QS = false;                      // reset the Quantified Self flag for next time
  }
  delay(20);                         //  take a break
}

