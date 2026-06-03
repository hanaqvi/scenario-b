// Motor pins (Role 3)
const int SIDE_P1 = 5;
const int SIDE_P2 = 6;
const int MOTOR_C_PIN_1 = 4;
const int MOTOR_C_PIN_2 = 5;
const int MOTOR_D_PIN_1 = 6;
const int MOTOR_D_PIN_2 = 7;

// Motor pins (Role 4)

// Motor A = Left
const int MOTOR_A_PWM = 3;
const int MOTOR_A_DIR = 12;
const int MOTOR_A_BRAKE = 9;

// Motor B = Right
const int MOTOR_B_PWM = 11;
const int MOTOR_B_DIR = 13;
const int MOTOR_B_BRAKE = 8;

const int TEST_SPEED = 50;

// Motor encoder pins
const int ENCODER_A_CHAN_A = 20;
const int ENCODER_A_CHAN_B = 26;

const int ENCODER_B_CHAN_A = 21;
const int ENCODER_B_CHAN_B = 28;

const int ENCODER_C_CHAN_A = 18;
const int ENCODER_C_CHAN_B = 22;

const int ENCODER_D_CHAN_A = 19;
const int ENCODER_D_CHAN_B = 24;

// Volatile pulse counters for the ISRs
volatile long encoderACount = 0;
volatile long encoderBCount = 0;
volatile long encoderCCount = 0;
volatile long encoderDCount = 0;

// LED pins
const int FRONT1_LED_PIN = 48;
const int FRONT2_LED_PIN = 46;
const int BACK1_LED_PIN = 44;
const int BACK2_LED_PIN = 42;

// Photodiode pins
const int FRONT_DIODE_PIN = A10;
const int BACK_DIODE_PIN = A11;

// Side switch pins
const int LEFT_SWITCH_PIN = 50;
const int RIGHT_SWITCH_PIN = 52;

// Circular buffer for gap detection
const int BUFFER_LENGTH = 10;
int photodiodeBuffer[BUFFER_LENGTH];

// Index of next value to be inserted in buffer
int bufferHead = 0;

// Number of readings to check to determine if there has been a consistent gap, rather than a stalagmite
const int BUFFER_CHECK_SIZE = 5;

// If diode reading is BELOW this it means there is some kind of gap (stalagmite or big gap) (arbtiraty for now)
const int GAP_DETECTION_THRESHOLD = 40;

// If diode reading is ABOVE this it means the robot is too close to the front wall (arbitraty for now)
const int WALL_DETECTION_THRESHOLD = 10;
const float IDEAL_DISTANCE_FROM_WALL = 2;

// Tells the loop whether the robot should move left or right - begins by moving left
bool movingLeft = true;

// Tells the loop what behaviour to execute
enum BotState {
  SEARCHING_FOR_GAP, // Moving left to right
  REVERSING,
  MOVING_FORWARD,
  TESTING,
};

// Begin by searching for a gap
BotState currentState = MOVING_FORWARD;

void setup() {

Serial.begin(9600);

  pinMode(SIDE_P1, OUTPUT);
  pinMode(SIDE_P2, OUTPUT);
  pinMode(MOTOR_C_PIN_1, OUTPUT);
  pinMode(MOTOR_C_PIN_2, OUTPUT);
  pinMode(MOTOR_D_PIN_1, OUTPUT);
  pinMode(MOTOR_D_PIN_2, OUTPUT);

  pinMode(MOTOR_A_PWM, OUTPUT);
  pinMode(MOTOR_A_DIR, OUTPUT);
  pinMode(MOTOR_A_BRAKE, OUTPUT);
  pinMode(MOTOR_B_PWM, OUTPUT);
  pinMode(MOTOR_B_DIR, OUTPUT);
  pinMode(MOTOR_B_BRAKE, OUTPUT);

  // Lock vertical breaks on startup for safety
  engageVerticalBrakes();

  pinMode(ENCODER_A_CHAN_A, INPUT_PULLUP);
  pinMode(ENCODER_A_CHAN_B, INPUT_PULLUP);
  pinMode(ENCODER_B_CHAN_A, INPUT_PULLUP);
  pinMode(ENCODER_B_CHAN_B, INPUT_PULLUP);
  pinMode(ENCODER_C_CHAN_A, INPUT_PULLUP);
  pinMode(ENCODER_C_CHAN_B, INPUT_PULLUP);
  pinMode(ENCODER_D_CHAN_A, INPUT_PULLUP);
  pinMode(ENCODER_D_CHAN_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENCODER_A_CHAN_A), updateEncoderA, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B_CHAN_A), updateEncoderB, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_C_CHAN_A), updateEncoderC, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_D_CHAN_A), updateEncoderD, RISING);

  pinMode(FRONT1_LED_PIN, OUTPUT);
  pinMode(FRONT2_LED_PIN, OUTPUT);
  pinMode(BACK1_LED_PIN, OUTPUT);
  pinMode(BACK2_LED_PIN, OUTPUT);

  // Turn on all LEDs
  digitalWrite(FRONT1_LED_PIN, HIGH);
  digitalWrite(FRONT2_LED_PIN, HIGH);
  digitalWrite(BACK1_LED_PIN, HIGH);
  digitalWrite(BACK2_LED_PIN, HIGH);

  pinMode(FRONT_DIODE_PIN, INPUT);
  pinMode(BACK_DIODE_PIN, INPUT);

  pinMode(LEFT_SWITCH_PIN, INPUT);
  pinMode(RIGHT_SWITCH_PIN, INPUT);

  // Populate buffer with high values to prevent false gap alert at the start
  for (int i = 0; i < BUFFER_LENGTH; i++) {

    photodiodeBuffer[i] = 1023;

  }

}

void loop() {

  switch(currentState) {

    case SEARCHING_FOR_GAP:

      if (checkPathClear()) { // If there is a gap, prepare robot to move through it

        currentState = MOVING_FORWARD; // Change state
        movingLeft = true; // Reset movement for next level

      }
      else if (nearWall(FRONT_DIODE_PIN)) { // If robot is too close to front wall prepare to reverse

        currentState = REVERSING;

      } 
      else if (nearWall(BACK_DIODE_PIN)) { // If robot is too close to back wall move forward

        currentState = MOVING_FORWARD;

      }
      else { // Continue to move left or right if no gap or reversing is required

        moveSideways();

      }

      break;

    case REVERSING:

      if (nearWall(FRONT_DIODE_PIN)) { // Move backwards while wall is too close

        moveBackward();

      }
      else { // Robot is safe distance away from wall, go back to searching for gaps

        engageVerticalBrakes();
        currentState = SEARCHING_FOR_GAP;

      }

      break;

    case MOVING_FORWARD:

      if (nearWall(FRONT_DIODE_PIN)) { // If robot has reached a front wall at the next level, begin looking for another gap
        engageVerticalBrakes();
        currentState = SEARCHING_FOR_GAP;

      }
      else { // Continue to move forward

        moveForward();

      } 

      break;
    
    // test rotating case
    case TESTING:
      
      moveLeft();
      moveRight();
      moveForward();
      moveBackward();
      while (true) {
        delay(10);
      }
  }

}

void moveBackward() {
  digitalWrite(MOTOR_A_BRAKE, LOW);
  digitalWrite(MOTOR_B_BRAKE, LOW);
  digitalWrite(MOTOR_A_DIR, HIGH);
  digitalWrite(MOTOR_B_DIR, LOW);
  analogWrite(MOTOR_A_PWM, TEST_SPEED);
  analogWrite(MOTOR_B_PWM, TEST_SPEED);
}

void moveForward() {
  digitalWrite(MOTOR_A_BRAKE, LOW);
  digitalWrite(MOTOR_B_BRAKE, LOW);
  digitalWrite(MOTOR_A_DIR, LOW);
  digitalWrite(MOTOR_B_DIR, HIGH);
  analogWrite(MOTOR_A_PWM, TEST_SPEED);
  analogWrite(MOTOR_B_PWM, TEST_SPEED);
}

void moveLeft() {
  analogWrite(MOTOR_C_PIN_2, TEST_SPEED);
  analogWrite(MOTOR_D_PIN_1, TEST_SPEED);
}

void moveRight() {
  analogWrite(MOTOR_C_PIN_1, TEST_SPEED);
  analogWrite(MOTOR_D_PIN_2, TEST_SPEED);
}

void rotate180() {

  Serial.println("ROTATING");
  engageVerticalBrakes();
  int startPosition = encoderACount;
  int endPosition = encoderBCount;

  digitalWrite(MOTOR_A_BRAKE, LOW);
  digitalWrite(MOTOR_B_BRAKE, LOW);   
  digitalWrite(MOTOR_A_DIR, HIGH); 
  digitalWrite(MOTOR_B_DIR, HIGH);    
  analogWrite(MOTOR_A_PWM, TEST_SPEED);
  analogWrite(MOTOR_B_PWM, TEST_SPEED);

  delay(3000);
  engageVerticalBrakes();

}

// Move left or right depending on the current value of movingLeft
void moveSideways() {
  if (movingLeft) {
    if (digitalRead(LEFT_SWITCH_PIN) == LOW) {
      analogWrite(MOTOR_C_PIN_1, 0); // stop right motor
      analogWrite(MOTOR_D_PIN_2, 0);
      moveLeft();
    } else {
      movingLeft = false;
      analogWrite(MOTOR_C_PIN_2, 0); // stop left motor
      analogWrite(MOTOR_D_PIN_1, 0);
    }
  } else {
    if (digitalRead(RIGHT_SWITCH_PIN) == LOW) {
      analogWrite(MOTOR_C_PIN_2, 0); // stop left motor
      analogWrite(MOTOR_D_PIN_1, 0);
      moveRight();
    } else {
      movingLeft = true;
      analogWrite(MOTOR_C_PIN_1, 0); // stop right motor
      analogWrite(MOTOR_D_PIN_2, 0);
    }
  }
}

// Find the distance to the wall from the front or back sensor
float distanceToWall(int diodePin) {

  float distance;

  // Different equation depending on front or back wall
  if (diodePin == FRONT_DIODE_PIN) {

    distance = sqrt(1094.8427 / (analogRead(diodePin) - 32.6798));
    //Serial.println("Distance to front wall: " + distance);

  } else if (diodePin == BACK_DIODE_PIN) {

    distance = sqrt(1312.7629 / (analogRead(diodePin) - 34.2690));
    //Serial.println("Distance to back wall: " + distance);

  }

  return distance;

}

bool checkPathClear() {

  // Read front photodiode and store in buffer
  photodiodeBuffer[bufferHead] = analogRead(FRONT_DIODE_PIN);
  bufferHead = (bufferHead + 1) % BUFFER_LENGTH;

  // Check if the last 5 readings (buffer_check_size) are all below the threshold so that stalagmites are not detected as a full gap
  // 5 is a filler number, this value needs to be tested in practice
  for (int i = 1; i <= BUFFER_CHECK_SIZE; i++) {

    int index = (bufferHead - i + BUFFER_LENGTH) % BUFFER_LENGTH;

    // If any of the last 5 values are high assume there is no gap
    if (photodiodeBuffer[index] >= GAP_DETECTION_THRESHOLD) { 

      return false;

    } 
  }

  // There is a gap
  return true;

}


// Check if the robot is too close to the front or back wall
bool nearWall(int diodePin) {

  return distanceToWall(diodePin) < IDEAL_DISTANCE_FROM_WALL;

}

// Stops both motors instantly
void engageVerticalBrakes() {

  analogWrite(MOTOR_A_PWM, 0);
  analogWrite(MOTOR_B_PWM, 0);
  digitalWrite(MOTOR_A_BRAKE, HIGH);
  digitalWrite(MOTOR_B_BRAKE, HIGH);

}

// Encoder ISRs
void updateEncoderA() {
  if (digitalRead(ENCODER_A_CHAN_B) == LOW) encoderACount++;
  else encoderACount--;
}

void updateEncoderB() {
  if (digitalRead(ENCODER_B_CHAN_B) == LOW) encoderBCount++;
  else encoderBCount--;
}

void updateEncoderC() {
  if (digitalRead(ENCODER_C_CHAN_B) == LOW) encoderCCount++;
  else encoderCCount--;
}

void updateEncoderD() {
  if (digitalRead(ENCODER_D_CHAN_B) == LOW) encoderDCount++;
  else encoderDCount--;
}
