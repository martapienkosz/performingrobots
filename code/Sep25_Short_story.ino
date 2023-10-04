void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

}

void loop() {

  for (int i = 0; i < 3; i = i+1) {
    backward(500); // move 0.5m
    stop(2000);
  }

  turnClockwise(300); // turn 90 degrees
  stop(1000);
  backward(300);
  stop(2000);
  turnClockwise(300);
  stop(1000);

  for (int i = 0; i < 2; i = i+1) {
    backward(500);
    stop(2000);
  }

  antiClockwise(300);
  stop(1000);
  backward(300);
  stop(2000);
  antiClockwise(300);
  stop(1000);

  for (int i = 0; i < 2; i = i+1) {
    backward(500);
    stop(2000);
  }
}

void turnClockwise (int duration) {
  digitalWrite(2, LOW);
  digitalWrite(3, HIGH);
  digitalWrite(5, LOW);
  digitalWrite(4, HIGH);
  delay(duration);
}

void antiClockwise (int duration) {
  digitalWrite(3, LOW);
  digitalWrite(2, HIGH);
  digitalWrite(4, LOW);
  digitalWrite(5, HIGH);
  delay(duration);
}

void stop(int duration) {
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  delay(duration);
}

void forward (int duration) {
  digitalWrite(2, HIGH);
  digitalWrite(3, LOW);
  digitalWrite(4, HIGH);
  digitalWrite(5, LOW);
  delay(duration);
}

void backward (int duration) {
  digitalWrite(3, HIGH);
  digitalWrite(2, LOW);
  digitalWrite(5, HIGH);
  digitalWrite(4, LOW);
  delay(duration);
}
