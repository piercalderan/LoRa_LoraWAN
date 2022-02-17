#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <dht.h>
dht DHT;
#define DHT11_PIN 4
#define LED_PIN 5

uint8_t mydata[] = "00000000";
uint8_t temp = "0";
uint8_t humi = "0";

static const PROGMEM u1_t NWKSKEY[16] = { 0x83, 0x5E, 0x53, 0x08, 0xC9, 0x05, 0x02, 0x61, 0x59, 0x8C, 0xD3, 0xD8, 0x9A, 0x4E, 0x8E, 0x5C };
static const u1_t PROGMEM APPSKEY[16] = { 0xF2, 0x23, 0x0E, 0x8E, 0xE0, 0xB7, 0xB7, 0xCB, 0xAF, 0xA2, 0x57, 0x06, 0x0D, 0xFA, 0xC5, 0x94 };
static const u4_t DEVADDR = 0x26011A61;

void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

static osjob_t initjob,sendjob;

const unsigned TX_INTERVAL = 30;

const lmic_pinmap lmic_pins = {
    .nss = 10,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 9,
    .dio = {2, 8, LMIC_UNUSED_PIN},
};
void do_send(osjob_t* j){

temp = DHT.temperature;
humi = DHT.humidity;
Serial.println(temp); //T
Serial.println(humi); //H
      mydata[0]=0x54;
      mydata[1]=temp;
      mydata[2]=0x48;
      mydata[3]=humi;
        Serial.print(mydata[0]); //T
        Serial.print("\t"); 
        Serial.println(mydata[1]); //temp
        
        Serial.print(mydata[2]); // H
        Serial.print("\t");
        Serial.println(mydata[3]); //humi
  
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println("OP_TXRXPEND, not sending");
    } else {
        LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 0);
        Serial.println("Packet queued");
        digitalWrite(LED_PIN,1);
    }
}

void onEvent (ev_t ev) {
//    Serial.print(os_getTime());
//    Serial.print(": ");
//    Serial.println(ev);
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Serial.println("EV_SCAN_TIMEOUT");
            break;
        case EV_BEACON_FOUND:
            Serial.println("EV_BEACON_FOUND");
            break;
        case EV_BEACON_MISSED:
            Serial.println("EV_BEACON_MISSED");
            break;
        case EV_BEACON_TRACKED:
            Serial.println("EV_BEACON_TRACKED");
            break;
        case EV_JOINING:
            Serial.println("EV_JOINING");
            break;
        case EV_JOINED:
            Serial.println("EV_JOINED");
            break;
        case EV_RFU1:
            Serial.println("EV_RFU1");
            break;
        case EV_JOIN_FAILED:
            Serial.println("EV_JOIN_FAILED");
            break;
        case EV_REJOIN_FAILED:
            Serial.println("EV_REJOIN_FAILED");
            break;
        case EV_TXCOMPLETE:
            Serial.println("EV_TXCOMPLETE (includes waiting for RX windows)");
            if(LMIC.dataLen) {
                // data received in rx slot after tx
                Serial.print("Data Received: ");
                Serial.write(LMIC.frame+LMIC.dataBeg, LMIC.dataLen);
                Serial.println();
            }
            // Schedule next transmission
            digitalWrite(LED_PIN,0);
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
            
            break;
        case EV_LOST_TSYNC:
            Serial.println("EV_LOST_TSYNC");
            break;
        case EV_RESET:
            Serial.println("EV_RESET");
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            Serial.println("EV_RXCOMPLETE");
            break;
        case EV_LINK_DEAD:
            Serial.println("EV_LINK_DEAD");
            break;
        case EV_LINK_ALIVE:
            Serial.println("EV_LINK_ALIVE");
            break;
         default:
            Serial.println("Unknown event");
            break;
    }
}

void setup() {
  pinMode (LED_PIN, OUTPUT);
    Serial.begin(9600);
    while(!Serial);
    Serial.println("Starting");
    os_init();
    LMIC_reset();
    #ifdef PROGMEM
    uint8_t appskey[sizeof(APPSKEY)];
    uint8_t nwkskey[sizeof(NWKSKEY)];
    memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
    memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
    LMIC_setSession (0x1, DEVADDR, nwkskey, appskey);
    #else
    LMIC_setSession (0x1, DEVADDR, NWKSKEY, APPSKEY);
    #endif
   
    LMIC_setLinkCheckMode(0);

    LMIC.dn2Dr = DR_SF9;
    
    LMIC_setDrTxpow(DR_SF7,14);

    do_send(&sendjob);
}

void loop() {
    os_runloop_once();
}
