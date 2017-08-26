#define DEBUG 1
#define MULT 1
/**** Added by CDM ****************/
#define NEW_SPEED_CONTROL
/*********************************/
// interrupts on mega2560 from http://www.arduino.org/learning/reference/attachinterrupt
#define INTERRUPT_PIN_2 0
#define INTERRUPT_PIN_3 1
#define INTERRUPT_PIN_18 5
#define INTERRUPT_PIN_19 4
#define INTERRUPT_PIN_20 3
#define INTERRUPT_PIN_21 2



// pin definitions and info

// cup turntable
#define CUP_HOLDER_MOTOR_ON         11 // PWM duty cycle speed control. Direction controlled by CUP_HOLDER_DIRECTION
#define CUP_HOLDER_DIRECTION        46 // LOW rotates cup in, HIGH rotates cup out

// cup turntable sensors (have a lot of analog noise)
#define DOOR_LEFT                   A4 // goes LOW when cup has rotated in
#define DOOR_RIGHT                  A5 // goes LOW when cup has rotated out

// cup size sensors
// (have lots of noise, and are difficult to read)
// read noisy analog 0 when cup missing, noisy analog 3 when cup present
// sensors are at different heights, and the highest triggered sensor determines the size of cup
#define LARGE_SENSOR                A6
#define MEDIUM_SENSOR               A7
#define SMALL_SENSOR                A8

// ice maker
#define ICE_MAKER                   22 // low output turns ice maker on
#define ICE_LEVEL                   A3 // should be INPUT_PULLUP, reads high when ice maker is full
#define ICE_MOTOR                   23 // low output dispenses ice


// blender
#define ZERO_CROSS                  21 // (interrupt 2 on the mega) raises short logic pulse when zero cross happens
#define BLENDER_MOTOR               39 // TRIAC fires after delay, shuts off automatically when crosses zero
#define BLENDER_TACHOMETER          19 // analog input with noise. needs lots of averaging to read correctly
#define THUMPER_MOTOR               41 // HIGH output shakes blender
#define DUMP_SOLENOID               50 // HIGH output pulls solenoids in to hold fluids in the blender jar

//blender PID control from tachometer
#define BLENDER_P 0.03
#define BLENDER_I 0.1
#define BLENDER_D 0.1

#define SPEED_1     4080
#define SPEED_2     6480
#define SPEED_3     8400
#define SPEED_4    11040
#define SPEED_5    12960
#define SPEED_6    13920
#define SPEED_7    15120
#define SPEED_8    18000
#define SPEED_9    20880
#define SPEED_10   29400


/**** Added by CDM ***/
#ifdef NEW_SPEED_CONTROL
  // Fire angle (in uS) needs to be in the range of 4,167 and 8,333 (full 60Hz cycle is 16,667 uS, so half is 8,333 uS, 1/4 is 4,167 uS)
  // Fire angle is inversely proportional to the rpm. These values will eventually need to be replaced once the PID loop is closed with the speed sensor
  #define SPEED_1_FA    7800
  #define SPEED_2_FA    7400
  #define SPEED_3_FA    7200
  #define SPEED_4_FA    6800
  #define SPEED_5_FA    6400
  #define SPEED_6_FA    6000
  #define SPEED_7_FA    5600
  #define SPEED_8_FA    5200
  #define SPEED_9_FA    4800
  #define SPEED_10_FA   4200

  #define HALF_CYCLE_MICROSEC_60HZ      8333
  #define MOTOR_FIRE_DURATION_MICROSEC  200
#endif
/*****************************************/

#define REV_1 14706
#define REV_2 9259
#define REV_3 7143
#define REV_4 5435
#define REV_5 4630
#define REV_6 4310
#define REV_7 3968
#define REV_8 3333
#define REV_9 2874
#define REV_10 2041

// sanitizing system
#define FV1                         25 // LOW output pressurizes sanitizer block
#define FV2                         26 // LOW output pressurizes sanitizer block
#define SNV1                        52
#define SNV2                        40
#define SNV3                        32
#define SNV4                        34
#define SNV5                        36
#define BSB                         48

#define SANITIZER_FLOW              A10
#define SANITIZER_LEVEL             A11
#define SANITIZER_TEMP              A9

#define SANITIZER_MOTOR_ON          12 // digital out
#define SANITIZER_PUMP_DIRECTION    30 // digital out
#define _12CSDA                     20 // untested sensor

// Flavor pump motor and clutches
#define PUMP_CUBE_MOTOR             24 // HIGH turns on motor
#define PUMP_1                      49 // HIGH engages clutches
#define PUMP_2                      47
#define PUMP_3                      44
#define PUMP_4                      45
#define PUMP_5                      43
#define PUMP_6                      53
#define PUMP_7                      51
#define PUMP_8                      42
#define DRINK_H2O                   27 // LOW dispenses clean water into blender

// calibration values: This is the time in milliseconds required to dispense one Milliliter of fluid from each output. (grams for ice)
#define C1 60 //39
#define C2 60 //28
#define C3 60 //50
#define C4 60 //47
#define C5 60 //49
#define C6 60 //52
#define C7 60 //36
#define C8 60 //40
#define CW 52 //47
#define CICE 20

// currently unused inputs and outputs
#define HPR_LID                     A1
#define SERVICE_DOOR                A2
#define CALIBRATION_1               A13 // sensors detect fluid touching contacts inside blender jar
#define CALIBRATION_2               A12



#define WATCHDOG_INTERVAL           1000

// not well defined behavior

// analog readings to detect from the cup sensor
#define CTRIES           50 // read this many times
#define CTHRESH         100 // total must be above this


// machine states:
#define STARTUP 0
#define READY 1
#define POST_SANITIZE 2
#define CUP_IN 3
#define CLEANING 4
#define RINSING 5
int machine_status;



//blender motor control values
#define ON 1
#define OFF 0
int motor_power = OFF;
//Motor states
#define WAITING 0
#define FIRING 1
#define DONE 2
int motor_status = WAITING;
#define MSPEED 20
#define MOFFSET 8000 // offset of 6300 barely turns the motor
// timing
#define FIRE_INTERVAL 200
unsigned long zero_cross_time; // set when zero cross interrupt fires (120 times per second)
unsigned long offset = 0; // microseconds to wait before firing motor (0 to 8333)
unsigned long start = 0; // set to zero when blender is turned on, used to check to see if blender should be turned off
unsigned long now = 0; // used to store micros at the start of interrupt script
unsigned long blend_time = 0; // used to store current total blend time

unsigned long tach_start = 0;
unsigned long tach_end = 0;
unsigned long tach_counter = 0;
unsigned long total_tach = 0;
unsigned long rev_time = 60000000;

float tach_speed = 0;
int rpm = 0;
/***** Added by CDM ********/
#ifdef NEW_SPEED_CONTROL
  int FireAngle = 0;
#endif
/**************************/
#define DEADZONE 100
#define OFFSET_CHANGE 100
#define TRUE 1


// 

// define all variables here
//unsigned long current_time;
//unsigned long last_action;
//unsigned long action_delay = 10000;

//unsigned long lastOnTime = 0;
//unsigned long setTime = 0;


String inputString = "";         // a string to hold incoming serial command data
boolean stringComplete = false;  // whether the string is complete
char x = 'a';

// need a few variables that describe the state of the machine.
// should check syrup levels, and make sure the machine is clean each time it powers on.
// should keep track of whether or not it is clean, as well as have an emergency variable that is checked periodically that exits functions and stops the machine.

// variables show current amounts in the cup...
// if the machine is clean or not,
// ensuring that the machine has been rinsed of cleaner before making smoothie

#define ICE_INTERVAL 30000
unsigned long ice_time = 0;

void setup() {
  delay(3000);
  machine_status = STARTUP;
    // initialize serial:
    Serial.begin(115200);
    //Serial.setTimeout(10);
    
    // setup the pins
    
    // set the ice motor pin to an output, and set it low.
    digitalWrite(ICE_MOTOR,HIGH);
    pinMode(ICE_MOTOR, OUTPUT);

    digitalWrite(ICE_MAKER,HIGH);
    pinMode(ICE_MAKER,OUTPUT);
    pinMode(ICE_LEVEL,INPUT_PULLUP);
    
    digitalWrite(CUP_HOLDER_DIRECTION, LOW);
    pinMode(CUP_HOLDER_DIRECTION, OUTPUT);
    
    digitalWrite(CUP_HOLDER_MOTOR_ON, LOW);
    pinMode(CUP_HOLDER_MOTOR_ON, OUTPUT);
    
    pinMode(DOOR_LEFT, INPUT);
    pinMode(DOOR_RIGHT, INPUT);
    digitalWrite(DOOR_LEFT, HIGH);
    digitalWrite(DOOR_RIGHT,HIGH);
    
    digitalWrite(BLENDER_MOTOR,HIGH);
    pinMode(BLENDER_MOTOR, OUTPUT);
    pinMode(ZERO_CROSS,INPUT_PULLUP);
    digitalWrite(ZERO_CROSS,HIGH);

    pinMode(BLENDER_TACHOMETER,INPUT_PULLUP);
    
    digitalWrite(THUMPER_MOTOR, HIGH);
    pinMode(THUMPER_MOTOR, OUTPUT);
    
    pinMode(DUMP_SOLENOID,OUTPUT);
    digitalWrite(DUMP_SOLENOID,LOW);
    
    pinMode(PUMP_CUBE_MOTOR,OUTPUT);
    digitalWrite(PUMP_CUBE_MOTOR,LOW);
    
    pinMode(PUMP_1, OUTPUT);
    digitalWrite(PUMP_1,LOW);
    pinMode(PUMP_2, OUTPUT);
    digitalWrite(PUMP_2,LOW);
    pinMode(PUMP_3, OUTPUT);
    digitalWrite(PUMP_3,LOW);
    pinMode(PUMP_4, OUTPUT);
    digitalWrite(PUMP_4,LOW);
    pinMode(PUMP_5, OUTPUT);
    digitalWrite(PUMP_5,LOW);
    pinMode(PUMP_6, OUTPUT);
    digitalWrite(PUMP_6,LOW);
    pinMode(PUMP_7, OUTPUT);
    digitalWrite(PUMP_7,LOW);
    pinMode(PUMP_8, OUTPUT);
    digitalWrite(PUMP_8,LOW);
    
    
    pinMode(SNV1, OUTPUT);
    digitalWrite(SNV1,LOW);
    pinMode(SNV2, OUTPUT);
    digitalWrite(SNV2,LOW);
    pinMode(SNV3, OUTPUT);
    digitalWrite(SNV3,LOW);
    pinMode(SNV4, OUTPUT);
    digitalWrite(SNV4,LOW);
    
    digitalWrite(FV1,HIGH);
    pinMode(FV1,OUTPUT);
    digitalWrite(FV2,HIGH);
    pinMode(FV2,OUTPUT);
    
    digitalWrite(DRINK_H2O,HIGH);
    pinMode(DRINK_H2O,OUTPUT);
    
    // set the sanitizer flow pin to an input, and activate the internal pull-up resistor to avoid fluctuations when left floating.
    //pinMode(SANITIZER_FLOW, INPUT_PULLUP);

    // set up a watchdog to check things at some set interval
    //Timer1.initialize(WATCHDOG_INTERVAL);
    //Timer1.attachInterrupt(MotorPulse);
    
    // setup an interrupt to activate when the zero crossing pin rises
    //attachInterrupt(ZERO_CROSS,zero_crossing,RISING);


    // run through the initialization routine to make sure we are ready to make a smoothie
    // rinse
    // check ice
    // etc
    
    // interrupts:
   // int.0 = pin 2
   // int.1 = pin 3
   // int.2 = pin 21
   // int.3 = pin 20
   // int.4 = pin 19
   // int.5 = pin 18
    //Timer3.initialize(FIRE_INTERVAL);
    //Timer3.attachInterrupt(fire_motor);
    
    attachInterrupt(INTERRUPT_PIN_21, zero_crossing, RISING); // actual signal on pin 21
    attachInterrupt(INTERRUPT_PIN_19, count_tach, FALLING); // actual signal on pin 19
    RotateOut();
    if(DEBUG)Serial.println("done with setup");

}

// this infinite loop will execute all the code
void loop() {
    // get current time
    //current_time = millis();
    
    // run the ice maker
    //if(DEBUG)Serial.println("checking ice level");
    if(millis()-ice_time > ICE_INTERVAL){
      if(digitalRead(ICE_LEVEL)==HIGH){
          digitalWrite(ICE_MAKER,HIGH);
          //if(DEBUG)Serial.println("Ice maker off");
      }else{
      digitalWrite(ICE_MAKER,LOW);
          //if(DEBUG)Serial.println("Ice maker on");

      }
      ice_time = millis();
    }
    //delay(30000); // just check the ice machine every 30 seconds
    //check the tachometer
    
    
    
    
    //Serial.print('A');   // send a capital A
    //delay(300);
    // check current time against next execution, and act
    // because these are unsigned values, this comparison should be valid even when current_time has rolled over, and last_action has not.
    //if (current_time - last_action > action_delay){
        //last_action = current_time;
        //get_next_action_delay(&action_delay)
    //}


    // Continually monitor the desired speed
    
    
}
    
 /*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
 
// this method of reading serial checks the first character, and calls different functions based on the character read

// serial communication should reject improperly formed recipies
// build up a list of numbered items
// the easiest way to confirm error free communication is to just repeat each line back, to confirm it
// if the sending end recieves the identical string back, then the next string is sent.
// if the sending end does not, re-send the string.
// on the recieving end, echo each line recieved, but look for the item number.
// if the next string recieved starts with the same number, then replace that line,
// if the next string starts with the next number, you can be sure you recieved the last line correctly
//
// some other logic is required... such as identifying missed lines, asking for a full restart, or confirming the last message was recieved.

// it's ok to be verbose because the available bandwidth is much more than needed.


// my guess is that there should be a list of actions, and the timing of each one.
// something like:
// 1 NEW
// 2 MOTOR ON 0
// 3 PUMP1 ON 300
// 4 PUMP1 OFF 600
// 5 RUN

// X EMERGENCY STOP
// Y END OF RECIPIE
// Z CLEAR ALL


void serialEvent() {
  //Serial.println("got some serial");
    while (Serial.available()) {
        x = Serial.read();
        // switch here, and evaluate each type of sentence.
        switch (x){
            case 'T':
                // take the cup to the back
                //Serial.println("got a T");
                TakeCup();
                break;
            case 'M':
                // Make a smoothie... 
                MakeSmoothie();
                break;
            case 'C':
                // Clean the machine
                break;
            case 'D':
                // Do a rinse cycle
                StandardRinseCycle();
                break;
            case 'S':
                // Status report
                break;
            case 'X':
                // Cancel smoothie
                // return cup to user
                // rinse stuff if required
                // send ready for service
            case 'R':
                // Reset the machine
                // set status back to beginning
                ResetMachine();
                break;
            
            case 'I':
                Initialize();
                // Initialize the machine
                // send "Ready for service\n" or "Error [explanation]"
                break;
            default:
                // send invalid sentence error
                break;
        }
    }
}
void Initialize(){
  RotateOut();
  machine_status = READY;
  noInterrupts();
  Serial.print("Ready for service\n");
  interrupts();
}
void StartCleaning()
{
  // set status to cleaning
  // run cleaning cycle
  // rinse machine
}

void TakeCup(){
  if (machine_status != READY){
    noInterrupts();
    Serial.print("Error machine not ready\n");
    interrupts();
    return;
  }
  // do pin states in the right order with the right timing to take the cup and check it's size
  // needs to return the cup size over serial
  // if there is a problem, return an error
  
  // check status for "READY"
    // if not ready, send error, and exit
  // perform cup in routine:
  
  // rotate cup in
  //Serial.println("taking cup");
  RotateIn();
  //Serial.println("should have cup");
  
  int lnum=0;
  int mnum=0;
  int snum=0;
  
  for(int x=0; x < CTRIES; x++){
    lnum+=analogRead(LARGE_SENSOR);
    mnum+=analogRead(MEDIUM_SENSOR);
    snum+=analogRead(SMALL_SENSOR);
  }
  
  if(lnum>CTHRESH){
    noInterrupts();
    Serial.print("Took large cup\n");
    interrupts();
  }
  else if(mnum>CTHRESH){
    noInterrupts();
    Serial.print("Took medium cup\n");
    interrupts();
  }
  else if(snum>CTHRESH){
    noInterrupts();
    Serial.print("Took small cup\n");
    interrupts();
  }
  else{
    noInterrupts();
    Serial.print("Error no cup: ");
    Serial.print(lnum);
    Serial.print(" ");
    Serial.print(mnum);
    Serial.print(" ");
    Serial.print(snum);
    Serial.print("\n");
    interrupts();
    RotateOut();
    machine_status = READY;
    noInterrupts();
    Serial.print("Ready for service\n");
    interrupts();
    return;
  }
  machine_status = CUP_IN;
    
  // lock door
  // read cup size
  // send cup size
  //reply: "Took [s,m,l] cup\n" (small,medium,large,no cup, error, inappropriate time to take cup)
  // set status to CUP_IN
  // if no cup, or error, just return cup tray... and send error
  //"Error no cup"
  //"Error machine not ready to take cup because it's really dirty... too dirty to make a smoothie"
  //"Error not ready\n"
}

void RotateIn(){
  int consec=0;
  //Serial.println("rotate in function entered");
  //if(digitalRead(DOOR_LEFT)==LOW)
  //Serial.println("DOOR_LEFT is LOW");
  //if(digitalRead(DOOR_LEFT)==HIGH)
  //Serial.println("DOOR_LEFT is HIGH");
  digitalWrite(CUP_HOLDER_DIRECTION,LOW);
  analogWrite(CUP_HOLDER_MOTOR_ON,100);
  while(TRUE){
    delay(20);
    if (digitalRead(DOOR_LEFT)==LOW){
      //if(DEBUG)Serial.println("door left sensor got LOW");
      consec +=1;
      if(consec > 10)
      break;
    }
    else{
      //if(DEBUG)Serial.println("door left sensor got HIGH");
      consec=0;
    }
  }
  machine_status = CUP_IN;
  digitalWrite(CUP_HOLDER_MOTOR_ON,LOW);
  delay(4000);

}
void RotateOut(){
  
  int consec=0;
  //Serial.println("rotate out function entered");
  digitalWrite(CUP_HOLDER_DIRECTION,HIGH);
  analogWrite(CUP_HOLDER_MOTOR_ON,100);
  //delay(2000);
  
  while(TRUE){
    delay(20);
    if (digitalRead(DOOR_RIGHT)==LOW){
      
    //if(DEBUG)Serial.println("door right sensor got LOW");
      consec +=1;
      if(consec > 10)
      break;
    }
    else{
      //if(DEBUG)Serial.println("door right sensor got HIGH");
      consec=0;
    }
  }
  digitalWrite(CUP_HOLDER_MOTOR_ON,LOW);
}

void MakeSmoothie(){
  // get a smoothie sentence, dispense things, and return the cup
  if (machine_status != CUP_IN){
    noInterrupts();
    Serial.print("Error no cup\n");
    interrupts();
    return;
  }
  
  // read all ingredient levels
  int f1= floor(Serial.parseInt() * MULT);
  int f2= floor(Serial.parseInt() * MULT);
  int f3= floor(Serial.parseInt() * MULT);
  int f4= floor(Serial.parseInt() * MULT);
  int f5= floor(Serial.parseInt() * MULT);
  int f6= floor(Serial.parseInt() * MULT);
  int f7= floor(Serial.parseInt() * MULT);
  int f8= floor(Serial.parseInt() * MULT);
  int water= floor(Serial.parseInt() * MULT);
  int ice= floor(Serial.parseInt() * MULT);
  //int cycle= Serial.parseInt();
  
  
  //int f1 = 150;
  //int f2 = 0;
  //int f3 = 0;
  //int f4 = 0;
  //int f5 = 0;
  //int f6 = 0;
  //int f7 = 0;
  //int f8 = 0;
  //int water = 0;
  //int ice = 250;
  int cycle = 3;
  
  //close dump chute
  digitalWrite(DUMP_SOLENOID,HIGH); 
  // put a little water in
  //digitalWrite(DRINK_H2O,LOW);
  //delay(100);
  //digitalWrite(DRINK_H2O,HIGH);
  
  // dispense water till level up?
  noInterrupts();
  Serial.print("Dispensing water\n");
  interrupts();
  digitalWrite(DRINK_H2O,LOW);
    delay(CW * water); // from calibration
  digitalWrite(DRINK_H2O,HIGH);
 
  // start dispense motor
  digitalWrite(PUMP_CUBE_MOTOR,HIGH);
  delay(1000);
  
  if (f1 > 0){dispense(PUMP_1,f1);}
  if (f2 > 0){dispense(PUMP_2,f2);}
  if (f3 > 0){dispense(PUMP_3,f3);}
  if (f4 > 0){dispense(PUMP_4,f4);}
  if (f5 > 0){dispense(PUMP_5,f5);}
  if (f6 > 0){dispense(PUMP_6,f6);}
  if (f7 > 0){dispense(PUMP_7,f7);}
  if (f8 > 0){dispense(PUMP_8,f8);}
  delay(500);
  digitalWrite(PUMP_CUBE_MOTOR,LOW);

  
  // dispense ice based on size?
  noInterrupts();
  Serial.print("Dispensing ice\n");
  interrupts();
  digitalWrite(ICE_MOTOR,LOW);
  delay(ice * CICE);// 1 second of ice?
  digitalWrite(ICE_MOTOR,HIGH);  
  
  
  
  // send "Blending\n"
  noInterrupts();
  Serial.print("Blending\n");
  interrupts();
  delay(2000);
  switch (cycle){
    case 1:
      cycle1();
      break;
    case 2:
      cycle2();
      break;
    case 3:
      cycle3();
      break;
    case 4:
      cycle4();
      break;
    case 5:
      cycle5();
      break;
    case 6:
      cycle6();
      break;
  }
  // run blender routine  // blending set amount of time?
  delay(2000);
  digitalWrite(DUMP_SOLENOID,LOW);
  digitalWrite(THUMPER_MOTOR,LOW);
  delay(2000);
  blend(2,2000);
  delay(12000);
  // dump smoothie into cup
  // send "Done\n"
  delay(4000);
  digitalWrite(THUMPER_MOTOR,HIGH);
  noInterrupts();
  Serial.print("Done\n");
  interrupts();
  //delay(2000);
  
  // final dump rinse
  /*
  digitalWrite(DUMP_SOLENOID,HIGH);
  digitalWrite(DRINK_H2O,LOW);
  delay(CW * 15); // from calibration
  digitalWrite(DRINK_H2O,HIGH);
  blend(2,2000);
  digitalWrite(DUMP_SOLENOID,LOW);
  */
  //delay(2000);
  // return cup
  RotateOut();
  // post smoothie rinse of blend cup?
  StandardRinseCycle();
  // set status to ready
  // send "Ready for service\n"
  machine_status = READY;
  noInterrupts();
  Serial.print("Ready for service\n");
  interrupts();
  
}

void cycle1(){
  blend(3,5000);
  blend(7,15000);
}
void cycle2(){
  blend(3,3000);
  blend(5,5000);
  blend(7,15000);
}
void cycle3(){
  blend(3,3000);
  blend(5,7000);
  blend(7,15000);
}
void cycle4(){
  blend(3,3000);
  blend(7,10000);
  blend(8,10000);
}
void cycle5(){
  blend(3,3000);
  blend(7,10);
  blend(7,10);
}
void cycle6(){
  blend(3,3000);
  blend(7,10000);
  blend(8,15000);
}

void dispense(int pin, int amount){
  // dispense each flavor in turn...
  // check for correct operational status
  
  //send serial to update status
  noInterrupts();
  Serial.print("Dispensing ");
  interrupts();
  Flavorname(pin);
  digitalWrite(pin,HIGH);

  switch(pin){
    case PUMP_1:
      delay( C1 * amount);
      break;
    case PUMP_2:
      delay( C2 * amount);
      break;
    case PUMP_3:
      delay( C3 * amount);
      break;
    case PUMP_4:
      delay( C4 * amount);
      break;
    case PUMP_5:
      delay( C5 * amount);
      break;
    case PUMP_6:
      delay( C6 * amount);
      break;
    case PUMP_7:
      delay( C7 * amount);
      break;
    case PUMP_8:
      delay( C8 * amount);
      break;
  }
  digitalWrite(pin,LOW);

  // if dispense motor is off
    // turn dispense motor on
  // wait for motor to spin up
  // engage clutch for flavor
  // wait till amount is dispensed
  // disengage clutch
  // return 
 
}

void Flavorname(int pin){
  // dirty hard coded flavor names
  noInterrupts();
  switch(pin){
    case PUMP_1:
      Serial.print("1\n");
      break;
    case PUMP_2:
      Serial.print("2\n");
      break;
    case PUMP_3:
      Serial.print("3\n");
      break;
    case PUMP_4:
      Serial.print("4\n");
      break;
    case PUMP_5:
      Serial.print("5\n");
      break;
    case PUMP_6:
      Serial.print("6\n");
      break;
    case PUMP_7:
      Serial.print("7\n");
      break;
    case PUMP_8:
      Serial.print("8\n");
      break;
  }
  interrupts();
}

void ResetMachine(){
  if(machine_status == CUP_IN){
    RotateOut();
    machine_status = READY;
    noInterrupts();
    Serial.print("Ready for service\n");
    interrupts();
    return;
  }
  else{
    noInterrupts();
    Serial.print("Error status not 'CUP IN'\n");
    interrupts();
  }
}
void blend(int motorspeed, unsigned long motortime)
{
  
  // could have blender ramp up and ramp down here
  // start offset, end offset, miliseconds to do it in.
  // the firing script could change the offset each cycle by some amount to move toward the desired offset
  if(DEBUG){
    Serial.print("blending speed ");
    Serial.println(motorspeed);
  }
  switch(motorspeed){
    case 1:
      rpm = SPEED_1;
#ifdef NEW_SPEED_CONTROL
      FireAngle = SPEED_1_FA;
#endif
      break;
    case 2:
      rpm = SPEED_2;
#ifdef NEW_SPEED_CONTROL
      FireAngle = SPEED_2_FA;
#endif
      break;
    case 3:
      rpm = SPEED_3;
#ifdef NEW_SPEED_CONTROL
      FireAngle = SPEED_3_FA;
#endif
      break;
    case 4:
      rpm = SPEED_4;
#ifdef NEW_SPEED_CONTROL
      FireAngle = SPEED_4_FA;
#endif
      break;
    case 5:
      rpm = SPEED_5;
#ifdef NEW_SPEED_CONTROL
      FireAngle = SPEED_5_FA;
#endif
      break;
    case 6:
      rpm = SPEED_6;
#ifdef NEW_SPEED_CONTROL
      FireAngle = SPEED_6_FA;
#endif
      break;
    case 7:
      rpm = SPEED_7;
#ifdef NEW_SPEED_CONTROL
      FireAngle = SPEED_7_FA;
#endif
      break;
    case 8:
      rpm = SPEED_8;
#ifdef NEW_SPEED_CONTROL
      FireAngle = SPEED_8_FA;
#endif
      break;
    case 9:
      rpm = SPEED_9;
#ifdef NEW_SPEED_CONTROL
      FireAngle = SPEED_9_FA;
#endif
      break;
    case 10:
      rpm = SPEED_10;
#ifdef NEW_SPEED_CONTROL
      FireAngle = SPEED_10_FA;
#endif
      break;

#ifdef NEW_SPEED_CONTROL
    default:
      // Don't continue if we got an invalid speed
      return;
      break;
#endif   
  }
  
  // set initial offsets for each speed... could be better than what is here
  motorspeed = constrain(motorspeed,0,MSPEED);
  // set globals to turn motor on
  blend_time = motortime * 1000;
  start = micros();
  offset = map(motorspeed, 0, MSPEED, MOFFSET,0); // 6300 is the max offset that allows motor to turn... longer than this and nothing happens. Offset of zero is full power.
  tach_counter = 0;
  tach_start=start;
  motor_power = ON;
  
  
  while(motor_power == ON){
    //delay(1);
#ifdef NEW_SPEED_CONTROL
#else
    fire_motor();
#endif
    // read tachometer to adjust offset
  }
}

void zero_crossing(){
  if(motor_power == OFF)return;
  
#ifdef NEW_SPEED_CONTROL
  /***************************************************************** 
   *  This a complete blocking routine - not best practice.
   *  Best practice is to use a timer with interrupts.
   *  Use 16-bit timer @ 1MHz (T = 1uS).
   *  When zero-cross interrupt occurs, set the OCRx (x = the timer
   *  number you're using) output compare register to the
   *  desired fire angle, clear the timer, enable the timer
   *  and interrupt. 
   *  When the timer value reaches the value in
   *  the OCRx register it will fire an interrupt. When that 
   *  interrupt occurs pull the blender motor pin low to activate
   *  the motor. Clear the timer and set the OCRx register to 
   *  200 (200uS - duration of the pulse). Again the timer will interrupt 
   *  after 200uS. Pull the blender motor pin high, clear the timer 
   *  and set the OCRx register to 8333 (representing 8333uS); 
   *  this will trigger the motor again on the negative side 
   *  of the sine wave. When the interrupt occurs clear the 
   *  timer and set the OCRx register to 200 again. When this 
   *  final interrupt occurs clear the timer, disable the timer and
   *  the interrupt flag. This process will repeat at every zero-
   *  crossing.
   *  
   *  See the following web sites to learn more about timers and interrupts:
   *  https://playground.arduino.cc/Code/Timer1
   *  http://www.instructables.com/id/Arduino-Timer-Interrupts/
   *  https://arduino-info.wikispaces.com/Timers-Arduino
   *  
   *  Or search "Arduino Timers & Interrupts".
   *  
   *  The code below will run the blender in open-loop (no speed feedback).
   */  
    delayMicroseconds(FireAngle);
    digitalWrite(BLENDER_MOTOR,LOW);
    delayMicroseconds(MOTOR_FIRE_DURATION_MICROSEC);
    digitalWrite(BLENDER_MOTOR, HIGH);
    delayMicroseconds(HALF_CYCLE_MICROSEC_60HZ);
    digitalWrite(BLENDER_MOTOR,LOW);
    delayMicroseconds(MOTOR_FIRE_DURATION_MICROSEC);
    digitalWrite(BLENDER_MOTOR, HIGH);
#endif
  zero_cross_time = micros();
  
  if(zero_cross_time - start > blend_time){
    motor_power = OFF;
    return;
  }
  //pid for new offset
  offset += floor(BLENDER_P * ((60000000.0/rev_time) - rpm));
  offset = constrain(offset,0,MOFFSET);
  motor_status = WAITING;
  if(DEBUG)Serial.print((60000000.0/rev_time));
  if(DEBUG)Serial.print(" , ");
  if(DEBUG)Serial.println(offset);
  
}

// motor_status can only be waiting or firing
void fire_motor(){
 if(motor_power == OFF)return;
  if(motor_status == FIRING)return;
  //motor_power is ON, and motor_status is WAITING

  // pulse motor if required
  if(micros() - zero_cross_time > offset){
    PORTG = PORTG & B11111011;
//     if(DEBUG)Serial.println("firing");

    delayMicroseconds(20);
    PORTG = PORTG | B00000100;
    motor_status = FIRING;
  }
}

void count_tach(){
  //max 3430 counts per second with 7 blades... maybe count every 7th blade?
  // counting from the same blade every time might give a better resolution to the speed value.
  tach_counter ++;
  if(tach_counter == 6){
    tach_counter = 0;
    rev_time = micros()-tach_start;
    tach_start = micros();
  }
  //   if(DEBUG)Serial.println("firing");
  // need to get timing of when each pulse happens, then average them
  // timing of the pulses needs to be very precise
}

//
//    Routines from Blendtec flowchart
//


//Drink Selection Pushed
// 
// Switch on operational mode
// 
// case Ready:
//    clear messages
//    Unlock Alcove Door, Rotate to cup in, lock
//    if cup not Present
//      NoCupPresentRoutine();
//    if incorrect cup size
//      WrongCupSizeRoutine();
//    if cup already filled
//      FilledCupRoutine();
//    SetDrinkSizeAndDispenseDrink();
//    break;;

//case PostSanitize:
//    initiate Pre-rinse(Standard Rinse Cycle)
//    SetNextSanitizeTime();
//    Set Cup Shelf Rinse Time
//    CSR = T + 15 minutes
//    Set Scrub Clean Time
//    SC = T + SCI
//    Set System to Ready

//case Default:
//    if rinsing
//      Serial.Println("Rinsing");
//      break;;
//    if scrub/clean cycle
//      Serial.Println("Scrub/Clean");
//      break;;
//    if sanitizing
//      Serial.Println("Sanitizing");
//      break;;

void SetNextSanitizeTime(){
  //calculate Latest Allowed Time (LAT)
  //  if LAT During Lockout
  //    Set Next Sanitize Cycle Time SSCT=SSLT# - 20 Minutes
  //  Set Next Sanitize Cycle Time NSCT=LAT-20 minutes
  //  if LAT during lockout
  //    Set Next Sanitize Cycle Time NSCT=ESLT#
} 

void NoCupPresentRoutine(){
  // Sound Customer alert
  // Serial.println("Place Cup");
  // Unlock Alcove Door
  // Rotate To cup out
  // Lock
}
void WrongCupSizeRoutine(){
  // sound customer alert
  // Serial.println("Wrong Cup Size");
  // Unlock alcove
  // rotate to cup out
  // lock alcove
}

void FilledCupRoutine(){
  //sound customer alert
  //Serial.println("Filled Cup");
  // unlock alcove
  // rotate to cup out
  // lock alcove
}

void SetDrinkSizeAndDispenseDrink(){
  // drink selection mode
  // set dispense quantity
  // dispense and mix drink
  // activate thumper, open dispense chute
  // close dispense chute, deactivate thumper
  // unlock alcove
  // rotate to cup out
  // lock
  // StandardRinseCycle();
  // Serial.println("Drink Ready");
  // sound customer alert
  // set system to ready
  // UpdateTimeAndFrequencyMonitors();
  // clear "cup shelf rinse" and "scrub/clean" flags
  // ClearFlags();
  // set system to ready mode
}

void UpdateTimeAndFrequencyMonitors(){
  //Update Time since Last Cycle (TSLC)
  // TSLC = (T-TPC)
  // Update Time of previous cycle
  // TPC = T
  // Use TSLC to Update Cycle Frequency (CF3)
}

void ClearFlags(){
  // Check and clear 'Cup Shelf Rinse Flag' (CSRF)
  // if CSRF == "yes"
  //    CSRF=no
  // Check and clear 'Scrub/Clean Flag' (SCF)
  // if CSF == "yes"
  //    CSF=no
}

void StandardStartup(){
  // attendant triggers on switch
  // set system to "start up" mode
  // if mode == "post sanitize"
  //    Clear any messages
  //    set system to ready
  //    return
  // if power down less than 1 hour and not past due for sanitize
  //    clear any messages
  //    set system to ready
  //    return
  //set system to "Reset Required"
  //Sanitize();
  // attendant triggers reset button
  // Clear startup & reset required modes
  // if post sanitize mode
  //     go to "b"
  // Sanitize();
  // go to "b"
  
}
void StandardShutdown(){
  // attendant triggersh "shutdown"
  // if not post sanitize mode
  //  Sanitize();
  // set system to "off"
}

void StandardRinseCycle()
{
  //Open Flow Control Valves FV1 & FV2
  digitalWrite(FV1,LOW);
  digitalWrite(FV2,LOW);
  StandardMainChamberRinse();
  //Open Dispense Chute Valve
  //Standard Main Chamber Rinse
  // maybe go to ready...
  // Close Dispense Chute Valve
  digitalWrite(DUMP_SOLENOID,HIGH);
  StandardAlcoveAreaRinseCycle();
  FoodZoneFinalRinse();
  // Close Flow Control Valves FV1 & FV2
  digitalWrite(FV1,HIGH);
  digitalWrite(FV2,HIGH);
  // Set System to Ready mode
}

void Sanitize(){
  //Serial.println("Sanitizing Cycle");
  //ExtendedScrubRinse();
  //ChamberAlcoveCleaningCycle();
  //ChamberAlcoveCleaningRinseCycle();
  //BlendingSystemSanitizeCycle();
  //CleanerSanitizerConcentrateReserveLevelChecks();
  //
}

void ExtendedScrubRinse(){
  //Open Flow Control Valves FV1 & FV2
  //BasicIceScrub();
  //ExtendedMainChamberRinse();
  //Close Dispense Chute Valve
  //ExtendedAlcoveAreaRinseCycle();
  //FoodZoneFinalRinse();
}

void cycle_snv(){
  digitalWrite(SNV3,HIGH);
  delay(1000);
  digitalWrite(SNV3,LOW);
  
  digitalWrite(SNV1,HIGH);
  delay(1000);
  digitalWrite(SNV1,LOW);
  
  digitalWrite(SNV2,HIGH);
  delay(1000);
  digitalWrite(SNV2,LOW);
}
void blend_snv(){
  
  digitalWrite(SNV3,HIGH);
  blend(2,1000);
  digitalWrite(SNV3,LOW);
  
  digitalWrite(SNV1,HIGH);
  blend(2,1000);
  digitalWrite(SNV1,LOW);
  
  digitalWrite(SNV2,HIGH);
  blend(2,1000);
  digitalWrite(SNV2,LOW);
  
}

void StandardMainChamberRinse(){
    //Cycle through Valves SNV3, SNV1 & SNV2 @ 1 Second Intervals
  cycle_snv();
  
  //if insufficient water flow
  //    Water Flow/Pressure Check
  //    StandardMainChamberRinse();
  //    return;
  //else if Water Temperature < 90F
  //    StandardMainChamberRinse();
  //    return;
  //
  //Close Dispense Chute Valve
  digitalWrite(DUMP_SOLENOID,HIGH);
  cycle_snv();
  //Start Rinse Cycle Timer (12 Seconds)
  //After 2 Seconds, Run Blender for 3 Seconds
  blend_snv();
  //After 6 Seconds, Open Dispense Chute Valve
  digitalWrite(DUMP_SOLENOID,LOW);
  cycle_snv();
  cycle_snv();
  //Close all Chamber Spray Control Valves
  //return
}
void StandardAlcoveAreaRinseCycle(){
  //Open Valve SNV4 for 3 Seconds
  digitalWrite(SNV4, HIGH);
  delay(3000);
  //Close Valve SNV4
  digitalWrite(SNV4, LOW);
  //Open Dispense Chute Valve
  digitalWrite(DUMP_SOLENOID,LOW);
}
void FoodZoneFinalRinse(){
  cycle_snv();
}
