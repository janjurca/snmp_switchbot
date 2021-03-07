#include "Agentuino.h"

class SNMP_OID_OBJECT {
private:
  const char * m_oid;
  const char * (*p_get)();
  bool (*p_set)(const char *);

public:
  SNMP_OID_OBJECT (const char * oid, const char *(*get)(),bool (*set)(const char*)) : m_oid(oid), p_get(get), p_set(set){};

  const char* oid(){
    return m_oid;
  }
  const char * get(){
    return p_get();
  }

  bool set(const char * value){
    return p_set(value);
  }
};


class SNMP {
private:
  SNMP_OID_OBJECT objects[5] = {
    SNMP_OID_OBJECT( "1.3.6.1.2.1.1.1.0", []() {return "Agentuino, a light-weight SNMP Agent.";}, nullptr),
    SNMP_OID_OBJECT( "1.3.6.1.2.1.1.2.0", []() {return "1.3.6.1.3.2009.0";}, nullptr),
    SNMP_OID_OBJECT( "1.3.6.1.2.1.1.4.0", []() {return "Petr Domorazek";}, nullptr),
    SNMP_OID_OBJECT( "1.3.6.1.2.1.1.5.0", []() {return "Agentuino";}, nullptr),
    SNMP_OID_OBJECT( "1.3.6.1.2.1.1.6.0", []() {return "Czech Republic";}, nullptr)
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
      for (size_t i = 0; i < 5; i++) {
        if (strcmp(objects[i].oid(), oid) == 0) {
          if (i < 4){
            strcpy( oid, objects[i+1].oid());
            pdu->OID.fromString(objects[i+1].oid());
          } else {
              strcpy( oid, "1.0" );
          }
          matched = true;
          break;
        }
      }
      if (!matched) {
        int ilen = strlen(oid);
        for (size_t i = 0; i < 5; i++) {
          if ( strncmp(oid, objects[i].oid(), ilen ) == 0 ) {
            strcpy( oid, objects[i].oid());
            pdu->OID.fromString(objects[i].oid());
            break;
          }
        }
      }
    }
    matched = false;
    for (size_t i = 0; i < 5; i++) {
      if ( strcmp(oid, objects[i].oid()) == 0 ) {
        // response packet from get-request - locDescr
        status = pdu->VALUE.encode(SNMP_SYNTAX_OCTETS, objects[i].get());
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
    for (size_t i = 0; i < 5; i++) {
      if ( strcmp(oid, objects[i].oid() ) == 0 ) {
        pdu->type = SNMP_PDU_RESPONSE;
        pdu->error = SNMP_ERR_READ_ONLY;
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
