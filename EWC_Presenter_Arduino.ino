#include <SoftwareSerial.h>

// define the pins used
#define VS1053_RX 2

// If you're using the Music Maker shield:
// Don't forget to connect the GPIO #1 pin to 3.3V and the RX pin to digital #2

// Constants from Adafruit code
#define MIDI_NOTE_ON  0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_CHAN_MSG 0xB0
#define MIDI_CHAN_BANK 0x00
#define MIDI_CHAN_VOLUME 0x07
#define MIDI_CHAN_PROGRAM 0xC0


// Define the input pins
int tempoPot = A1;
int velocityPot = A0;
int altPot1 = A2;
int altPot2 = A3;
int onboardLED = 13;

int horizontalInputPins[] = {8,9,10,11};
int verticalInputPins[] = {3,4,5,6,7};
const int hButtonCount = 4;
const int vButtonCount = 5;
const int buttonCount = hButtonCount * vButtonCount;
boolean buttonStates[vButtonCount][hButtonCount];
boolean oldButtonStates[vButtonCount][hButtonCount] = {
  {true,true,true,true},
  {true,true,true,true},
  {true,true,true,true},
  {true,true,true,true},
  {true,true,true,true}
};
boolean buttonRisingEdges[vButtonCount][hButtonCount];
boolean buttonFallingEdges[vButtonCount][hButtonCount];
boolean altPot2State;
boolean altPot2InitialState;
boolean altPot2OldState;
boolean altPot2RisingEdge;
boolean altPot2FallingEdge;

int mode = 0;
int numberOfModes = 4;  // Zero justified.
int playingFanfare = 0;
byte fanfareSequence[] = {127,3,1,0,2,4,7,9};

int potentiometerInputPins[] = {A0,A1};

// Create some useful variables
int currentNote = 60;
int tempoValue = 1;
int maxTempo = 600;
int minTempo = 25;
int velocityValue = 0;

int activeChannels = 0;
int runningChannels = 0;

boolean oldManualPlayInputValue = HIGH;
boolean manualPlayRisingEdge = LOW;
boolean manualPlayFallingEdge = LOW;
boolean oldRandomizeChannel = false;
boolean randomizeChannelInput = false;
boolean randomizeChannelRisingEdge = false;
boolean oldConfirmChannel = false;
boolean confirmChannelInput = false;
boolean confirmChannelRisingEdge = false;
boolean manualPlayInput= false;
int lowerThreshold = 48;
int upperThreshold = 72;

// Copy over some variables from old code
int noteDelta = 0;
int mainNote = 60;
int mainVelocity = 70;
int mainTempo = 500;
int mainBPM = 120;
int mainIntensity = 0;
int oldMainIntensity = 0;
long nextNoteIteration = 0;
int mainChannel = 0;
long timeThisCycle;
int minIntensity = 0;
int maxIntensity = 299;
boolean updateIntensity = false;
int intensityBreaks = 6;
int intensityBreakpoint = (maxIntensity / intensityBreaks) + 1;
int BGMSet = 0;
int totalBGMs = 1;  //Zero justified

// We only need one scale and we don't have any chords.
const int minorPentatonicScale[] = {-12, -9, -8, -5, -2, 0, 3, 4, 7, 10, 12};

int prandomInstrument = 0;
int prandomPitchOffset = 0;
int prandomTimeOffset = 0;
int prandomPlayFrequency = 0;
int prandomDuration = 0;

// We are creating an array to contain all the notes which need to play and
// it will contain a slot for each channel, 0-15, although channel 0x9 (percussion), may
// need to be used in a different way.
// Note, Time on, Time off, Velocity, Expired
// 0000, 1111111, 22222222, 33333333, 4444444
long primaryNotes[16][5][2];
boolean evenOddNotePlaying = false;

// Each time a channel gets prandom values assigned to it, they
// get stored here. The instrument is assigned another way, but
// that will get prandomized another way.
// Velocity is on this list, but that value is set by the 
// potentiometer.
// Pitch offset, Time offset, Play frequency, Duration, Velocity, Instrument
// 000000000000, 11111111111, 22222222222222, 33333333, 44444444, 5555555555
int channelTweaks[16][6];

//  Set MIDI baud rate:
SoftwareSerial VS1053_MIDI(0, 2); // TX only, do not use the 'rx' side
// on a Mega/Leonardo you may have to change the pin to one that 
// software serial support uses OR use a hardware serial port!
  
/////////////////////////////////////////////////
/////////2/4///H/O/U/R///E/N/G/I/N/E/E/R/////////
/////////////////////////////////////////////////

void setup(){
//  // This delay is from the feather_midi example from Adafruit
//  // But I removed it. It did not seem necessary for a 328P controller.
//  delay(100);
  
  Serial.begin(9600);
  VS1053_MIDI.begin(31250); // MIDI uses a 'strange baud rate'
  
  // Pin 13 is the onboard LED.
  pinMode(onboardLED, OUTPUT);
  digitalWrite(onboardLED, LOW);

  // Declare the analog input purposes
  pinMode(tempoPot, INPUT);
  pinMode(velocityPot, INPUT);
  pinMode(altPot1, INPUT);
  pinMode(altPot2, INPUT_PULLUP);
  altPot2State = digitalRead(A3);
  altPot2InitialState = altPot2State;
  altPot2OldState = altPot2State;

  // Declare the digital pins, which are read from a matrix arrangement
  for(int i=0; i<hButtonCount; i++){
    pinMode(horizontalInputPins[i], OUTPUT);
  }
  for(int i=0; i<vButtonCount; i++){
    pinMode(verticalInputPins[i], INPUT_PULLUP);
  }
  
  Serial.println(F("EWC_Presenter Running"));
  for (int i = 0; i < 16; i++){
    midiSetChannelBank(i, 0x00);
    midiSetChannelVolume(i, 127);
  }
  midiSetInstrument(0, 4);
  channelTweaks[0][0] = 0;
  channelTweaks[0][1] = 0;
  channelTweaks[0][2] = 100;
  channelTweaks[0][3] = 10;
  channelTweaks[0][4] = mainVelocity;
}

void loop() {
  checkInputs();
  timeThisCycle = millis();
  checkRisingEdges();
  percussionSoundBoard();
  switch (mode){
    case 0:
      actOnMode00();
      break;
    case 1:
      actOnMode01();
      break;
    case 2:
      actOnMode02();
      break;
    case 3:
      actOnMode03();
      break;
    case 4:
      actOnMode04();
      break;
  }
}
