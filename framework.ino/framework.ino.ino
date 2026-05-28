void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

  scanLeft();
  
  scanRight();

}

void moveForward(int motorPinL, int motorPinR, int motorPinB, int motorPinF) {

  digitalWrite(motorPinL, LOW);
  digitalWrite(motorPinR, HIGH);
  digitalWrite(motorPinB, LOW);
  digitalWrite(motorPinF, LOW);

}

void moveForward() {

  digitalWrite(motorPinL, LOW);
  digitalWrite(motorPinR, HIGH);
  digitalWrite(motorPinB, LOW);
  digitalWrite(motorPinF, LOW);

}

void moveBackward() {

  digitalWrite(motorPinL, HIGH);
  digitalWrite(motorPinR, LOW);
  digitalWrite(motorPinB, LOW);
  digitalWrite(motorPinF, LOW);

}

void moveLeft() {

  digitalWrite(motorPinL, LOW);
  digitalWrite(motorPinR, LOW);
  digitalWrite(motorPinB, HIGH);
  digitalWrite(motorPinF, LOW);

}

void moveRight() {

  digitalWrite(motorPinL, LOW);
  digitalWrite(motorPinR, LOW);
  digitalWrite(motorPinB, LOW);
  digitalWrite(motorPinF, HIGH);

}


void scanLeft() {

  // move left and scan for gaps

}

void scanRight() {

  // move right and scan for gaps

}






