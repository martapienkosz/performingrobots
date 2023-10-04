#include <EnableInterrupt.h>

#define SERIAL_PORT_SPEED 9600
#define RC_NUM_CHANNELS  4

#define RC_CH1  0
#define RC_CH2  1
#define RC_CH3  2
#define RC_CH4  3

#define RC_CH1_PIN  8
#define RC_CH2_PIN  7
#define RC_CH3_PIN  4
#define RC_CH4_PIN  2

uint16_t rc_values[RC_NUM_CHANNELS];
uint32_t rc_start[RC_NUM_CHANNELS];
volatile uint16_t rc_shared[RC_NUM_CHANNELS];

void rc_read_values() {
  noInterrupts();
  memcpy(rc_values, (const void *)rc_shared, sizeof(rc_shared));
  interrupts();
}

void calc_input(uint8_t channel, uint8_t input_pin) {
  if (digitalRead(input_pin) == HIGH) {
    rc_start[channel] = micros();
  } else {
    uint16_t rc_compare = (uint16_t)(micros() - rc_start[channel]);
    rc_shared[channel] = rc_compare;
  }
}

void calc_ch1() {
  calc_input(RC_CH1, RC_CH1_PIN);
}
void calc_ch2() {
  calc_input(RC_CH2, RC_CH2_PIN);
}
void calc_ch3() {
  calc_input(RC_CH3, RC_CH3_PIN);
}
void calc_ch4() {
  calc_input(RC_CH4, RC_CH4_PIN);
}

// DEFINE PINS
#define motor1A 8
#define motor1B 7
#define motor2A 4
#define motor2B 2

// FORWARD FUNCTION
void forward() {
  digitalWrite(motor1A, LOW);
  digitalWrite(motor1B, HIGH);
  digitalWrite(motor2A, LOW);
  digitalWrite(motor2B, HIGH);
  Serial.println("FORWARD");
}

// BACKWARD FUNCTION
void backward() {
  digitalWrite(motor1A, HIGH);
  digitalWrite(motor1B, LOW);
  digitalWrite(motor2A, HIGH);
  digitalWrite(motor2B, LOW);
  Serial.println("BACKWARD");
}

// TURN RIGHT FUNCTION
void turnRight() {
  digitalWrite(motor1A, LOW);
  digitalWrite(motor1B, HIGH);
  digitalWrite(motor2A, HIGH);
  digitalWrite(motor2B, LOW);
  Serial.println("RIGHT");
}

// TURN LEFT FUNCTION
void turnLeft() {
  digitalWrite(motor1A, HIGH);
  digitalWrite(motor1B, LOW);
  digitalWrite(motor2A, LOW);
  digitalWrite(motor2B, HIGH);
  Serial.println("LEFT");
}

// STOP MOTORS FUNCTION
void stop() {
  digitalWrite(motor1A, LOW);
  digitalWrite(motor1B, LOW);
  digitalWrite(motor2A, LOW);
  digitalWrite(motor2B, LOW);
  Serial.println("STOP");
}

void setup() {
  Serial.begin(SERIAL_PORT_SPEED);

  pinMode(RC_CH1_PIN, INPUT);
  pinMode(RC_CH2_PIN, INPUT);
  pinMode(RC_CH3_PIN, INPUT);
  pinMode(RC_CH4_PIN, INPUT);

  enableInterrupt(RC_CH1_PIN, calc_ch1, CHANGE);
  enableInterrupt(RC_CH2_PIN, calc_ch2, CHANGE);
  enableInterrupt(RC_CH3_PIN, calc_ch3, CHANGE);
  enableInterrupt(RC_CH4_PIN, calc_ch4, CHANGE);

  // INITIALIZE PINS AS INPUT
  pinMode(motor1A, INPUT);
  pinMode(motor1B, INPUT);
  pinMode(motor2A, INPUT);
  pinMode(motor2B, INPUT);
}

void loop() {
  rc_read_values();

  // PRINT CHANNEL NUMBERS
  // Serial.print("CH1:"); Serial.print(rc_values[RC_CH1]); Serial.print("\t");
  // Serial.print("CH2:"); Serial.println(rc_values[RC_CH2]);

  if (rc_values[RC_CH2] > 1700) {
    forward();
  } else if (rc_values[RC_CH2] < 1300) {
    backward();
  } else if (rc_values[RC_CH1] > 1700) {
    turnRight();
  } else if (rc_values[RC_CH1] < 1300) {
    turnLeft();
  } else {
    stop();
  }


  delay(200);
}
