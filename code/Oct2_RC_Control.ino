    #include <EnableInterrupt.h>
    
    #define SERIAL_PORT_SPEED 9600
    #define RC_NUM_CHANNELS  4
    
    #define RC_CH1  0
    #define RC_CH2  1
    #define RC_CH3  2
    #define RC_CH4  3

    // pin usage
    // radio pins
    #define RC_CH1_PIN  2
    #define RC_CH2_PIN  4
    #define RC_CH3_PIN  7
    #define RC_CH4_PIN  8
    
    // motor driver pins
    const int IN1_PIN = 3;
    const int IN2_PIN = 5;
    const int IN3_PIN = 6;
    const int IN4_PIN = 9;


    
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
    
    void setup() {
      Serial.begin(SERIAL_PORT_SPEED);
    
      pinMode(RC_CH1_PIN, INPUT); //side wheel
      pinMode(RC_CH2_PIN, INPUT); //trigger
      pinMode(RC_CH3_PIN, INPUT); //side knob
      pinMode(RC_CH4_PIN, INPUT); //bottom button
    
      enableInterrupt(RC_CH1_PIN, calc_ch1, CHANGE);
      enableInterrupt(RC_CH2_PIN, calc_ch2, CHANGE);
      enableInterrupt(RC_CH3_PIN, calc_ch3, CHANGE);
      enableInterrupt(RC_CH4_PIN, calc_ch4, CHANGE);
    
      pinMode(3, OUTPUT);
      pinMode(5, OUTPUT);
      pinMode(6, OUTPUT);
      pinMode(9, OUTPUT);
    }
    
    void loop() {
      rc_read_values();
    
      Serial.print("CH1:"); Serial.print(rc_values[RC_CH1]); Serial.print("\t");
      Serial.print("CH2:"); Serial.print(rc_values[RC_CH2]); Serial.print("\t");
      Serial.print("CH3:"); Serial.print(rc_values[RC_CH3]); Serial.print("\t");
      Serial.print("CH4:"); Serial.println(rc_values[RC_CH4]);
    
      delay(200);
    
    // robot moving forward and in reverse using Hobby RC controller
      if (rc_values[RC_CH3] > 1700) forward(map(rc_values[RC_CH3], 1700, 2300, 0, 255));
      if (rc_values[RC_CH3] < 1300) reverse(map(rc_values[RC_CH3], 300, 1300, 0, 255));
      if ((rc_values[RC_CH3] < 1700) && (rc_values[RC_CH3] > 1300)) stop();
    
    // robot moving right and left using Hobby RC controller
    
      if (rc_values[RC_CH4] > 1700) right(map(rc_values[RC_CH4], 1700, 3000, 0, 255));
      if (rc_values[RC_CH4] < 1300) left (map(rc_values[RC_CH4], 0, 1300, 0, 255));
    
    }
    
    void forward(int value) {
    
     analogWrite(IN1_PIN, 0);
     analogWrite(IN2_PIN, value);
     analogWrite(IN3_PIN, 0);
     analogWrite(IN4_PIN, value);
      
    }
    
    void reverse(int value) {
    
     analogWrite(IN1_PIN, value);
     analogWrite(IN2_PIN, 0);
     analogWrite(IN3_PIN, value);
     analogWrite(IN4_PIN, 0);
      
    }
    
    void right(int value) {
    
     analogWrite(IN1_PIN, 0);
     analogWrite(IN2_PIN, value);
     analogWrite(IN3_PIN, 0);
     analogWrite(IN4_PIN, 0);
      
    }
    
    void left(int value) {
    
     analogWrite(IN1_PIN, 0);
     analogWrite(IN2_PIN, 0);
     analogWrite(IN3_PIN, 0);
     analogWrite(IN4_PIN, value);
      
    }
    
    
    void stop(){
      digitalWrite(IN1_PIN, LOW);
      digitalWrite(IN2_PIN, LOW);
      digitalWrite(IN3_PIN, LOW);
      digitalWrite(IN4_PIN, LOW);
    }
