#include <stdint.h>
#include "STM32PinDriver.h"

typedef struct Button
{
	uint32_t gpioBaseAddr;
	uint8_t pin;
	uint8_t isButtonReset;
} Button;


Button CreateGPIOButton(uint32_t, uint8_t);
PIN_STATE GetButtonValue(Button*);

Button CreateGPIOButton(uint32_t gpioBaseAddr, uint8_t pin)
{
	SetPinmode(gpioBaseAddr, pin, PINMODE_INPUT);
	Button button;
	button.gpioBaseAddr = gpioBaseAddr;
	button.pin = pin;
	button.isButtonReset = 0;
	return button;
}

PIN_STATE GetButtonValue(Button* button)
{
	PIN_STATE buttonValue = GetPinValue(button->gpioBaseAddr, button->pin);

	if(button->isButtonReset == 1 && buttonValue == PIN_HIGH)
	{
		button->isButtonReset = 0;
	}
	else if(button->isButtonReset == 0)
	{
		if(buttonValue == PIN_LOW)
		{
			button->isButtonReset = 1;
		}
		buttonValue = PIN_LOW;
	}

	return buttonValue;
}
