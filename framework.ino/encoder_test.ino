// --- Motor A Encoder Pins ---
const int ENCODER_A_CHAN_A = 20;  // Hardware Interrupt Pin on Arduino Mega
const int ENCODER_A_CHAN_B = 26;  // Standard Digital Pin

// --- Motor B Encoder Pins ---
const int ENCODER_B_CHAN_A = 21;  // Hardware Interrupt Pin on Arduino Mega
const int ENCODER_B_CHAN_B = 28;  // Standard Digital Pin

// --- Pulse Counters ---
// The 'volatile' keyword tells the compiler these variables 
// can change at any time outside the main loop (via the ISR).
volatile long encoderACount = 0;
volatile long encoderBCount = 0;

void setup() {
  Serial.begin(9600);

  // Set pins as inputs with internal pull-up resistors. 
  // This prevents floating signals and gives cleaner readings.
  pinMode(ENCODER_A_CHAN_A, INPUT_PULLUP);
  pinMode(ENCODER_A_CHAN_B, INPUT_PULLUP);
  pinMode(ENCODER_B_CHAN_A, INPUT_PULLUP);
  pinMode(ENCODER_B_CHAN_B, INPUT_PULLUP);

  // Attach the hardware interrupts.
  // Whenever Pin 20 or Pin 21 goes from LOW to HIGH (RISING edge), 
  // the Arduino instantly pauses and runs the designated update function.
  attachInterrupt(digitalPinToInterrupt(ENCODER_A_CHAN_A), updateEncoderA, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B_CHAN_A), updateEncoderB, RISING);
}

void loop() {
  // Print the current counts to the Serial Monitor every half second
  Serial.print("Motor A Count: ");
  Serial.print(encoderACount);
  Serial.print("  |  Motor B Count: ");
  Serial.println(encoderBCount);
  
  delay(500); 
}

// --- Interrupt Service Routine for Motor A ---
void updateEncoderA() {
  // When Channel A rises, we check the state of Channel B.
  // The state of Channel B tells us the direction of rotation.
  if (digitalRead(ENCODER_A_CHAN_B) == LOW) {
    encoderACount++; // Spinning Forward
  } else {
    encoderACount--; // Spinning Backward
  }
}

// --- Interrupt Service Routine for Motor B ---
void updateEncoderB() {
  if (digitalRead(ENCODER_B_CHAN_B) == LOW) {
    encoderBCount++;
  } else {
    encoderBCount--;
  }
}