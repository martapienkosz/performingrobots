# performingrobots
### by Marta Pienkosz

Week three | Sep 13

<img src="https://github.com/martapienkosz/performingrobots/blob/main/images/robotSketch.png" width="300">


Week four | Sep 18 & 20

<img src="https://github.com/martapienkosz/performingrobots/blob/main/images/base1.JPG" width="200"> <img src="https://github.com/martapienkosz/performingrobots/blob/main/images/base2.JPG" width="200"> <img src="https://github.com/martapienkosz/performingrobots/blob/main/images/base6.JPG" width="200">

Reading assignment: [Machines/Mechanicals in Entangled, Chris Salter](https://github.com/martapienkosz/performingrobots/blob/main/september18/README.md)

```
void loop() {

  clockwise( 5000);
  stop();
  delay(1000); // allow it to stop

  // now reverse direction
  counterClockwise( 5000);
  stop();
  delay(1000); // allow it to stop
}

void clockwise (int duration) {
  digitalWrite(2, LOW);
  digitalWrite(3, HIGH);
  delay(duration);
}

void stop() {
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
}

void counterClockwise (int duration) {
  digitalWrite(2, HIGH);
  digitalWrite(3, LOW);
  delay(duration);
}
```
