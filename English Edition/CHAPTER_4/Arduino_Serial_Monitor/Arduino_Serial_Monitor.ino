void setup() {
Serial.begin (115200);
}

void loop () 
{
if (Serial.available())
{
char in_byte = Serial.read();
Serial.print(in_byte);
}
}
