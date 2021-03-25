/*
Author: Jan Jurca
 */


#include <Arduino.h>
#include <Ethernet.h>          // Include the Ethernet library
#include <SPI.h>
#include <Agentuino.h>
#include "snmp.hpp"
#include "switch.hpp"
#include "scheduler.hpp"

#define SERVO_PIN 3
#define USB_PIN 7

SNMP snmp = SNMP();
Switch sw = Switch(SERVO_PIN, USB_PIN);
Scheduler sched = Scheduler();

static byte mac[] = { 0xE2, 0x21, 0xC9, 0x36, 0xCA, 0xF6 };
static byte ip[] = { 10, 43, 134, 175 };

static byte gateway[] = { 10, 43, 135, 254 };  // NOT used if line ETHERNET BEGIN is commented (41)
static byte dns[] = {10, 38, 5, 26 };  // NOT used if line ETHERNET BEGIN is commented (41)
static byte subnet[] = { 255, 255, 254, 0 };  // NOT used if line ETHERNET BEGIN is commented (41)

SNMP_API_STAT_CODES api_status;


void proccess() {
  snmp.proccess();
}

void setup()
{
  Serial.begin(9600);
  sw.begin();

  //Ethernet.begin(mac, ip);
  Ethernet.begin(mac, ip, dns, gateway, subnet);
  api_status = Agentuino.begin();
  if ( api_status == SNMP_API_STAT_SUCCESS ) {
    Agentuino.onPduReceive(proccess);
  }
  delay(10);
}

void loop()
{
  Agentuino.listen();
  sched.run();
}
