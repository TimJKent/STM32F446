#include "main.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <GPIOButton.h>
#include <MenuItem.h>
#include <STM32PinDriver.h>

#define I2C_ADDR 0x27 // I2C address of the PCF8574
#define RS_BIT 0 // Register select bit
#define EN_BIT 2 // Enable bit
#define BL_BIT 3 // Backlight bit
#define D4_BIT 4 // Data 4 bit
#define D5_BIT 5 // Data 5 bit
#define D6_BIT 6 // Data 6 bit
#define D7_BIT 7 // Data 7 bit

static void MX_I2C1_Init(void);

void lcd_write_nibble(uint8_t, uint8_t);
void lcd_send_cmd(uint8_t);
void lcd_send_data(uint8_t);
void lcd_init();
void lcd_set_cursor(uint8_t, uint8_t);
void lcd_clear(void);
void lcd_backlight(uint8_t);
void lcd_write_string(char *str);
void UpdateCursor();
void ShowBootScreen();
void UpdateMenuList(MenuItem*);
void EnterButtonPressed();

I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_rx;
DMA_HandleTypeDef hdma_i2c1_tx;

uint8_t backlight_state;

MenuItem* activeMenuItem;
int selectedMenuIndex = 0;

int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  MX_I2C1_Init();

  EnableGPIOCLOCK(GPIOA_CLOCK_ID);
  EnableGPIOCLOCK(GPIOC_CLOCK_ID);

  SetPinmode(GPIOA_BASE_ADDR, LED_PIN, PINMODE_OUTPUT);

  lcd_init();
  lcd_backlight(1); // Turn on backlight


  Button navigateButton = CreateGPIOButton(GPIOC_BASE_ADDR, USER_BUTTON_1);
  Button selectButton = CreateGPIOButton(GPIOA_BASE_ADDR, PA_10);



  MenuItem* mainMenu = CreateMenuItem("MainMenu");
  mainMenu->isRoot = 1;


  MenuItem* counterMenu = CreateMenuItem("Counter");
  counterMenu->usesLine1Text = 1;
  counterMenu->line1Text = "Count = 0";


  MenuItem* aboutMenu = CreateMenuItem("About");
  MenuItem* systemInfoMenu = CreateMenuItem("System Info");

  systemInfoMenu->usesLine1Text = 1;
  systemInfoMenu->usesLine2Text = 1;

  systemInfoMenu->line1Text = "CPU: ARM CORTEX-M4";
  systemInfoMenu->line2Text = "RAM: 128K";

  AddSubMenuItem(aboutMenu, systemInfoMenu);

  AddSubMenuItem(mainMenu, counterMenu);
  AddSubMenuItem(mainMenu, aboutMenu);

  ShowBootScreen();
  HAL_Delay(2000);
  UpdateMenuList(mainMenu);

  while (1)
  {
	  	if(GetButtonValue(&navigateButton) == PIN_HIGH)
	    {
	  		selectedMenuIndex++;
	  		UpdateCursor();
	    }
	  	if(GetButtonValue(&selectButton) == PIN_HIGH)
	  	{
	  		EnterButtonPressed();
	  	}
	  	HAL_Delay(150);
  }
}

void EnterButtonPressed()
{
	if(activeMenuItem->isRoot == 0 && selectedMenuIndex >= activeMenuItem->subMenuCount)
	{
		UpdateMenuList(activeMenuItem->parentMenuItem);
	}
	else if(activeMenuItem->subMenuCount > 0)
	{

		UpdateMenuList(activeMenuItem->subMenuItems[selectedMenuIndex]);
	}
}

void ShowBootScreen()
{
	char* text = "INNOV8TORS ARM32";

		lcd_clear();
		lcd_set_cursor(0, 0);
		lcd_write_string(text);
}

void UpdateMenuList(MenuItem* activeMenu)
{

	activeMenuItem = activeMenu;
	lcd_clear();

	lcd_set_cursor(0, 0);
	lcd_write_string(activeMenu->name);

	for (int i = 0; i < activeMenu->subMenuCount; i++)
	{
		lcd_set_cursor(i + 1, 1);
		lcd_write_string(activeMenu->subMenuItems[i]->name);
	}

	int backMenuOffset = 0;

	if(activeMenu->usesLine1Text == 1)
	{
		backMenuOffset = 1;
		lcd_set_cursor(1, 1);
		lcd_write_string(activeMenu->line1Text);
	}

	if(activeMenu->usesLine2Text == 1)
	{
		backMenuOffset = 2;
		lcd_set_cursor(2, 1);
		lcd_write_string(activeMenu->line2Text);
	}

	if(activeMenu->isRoot == 0)
	{
		lcd_set_cursor(activeMenu->subMenuCount + 1 + backMenuOffset, 1);
		lcd_write_string("Back");
	}

	selectedMenuIndex = 0;
	UpdateCursor();
}

void UpdateCursor()
{
	int backMenuOffset = activeMenuItem->isRoot ? 0 : 1;


	if(selectedMenuIndex >= activeMenuItem->subMenuCount + backMenuOffset)
	{
		selectedMenuIndex = 0;
	}

	for(int i = 1; i < 4; i ++)
	{
		lcd_set_cursor(i, 0);
		lcd_write_string(" ");
	}


	int drawBackMenuOffset = 0;
	if(activeMenuItem->usesLine1Text == 1)
	{
		drawBackMenuOffset = 1;
	}
	if(activeMenuItem->usesLine2Text == 1)
	{
		drawBackMenuOffset = 2;
	}

	lcd_set_cursor(1+selectedMenuIndex+drawBackMenuOffset, 0);
	lcd_write_string(">");
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
   // Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}


void lcd_write_nibble(uint8_t nibble, uint8_t rs) {
  uint8_t data = nibble << D4_BIT;
  data |= rs << RS_BIT;
  data |= backlight_state << BL_BIT; // Include backlight state in data
  data |= 1 << EN_BIT;
  HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDR << 1, &data, 1, 100);
  HAL_Delay(1);
  data &= ~(1 << EN_BIT);
  HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDR << 1, &data, 1, 100);
}

void lcd_send_cmd(uint8_t cmd) {
  uint8_t upper_nibble = cmd >> 4;
  uint8_t lower_nibble = cmd & 0x0F;
  lcd_write_nibble(upper_nibble, 0);
  lcd_write_nibble(lower_nibble, 0);
  if (cmd == 0x01 || cmd == 0x02) {
    HAL_Delay(2);
  }
}

void lcd_send_data(uint8_t data) {
  uint8_t upper_nibble = data >> 4;
  uint8_t lower_nibble = data & 0x0F;
  lcd_write_nibble(upper_nibble, 1);
  lcd_write_nibble(lower_nibble, 1);
}

void lcd_init() {
  HAL_Delay(50);
  lcd_write_nibble(0x03, 0);
  HAL_Delay(5);
  lcd_write_nibble(0x03, 0);
  HAL_Delay(1);
  lcd_write_nibble(0x03, 0);
  HAL_Delay(1);
  lcd_write_nibble(0x02, 0);
  lcd_send_cmd(0x28);
  lcd_send_cmd(0x0C);
  lcd_send_cmd(0x06);
  lcd_send_cmd(0x01);
  HAL_Delay(2);
}

void lcd_set_cursor(uint8_t row, uint8_t column) {
    uint8_t address;
    switch (row) {
        case 0:
            address = 0x00;
            break;
        case 1:
            address = 0x40;
            break;
        case 2:
        	address = 0x14;
        	break;
        case 3:
       		address = 0x54;
       		break;
        default:
            address = 0x80;
    }
    address += column;
    lcd_send_cmd(0x80 | address);
}

void lcd_clear(void) {
	lcd_send_cmd(0x01);
    HAL_Delay(2);
}

void lcd_backlight(uint8_t state) {
  if (state) {
    backlight_state = 1;
  } else {
    backlight_state = 0;
  }
}

void lcd_write_string(char *str) {
  while (*str) {
    lcd_send_data(*str++);
  }
}

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
