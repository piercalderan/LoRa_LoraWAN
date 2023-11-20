#include <SPI.h>
#include <LoRa.h>
#include <LiquidCrystal.h>

const int rs = 8, en = 7, d4 = 6, d5 = 5, d6 = 4, d7 = 3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Temp C: RSSI:");
  
  if (!LoRa.begin(868E6)) {
    lcd.setCursor(0, 1);
    lcd.println("LoRa failed!");
    while (1);
  }
  LoRa.onReceive(onReceive);
  LoRa.receive();
}

void loop() {
}

void onReceive(int packetSize) {
  char lorainbyte; 
  lcd.setCursor(0, 1);
    for (int i = 0; i < packetSize-3; i++) {
    lorainbyte=LoRa.read();  
    lcd.print(lorainbyte);
    }
  lcd.setCursor(8, 1);
  lcd.print(LoRa.packetRssi());
}
