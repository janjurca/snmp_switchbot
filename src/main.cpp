#include <Arduino.h>

#include <Streaming.h>         // Include the Streaming library
#include <Ethernet.h>          // Include the Ethernet library
#include <SPI.h>
#include <MemoryFree.h>
#include <Agentuino.h>
#include "snmp.hpp"

static byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
static byte ip[] = { 192, 168, 1, 46 };
//static byte gateway[] = { 192, 168, 20, 1 };
//static byte subnet[] = { 255, 255, 255, 0 };
SNMP_API_STAT_CODES api_status;

SNMP snmp = SNMP();

void proccess() {
  snmp.proccess();
}

void setup()
{
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  api_status = Agentuino.begin();
  if ( api_status == SNMP_API_STAT_SUCCESS ) {
    Agentuino.onPduReceive(proccess);
  }
  delay(10);
}

void loop()
{
  Agentuino.listen();
}
