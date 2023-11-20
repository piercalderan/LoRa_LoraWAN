#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define BUTTON_PIN 9
#define LED_PIN 15
int main()
{
	stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
gpio_init (BUTTON_PIN);
gpio_set_dir (BUTTON_PIN, GPIO_IN);
gpio_pull_up(BUTTON_PIN);

while (true)
{
 if (!gpio_get (BUTTON_PIN))
 {
printf ("Pulsante premuto\n");
gpio_put(LED_PIN, 1);

sleep_ms(250);
}
else
gpio_put(LED_PIN, 0);
}
}
