#include <stdint.h>
#include "STM32PinDriver.h"

typedef struct MenuItem MenuItem;

MenuItem* CreateMenuItem(char*);
void AddSubMenuItem(MenuItem*, MenuItem*);

struct MenuItem
{
	char* name;
	MenuItem* subMenuItems[2];
	MenuItem* parentMenuItem;
	uint8_t subMenuCount;
	uint8_t isRoot;
	char* line1Text;
	uint8_t usesLine1Text;
	char* line2Text;
	uint8_t usesLine2Text;
};

MenuItem* CreateMenuItem(char* name)
{
	MenuItem* menuItem = malloc(sizeof *menuItem);
	menuItem->name = name;
	menuItem->parentMenuItem = NULL;
	menuItem->subMenuCount = 0;
	menuItem->isRoot = 0;
	menuItem->usesLine1Text = 0;
	menuItem->usesLine2Text = 0;
	return menuItem;
}

void AddSubMenuItem(MenuItem* parentMenuItem, MenuItem* subMenuItem)
{
	subMenuItem->parentMenuItem = parentMenuItem;

	parentMenuItem->subMenuItems[parentMenuItem->subMenuCount] = subMenuItem;
	parentMenuItem->subMenuCount++;
}
