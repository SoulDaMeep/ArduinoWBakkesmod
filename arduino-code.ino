const int yellowPin = 13;
const int greenPin = 12;
const int redPin = 11;
const int bluePin = 10;

void setup() {
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  Serial.begin(9600);
}

void loop() {

  if (Serial.available() > 0) {


    char data = Serial.read(); // gets the data from the serial port


    //Write File commands from Bakkesmod Plugin
    if(data == '1') {
      digitalWrite(yellowPin, HIGH);
      delay(1000);
      digitalWrite(yellowPin, LOW);
    }
    if(data == '2') {
      digitalWrite(greenPin, HIGH);
      delay(60);
      digitalWrite(greenPin, LOW);
    }
    if(data == '3') {
      digitalWrite(redPin, HIGH);
      delay(2000);
      digitalWrite(redPin, LOW);
    }
    if(data == '4') {
      digitalWrite(bluePin, HIGH);
      delay(3000);
      digitalWrite(bluePin, LOW);
    }
  }
}
