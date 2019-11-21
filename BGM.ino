// Play different instruments at different intervals
void BGMSet02(){
  Serial.print(F("Main Intensity: "));
  Serial.print(mainIntensity);
  Serial.print("\n");
  BGMSet01();
}

// Fade one instrument into another.
void BGMSet01(){
  // Ascending and descending values use mainIntensity to scale from 0 to 127.
  int ascendingValue = map(mainIntensity % intensityBreakpoint, 0, intensityBreakpoint, 0, 127);
  int descendingValue = map(mainIntensity % intensityBreakpoint, 0, intensityBreakpoint, 127, 0);
  ascendingValue = constrain(ascendingValue, 0, 127);
  descendingValue = constrain(descendingValue, 0, 127);
  activeChannels = 5;
  
  mainBPM = map(mainIntensity, 0, maxIntensity, 50, 500);
  mainTempo = (60000 / mainBPM);
  
  // Write the appropriate settings based on mainIntensity
  
  if (mainIntensity < intensityBreakpoint){
    // Pitch offset, Time offset, Play frequency, Duration, Velocity, Instrument
    // 000000000000, 11111111111, 22222222222222, 33333333, 44444444, 5555555555
    // Segment 1 / 6
    int funChannel0Specs[] = {0, 0, 80, 10, 127, 97};
    int funChannel1Specs[] = {0, 0, 40, 10, 127, 12};
    int funChannel2Specs[] = {0, 0, 0, 10, 0, 44};
    int funChannel3Specs[] = {0, 0, 0, 10, 0, 55};
    int funChannel4Specs[] = {0, 0, 70, 10, ascendingValue, 125};
    int funChannel5Specs[] = {0, 0, 0, 10, 0, 114};
    
    for(int i = 0; i < 6; i++){
      channelTweaks[0][i] = funChannel0Specs[i];
      channelTweaks[1][i] = funChannel1Specs[i];
      channelTweaks[2][i] = funChannel2Specs[i];
      channelTweaks[3][i] = funChannel3Specs[i];
      channelTweaks[4][i] = funChannel4Specs[i];
      channelTweaks[5][i] = funChannel5Specs[i];
    }
    
    for(int i = 0; i < 6; i++){
      midiSetInstrument(i, channelTweaks[i][5]);
    }
  }
  
  if ((mainIntensity >= intensityBreakpoint*1) && (mainIntensity < intensityBreakpoint*2)){
    // Segment 2 / 6
    int funChannel0Specs[] = {0, 0, 80, 0, (descendingValue/2)+63, 97};
    int funChannel1Specs[] = {0, 0, 40, 10, 127, 12};
    int funChannel2Specs[] = {0, 0, 70, 10, (ascendingValue/2), 44};
    int funChannel3Specs[] = {0, 0, 0, 10, 0, 55};
    int funChannel4Specs[] = {0, 0, 70, 10, 127, 125};
    int funChannel5Specs[] = {0, 0, 0, 10, 0, 114};
    
    for(int i = 0; i < 6; i++){
      channelTweaks[0][i] = funChannel0Specs[i];
      channelTweaks[1][i] = funChannel1Specs[i];
      channelTweaks[2][i] = funChannel2Specs[i];
      channelTweaks[3][i] = funChannel3Specs[i];
      channelTweaks[4][i] = funChannel4Specs[i];
      channelTweaks[5][i] = funChannel5Specs[i];
    }
    
    for(int i = 0; i < 6; i++){
      midiSetInstrument(i, channelTweaks[i][5]);
    }
  }
  
  if ((mainIntensity >= intensityBreakpoint*2) && (mainIntensity < intensityBreakpoint*3)){
    // Segment 3 / 6
    int funChannel0Specs[] = {0, 0, 80, 0, (descendingValue/2), 97};
    int funChannel1Specs[] = {0, 0, 40, 10, descendingValue, 12};
    int funChannel2Specs[] = {0, 0, 70, 10, (ascendingValue/2)+63, 44};
    int funChannel3Specs[] = {0, 0, 0, 10, 0, 55};
    int funChannel4Specs[] = {0, 0, 70, 10, 127, 125};
    int funChannel5Specs[] = {0, 0, 0, 10, 0, 114};
    
    for(int i = 0; i < 6; i++){
      channelTweaks[0][i] = funChannel0Specs[i];
      channelTweaks[1][i] = funChannel1Specs[i];
      channelTweaks[2][i] = funChannel2Specs[i];
      channelTweaks[3][i] = funChannel3Specs[i];
      channelTweaks[4][i] = funChannel4Specs[i];
      channelTweaks[5][i] = funChannel5Specs[i];
    }
    
    for(int i = 0; i < 6; i++){
      midiSetInstrument(i, channelTweaks[i][5]);
    }
  }
  
  if ((mainIntensity >= intensityBreakpoint*3) && (mainIntensity < intensityBreakpoint*4)){
    // Segment 4 / 6
    int funChannel0Specs[] = {0, 0, 0, 10, 0, 97};
    int funChannel1Specs[] = {0, 0, 0, 10, 0, 12};
    int funChannel2Specs[] = {0, 0, 70, 10, (descendingValue/2)+63, 44};
    int funChannel3Specs[] = {0, 0, 0, 10, 0, 55};
    int funChannel4Specs[] = {0, 0, 70, 10, descendingValue, 125};
    int funChannel5Specs[] = {0, 0, 0, 10, 0, 114};
    
    for(int i = 0; i < 6; i++){
      channelTweaks[0][i] = funChannel0Specs[i];
      channelTweaks[1][i] = funChannel1Specs[i];
      channelTweaks[2][i] = funChannel2Specs[i];
      channelTweaks[3][i] = funChannel3Specs[i];
      channelTweaks[4][i] = funChannel4Specs[i];
      channelTweaks[5][i] = funChannel5Specs[i];
    }
    
    midiSetInstrument(0, channelTweaks[0][5]);
    midiSetInstrument(1, channelTweaks[1][5]);
    midiSetInstrument(2, channelTweaks[2][5]);
    midiSetInstrument(3, channelTweaks[3][5]);
    midiSetInstrument(4, channelTweaks[4][5]);
    midiSetInstrument(5, channelTweaks[5][5]);
  }
  
  if ((mainIntensity >= intensityBreakpoint*4) && (mainIntensity < intensityBreakpoint*5)){
    // Segment 5 / 6
    int funChannel0Specs[] = {0, 0, 0, 10, 0, 97};
    int funChannel1Specs[] = {0, 0, 0, 10, 0, 12};
    int funChannel2Specs[] = {0, 0, 70, 10, (descendingValue/2), 44};
    int funChannel3Specs[] = {0, 0, 100, 10, ascendingValue, 55};
    int funChannel4Specs[] = {0, 0, 0, 10, 0, 125};
    int funChannel5Specs[] = {0, 0, 25, 10, (ascendingValue/2), 114};
    
    for(int i = 0; i < 6; i++){
      channelTweaks[0][i] = funChannel0Specs[i];
      channelTweaks[1][i] = funChannel1Specs[i];
      channelTweaks[2][i] = funChannel2Specs[i];
      channelTweaks[3][i] = funChannel3Specs[i];
      channelTweaks[4][i] = funChannel4Specs[i];
      channelTweaks[5][i] = funChannel5Specs[i];
    }
    
    for(int i = 0; i < 6; i++){
      midiSetInstrument(i, channelTweaks[i][5]);
    }
  }
  
  if ((mainIntensity >= intensityBreakpoint*5) && (mainIntensity < intensityBreakpoint*6)){
    // Segment 6 / 6
    int funChannel0Specs[] = {0, 0, 0, 10, 0, 97};
    int funChannel1Specs[] = {0, 0, 0, 10, 0, 12};
    int funChannel2Specs[] = {0, 0, 0, 10, 0, 44};
    int funChannel3Specs[] = {0, 0, 100, 10, 127, 55};
    int funChannel4Specs[] = {0, 0, 0, 10, 0, 125};
    int funChannel5Specs[] = {0, 0, 25, 10, (ascendingValue/2)+63, 114};
    
    for(int i = 0; i < 6; i++){
      channelTweaks[0][i] = funChannel0Specs[i];
      channelTweaks[1][i] = funChannel1Specs[i];
      channelTweaks[2][i] = funChannel2Specs[i];
      channelTweaks[3][i] = funChannel3Specs[i];
      channelTweaks[4][i] = funChannel4Specs[i];
      channelTweaks[5][i] = funChannel5Specs[i];
    }
    
    for(int i = 0; i < 6; i++){
      midiSetInstrument(i, channelTweaks[i][5]);
    }
  }
  
  if (mainIntensity > intensityBreakpoint*6){
    // Clear all the data from channelTweaks[][]
    for(int i = 0; i < 6; i++){
      for(int k = 0; k < 16; k++){
        channelTweaks[k][i] = 0;
      }
    }
    stopActiveNotes();
    playingFanfare = 1;
    activeChannels = 0;
    Serial.print(F("\n\nMain Intensity: "));
    Serial.println(mainIntensity);
    Serial.print(F("Ascending Value: "));
    Serial.print(ascendingValue);
    Serial.print(F("\tDescending Value: "));
    Serial.println(descendingValue);
//    printChannelInformation();
  }
}
