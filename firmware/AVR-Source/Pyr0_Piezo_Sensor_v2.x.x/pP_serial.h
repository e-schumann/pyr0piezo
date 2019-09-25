
/*------------------------------------------------*/

void parseData() {

  // split the data into its parts
  
  char * strtokIndx; // this is used by strtok() as an index
  
  strtokIndx = strtok(inputBuffer,",");      // get the first part - the string
  strcpy(serialMessageIn, strtokIndx); // copy it to serialMessageIn
  
  strtokIndx = strtok(NULL, ",");   // this continues where the previous call left off
  serialInt = atoi(strtokIndx);     // convert this part to an integer

}
/*------------------------------------------------*/

void identifyMarkers() {
  
  char x = Serial.read();
//  char y = Wire.read();

  if (x == endMarker) {
    readInProgress = false;
    serialIncoming = true;
    inputBuffer[bytesRecvd] = 0;
    parseData();
  }

  else if(readInProgress) {
    inputBuffer[bytesRecvd] = x;
    bytesRecvd ++;
    if (bytesRecvd == buffSize) {
      bytesRecvd = buffSize - 1;
    }
  }

  else if (x == startMarker) { 
    bytesRecvd = 0; 
    readInProgress = true;
  }
  #ifdef I2C
    if (y == endMarker) {
      readInProgress = false;
      serialIncoming = true;
      inputBuffer[bytesRecvd] = 0;
      parseData();
    }

    if(readInProgress) {
      inputBuffer[bytesRecvd] = y;
      bytesRecvd ++;
      if (bytesRecvd == buffSize) {
      bytesRecvd = buffSize - 1;
      }
    }

    if (y == startMarker) { 
      bytesRecvd = 0; 
      readInProgress = true;
    }
  #endif
}

/*------------------------------------------------*/

void updateTrigDuration() {
  if (serialInt >= 0) {
    TRG_DUR = serialInt;
  }
}
/*------------------------------------------------*/

void updateGainFactor() {
  if (serialInt >= 0) {
    GAIN_FACTOR = serialInt;
  }
}
/*------------------------------------------------*/

void updateVComp() {
  if (serialInt >= 0) {
    compInt = serialInt;
    //senseInt = compInt; // syncing these params til #24 is fixed
  }
}
/*------------------------------------------------*/

void updateVAdj() {
  if (serialInt >= 0) {
    senseThrs = serialInt;
    //compInt = senseInt; // syncing these params til #24 is fixed
  }
}
/*------------------------------------------------*/

void updateHysteresis() {
  if (serialInt >= 0) {
    Hyst = serialInt;
  }
}
/*------------------------------------------------*/

void updateParams() {
  if (strcmp(serialMessageIn, "TRG_D") == 0) {
    updateTrigDuration();
  }
  else if (strcmp(serialMessageIn, "GAIN_F") == 0) {
    updateGainFactor();
  }
  else if (strcmp(serialMessageIn, "VCOMP") == 0) {
    updateVComp();
  }
  else if (strcmp(serialMessageIn, "VADJ") == 0) {
    updateVAdj();
  }
  else if (strcmp(serialMessageIn, "HYST") == 0) {
    updateHysteresis();
  }
  else if (strcmp(serialMessageIn, "HELP") == 0) {
    Serial.println("To change trigger active duration: TRG_D [integer for milliseconds]");
    Serial.println("To change gain factor: GAIN_F [integer for gain state - see note*]");
    Serial.println("To change ADC hysteresis value: HYST [integer]");
    Serial.println("To change sensor input pullup vRef low threshold: VADJ [float value]");
    Serial.println("To change comparator trigger high threshold: VCOMP [float value]");
    Serial.println("");
    Serial.println("These commands should be wrapped in this format:");
    Serial.println("<CMD, INT>");
    Serial.println("");
    Serial.println("Examples:");
    Serial.println("<GAIN_F, 3> <~ set gain factor to index 3 (6x)");
    Serial.println("<VADJ, 2350> <~ set the vref floor to 2.35V");
    parseData();
  }
}

/*------------------------------------------------*/

void serialInput() {

  // receive data from Serial and save it into inputBuffer
  
  if(Serial.available() > 0) {

    // the order of these IF clauses is significant
    identifyMarkers();

  }
}

/*------------------------------------------------*/

void serialReply() {
  #ifndef VERBOSE
    if (serialIncoming) {
    serialIncoming = false;
  #endif
  #ifdef DEBUG
    Serial.print("Vcc:");
    Serial.println(Vin);
    Serial.print("Comp Sense:");
    Serial.print(VComp);
    Serial.print(" ");
    Serial.print("Comparator State:");
    Serial.print(ADJ_COMP);
    Serial.print(" ");
    Serial.println(compInt);

    Serial.print("Amp Sense:");
    Serial.print(VAdj);
    Serial.print(" ");
    Serial.print("Follower State:");
    Serial.print(ADJ_FOLLOW);
    Serial.print(" ");
    Serial.println(senseInt);

    Serial.print("Gain Factor:");
    Serial.print(GAIN_FACTOR);
    switch (GAIN_FACTOR) {
      case 0:
        Serial.println(" 3x");
        break;
      case 1:
        Serial.println(" 3.5x");
        break;
      case 2:
        Serial.println(" 4.33x");
        break;
      case 3:
        Serial.println(" 6x");
        break;
      case 4:
        Serial.println(" 11x");
        break;
      default:
        Serial.println(" INVALID");
        break;
    }

  #endif
  
  Serial.print("Delay:");
  Serial.println(TRG_DUR);
  Serial.print("Error State:");
  Serial.println(ERR_STATE);
  Serial.println("------------------");
  #ifndef VERBOSE
  }
  #endif
}
