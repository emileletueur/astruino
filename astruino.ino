
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
int frequency = 1000 / STEP_FREQUENCY;

void setup()
{ 
  Serial.begin(BAUD);
  pinMode(stepPin, OUTPUT);  
  if(EEPROM.read(IS_ABSOLUTE_ADDR)) isAbsolute = true;
  else isAbsolute = false;
  if(isAbsolute) {
  }
  //led  
  pinMode(13, OUTPUT);  
}

void loop()
{
  if(commandComplete){    
    if(inputCommand == COMMAND_IS_MOVING){  //IsMoving
      if(isMoving) Serial.print(COMMANDIS_MOVING_TRUE);
      else Serial.print(COMMANDIS_MOVING_FALSE);
    }
    else if (inputCommand == COMMAND_MAX_INCREMENT){  //MaxIncrement
      Serial.print("MI" + String(MAX_INCREMENT) + "#");
    }
    else if (inputCommand  == COMMAND_MAX_STEP){  //MaxStep
      Serial.print("MS" + String(MAX_STEP) + "#");
    }
    else if (inputCommand == COMMAND_STEP_SIZE){  //StepSize   
      Serial.print("SS" + String(STEP_SIZE) + "#"); 
    }
    else if (inputCommand == COMMAND_HALT){  //Halt
      Serial.print("#");
      isHalt = true;  
    }
    else if (inputCommand == COMMAND_SET_ABSOLUTE){  //setAbsolute
      Serial.print("#");
      isAbsolute = true;      
      EEPROM.write(IS_ABSOLUTE_ADDR, 1) ;
    }
    else if (inputCommand == COMMAND_SET_RELATIVE){  //setRelative
      Serial.print("#");
      isAbsolute = false;     
      EEPROM.write(IS_ABSOLUTE_ADDR, 0) ; 
    }
    else if (inputCommand == COMMAND_IS_ABSOLUTE){  //isAbsolute   
      Serial.print("IA" + String(isAbsolute) + "#"); 
    }
    else if (inputCommand == COMMAND_INIT_POSITION){  //initPosition   
      Serial.print("#");
      initPosition();
    }
    else if(inputCommand == COMMAND_SAVE_POSITION){
      Serial.print("#");
      savePosition();
    }
    else if (inputCommand.substring(0,2) == COMMAND_MOVE_IN){  //moveIn
      Serial.print("#");
      moveIn(getStepsFromMoveCommandArgument(inputCommand, COMMAND_MOVE_IN));
    }
    else if (inputCommand.substring(0,2) == COMMAND_MOVE_OUT){  //moveOut
      Serial.print("#");
      moveOut(getStepsFromMoveCommandArgument(inputCommand, COMMAND_MOVE_OUT));
    }
    else if (inputCommand.substring(0,2) == COMMAND_MOVE_TO){  //moveTo
      Serial.print("#");
      moveTo(getStepsFromMoveCommandArgument(inputCommand, COMMAND_MOVE_TO));
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
  actualPosition = 0;
}

void savePosition(){  
  EEPROM.write(POSITION_ADDR, actualPosition >> 8) ; 
  EEPROM.write(POSITION_ADDR + 1, actualPosition) ; 
}

int getStepsFromMoveCommandArgument(String command, String commandName){
    int indexPositionCommand = command.indexOf(commandName);
    int indexEndCommand = command.indexOf("#");
    return command.substring(indexPositionCommand + 2, indexEndCommand).toInt();
}


