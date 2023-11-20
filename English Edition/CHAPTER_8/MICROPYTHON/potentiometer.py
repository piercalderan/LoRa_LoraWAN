import machine
import utime

potentiometer = machine.ADC(26)
led = machine.PWM(machine.Pin(15))
led.freq(1000)

while True:
    reading = potentiometer.read_u16()
    print(reading)
    led.duty_u16(reading)
    utime.sleep(0.5)