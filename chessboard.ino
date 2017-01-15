/*
   Using a shift register to scan the files of a chess board and
   arduino pins to read what ranks contain pieces on file
*/

#include <Arduino.h>
#include <SPI.h>

const int latchPin = A0;
const int clockPin = A1;
const int dataPin = A2;
const int colPins[8] = {2, 3, 4, 5, 6, 7, 8, 9};

//const int led1 = 10;
//const int led2 = 11;

byte data = 0;

unsigned long previousMillis = 0; //holds time since last move

byte previousState[8];

String strState = "195.195.195.195.195.195.195.195";
String header = "\nBOARD STATE.(TIME STABLE)\tSTATE CHANGE\n";
String change = "e4";

bool lifted;

bool moveResolved;
/************************************************
                  SETUP
************************************************/


void setup() {

  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  for (int i = 0; i < 8; i++) {
    pinMode(colPins[i], INPUT);
  }

  Serial.begin(9600);
  Serial.println("reset");

  Serial.println(header);

  getBoardState(previousState);
  stateToString(previousState);
  Serial.print(strState);

}

/************************************************
                  LOOP
************************************************/

void loop() {

  if (!compareState()) {

    printChange();

  }

}

/************************************************
  Compare state of board to previous state and
  return FALSE if board has changed
************************************************/

bool compareState() {

  bool result = true;

  byte tempState[8];

  getBoardState(tempState);

  for (int i = 0; i < 8; i++) {

    int test1 = tempState[i];
    int test2 = previousState[i];

    if (test1 != test2) {

      result = false;

      isLift(test1, test2);

      getChange(i, (test1 ^ test2));

    }

    previousState[i] = tempState[i];

  }

  return result;
}

/*************************************
   Get the current state of the board by scanning each file one at a time using the
   shift register and reading colPins
*/
void getBoardState(byte state[]) {

  byte colState;

  for (int i = 0; i < 8; i++) {

    shiftWrite(i, HIGH);

    for (int j = 0; j < 8; j++) {
      bitWrite(colState, j, digitalRead(colPins[j]));;
    }

    state[i] = colState;
    shiftWrite(i, LOW);

    colState = 0;

  }

}

void shiftWrite(int desiredPin, boolean desiredState) {

  bitWrite(data, desiredPin, desiredState);

  shiftOut(dataPin, clockPin, MSBFIRST, data);

  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
}

void getChange(int c, int colData) {

  int r = 0;
  change =  "";

  for (int i = 0; i < 8; i++) {
    if (colData & 1 << i) {
      r = i;
      break;
    }
  }

  change += char(97 + c);
  change += char(49 + r);

}

void isLift(byte test1, byte test2) {

  lifted = false;

  if (test1 < test2) {
    lifted = true;
  }

}

void printChange() {

  stateToString(previousState);

  Serial.print(millis() - previousMillis);
  Serial.print("\t");

  if (lifted) {
    Serial.print("l: ");
  }
  else {
    Serial.print("d: ");
  }

  Serial.println(change);
  Serial.print(strState);

  previousMillis = millis();

}


void stateToString(byte boardState[]) {

  strState = "";

  for (int i = 0; i < 8; i++) {
    strState += boardState[i];
    strState += ".";
  }

}


/*


  //printBoard takes byte array of current board state and creates 8x8 bitmap of the board and prints it
  void printBoard(byte state[]) {
  int mask = 1;
  int bitMap[8][8];
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      bitMap[i][j] = (state[j] & (mask << i)) != 0;
    }
  }

  Serial.println("");
  Serial.println("- - - - - - - - - -");
  for (int i = 7; i >= 0; i--) {
    Serial.print("|");
    for (int j = 0; j <= 7; j++) {
      Serial.print(bitMap[i][j]);
      Serial.print("|");
    }
    Serial.println("");
  }
  }

  void ledSwap(boolean whichLED) {
  if (whichLED) {
    digitalWrite(led2, LOW);
    digitalWrite(led1, HIGH);
  }
  else {
    digitalWrite(led2, HIGH);
    digitalWrite(led1, LOW);
  }
  }

*/
