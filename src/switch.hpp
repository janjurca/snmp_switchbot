#ifndef SWITCH_H
#define SWITCH_H value


#include <Arduino.h>
#include <Servo.h>


class Switch : public Servo {
//  using Servo::Servo;

private:
  int usb;
  int servo_pin;
  int UP = 90;
  int DOWN = 110;


public:
  enum STATES {
    ON = 1,
    OFF = 2,
    REBOOT = 3
  };
  Switch(int servo_pin, int usb) : Servo(), usb(usb),  servo_pin(servo_pin){ };

  void begin(){
    pinMode(usb, INPUT);
    attach(servo_pin);
    write(UP);

  }

  bool turn_on(){
    if (isOn()) {
      return true;
    } else {
      Serial.print(" Truning off ");
      Serial.print(" down ");
      write(DOWN);
      delay(1000);
      Serial.println(" up ");
      write(UP);
      delay(1000);
      if (isOn()) {
        return true;
      }
    }
    return false;
  };

  bool turn_off(){
      if (!isOn()) {
        return true;
      } else {
        Serial.print(" Truning off ");
        Serial.print(" down ");
        write(DOWN);
        delay(6000);
        Serial.println(" up ");
        write(UP);
        delay(1000);
        if (!isOn()) {
          return true;
        }
      }
      return false;
  }

  bool isOn(){
    if (digitalRead(usb) == HIGH) {
      return true;
    }
    return false;
  }

  bool set(const char *val){
    switch (atoi(val)) {
      case ON:
        return turn_on();
      case OFF:
        return turn_off();
      case REBOOT:{
        if (turn_off()) {
          delay(1000);
          return turn_on();
        } else {
          return false;
        }
      }
      default:{
        return false;
      }
    }
  }

  const char * get(){
    if (isOn()) {
      return "1";
    }
    return "2";
  }


};

#endif
