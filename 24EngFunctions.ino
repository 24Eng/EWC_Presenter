// Ensure that the note about to be played is within the limits.
int maintainNoteLimits(int funStartingPoint, int funRequestedChange, int funLowerLimit, int funUpperLimit){
  int safelyRangedChange;
  boolean unaltered = true;
  int requestedNote = funStartingPoint + minorPentatonicScale[funRequestedChange];
  // Assess if the requested note needs to be raised to keep it above the lower threshold.
  if (requestedNote < funLowerLimit){
    // If the requested change is already greater than six, the limits were probably
    // changed recently so just keep going to a higher note. Else, add six to make the change positive.
    if (funRequestedChange > 6){
      safelyRangedChange = funRequestedChange;
      unaltered = false;
    }else{
      safelyRangedChange = 6 + funRequestedChange;
      unaltered = false;
    }
  }
  // Assess if the requested note needs to be lowered to keep it below the upper threshold.
  if ((requestedNote > funUpperLimit) && unaltered){
    // If the requested change is already less than six, the limits were probably
    // changed recently so just keep going to a lower note. Else, subtract six to make the change negative.
    if (funRequestedChange < 6){
      safelyRangedChange = funRequestedChange;
    }else{
      safelyRangedChange = funRequestedChange - 6;
    }
  }
  // For an unknown reason, the next print statement must be intact for
  // the code to run properly.
  // DO NOT DELETE THIS PRINTLN STATEMENT.
  // But sometimes deleting it works too. YMMV
//  Serial.print("");
//  Serial.println(safelyRangedChange);
  // For an unknown reason, the previous print statement must be intact for
  // the code to run properly.
  // DO NOT DELETE THIS PRINTLN STATEMENT
  // But sometimes deleting it works too. YMMV
  return safelyRangedChange;
}

void MIDICommand(int statusByte, int data1Byte, int data2Byte){
  VS1053_MIDI.write(statusByte);
  VS1053_MIDI.write(data1Byte);
  VS1053_MIDI.write(data2Byte);
}

void readDigitalInputs(){
  for(int y=0; y<hButtonCount; y++){
    digitalWrite(horizontalInputPins[y], LOW);
    for(int x=0; x<vButtonCount; x++){
      boolean funButtonState = digitalRead(verticalInputPins[x]);
      buttonStates[x][y]= funButtonState;
    }
    digitalWrite(horizontalInputPins[y], HIGH);
  }
}

void compareButtonStates(){
  for(int y=0; y<hButtonCount; y++){
    for(int x=0; x<vButtonCount; x++){
      buttonRisingEdges[x][y] = LOW;
      buttonFallingEdges[x][y] = LOW;
      if (buttonStates[x][y] != oldButtonStates[x][y]){
        if (oldButtonStates[x][y] > buttonStates[x][y]){
          buttonRisingEdges[x][y] = HIGH;
          buttonFallingEdges[x][y] = LOW;
        }else{
          buttonRisingEdges[x][y] = LOW;
          buttonFallingEdges[x][y] = HIGH;
        }
        oldButtonStates[x][y] = buttonStates[x][y];
//        printInputs(x, y);
      }
    }
  }
}

void checkInputs(){
  mainVelocity = analogRead(velocityPot);
  if (mode < 2){
    mainVelocity = map(mainVelocity, 0, 1024, 0, 127);
    // Read the input to update the tempo
    updateTempo();
    channelTweaks[activeChannels][4] = mainVelocity;
  }else{
    mainVelocity = 127;
    updateIntensityReading();
  }
  for (int e=0; e <= 1; e++){
    primaryNotes[activeChannels][3][e] = mainVelocity;
  }
  readDigitalInputs();
  compareButtonStates();
}

void updateIntensityReading(){
  // tempoPot controls the tempo and is reported in BPM
  int funPinData = 0;
  if (mode == 2){
    funPinData = analogRead(tempoPot);
  }else if(mode == 4){
    funPinData = analogRead(altPot1);
  }
  mainIntensity = map(funPinData, 0, 1024, minIntensity, maxIntensity);
  if (mainIntensity != oldMainIntensity){
    updateIntensity = true;
    oldMainIntensity = mainIntensity;
  }
}

void updateTempo(){
  // tempoPot controls the tempo and is reported in BPM
  int funPinData = analogRead(tempoPot);
  mainBPM = map(funPinData, 0, 1024, minTempo, maxTempo);
  mainIntensity = map(funPinData, 0, 1024, minIntensity, maxIntensity);
  if (mainIntensity != oldMainIntensity){
    updateIntensity = true;
    oldMainIntensity = mainIntensity;
  }
  mainTempo = (60000 / mainBPM);
}

//void printInputs(int funX, int funY){
//  if (funX==9 && funY==9){
//    delay(1);
//  }else{
//    Serial.print("x: ");
//    Serial.print(funX);
//    Serial.print("\ty: ");
//    Serial.print(funY);
//    Serial.print("\t\tTime:\t");
//    Serial.println(millis());
//  }
//}
//
//void printInputs(){
//  Serial.print("BPM: ");
//  Serial.print(mainBPM);
//  Serial.print("\t");
//  Serial.print("Velocity: ");
//  Serial.print(mainVelocity);
//  Serial.print("\t");
//  Serial.print(randomizeChannelInput);
//  Serial.print("\t");
//  Serial.print(confirmChannelInput);
//  Serial.print("\t");
//  Serial.print(manualPlayInput);
//  Serial.print("\n");
//}

void calculateMainNote(){
  // Generate the next note.
  noteDelta = random(0, 12);
  // Check to see that the note will be within the prescribed range.
  noteDelta = maintainNoteLimits(mainNote, noteDelta, lowerThreshold, upperThreshold);
  // Change the mainNote variable according to the selected scale.
  mainNote = mainNote + minorPentatonicScale[noteDelta];
  nextNoteIteration = timeThisCycle + (mainTempo);
}

void calculateChannel00(){
  // Channel zero always has a time offset of zero, a pitch offset of
  // zero, and plays at full volume so it is calculated differently.
  int chanceToPlay = random(0, 100);
  if (mainBPM == minTempo){
    chanceToPlay = 100;
  }
  if (chanceToPlay < channelTweaks[0][2]){
    primaryNotes[0][0][0] = mainNote;
    primaryNotes[0][1][0] = timeThisCycle;
    primaryNotes[0][2][0] = timeThisCycle + mainTempo;
    primaryNotes[0][3][0] = channelTweaks[0][4];
    primaryNotes[0][4][0] = 1;
  }
}

void calculateChannels(boolean funEvenOdd){
//  Serial.print("\n");
  for (int i=1; i <= activeChannels; i++){
    if (i != 9){
      // Calculate the Pitch offset
      primaryNotes[i][0][funEvenOdd] = mainNote + channelTweaks[i][0];
      // Calculate the Time on value
      long funTimeOffset = (channelTweaks[i][1] * mainTempo) / 10;
      long funTimeOn = timeThisCycle + funTimeOffset;
      primaryNotes[i][1][funEvenOdd] = funTimeOn;
      // Calculate the Time off value
      long funDuration =  (channelTweaks[i][3]*mainTempo) / 10;
      primaryNotes[i][2][funEvenOdd] = funTimeOn + funDuration;
      // Calculate the note Velocity
      primaryNotes[i][3][funEvenOdd] = channelTweaks[i][4];
      // Set value [4] to one so it can begin playing
      primaryNotes[i][4][funEvenOdd] = 1;
//      Serial.print("Channel: ");
//      Serial.print(i);
//      Serial.print("\t");
//      for (int e=0; e<5; e++){
//        Serial.print(primaryNotes[i][e][funEvenOdd]);
//        Serial.print(", ");
//      }
//      Serial.print("\n");
    }
  }
}

void randomizeChannel(){
  stopActiveNotes();
  
  // Select an instrument
  prandomInstrument = random(0, 127);
  midiSetInstrument(activeChannels, prandomInstrument);
  channelTweaks[activeChannels][5] = prandomInstrument;
  
  // Record a prandom Pitch offset [0]
  prandomPitchOffset = random(-12, 12);
  channelTweaks[activeChannels][0] = prandomPitchOffset;
  
  // Record a prandom Time offset [1]
  prandomTimeOffset = random(0, 10);
  channelTweaks[activeChannels][1] = prandomTimeOffset;
  
  // Record a prandom Play frequency [2]
  prandomPlayFrequency = random(10, 100);
  channelTweaks[activeChannels][2] = prandomPlayFrequency;
  
  // Record a prandom Duration [3]
  prandomDuration = random(1, 10);
  channelTweaks[activeChannels][3] = prandomDuration;
  
  // Start with the system velocity
  channelTweaks[activeChannels][4] = mainVelocity;
}

void printChannelInformation(){
  Serial.print("\n");
  Serial.print(F("* * * * * Channel info * * * * *"));
  Serial.print("\n");
  for (int i = 0; i<=activeChannels; i++){
    if (i == 9){
      i = 10;
    }
    Serial.print(F("Channel: "));
    Serial.print(i);
    Serial.print(F("\tInstrument: "));
    Serial.print(channelTweaks[i][5]);
    Serial.print(F("\tPitch offset: "));
    Serial.print(channelTweaks[i][0]);
    Serial.print(F(" \tTime offset: "));
    Serial.print(channelTweaks[i][1]);
    Serial.print(F("\tPlay frequency: "));
    Serial.print(channelTweaks[i][2]);
    Serial.print(F("\tVelocity: "));
    Serial.print(channelTweaks[i][4]);
    Serial.print("\n");
  }
  Serial.print(F("Mode: "));
  Serial.print(mode);
  Serial.print(F("\tIntensity: "));
  Serial.print(mainIntensity);
  Serial.print("\n");
}

void panic(){
  for (int e=0; e <= 1; e++){
    for (int i = 0; i < 16; i++){
      for (int k = 0; k < 127; k++){
        midiNoteOff(i, k, 0);
      }
      midiSetChannelVolume(i, 127);
      primaryNotes[i][4][e] = 0;
    }
  }
  activeChannels = 0;
  Serial.println(F("Panic"));
//  delay(1000);
}

void stopAllNotes(){
  for (int e=0; e <= 1; e++){
    for (int i = 0; i < 16; i++){
      for (int k = 0; k < 127; k++){
        midiNoteOff(i, k, 0);
      }
      primaryNotes[i][4][e] = 0;
    }
  }
}

void stopActiveNotes(){
  for (int e=0; e <= 1; e++){
    for (int i = 0; i <= activeChannels; i++){
      for (int k = 0; k < 127; k++){
        midiNoteOff(i, k, 0);
      }
      primaryNotes[i][4][e] = 0;
    }
  }
}

void printMIDIToTerminal(uint8_t chan, uint8_t n, uint8_t vel){
  Serial.print("\n");
  Serial.print(chan, HEX);
  Serial.print(", ");
  Serial.print(n);
  Serial.print(", ");
  Serial.print(vel);
}

void checkRisingEdges(){
  // When the "Mode" button is pressed,
  // switch between the available modes.
  if (buttonRisingEdges[1][1]){
    mode++;
    stopActiveNotes();
    if (mode > numberOfModes){
      mode = 0;
    }
    if ((mode == 0) || (mode == 1)){
      activeChannels = 0;
    }
  }
  
  // When the "Prandomize" button is pressed,
  // scramble the settings for the current channel.
  if (buttonRisingEdges[0][3]){
    randomizeChannel();
  }

  // When the "Add Channel" button is pressed,
  // cement the current channel settings and move to another with
  // prandom settings.
  if (buttonRisingEdges[1][3]){
    activeChannels++;
    if (activeChannels == 9){
      activeChannels = 10;
    }
    randomizeChannel();
  }
  
  // When the "Panic" button is pressed,
  // stop all channels and print a message.
  if (buttonRisingEdges[0][0]){
    panic();
  }
  
  // When the "Print" button is pressed,
  // print the current instruments and specs in the mix.
  if (buttonRisingEdges[0][1]){
    printChannelInformation();
  }
}

void percussionSoundBoard(){
  //Increase the BGM set
  if (buttonRisingEdges[1][0]){
    BGMSet++;
    if (BGMSet > totalBGMs){
      BGMSet = 0;
    }
  }
  
  //Play a fanfare sequence 
  if (buttonRisingEdges[2][3]){
    playingFanfare = 1;
    midiSetInstrument(15, 99);
  }
  if (playingFanfare>0){
    int fanfareOffset = 60;
    if (millis()%300==0){
      midiNoteOff(15, fanfareSequence[playingFanfare-1] + fanfareOffset, 127);
      midiNoteOn(15, fanfareSequence[playingFanfare] + fanfareOffset, 127);
      playingFanfare++;
    }
    if (playingFanfare >= (sizeof(fanfareSequence))){
      delay(1000);
      midiNoteOff(15, fanfareSequence[sizeof(fanfareSequence)-1] + fanfareOffset, 127);
      playingFanfare = 0;
    }
  }
  
  // Play and release percussion buttons
  if (buttonRisingEdges[2][0]){
    //Snare
    midiNoteOn(9, 38, 127);
  }
  
  if (buttonRisingEdges[2][1]){
    //Crash
    midiNoteOn(9, 49, 127);
  }
  
  if (buttonRisingEdges[2][2]){
    //Hi hat foot
    midiNoteOn(9, 44, 127);
  }

  if (buttonRisingEdges[3][1]){
    //Low tom
    midiNoteOn(9, 45, 127);
  }

  if (buttonRisingEdges[3][2]){
    //Ride
    midiNoteOn(9, 51, 127);
  }

  if (buttonRisingEdges[3][3]){
    //Applause
    midiSetInstrument(15, 126);
    midiNoteOn(15, 63, mainVelocity);
  }
  if (buttonFallingEdges[3][3]){
    //Applause
    midiNoteOff(15, 63, 0);
  }

  if (buttonRisingEdges[4][0]){
    //Bass drum
    midiNoteOn(9, 35, 127);
  }

  if (buttonRisingEdges[4][1]){
    //High tom
    midiNoteOn(9, 50, 127);
  }

  if (buttonRisingEdges[4][2]){
    //Floor tom
    midiNoteOn(9, 43, 127);
  }

  if (!buttonStates[4][3]){
    //Drum roll, snare
    if (millis() % 90 == 0){
      midiNoteOn(9, 38, mainVelocity);
    }
  }
  if (buttonFallingEdges[4][3]){
    //Drum roll, snare
    midiNoteOff(9, 38, 0);
  }
}
