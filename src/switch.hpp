#ifndef SWITCH_H
#define SWITCH_H value


#include <Arduino.h>
#include <Servo.h>
#include "scheduler.hpp"

extern Scheduler sched;
class Switch;
extern Switch sw;

class Switch : public Servo {

public:
  enum STATES {
    ON = 1,
    OFF = 2,
    REBOOT = 3
  };
  enum VALS { UP = 90, DOWN = 110 };

private:
  int usb;
  int servo_pin;


  int actual_value = 0;

  enum STATES desired_state = ON;
  int retries = 0;

public:

  Switch(int servo_pin, int usb) : Servo(), usb(usb),  servo_pin(servo_pin){ };

  void begin(){
    pinMode(usb, INPUT);
    attach(servo_pin);
    turn(UP);

  }

  void turn(int val) {
    actual_value = val;
    write(val);
  }

  bool turn_on(){
    desired_state = ON;
    if (!isOn()) {
      if (actual_value == UP) {
        turn(DOWN);
        sched.schedule(Task(millis() + 1000, [](){ sw.turn(Switch::UP); return true;}));
      }
    }
    return true;
  };

  bool turn_off(){
      desired_state = OFF;
      if (isOn()) {
        if (actual_value == UP) {
          turn(DOWN);
          sched.schedule(Task(millis() + 6000, [](){ sw.turn(Switch::UP); return true;}));
        }
      }
      return true;
  }


  bool isOn(){
    if (digitalRead(usb) == HIGH) {
      return true;
    }
    return false;
  }

  bool set(const char *val){
    retries = 0;
    switch (atoi(val)) {
      case ON:
        return turn_on();
      case OFF:
        return turn_off();
      case REBOOT:{
        desired_state = REBOOT;
        return true;
      }
      default:{
        return false;
      }
    }
  }

  const char * get(){
    if (actual_value == DOWN) {
      switch (desired_state) {
        case ON:
          return "1";
        case OFF:
          return "2";
        default:
          return "0";
      }
    } 

    if (isOn()) {
      return "1";
    }
    return "2";
  }


};

#endif
