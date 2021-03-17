/*
Author: Jan Jurca
 */

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
    if (!isOn() && actual_value == UP) {
      turn(DOWN);
      sched.schedule(Task(millis() + 1000, [](){ return sw.isOn();}, [](){ sw.turn(Switch::UP); delay(400); return true;}));
    }
    return true;
  };

  bool turn_off(){
      if (isOn() && actual_value == UP) {
        turn(DOWN);
        sched.schedule(Task(millis() + 6000, [](){ return (!sw.isOn());}, [](){ sw.turn(Switch::UP); delay(400); return true;}));
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
    switch (atoi(val)) {
      case ON:
        return turn_on();
      case OFF:
        return turn_off();
      case REBOOT:{
        return true;
      }
      default:{
        return false;
      }
    }
  }

  const char * get(){
    bool ison = isOn();
    sched.run();
    if (ison) {
      return "1";
    }
    return "2";
  }


};

#endif
