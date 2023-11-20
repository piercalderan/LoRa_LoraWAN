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
//***********RILEVAMENTO DELLA TEMPERATURA************
int valore = analogRead(A0);
float tensione = valore * 0.0048;
float temperatura = tensione /= 0.01;
//****************************************************

Serial.print("Valore temperatura: ");
Serial.println(temperatura);

  Serial.print("Sending packet: ");
  Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.print(temperatura);
  LoRa.print(counter);
  LoRa.endPacket();
  
  counter++;

  digitalWrite(5,1);
  delay(1000);
  digitalWrite(5,0);
  delay(1000);
}
