#ifndef SCHEDULER_H
#define SCHEDULER_H value

#define SIZE 5
#include "Arduino.h"

class Task {
private:

  unsigned long time;
  bool (*f)();

public:
  Task (unsigned long time, bool (*f)()) : time(time), f(f){}

  bool exec(){
    time = 0;
    return f();

  }

  unsigned long getTime(){
    return time;
  }

  bool (*getF(void))() {
    return f;
  }

  void copy(Task t){
    f = t.getF();
    time = t.getTime();
  };

};



class Scheduler {
private:
  Task tasks[SIZE]{
    Task(0, nullptr),
    Task(0, nullptr),
    Task(0, nullptr),
    Task(0, nullptr),
    Task(0, nullptr),
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
      if (tasks[i].getTime() != 0 && tasks[i].getTime() <= m) {
        tasks[i].exec();
      }
    }
  }

};
#endif
