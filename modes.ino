void actOnMode00(){
  // Scan the primaryNotes array for any notes which are ready to
  // play or stop. 
  for (int e=0; e <= 1; e++){
    for (int i=0; i <= activeChannels; i++){
      if (primaryNotes[i][4][e] > 0){
        // If the fourth value is two, that means the note
        // wants to stop when the time arrives.
        if ((primaryNotes[i][4][e] == 2) && (timeThisCycle >= primaryNotes[i][2][e])){
          midiNoteOff(i, primaryNotes[i][0][e], 0);
          primaryNotes[i][4][e] = 0;
        }
        // If the fourth value is one, that means the note
        // wants to play when the time arrives.
        if ((primaryNotes[i][4][e] == 1) && (timeThisCycle >= primaryNotes[i][1][e])){
          midiNoteOn(i, primaryNotes[i][0][e], primaryNotes[i][3][e]);
//          Serial.print("\nNote: ");
//          Serial.print(i);
//          Serial.print(", ");
//          Serial.print(primaryNotes[i][0][e]);
//          Serial.print(", ");
//          Serial.println(primaryNotes[i][3][e]);
          primaryNotes[i][4][e] = 2;
        }
      }
    }
  }
  // Generate the mainNote which will seed the rest of the playable notes
  if (timeThisCycle >= nextNoteIteration){
    calculateMainNote();
    calculateChannel00();
    calculateChannels(evenOddNotePlaying);
    evenOddNotePlaying = !evenOddNotePlaying;
//    Serial.print("\n");
  }
}

void actOnMode01(){
  // Noodle noodling
  // Read the discrete state of A3, which will be an external noodling button
  altPot2State = digitalRead(A3);
  
  if (altPot2State != altPot2OldState){
    if (altPot2State != altPot2InitialState){
      altPot2RisingEdge = true;
      altPot2FallingEdge = false;
    }else{
      altPot2RisingEdge = false;
      altPot2FallingEdge = true;
    }
    altPot2OldState = altPot2State;
  }else{
    altPot2FallingEdge = false;
    altPot2RisingEdge = false;
  }
  
  if (buttonFallingEdges[0][2] || altPot2FallingEdge){
    midiNoteOff(mainChannel, mainNote, 0);
  }
  if (buttonRisingEdges[0][2] || altPot2RisingEdge){
    // Do the same things as the tempo-controlled algorithm except
    // without turning off the previous note. That is handled elsewhere.
    noteDelta = random(0, 12);
    noteDelta = maintainNoteLimits(mainNote, noteDelta, lowerThreshold, upperThreshold);
    mainNote = mainNote + minorPentatonicScale[noteDelta];
    midiNoteOn(mainChannel, mainNote, mainVelocity);
  }
}

void actOnMode02(){
  if (updateIntensity){
    switch (BGMSet){
      case 0:
        BGMSet01();
        break;
      case 1:
        BGMSet02();
        break;
      default:
        break;
    }
    updateIntensity = false;
  }
  actOnMode00();
}

void actOnMode03(){
    while (Serial.available() > 0) {
    // look for a valid integer in the incoming serial stream:
    int red = Serial.parseInt();

    // look for the newline. That's the end of your sentence:
    if (Serial.read() == '\n') {
      mainIntensity = red;
      updateIntensity = true;
      Serial.println(red, DEC);
    }
  }
  actOnMode02();
}

void actOnMode04(){
  // In mode 04, we will check the alternate potentiometer to determine
  // the intensity of the soundscape.
  updateIntensityReading();
  actOnMode02();
}
