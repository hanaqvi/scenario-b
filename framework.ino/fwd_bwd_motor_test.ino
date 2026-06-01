// --- Motor A Pins (Left Side) ---
const int MOTOR_A_PWM = 3;    // Speed control
const int MOTOR_A_DIR = 12;   // Direction control
const int MOTOR_A_BRAKE = 9;  // Brake control

// --- Motor B Pins (Right Side) ---
const int MOTOR_B_PWM = 11;   // Speed control
const int MOTOR_B_DIR = 13;   // Direction control
const int MOTOR_B_BRAKE = 8;  // Brake control

// Test speed (0 to 255). 
// 150 is a good, safe speed for a bench test without drawing massive current spikes.
const int TEST_SPEED = 150; 

void setup() {
  // Initialize all motor control pins as outputs
  pinMode(MOTOR_A_PWM, OUTPUT);
  pinMode(MOTOR_A_DIR, OUTPUT);
  pinMode(MOTOR_A_BRAKE, OUTPUT);
  
  pinMode(MOTOR_B_PWM, OUTPUT);
  pinMode(MOTOR_B_DIR, OUTPUT);
  pinMode(MOTOR_B_BRAKE, OUTPUT);

  // Ensure brakes are engaged and speed is 0 at startup for safety
  engageBrakes();
  
  // A brief delay before the sequence starts so you can step back
  delay(3000); 
}

void loop() {
  // ---------------------------------------------------------
  // PHASE 1: Test Motor A (Forward and Backward)
  // ---------------------------------------------------------
  
  // Motor A Forward
  digitalWrite(MOTOR_A_BRAKE, LOW);  // Disengage brake
  digitalWrite(MOTOR_A_DIR, HIGH);   // Set direction Forward
  analogWrite(MOTOR_A_PWM, TEST_SPEED); // Spin up
  delay(2000); // Run for 2 seconds
  
  // Stop Motor A
  engageBrakes();
  delay(1000); // Pause for 1 second
  
  // Motor A Backward
  digitalWrite(MOTOR_A_BRAKE, LOW);  // Disengage brake
  digitalWrite(MOTOR_A_DIR, LOW);    // Set direction Backward (Reversed)
  analogWrite(MOTOR_A_PWM, TEST_SPEED); // Spin up
  delay(2000); // Run for 2 seconds
  
  // Stop Motor A
  engageBrakes();
  delay(2000); // Longer pause before switching motors

  // ---------------------------------------------------------
  // PHASE 2: Test Motor B (Forward and Backward)
  // ---------------------------------------------------------
  
  // Motor B Forward
  digitalWrite(MOTOR_B_BRAKE, LOW);  
  digitalWrite(MOTOR_B_DIR, HIGH);   
  analogWrite(MOTOR_B_PWM, TEST_SPEED); 
  delay(2000); 
  
  // Stop Motor B
  engageBrakes();
  delay(1000); 
  
  // Motor B Backward
  digitalWrite(MOTOR_B_BRAKE, LOW);  
  digitalWrite(MOTOR_B_DIR, LOW);    
  analogWrite(MOTOR_B_PWM, TEST_SPEED); 
  delay(2000); 
  
  // Stop Motor B
  engageBrakes();
  delay(4000); // Long pause before the whole loop repeats
}

// --- Helper Function ---
// A clean way to stop both motors instantly
void engageBrakes() {
  analogWrite(MOTOR_A_PWM, 0);
  analogWrite(MOTOR_B_PWM, 0);
  digitalWrite(MOTOR_A_BRAKE, HIGH);
  digitalWrite(MOTOR_B_BRAKE, HIGH);
}