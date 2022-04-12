#include <Arduino.h>

#define BLYNK_TEMPLATE_ID  "TMPLxxxxxx"
#define BLYNK_DEVICE_NAME  "Device"
#define BLYNK_AUTH_TOKEN   "YourAuthToken"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "ssid";
char pass[] = "pass";

// Pin for Motor
// Front motor
int frontIn1 = 22;//left
int frontIn2 = 23;
int frontIn3 = 24;//right
int frontIn4 = 25;

// Back motor
int backIn1 = 26;//left
int backIn2 = 27;
int backIn3 = 28;//right
int backIn4 = 29;

//Enable pin
int enLF = 10;
int enRF = 11;
int enLR = 12;
int enRR = 13;

//speed value
int pinValue;
int pinValueAdjusted;

//GM detector sensor
int GMpin = 18;
BlynkTimer GMtimer;

int countGM;

void pushGM() {
  //execute every time setInterval is called
  detachInterrupt(digitalPinToInterrupt(GMpin));
  Blynk.virtualWrite(V4, countGM);
  countGM = 0;
  attachInterrupt(digitalPinToInterrupt(GMpin), countGM++, FALLING);
}

void setMotor(String motor, String direction) {
    //case "LF":
    if (strcmp(motor.c_str(), "LF") == 0) {
      if (strcmp(direction.c_str(), "forward") == 0) {
        digitalWrite(frontIn1, HIGH);
        digitalWrite(frontIn2, LOW);
      } if (strcmp(direction.c_str(), "backward")) {
        digitalWrite(frontIn1, LOW);
        digitalWrite(frontIn2, HIGH);
      } if (strcmp(direction.c_str(), "stop")) {
        digitalWrite(frontIn1, LOW);
        digitalWrite(frontIn2, LOW);
      }
    }
    //case "RF":
    else if (strcmp(motor.c_str(), "RF") == 0) {
      if (strcmp(direction.c_str(), "forward")) {
        digitalWrite(frontIn3, HIGH);
        digitalWrite(frontIn4, LOW);
      } if (strcmp(direction.c_str(), "backward")) {
        digitalWrite(frontIn3, LOW);
        digitalWrite(frontIn4, HIGH);
      } if (strcmp(direction.c_str(), "stop")) {
        digitalWrite(frontIn3, LOW);
        digitalWrite(frontIn4, LOW);
      }
    }
    //case "LR":
    else if (strcmp(motor.c_str(), "LR") == 0) {
      if (strcmp(direction.c_str(), "forward")) {
        digitalWrite(backIn1, HIGH);
        digitalWrite(backIn2, LOW);
      } if (strcmp(direction.c_str(), "backward")) {
        digitalWrite(backIn1, LOW);
        digitalWrite(backIn2, HIGH);
      } if (strcmp(direction.c_str(), "stop")) {
        digitalWrite(backIn1, LOW);
        digitalWrite(backIn2, LOW);
      }
    }
    //case "RR":
    else if (strcmp(motor.c_str(), "RR") == 0) {
      if (strcmp(direction.c_str(), "forward")) {
        digitalWrite(backIn3, HIGH);
        digitalWrite(backIn4, LOW);
      } if (strcmp(direction.c_str(), "backward")) {
        digitalWrite(backIn3, LOW);
        digitalWrite(backIn4, HIGH);
      } if (strcmp(direction.c_str(), "stop")) {
        digitalWrite(backIn3, LOW);
        digitalWrite(backIn4, LOW);
      }
    }
  }

void setup() {

  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  pinMode(frontIn1, OUTPUT);
  pinMode(frontIn2, OUTPUT);
  pinMode(frontIn3, OUTPUT);
  pinMode(frontIn4, OUTPUT);
  pinMode(backIn1, OUTPUT);
  pinMode(backIn2, OUTPUT);
  pinMode(backIn3, OUTPUT);
  pinMode(backIn4, OUTPUT);
  pinMode(enLF, OUTPUT);
  pinMode(enLR, OUTPUT);
  pinMode(enRF, OUTPUT);
  pinMode(enRR, OUTPUT);

  GMtimer.setInterval(100L, pushGM);
}

void loop() {
  Blynk.run();
  GMtimer.run();
}

//**********Read Blynk Slider Swtich and Convert to Motor Speed Variables**********
BLYNK_WRITE(V0) {
  pinValue = param.asInt();
  pinValueAdjusted = pinValue * 0.6;

  analogWrite(enLF, pinValue);
  analogWrite(enLR, pinValue);
  analogWrite(enRF, pinValue);
  analogWrite(enRR, pinValue);
}


//**********Translate the Joystick Position to a Rover Direction**********
//
//This function translates the joystick movement to a Rover direction.
//Blynk Joysick is centered at y=128, x=128 with a range of 0-255. Thresholds set the Joystick 
//sensitivity. These are my adjustments for my touch, you may need something different. Making
//the range too tight will make the rover hard to control. Note: Y values greater than 128 will
//drive the motors FOWARD. Y values less than 128 will drive the motorS in REVERSE. The Rover will
//turn in the direction of the "slow" or unpowered (RELEASE) wheels.
//
//  Joystick Movement along x, y Axis
// (Inside the * is the Threshold Area)
//            y=255--(y_position=255, x_position=128; y_direction=+1, x_direction=0)
//           * | *
//           * | *
//           * | *
//   ********* | *********
//x=0---------------------x=255--(y_position=128, x_position=255; y_direction=0, x_direction=0)
//   ********* | *********
//           * | *
//           * | * (Inside the * is the Threshold Area)
//           * | *
//            y=0--(y_position=0, x_position=128; y_direction=-1, x_direction=0)

BLYNK_WRITE(V1) {
  const int X_THRESHOLD_LOW = 108; //x: 128 - 20
  const int X_THRESHOLD_HIGH = 148; //x: 128 + 20   

  const int Y_THRESHOLD_LOW = 108; //y: 128 - 20
  const int Y_THRESHOLD_HIGH = 148; //y: 128 + 20
      
  int x_position = param[0].asInt();  //Read the Blynk Joystick x Position 0-255
  int y_position = param[1].asInt();  //Read the Blynk Joystick y Position 0-255

  int x_direction;  //Variable for Direction of Joystick Movement: x= -1, 0, 1
  int y_direction;  //Variable for Direction of Joystick Movement: y= -1, 0, 1

  //Determine the direction of the Joystick Movement

  x_direction = 0;
  y_direction = 0;

  if (x_position > X_THRESHOLD_HIGH) {
    x_direction = 1;
  } else if (x_position < X_THRESHOLD_LOW) {
    x_direction = -1;
  }
  if (y_position > Y_THRESHOLD_HIGH) {
    y_direction = 1;
  } else if (y_position < Y_THRESHOLD_LOW) {
    y_direction = -1;
  }

  //**********BACKWARD DIAGONAL (SOFT) LEFT**********
  //x = -1 and y = -1 
  if (x_direction == -1) {
    if (y_direction == -1) {
      analogWrite(enLF, pinValueAdjusted);
      analogWrite(enLR, pinValueAdjusted);
      analogWrite(enRF, pinValue);
      analogWrite(enRR, pinValue);

      setMotor("LF", "backward");
      setMotor("LR", "backward");
      setMotor("RF", "backward");
      setMotor("RR", "backward");
    }
  }

  //**********HARD LEFT TURN**********
  //x = -1 and y = 0
  if (x_direction == -1) {
    if (y_direction == 0) {
      analogWrite(enLF, pinValue);
      analogWrite(enLR, pinValue);
      analogWrite(enRF, pinValue);
      analogWrite(enRR, pinValue);

      setMotor("LF", "stop");
      setMotor("LR", "stop");
      setMotor("RF", "forward");
      setMotor("RR", "forward");
    }
  }

  //FORWARD DIAGONAL (SOFT) LEFT
  //x = -1 and y = 1   
  if (x_direction == -1) {
    if (y_direction == 1) {
      analogWrite(enLF, pinValueAdjusted);
      analogWrite(enLR, pinValueAdjusted);
      analogWrite(enRF, pinValue);
      analogWrite(enRR, pinValue);

      setMotor("LF", "forward");
      setMotor("LR", "forward");
      setMotor("RF", "forward");
      setMotor("RR", "forward");
    }
  }

  //**********BACKWARD**********
  //x = 0 and y = -1 
  if (x_direction == 0) {
    if (y_direction == -1) {
      analogWrite(enLF, pinValue);
      analogWrite(enLR, pinValue);
      analogWrite(enRF, pinValue);
      analogWrite(enRR, pinValue);

      setMotor("LF", "backward");
      setMotor("LR", "backward");
      setMotor("RF", "backward");
      setMotor("RR", "backward");
    }
  }

  //**********STOP**********
  //x = 0 and y = 0 
  if (x_direction == 0) {
    if (y_direction == 0) {
      setMotor("LF", "stop");
      setMotor("LR", "stop");
      setMotor("RF", "stop");
      setMotor("RR", "stop");
    }
  }

  //**********FORWARD**********
  //x = 0 and y = 1 
  if (x_direction == 0) {
    if (y_direction == 1) {
      analogWrite(enLF, pinValue);
      analogWrite(enLR, pinValue);
      analogWrite(enRF, pinValue);
      analogWrite(enRR, pinValue);

      setMotor("LF", "forward");
      setMotor("LR", "forward");
      setMotor("RF", "forward");
      setMotor("RR", "forward");
    }
  }

  //**********BACKWARD DIAGONAL (SOFT) RIGHT**********
  //x = 1 and y = -1 
  if (x_direction == 1) {
    if (y_direction == -1) {
      analogWrite(enLF, pinValue);
      analogWrite(enLR, pinValue);
      analogWrite(enRF, pinValueAdjusted);
      analogWrite(enRR, pinValueAdjusted);

      setMotor("LF", "backward");
      setMotor("LR", "backward");
      setMotor("RF", "backward");
      setMotor("RR", "backward");
    }
  }

  //**********HARD RIGHT TURN**********
  //x = 1 and y = 0 Right on x-axis 
  if (x_direction == 1) {
    if (y_direction == 0) {
      analogWrite(enLF, pinValue);
      analogWrite(enLR, pinValue);
      analogWrite(enRF, pinValue);
      analogWrite(enRR, pinValue);

      setMotor("LF", "forward");
      setMotor("LR", "forward");
      setMotor("RF", "stop");
      setMotor("RR", "stop");
    }
  }

  //**********FORWARD DIAGONAL (SOFT) RIGHT**********
  //x = 1 and y = 1 
  if (x_direction == 1) {
    if (y_direction == 1) {
      analogWrite(enLF, pinValue);
      analogWrite(enLR, pinValue);
      analogWrite(enRF, pinValueAdjusted);
      analogWrite(enRR, pinValueAdjusted);

      setMotor("LF", "forward");
      setMotor("LR", "forward");
      setMotor("RF", "forward");
      setMotor("RR", "forward");
    }
  } 

   //**********END Translate the Joystick Position to a Rover Direction********** 
}
//Right spin
BLYNK_WRITE(V2) {
  int pinValue = param.asInt();
  if (pinValue == 1) {
    setMotor("LF", "forward");
    setMotor("LR", "forward");
    setMotor("RF", "backward");
    setMotor("RR", "backward");
  }

  if (pinValue == 0) {
    setMotor("LF", "stop");
    setMotor("LR", "stop");
    setMotor("RF", "stop");
    setMotor("RR", "stop");
  }
}

//Left spin
BLYNK_WRITE(V3) {
  int pinValue = param.asInt();
  if (pinValue == 1) {
    setMotor("LF", "backward");
    setMotor("LR", "backward");
    setMotor("RF", "forward");
    setMotor("RR", "forward");
  }

  if (pinValue == 0) {
    setMotor("LF", "stop");
    setMotor("LR", "stop");
    setMotor("RF", "stop");
    setMotor("RR", "stop");
  }
}

//TODO: radioactive counter

//TODO: gripper

//TODO: conveyor belt