// Motor pins
// to be added

// LED pins
const int FRONT1_LED_PIN = 48;
const int FRONT2_LED_PIN = 46;
const int BACK1_LED_PIN = 44;
const int BACK2_LED_PIN = 42;

// Photodiode pins
const int FRONT_DIODE_PIN = A10;
const int BACK_DIODE_PIN = A11;

// switch pins
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
const int GAP_DETECTION_THRESHOLD = 2.0;

// If diode reading is ABOVE this it means the robot is too close to the front wall (arbitraty for now)
const int WALL_DETECTION_THRESHOLD = 10.0;

// Tells the loop whether the robot should move left or right - begins by moving left
bool movingLeft = true;

// Tells the loop what behaviour to execute
enum BotState {
  SEARCHING_FOR_GAP, // Moving left to right
  REVERSING,
  MOVING_THROUGH_GAP
};

// Begin by searching for a gap
BotState currentState = SEARCHING_FOR_GAP;

void setup() {

  pinMode(FRONT1_LED_PIN, OUTPUT);
  pinMode(FRONT2_LED_PIN, OUTPUT);
  pinMode(BACK1_LED_PIN, OUTPUT);
  pinMode(BACK2_LED_PIN, OUTPUT);

  pinMode(FRONT_DIODE_PIN, INPUT);
  pinMode(BACK_DIODE_PIN, INPUT);

  pinMode(LEFT_SWITCH_PIN, INPUT);
  pinMode(RIGHT_SWITCH_PIN, INPUT);

  // populate buffer with high value to prevent false gap alert at the start
  // could be changed as robot might start outside of the maze
  for (int i = 0; i < BUFFER_LENGTH; i++) {

    photodiodeBuffer[i] = 1023.0;

  }

}

void loop() {

  switch(currentState) {

    case SEARCHING_FOR_GAP:

      if (checkPathClear()) { // If there is a gap, prepare robot to move through it

        // Change state
        currentState = MOVING_THROUGH_GAP;

      }
      else if (nearFrontWall()) { // If robot is too close to wall prepare to reverse

        currentState = REVERSING;

      }
      else { // Continue to move left or right if no gap or reversing is required

        moveSideways();

      }

      break;

    case REVERSING:

      if (nearFrontWall()) { // Move backwards while wall is too close

        moveBackward();

      }
      else { // Robot is safe distance away from wall, go back to searching for gaps

        currentState = SEARCHING_FOR_GAP;

      }

      break;

    case MOVING_THROUGH_GAP:

      if (nearFrontWall()) { // If robot has reached a front wall at the next level, begin looking for another gap

        currentState = SEARCHING_FOR_GAP;
        movingLeft = true; // Reset movement for next level

      }
      else { // Continue to move forward

        moveForward();

      } 

      break;
  }

}

// All movement functions move the robot a constant distance
void moveForward() {

}

void moveBackward() {

}

void moveLeft() {

}

void moveRight() {

}

// Move left or right depending on the current value of movingLeft
void moveSideways() {

  if (movingLeft) {

    if (digitalRead(LEFT_SWITCH_PIN) == LOW) { // If not touching left side wall move left

      moveLeft();

    }
    else { // Side wall has been touched, switch directions

      movingLeft = false;

    }

  } 
  else {

    if (digitalRead(RIGHT_SWITCH_PIN) == LOW) { // If not touching right side wall

      moveRight();

    }
    else { // Side wall has been touched, switch directions

      movingLeft = true;

    }

  }

}

bool checkPathClear() {

  // Read front photodiode and store in buffer
  photodiodeBuffer[bufferHead] = readPhotodiode(FRONT1_LED_PIN, FRONT_DIODE_PIN);
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

// Check if the robot is too close the front wall
bool nearFrontWall() {

  return readPhotodiode(FRONT1_LED_PIN, FRONT_DIODE_PIN) > WALL_DETECTION_THRESHOLD;

}

// Get a reading from a photodiode
int readPhotodiode(int ledPin, int diodePin) {

  digitalWrite(ledPin, HIGH); // Turn on LED
  delay(1); // Delay to allow value to be read
  int diodeReading = analogRead(diodePin); // Read reflected value
  digitalWrite(ledPin, LOW); // Turn off LED
  return diodeReading;

}
