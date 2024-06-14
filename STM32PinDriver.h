#ifndef STM32PINDRIVER
#define STM32PINDRIVER

/*
 * STM32PinDriver.h
 *
 *  Created on: Jun 11, 2024
 *      Author: tkent
 */

//F446 UserManual 2.2.2 MemoryMap
#define PERIPHERALS_BASE_ADDR (uint32_t) 0x40000000UL
#define AHB1_OFFSET 		  (uint32_t) 0x00020000UL

//GPIOA
#define GPIOA_BASE_ADDR (uint32_t)(PERIPHERALS_BASE_ADDR + AHB1_OFFSET + 0x000UL)
#define GPIOB_BASE_ADDR (uint32_t)(PERIPHERALS_BASE_ADDR + AHB1_OFFSET + 0x400UL)
#define GPIOC_BASE_ADDR (uint32_t)(PERIPHERALS_BASE_ADDR + AHB1_OFFSET + 0x800UL)
#define GPIOD_BASE_ADDR (uint32_t)(PERIPHERALS_BASE_ADDR + AHB1_OFFSET + 0xC00UL)

#define GPIO_MODE_OFFSET   (uint32_t) 0x00UL
#define GPIO_INPUT_OFFSET  (uint32_t) 0x10UL
#define GPIO_OUTPUT_OFFSET (uint32_t) 0x14UL

#define RCC_BASE_ADDR (uint32_t) 0x40023830UL

//F446 UserManual 6.3.28 RCC Map - RCC-AHB1ENR
#define GPIOA_CLOCK_ID (uint8_t) 0
#define GPIOB_CLOCK_ID (uint8_t) 1
#define GPIOC_CLOCK_ID (uint8_t) 2
#define GPIOD_CLOCK_ID (uint8_t) 3

//Pins
#define PA_5 5
#define PA_6 6
#define PA_7 7
#define PA_10 10

#define PC_13 13

//Mapped Pins
#define LED_PIN PA_5
#define USER_BUTTON_1 PC_13

//PinModes F446 Usermanual 7.4.1 Port Mode Register
#define PINMODE_INPUT     (uint8_t) 0x00
#define PINMODE_OUTPUT    (uint8_t) 0x01
#define PINMODE_ALTERNATE (uint8_t) 0x10
#define PINMODE_ANALOG    (uint8_t) 0x11

#define PIN_HIGH (uint8_t) 0 // it seems strange high is 0, but I think pins are high when grounded so 0
#define PIN_LOW  (uint8_t) 1

typedef uint8_t PIN_STATE;

void EnableGPIOCLOCK(uint8_t);
void SetPinmode(uint32_t, uint8_t, uint8_t);

void SetGPIOPinValue(uint32_t, uint8_t, uint8_t);
uint8_t GetPinValue(uint32_t, uint8_t);

void SetGPIOPinValue(uint32_t gpioBaseAddr, uint8_t pin, PIN_STATE value)
{
	uint32_t* gpioPort = (uint32_t*)(gpioBaseAddr + GPIO_OUTPUT_OFFSET);
	if(value == PIN_HIGH)
	{
		*gpioPort |= ((uint32_t)1 << pin);
	}
	else
	{
		*gpioPort &= ~((uint32_t)1 << pin);
	}
}

PIN_STATE GetPinValue(uint32_t gpioBaseAddr, uint8_t pin)
{
	uint32_t* gpioPort = (uint32_t*)(gpioBaseAddr + GPIO_INPUT_OFFSET);
	uint32_t portValue = *gpioPort;
	return (uint8_t)(portValue >> pin) & 0x01;
}

void EnableGPIOCLOCK(uint8_t gpioPort)
{
	uint32_t* pClockControlRegister = (uint32_t*) RCC_BASE_ADDR;
	*pClockControlRegister |= ((uint32_t)1 << gpioPort);
}

void SetPinmode(uint32_t gpioBaseAddr, uint8_t pin, uint8_t mode)
{
	 uint32_t* gpioMode = (uint32_t*)(gpioBaseAddr + GPIO_MODE_OFFSET);
	 *gpioMode &= ~((uint32_t)3 << pin*2); //Maybe a better way to do this in one call?
	 *gpioMode |=  ((uint32_t)mode << pin*2);
}

#endif
