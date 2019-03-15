int RECV_PIN = 11;
//motor pins
int M1a = 10;
int M1b = 9;
int M2a = 6;
int M2b = 5;
int orangePin = 7;
int bluePin = 2;
int whitePin = 4;


//servo
long int check = 0;
int servoTime = 500;
int pos = 0;
#include <Servo.h>

Servo myservo;

//Values controlling the movement
//Forward Backwards
float FB = 0;
//Left and Right
float LR = 0;

//Value for the last button pressed
int pressed = 0x00;
int piezoPin = 3;


#include <IRremote.h>


IRrecv irrecv(RECV_PIN);
decode_results results;

int irCheck(boolean Send) {
  int value = 0x00;
  if (irrecv.decode(&results)) {
    if (Send) {
      Serial.println(results.value, HEX);
    }
    value = results.value;
    irrecv.resume(); // Receive the next value
  }
  return (value);
  delay(100);
}



//Here I'm just making an array of buttons so we can just call numbers easily when checking things.
//If you want to add more buttons, jsut add to the initial array value and add the button hex code to the array. Make sure to add the name into the comment next to it so we can refer to it later.
//also don't forget to change irCheck(false) to true. it will let you see the values you need to add the the array.

int IRButtons[9] = {0xFDA05F, 0xFDB04F, 0xFD10EF, 0xFD50AF, 0xFD906F, 0xFD08F7, 0xFD8877, 0xFD40BF, 0xFD40BF};
//UP[0], DOWN[1], LEFT[2], RIGHT[3], CENTER[4]

void setup() {
  Serial.begin(9600);
  myservo.attach(8);
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");
  pinMode(whitePin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(piezoPin, OUTPUT);


}

void loop() {



  //TEMP value for the Reading
  //irCheck is a function I made that holds the ir reading function and returns an int holding the sent data.
  //the false in the irCheck function enables the function to print to the Serial monitor. This is useful for adding new button controls.
  int currentVal = irCheck(true);
  //0xFFFFFFFF is the int value given when you hold a button down. So you can continuiously hold down a button and it'll read.
  if (currentVal == 0xFFFFFFFF) {
    //Serial.println("HELD");
    if (pressed == IRButtons[0]) Up();
    if (pressed == IRButtons[1]) Down();
    if (pressed == IRButtons[2]) Left();
    if (pressed == IRButtons[3]) Right();
    if (pressed == IRButtons[4]) Stop();
    if (pressed == IRButtons[5]) LightsOn();
    if (pressed == IRButtons[6]) LightsOff();
    if (pressed == IRButtons[7]) beep();
    if (pressed == IRButtons[8]) wave();
  } else if (currentVal != 0) {
    //stores the initial button press.
    pressed = currentVal;
  }
  //If statements and ternary operators that handle the resetting to default values.
  //Serial.println(FB);
  if (FB != 0) FB = FB < 0 ? FB += .7 : FB -= .7;
  if (LR != 0) LR = LR < 0 ? LR += .7 : LR -= .7;

  //custom Function that handles the motor analog write functions
  UpdateMotors();
  LightsOnIfNotMoving();
}
bool Stopped = true;

void beep() {
  delayMicroseconds(sin(micros()*PI / 100) * 1000);
  digitalWrite(piezoPin, HIGH);
  delayMicroseconds(sin(micros()*PI / 100) * 1000);
  digitalWrite(piezoPin, LOW);
}

void UpdateMotors() {
  //LEFT MOTOR
  analogWrite(M1a, constrain(FB  - LR, 0, 255));
  analogWrite(M1b, constrain(-FB + LR, 0, 255));
  //RIGHT MOTOR
  analogWrite(M2a, constrain(FB  + LR, 0, 255));
  analogWrite(M2b, constrain(-FB - LR, 0, 255));
}

//Functions to handle the button presses.
void Up() {
  setColor(0, 255, 0);
  Serial.println("Up");
  FB = constrain(FB + 50, 0, 255);

}

void Down() {
  setColor(0, 0, 255);
  Serial.println("Down");
  FB = constrain(FB - 50, -255, 0);



}

void Left() {
  setColor(255, 0, 0);
  Serial.println("Left");
  LR = constrain(LR + 50, 0, 255);

}

void Right() {
  setColor(255, 0, 0);
  Serial.println("Right");
  LR = constrain(LR - 50, -255, 0);
}


void Stop() {
  Serial.println("Stop");
  FB = 0;
  LR = 0;

  LightsOn();


}


void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(orangePin, redValue);
  analogWrite(bluePin, greenValue);
  analogWrite(whitePin, blueValue);
}

void LightsOnIfNotMoving() {
  if (abs(LR) < 1 && abs(FB) < 1) {
    LeftOn();
    RightOn();
    wave();
  }
  else {
    if (LR > 1) {
      LeftOn();
    } else {
      LeftOff();
    }
    if (LR < -1) {
      RightOn();
    } else {
      RightOff();
    }
  }

}

void LeftOn() {
  digitalWrite(13, LOW);

}
void LeftOff() {
  digitalWrite(13, HIGH);

}

void RightOn() {
  digitalWrite(12, LOW);
}
void RightOff() {
  digitalWrite(12, HIGH);
}
void LightsOn() {
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  Serial.println("LightsOn");
  setColor(255, 0, 0);

}

void LightsOff() {
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
  Serial.println("LightsOn");
  setColor(0, 0, 0);
}

void wave() {
  if (millis() - check < servoTime) {
    myservo.write(map(millis() - check, 0, servoTime, 0, 60));
  } else if (millis() - check < servoTime * 2) {
    myservo.write(map(millis() - check, servoTime, servoTime * 2, 60, 0));
  } else {
    check = millis();
  }
}

