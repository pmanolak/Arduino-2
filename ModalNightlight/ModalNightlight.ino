/*
TODO: 

  change ints to bytes
  



*/
//operation modes
const int TRANSITION_MODE  = 0; 
const int RED_MODE         = 1; 
const int GREEN_MODE       = 2;
const int BLUE_MODE        = 3;
const int YELLOW_MODE      = 4;
const int OFF_MODE         = 5;

// pin enums
const int redPin = 11;
const int greenPin = 10;
const int bluePin = 9;
const int buttonPin = 4;

// operational parameter enums
const int FULLY_OFF = 255;   // RGB LED is common cathode
const int FULLY_ON = 0;      // so we need reverse logic

// led transition status
const int OFF = 0;
const int RAMPING_UP = 1;
const int HOLDING = 2;
const int RAMPING_DOWN = 3;

int currentMode = TRANSITION_MODE; // default

// init values and states for transition mode

// pin indexes
const int RED = 0;
const int GREEN = 1;
const int BLUE = 2;

// steady state outputs
int outputValuesTable[6][3] = 
{ 
  {FULLY_OFF,FULLY_OFF,FULLY_OFF},     //TRANSITION MODE
  {FULLY_ON, FULLY_OFF,FULLY_OFF},     //RED
  {FULLY_OFF, FULLY_ON, FULLY_OFF},    //GREEN
  {FULLY_OFF, FULLY_OFF, FULLY_ON},    // BLUE
  {FULLY_ON, FULLY_ON, FULLY_OFF},     // YELLOW
  {FULLY_OFF, FULLY_OFF, FULLY_OFF}    // OFF
};

// vars for transition mode
const int transitionStateTable[7][3] = 
{
  {RAMPING_UP, OFF, OFF},             // to RED
  {HOLDING, OFF, RAMPING_UP},         // to PURPLE
  {RAMPING_DOWN, OFF, HOLDING},       // TO BLUE
  {OFF, RAMPING_UP, HOLDING},         // TO AQUA
  {OFF, HOLDING, RAMPING_DOWN},       // TO GREEN
  {RAMPING_UP, HOLDING, OFF},         // TO YELLOW
  {RAMPING_DOWN, RAMPING_DOWN, OFF}   // TO OFF
  
};
unsigned int transitionStateIndex = 0;
unsigned int cycleCount = 0;

// output counter
int counter = 0;
const int OUTPUTCOUNT = 8192;   // adjust to change cycling rate of transition mode

//switch debouncing
// volatile to force the compiler to not optimize with these vars
volatile int buttonState = HIGH; // initial state of the button
volatile unsigned int shiftReg = 0xFF;    // keypress forces LO bit to 0

/********************************************************************************************************
Code
*********************************************************************************************************/
void setup()
{
  // set up LED pins
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT); 
  
  // set up switch pin
  pinMode(buttonPin, INPUT);     // pin 4 is switch input
  digitalWrite(buttonPin, HIGH); // turn on internal pull up resistor
  
  // LEDS to OFF
  writeToLeds();
  
}

void loop()
{ 
  checkForModeChange(); 
  counter++;
  
  if(counter == OUTPUTCOUNT)   // output to pins each time the counter expires 
  {
    doOutput();
    counter = 0;
  }
}

void doOutput()   
{
  if(currentMode == TRANSITION_MODE)
  {  
    doNextTransitionStep();
  }
  writeToLeds();

}

void checkForModeChange()
{
  // debounce switch and check for button state change
  sampleSwitch();

  if(shiftReg == 0 && buttonState == HIGH)
  {
    buttonState = LOW;
    nextMode();
  }
  if(shiftReg == 0xFF && buttonState == LOW)
  {
    buttonState = HIGH;
  } 

}

void sampleSwitch()
{
  int currentState = digitalRead(buttonPin);
  shiftReg = shiftReg << 1;                 // shift left 1 bit       
  shiftReg += (currentState & 0x01);        // set LO bit to current switch pin reading

}

void initTransitionMode()
{  
  transitionStateIndex = 0;
  cycleCount = 0;
  outputValuesTable[TRANSITION_MODE][RED] = FULLY_OFF;
  outputValuesTable[TRANSITION_MODE][GREEN] = FULLY_OFF;
  outputValuesTable[TRANSITION_MODE][BLUE] = FULLY_OFF;
 
  writeToLeds();

}

void doNextTransitionStep()
{

  for(int i = 0; i < 3; i++)   // update the RGB values for the next output step
  {
    int state = transitionStateTable[transitionStateIndex][i];   

    switch(state)
    {
    case RAMPING_UP:
      outputValuesTable[TRANSITION_MODE][i] = FULLY_OFF - cycleCount;
      break;
    case RAMPING_DOWN:
      outputValuesTable[TRANSITION_MODE][i] = cycleCount;  
      break;
    default:       // if its anything else, leave it alone
      break;
    }     
  }

  cycleCount++;
 
  if(cycleCount == 256)   // advance state every 255 cycles
  {
    cycleCount = 0;
    transitionStateIndex++;
    transitionStateIndex = transitionStateIndex % 7;  // cycle 0-6
  }

}

void writeToLeds()
{
  analogWrite(redPin, outputValuesTable[currentMode][RED]);
  analogWrite(greenPin, outputValuesTable[currentMode][GREEN]);
  analogWrite(bluePin, outputValuesTable[currentMode][BLUE]);
}

void nextMode()
{

  if(currentMode == OFF_MODE)
  {
    currentMode = 0;
  }
  else
  {
    currentMode++;
  }

  if(currentMode == TRANSITION_MODE)
  {
    initTransitionMode();
  } 
}

