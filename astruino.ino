
#include "conf.h"
#include "def.h"
#include <EEPROM.h>

String inputCommand = "";
boolean commandComplete = false;
boolean isMoving;
boolean isHalt = false;
boolean isAbsolute = false;
int actualPosition = 0;
boolean stepDirection = false; //true = moveIn
int stepPin = STEP_PIN;      
int frequency = 1000/STEP_FREQUENCY;

void setup()
{ 
  Serial.begin(BAUD);
  pinMode(stepPin, OUTPUT);  
  if(EEPROM.read(IS_ABSOLUTE_ADDR)) isAbsolute = true;
  else isAbsolute = false;
  //led  
  pinMode(13, OUTPUT);  
}

void loop()
{
  if(commandComplete){    
    if(inputCommand == "IM#"){  //IsMoving
      if(isMoving) Serial.print("IMT#");
      else Serial.print("IMF#");
    }
    else if (inputCommand == "MI#"){  //MaxIncrement
      Serial.print("MI" + String(MAX_INCREMENT) + "#");
    }
    else if (inputCommand  == "MS#"){  //MaxStep
      Serial.print("MS" + String(MAX_STEP) + "#");
    }
    else if (inputCommand == "SS#"){  //StepSize   
      Serial.print("SS" + String(STEP_SIZE) + "#"); 
    }
    else if (inputCommand == "H#"){  //Halt
      Serial.print("#");
      isHalt = true;  
    }
    else if (inputCommand == "A#"){  //Absolute
      Serial.print("#");
      isAbsolute = true;      
      EEPROM.write(IS_ABSOLUTE_ADDR, 1) ;
    }
    else if (inputCommand == "R#"){  //Relative
      Serial.print("#");
      isAbsolute = false;     
      EEPROM.write(IS_ABSOLUTE_ADDR, 0) ; 
    }
    else if (inputCommand == "IA#"){  //isAbsolute   
      Serial.print("IA" + String(isAbsolute) + "#"); 
    }
    else if (inputCommand == "IP#"){  //isAbsolute   
      Serial.print("#");
      initPosition();
    }
    else if (inputCommand.substring(0,2) == "MI"){  //moveIn
      Serial.print("#");
      int indexPositionCommand = inputCommand.indexOf("MI");
      int indexEndCommand = inputCommand.indexOf("#");
      int stepNumbers = inputCommand.substring(indexPositionCommand + 2, indexEndCommand).toInt();
      moveIn(stepNumbers);
    }
    else if (inputCommand.substring(0,2) == "MO"){  //moveOut
      Serial.print("#");
      int indexPositionCommand = inputCommand.indexOf("MT");
      int indexEndCommand = inputCommand.indexOf("#");
      int stepNumbers = inputCommand.substring(indexPositionCommand + 2, indexEndCommand).toInt();
      moveOut(stepNumbers);
    }
    else if (inputCommand.substring(0,2) == "MT"){  //moveTo
      Serial.print("#");
      int indexPositionCommand = inputCommand.indexOf("MT");
      int indexEndCommand = inputCommand.indexOf("#");
      int stepNumbers = inputCommand.substring(indexPositionCommand + 2, indexEndCommand).toInt();
      moveTo(stepNumbers);
    }    
    inputCommand   = "";
    commandComplete = false;
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputCommand += inChar;
    if (inChar == '#') {
      commandComplete = true;
    } 
  }
}

void moveTo(int steps){ 
  if(steps > actualPosition) moveOut(steps - actualPosition);
  else moveIn(actualPosition - steps);
}

void moveIn(int steps){
  stepDirection = true;
  for(int i = 0; i < steps; i++){
    if(isHalt){
      isHalt = false;
      digitalWrite(13, LOW);
      break;
    }    
    if(i%20) digitalWrite(13, LOW);
    else digitalWrite(13, HIGH);    
    digitalWrite(stepPin, LOW);
    digitalWrite(stepPin, HIGH);
    if(isAbsolute)actualPosition --;
    delay(frequency);
  }
  digitalWrite(13, LOW);
}

void moveOut(int steps){
  stepDirection = false;
  for(int i = 0; i < steps; i++){
    if(isHalt){
      isHalt = false;
      digitalWrite(13, LOW);
      break;
    }    
    if(i%20) digitalWrite(13, LOW);
    else digitalWrite(13, HIGH);    
    digitalWrite(stepPin, LOW);
    digitalWrite(stepPin, HIGH);
    if(isAbsolute)actualPosition ++;
    delay(frequency);
  }
  digitalWrite(13, LOW);
}

void initPosition(){
}


