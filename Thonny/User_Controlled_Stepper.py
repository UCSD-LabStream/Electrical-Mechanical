from machine import Pin
from time import sleep

# Define GPIO pins
IN1 = Pin(2, Pin.OUT)
IN2 = Pin(4, Pin.OUT)
IN3 = Pin(16, Pin.OUT)
IN4 = Pin(17, Pin.OUT)

# Define step sequence for 28BYJ-48 stepper motor
step_sequence = [
    [1, 0, 0, 0],
    [1, 1, 0, 0],
    [0, 1, 0, 0],
    [0, 1, 1, 0],
    [0, 0, 1, 0],
    [0, 0, 1, 1],
    [0, 0, 0, 1],
    [1, 0, 0, 1]
]

# Manage motor state
is_running = False
direction = 1  # 1 for CW, -1 for CCW
step_delay = 0.01  # Adjust speed (lower is faster)

# Function to move stepper motor
def step_motor(direction):
    global step_sequence
    steps = range(8) if direction == 1 else range(7, -1, -1)
    for step in steps:
        IN1.value(step_sequence[step][0])
        IN2.value(step_sequence[step][1])
        IN3.value(step_sequence[step][2])
        IN4.value(step_sequence[step][3])
        sleep(step_delay)

print("Stepper Motor Ready!")
print("Press 'w' for CW, 's' for CCW, and 'x' to stop.")

while True:
    try:
        user_input = input().strip().lower()
        if user_input == 'w':
            direction = 1
            is_running = True
            print("Rotating clockwise...")
        elif user_input == 's':
            direction = -1
            is_running = True
            print("Rotating counterclockwise...")
        elif user_input == 'x':
            is_running = False
            print("Stopping motor...")
        else:
            print("Invalid input. Press 'w' for CW, 's' for CCW, and 'x' to stop.")

        while is_running:
            step_motor(direction)
    except KeyboardInterrupt:
        print("Stopping script.")
        break
