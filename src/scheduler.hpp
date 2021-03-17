/*
Author: Jan Jurca
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H value

#define SIZE 4
#include "Arduino.h"

class Task {
private:

  unsigned long time;
  bool (*or_cond)();
  bool (*f)();

public:
  Task (unsigned long time,  bool (*or_cond)(), bool (*f)()) : time(time), or_cond(or_cond), f(f){}

  bool exec(){
    time = 0;
    or_cond = nullptr;
    if (f != nullptr) {
      return f();
    }
    return false;
  }

  bool isOr(){
    if (or_cond != nullptr) {
      return or_cond();
    }
    return false;
  }

  unsigned long getTime(){
    return time;
  }

  bool (*getF(void))() {
    return f;
  }

  bool (*getOr_cond(void))() {
    return or_cond;
  }

  void copy(Task t){
    f = t.getF();
    time = t.getTime();
    or_cond = t.getOr_cond();
  };

};



class Scheduler {
private:
  Task tasks[SIZE]{
    Task(0, nullptr, nullptr),
    Task(0, nullptr, nullptr),
    Task(0, nullptr, nullptr),
    Task(0, nullptr, nullptr),
  };


public:
  Scheduler (){};

  bool schedule(Task t){
    for (size_t i = 0; i < SIZE; i++) {
      if (tasks[i].getTime() == 0) {
        tasks[i].copy(t);
        return true;
      }
    }
    return false;
  }

  void run(){
    unsigned long m = millis();
    for (size_t i = 0; i < SIZE; i++) {
      if (tasks[i].getTime() != 0) {
        if (tasks[i].getTime() <= m || tasks[i].isOr()) {
          tasks[i].exec();
        }
      }
    }
  }

};
#endif
