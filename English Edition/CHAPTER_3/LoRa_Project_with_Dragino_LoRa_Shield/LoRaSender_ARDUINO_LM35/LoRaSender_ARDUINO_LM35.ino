#include <SPI.h>
#include <LoRa.h>

int counter = 0;

void setup() {
  pinMode(5,OUTPUT);
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Sender");

  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
//***********TEMPERATURE DETECTION************
int value = analogRead(A0);
float voltage = value * 0.0048;
float temperature = voltage /= 0.01;
//****************************************************

Serial.print("Temperature value: ");
Serial.println(temperature);

  Serial.print("Sending packet: ");
  Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.print(temperature);
  LoRa.print(counter);
  LoRa.endPacket();
  
  counter++;

  digitalWrite(5,1);
  delay(1000);
  digitalWrite(5,0);
  delay(1000);
}
