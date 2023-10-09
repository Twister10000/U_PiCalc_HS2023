/*
 * U_PiCalc_HS2023.c
 *
 * Created: 3.10.2023:18:15:00
 * Author : -
 */ 

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "avr_compiler.h"
#include "pmic_driver.h"
#include "TC_driver.h"
#include "clksys_driver.h"
#include "sleepConfig.h"
#include "port_driver.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "stack_macros.h"

#include "mem_check.h"

#include "init.h"
#include "utils.h"
#include "errorHandler.h"
#include "NHD0420Driver.h"

#include "ButtonHandler.h"

/*DEFINES für einen sauberen C CODE */

#define EVBUTTONS_S1	0x1
#define EVBUTTONS_S2	0x2
#define EVBUTTONS_S3	0x4
#define EVBUTTONS_S4	0x8
#define EVBUTTONS_L1	0x10
#define EVBUTTONS_L2	0x20
#define EVBUTTONS_L3	0x40
#define EVBUTTONS_L4	0x80
#define EVBUTTONS_CLEAR	0xFF
#define EVSYSTEM_START	0x100
#define EVSYSTEM_STOP	0x200
#define EVSYSTEM_RESET	0x400
#define LEIBNIZ_METHOD	0x800
#define NILA_METHOD		0x1000
#define EVSYSTEM_CLEAR	0x0000FF00
#define EVBUTTON_MASK	0x000000FF
#define EVSTATUS_MASK	0x0000FF00
#define UIMODE_HOME 1
#define UIMODE_LEIBNIZ 2
#define UIMODE_NILA 3

/*DEFINES fertig*/

void controllerTask(void* pvParameters);
void vUserInterface(void *pvParameters);
void vPICalcLeibniz(void *pvParameters);
void vPICalcNila(void *pvParameters);

TaskHandle_t UserInt;
TaskHandle_t LeibnizCalc;
TaskHandle_t NilaCalc;
TaskHandle_t controllertask;
EventGroupHandle_t evButtonState;

uint32_t buttonstate = 0;
uint32_t systemstate = 0;

int main(void)
{
	vInitClock();
	vInitDisplay();
	
	xTaskCreate(controllerTask, (const char *) "control_tsk", configMINIMAL_STACK_SIZE+150, NULL, 3, &controllertask);
	xTaskCreate(vUserInterface, (const char *) "UserInt_tsk", configMINIMAL_STACK_SIZE+30, NULL, 2, &UserInt);
	xTaskCreate(vPICalcLeibniz, (const char * ) "Leibniz_tsk", configMINIMAL_STACK_SIZE+30, NULL, 1, &LeibnizCalc);
	xTaskCreate(vPICalcNila, (const char * ) "Nila_tsk", configMINIMAL_STACK_SIZE+30, NULL, 1, &NilaCalc);
	
	evButtonState = xEventGroupCreate();

	vDisplayClear();
	vDisplayWriteStringAtPos(0,0,"PI-Calc HS2023");
	
	vTaskStartScheduler();
	return 0;
}

void vUserInterface(void *pvParameters){
	
	uint8_t Calc_Mode = 0;  //Eine Variabel welche einen Zukfunts Nutzen hat.
	(void) pvParameters;
	
	for (;;)
	{
		
		buttonstate = (xEventGroupGetBits(evButtonState)) & EVBUTTON_MASK;
		systemstate = (xEventGroupGetBits(evButtonState)) & EVSTATUS_MASK;
		xEventGroupClearBits(evButtonState, EVBUTTONS_CLEAR);
		vDisplayClear();
		vDisplayWriteStringAtPos(0,0,"PI-Calc HS2023");
		vDisplayWriteStringAtPos(1,0,"Mode:%d", Calc_Mode);
		vDisplayWriteStringAtPos(2,0,"PI:");
		vDisplayWriteStringAtPos(3,0,"Start Stop CHG RST");
		
		switch(buttonstate){
			
			case EVBUTTONS_S1:
				xEventGroupSetBits(evButtonState, EVSYSTEM_START);
				systemstate = (xEventGroupGetBits(evButtonState) & EVSTATUS_MASK);
				break;
			case EVBUTTONS_S2:
				vDisplayWriteStringAtPos(1,0, "Button 2 wurde gedrückt");
				break;
			case EVBUTTONS_S3:
				vDisplayWriteStringAtPos(1,0, "Button 3 wurde gedrückt");
				break;
			case EVBUTTONS_S4:
				xEventGroupClearBits(evButtonState, EVSYSTEM_CLEAR);
				break;		
		}
		vTaskDelay(500/portTICK_RATE_MS);
	}
}

void vPICalcLeibniz(void *pvParameters){
	(void) pvParameters;
	
	for (;;)
	{
	
	
	//Empty Task
	vTaskDelay(500/portTICK_RATE_MS);
	}
}

void vPICalcNila(void *pvParameters){
	
	(void) pvParameters;
	
	for (;;)
	{
	
	
	//Empy Task
	vTaskDelay(500/portTICK_RATE_MS);
	}
}

void controllerTask(void* pvParameters) { //Button Task
	initButtons();
	for(;;) {
		updateButtons();
		if(getButtonPress(BUTTON1) == SHORT_PRESSED) {
			/*char pistring[12];
			sprintf(&pistring[0], "PI: %.8f", M_PI);
			vDisplayWriteStringAtPos(1,0, "%s", pistring);*/
			xEventGroupSetBits(evButtonState, EVBUTTONS_S1);
		}
		if(getButtonPress(BUTTON2) == SHORT_PRESSED) {
			xEventGroupSetBits(evButtonState, EVBUTTONS_S2);
		}
		if(getButtonPress(BUTTON3) == SHORT_PRESSED) {
			xEventGroupSetBits(evButtonState, EVBUTTONS_S3);
		}
		if(getButtonPress(BUTTON4) == SHORT_PRESSED) {
			xEventGroupSetBits(evButtonState, EVBUTTONS_S4);
		}
		if(getButtonPress(BUTTON1) == LONG_PRESSED) {
			xEventGroupSetBits(evButtonState, EVBUTTONS_L1);
		}
		if(getButtonPress(BUTTON2) == LONG_PRESSED) {
			xEventGroupSetBits(evButtonState, EVBUTTONS_L2);
		}
		if(getButtonPress(BUTTON3) == LONG_PRESSED) {
			xEventGroupSetBits(evButtonState, EVBUTTONS_L3);
		}
		if(getButtonPress(BUTTON4) == LONG_PRESSED) {
			xEventGroupSetBits(evButtonState, EVBUTTONS_L4);
		}
		vTaskDelay(10/portTICK_RATE_MS);
	}
}