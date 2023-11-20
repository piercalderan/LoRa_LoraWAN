#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

int LED_6 = 6; //LED 
int LED_GND = 7; //ground for the LED

int data_frame[2]={0,0}; //copy data frame
int frame_counter=0; //count data frame

static const int RXPin = 3, TXPin = 4; //software serial comm
static const uint32_t GPSBaud = 9600; //GPS module baudrate

uint8_t coords[11]; //array for coordinates
uint8_t channel; //LPP channel
#define LPP_GPS 136 //define LPP  
TinyGPSPlus gps; //create a gps object
SoftwareSerial ss(RXPin, TXPin); //create a software serial object

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8]={ 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8]={ 0xB7, 0x2B, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70   };
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
// The key shown here is the semtech default key.

static const u1_t PROGMEM APPKEY[16] = { 0x8D, 0x36, 0x25, 0x5F, 0x71, 0x17, 0xD6, 0x7A, 0xB3, 0x81, 0x7F, 0x84, 0x59, 0xAA, 0x6B, 0x60 };
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}
static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 15;

// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = 10,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 9,
    .dio = {2, 8, LMIC_UNUSED_PIN},
};


  
void get_coords () {
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;
  float flat,flon,faltitudeGPS,fhdopGPS; 
  unsigned long age;

 
  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;) {

    while (ss.available()) {
      char c = ss.read();
     // Serial.write(c); 
      if (gps.encode(c)) { 
        newData = true;
      }
    }
  }
 if ( newData ) {
    flat=gps.location.lat();
    flon=gps.location.lng();
    if (gps.altitude.isValid())
         faltitudeGPS = gps.altitude.meters();
     else
        faltitudeGPS=0;
    fhdopGPS = gps.hdop.value();    
}

  //gps.stats(&chars, &sentences, &failed);
  int32_t lat = flat * 10000;
  int32_t lon = flon * 10000;
  int16_t altitudeGPS = faltitudeGPS * 100;
  int8_t hdopGPS = fhdopGPS;
  channel = 0x01;
  coords[0] = channel; 
  coords[1] = LPP_GPS; 
  coords[2] = lat >> 16;
  coords[3] = lat >> 8;
  coords[4] = lat;
  coords[5] = lon >> 16; 
  coords[6] = lon >> 8;
  coords[7] = lon;
  coords[8] = altitudeGPS;
  coords[9] = altitudeGPS >> 8;
  coords[10] = hdopGPS;
  Serial.println(coords[0]);
  Serial.println(coords[1]);
  Serial.println(coords[2]);
  Serial.println(coords[3]);
  Serial.println(coords[4]);
  Serial.println(coords[5]);
  Serial.println(coords[6]);
  Serial.println(coords[7]);
  Serial.println(coords[8]);
  Serial.println(coords[9]);
  Serial.println(coords[10]);
}


void onEvent (ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Serial.println(F("EV_SCAN_TIMEOUT"));
            break;
        case EV_BEACON_FOUND:
            Serial.println(F("EV_BEACON_FOUND"));
            break;
        case EV_BEACON_MISSED:
            Serial.println(F("EV_BEACON_MISSED"));
            break;
        case EV_BEACON_TRACKED:
            Serial.println(F("EV_BEACON_TRACKED"));
            break;
        case EV_JOINING:
            Serial.println(F("EV_JOINING"));
            break;
        case EV_JOINED:
            Serial.println(F("EV_JOINED"));

            // Disable link check validation (automatically enabled
            // during join, but not supported by TTN at this time).
            LMIC_setLinkCheckMode(0);
            break;
        case EV_RFU1:
            Serial.println(F("EV_RFU1"));
            break;
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;
            break;
        case EV_TXCOMPLETE:
            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("Received ack"));
      if (LMIC.dataLen) {
        Serial.println(F("Received "));
        Serial.println(LMIC.dataLen);        
        for (int i = LMIC.dataBeg; i < LMIC.dataBeg + LMIC.dataLen; i++) {
                   Serial.print(LMIC.frame[i], HEX); 
                   LMIC.frame[i];    
                   data_frame[frame_counter]=LMIC.frame[i];              
                   Serial.print(" ");
                   frame_counter++;
                }
      Serial.println(F(" bytes of payload"));

if (LMIC.dataLen == 2 ) //if frame is 2 bytes = "On"
{
Serial.println("LED ON");
Serial.println(data_frame[0],HEX);
Serial.println(data_frame[1],HEX);
frame_counter=0;
if (data_frame[0]==0x6F && data_frame[1]==0x6E) digitalWrite (LED_6,1);
};

if (LMIC.dataLen == 3 )//if frame is 3 bytes = "Off"
{
Serial.println("LED OFF");
Serial.println(data_frame[0],HEX);
Serial.println(data_frame[1],HEX);
frame_counter=0;
if (data_frame[0]==0x6F && data_frame[1]==0x66 && data_frame[2]==0x66)
digitalWrite (LED_6,0);
};
}
            // Schedule next transmission
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
//                 get_coords();
//    LMIC_setTxData2(1, (uint8_t*) coords, sizeof(coords), 0);
            break;
        case EV_LOST_TSYNC:
            Serial.println(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            Serial.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            Serial.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            Serial.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            Serial.println(F("EV_LINK_ALIVE"));
            break;
         default:
            Serial.println(F("Unknown event"));
            break;
    }
}

void do_send(osjob_t* j){
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
        // Prepare upstream data transmission at the next possible time.
    get_coords();
    LMIC_setTxData2(1, (uint8_t*) coords, sizeof(coords), 0);
        
        Serial.println(F("Packet queued"));
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

void setup() {
pinMode(LED_6,OUTPUT);
pinMode(LED_GND,OUTPUT);
digitalWrite(LED_GND,0);
digitalWrite(LED_6,1);
delay(400);
digitalWrite(LED_6,0);
ss.begin(GPSBaud);
delay(400);

    Serial.begin(115200);
    Serial.println(F("Starting"));
get_coords ();
    #ifdef VCC_ENABLE
    // For Pinoccio Scout boards
    pinMode(VCC_ENABLE, OUTPUT);
    digitalWrite(VCC_ENABLE, HIGH);
    delay(1000);
    #endif

    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

    // Start job (sending automatically starts OTAA too)
    do_send(&sendjob);
}

void loop() {
    os_runloop_once();
}
