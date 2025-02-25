#include <Stepper.h>

// Define the number of steps per revolution for your stepper motor
#define STEPS_PER_REV 2048 // For 5VDC Stepper Motor (28BYJ-48)

// Define GPIO pins connected to the motor driver
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5

Stepper stepperMotor(STEPS_PER_REV, IN1, IN3, IN2, IN4);

// Variables to manage motor state
bool isRunning = false;
int direction = 1; // 1 for CW, -1 for CCW

void setup() {
  // Speed of the stepper motor (RPM)
  stepperMotor.setSpeed(2.5); // Adjust speed as needed

  Serial.begin(9600);
  Serial.println("Stepper Motor Ready!");
  Serial.println("Press 'w' for CW, 's' for CCW, and 'x' to stop.");
}

void loop() {
  // Check if user entered data in the Serial Monitor
  if (Serial.available() > 0) {
    char input = Serial.read(); // Read a single character input
    input = tolower(input);

    if (input == 'w') {
      direction = 1;
      isRunning = true;
      Serial.println("Rotating clockwise...");
    } else if (input == 's') {
      direction = -1;
      isRunning = true;
      Serial.println("Rotating counterclockwise...");
    } else if (input == 'x') {
      isRunning = false;
      Serial.println("Stopping motor...");
    } else {
      Serial.println("Invalid input. Press 'w' for CW, 's' for CCW, and 'x' to stop.");
    }
  }

  // Rotate the motor continuously while `isRunning` is true
  if (isRunning) {
    stepperMotor.step(direction); // Step in the specified direction
  }
}
