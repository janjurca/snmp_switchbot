/*
Author: Jan Jurca
 */

#ifndef SNMP_H
#define SNMP_H value


#include "Agentuino.h"
#include "switch.hpp"

extern Switch sw;

typedef enum {INT, CHAR } types;

class SNMP_OID_OBJECT {

public:
  const char * m_oid;
  const char * (*p_get)();
  bool (*p_set)(const char *);
  types t;


  SNMP_OID_OBJECT (const char * oid, const char *(*get)(),bool (*set)(const char*), types t) : m_oid(oid), p_get(get), p_set(set), t(t) {};

  const char* oid(){
    return m_oid;
  }
  const char * get(){
    return p_get();
  }

  bool set(const char * value){
    return p_set(value);
  }

  types type(){
    return t;
  }
};


class SNMP {
private:
  SNMP_OID_OBJECT objects[5] = {
    SNMP_OID_OBJECT( "1.3.6.1.2.1.1.1.0", []() {return "KernelPerf Switchbot";}, nullptr, CHAR),
    //SNMP_OID_OBJECT( "1.3.6.1.2.1.1.2.0", []() {return "1.3.6.1.4.1.318";}, nullptr, CHAR),
    SNMP_OID_OBJECT( "1.3.6.1.2.1.1.4.0", []() {return "Jan Jurca";}, nullptr, CHAR),
    SNMP_OID_OBJECT( "1.3.6.1.2.1.1.5.0", []() {return "KernelPerf Switchbot";}, nullptr, CHAR),
    SNMP_OID_OBJECT( "1.3.6.1.2.1.1.6.0", []() {return "Czech Republic";}, nullptr, CHAR),
    SNMP_OID_OBJECT( "1.3.6.1.4.1.318.1.1.4.4.2.1.3.1", []() {return sw.get();}, [](const char * value) {return sw.set(value);}, INT),
  };
  SNMP_API_STAT_CODES api_status;
  SNMP_ERR_CODES status;

public:
  SNMP (){};

  void get(SNMP_PDU *pdu){
    char oid[SNMP_MAX_OID_LEN];
    pdu->OID.toString(oid);
    bool matched = false;
    if ( pdu->type == SNMP_PDU_GET_NEXT ) {
      for (size_t i = 0; i < sizeof(objects) / sizeof(objects[0]); i++) {
        if (strcmp(objects[i].oid(), oid) == 0) {
          if (i == (sizeof(objects) / sizeof(objects[0]))-1){
            strcpy( oid, "1.0" );
          } else {
            strcpy( oid, objects[i+1].oid());
            pdu->OID.fromString(objects[i+1].oid());
          }
          matched = true;
          break;
        }
      }
      if (!matched) {
        int ilen = strlen(oid);
        for (size_t i = 0; i < sizeof(objects) / sizeof(objects[0]); i++) {
          if ( strncmp(oid, objects[i].oid(), ilen ) == 0 ) {
            strcpy( oid, objects[i].oid());
            pdu->OID.fromString(objects[i].oid());
            break;
          }
        }
      }
    }
    matched = false;
    for (size_t i = 0; i < sizeof(objects) / sizeof(objects[0]); i++) {
      if ( strcmp(oid, objects[i].oid()) == 0 ) {
        // response packet from get-request - locDescr
        switch (objects[i].type()) {
          case INT:{
            status = pdu->VALUE.encode(SNMP_SYNTAX_INT32,(int32_t) atoi(objects[i].get()));
            break;
          }
          case CHAR:{
            status = pdu->VALUE.encode(SNMP_SYNTAX_OCTETS, objects[i].get());
            break;
          }
        }
        pdu->type = SNMP_PDU_RESPONSE;
        pdu->error = status;
        matched = true;
        break;
      }
    }
    if (!matched) {
      pdu->type = SNMP_PDU_RESPONSE;
      pdu->error = SNMP_ERR_NO_SUCH_NAME;
    }
  }

  void set(SNMP_PDU *pdu){
    char oid[SNMP_MAX_OID_LEN];
    pdu->OID.toString(oid);
    for (size_t i = 0; i < sizeof(objects) / sizeof(objects[0]); i++) {
      if ( strcmp(oid, objects[i].oid() ) == 0 ) {
        if (objects[i].p_set == nullptr) {
          pdu->error = SNMP_ERR_READ_ONLY;
        } else {
          char dst[20];
          switch (objects[i].type()) {
            case INT:{
              int32_t val = 1;
              status = pdu->VALUE.decode(&val);
              snprintf(dst, 2, "%d",(int) val);
              break;
            }
            case CHAR:{
              status = pdu->VALUE.decode(dst, 20);
              break;
            }
          }
          Serial.println("SNMP: running set method");
          if (objects[i].set(dst)) {
            pdu->error = status;
          } else {
            pdu->error = SNMP_ERR_COMMIT_FAILED;
          }
        }
        pdu->type = SNMP_PDU_RESPONSE;
      }
    }
  }

  void proccess() {
    SNMP_PDU pdu;
    api_status = Agentuino.requestPdu(&pdu);

    if (pdu.error == SNMP_ERR_NO_ERROR && api_status == SNMP_API_STAT_SUCCESS) {
      switch (pdu.type) {
        case SNMP_PDU_GET:{
          get(&pdu);
          break;
        }
        case SNMP_PDU_GET_NEXT:{
          get(&pdu);
          break;
        }
        case SNMP_PDU_SET:{
          set(&pdu);
          break;
        }
        default:{
          break;
        }
      }
      Agentuino.responsePdu(&pdu);
    }
    //Agentuino.freePdu(&pdu);
  }
};

#endif
