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

#define EVSYSTEM_START	0x1
#define EVSYSTEM_STOP	0x2
#define EVSYSTEM_RESET	0x4
#define LEIBNIZ_METHOD	0x8
#define NILA_METHOD		0x10
#define EVSYSTEM_CLEAR	0x00000007
#define EVSTATUS_MASK	0x00000007
#define EVCALC_MASK		0x00000018
#define START_LEIBNIZ	LEIBNIZ_METHOD|EVSYSTEM_START
#define UIMODE_HOME 1
#define UIMODE_LEIBNIZ 2
#define UIMODE_NILA 3

/*DEFINES fertig*/

void controllerTask(void* pvParameters);
void vPICalcLeibniz(void *pvParameters);
void vPICalcNila(void *pvParameters);

TaskHandle_t LeibnizCalc;
TaskHandle_t NilaCalc;
TaskHandle_t controllertask;
EventGroupHandle_t evButtonState;

uint32_t systemstate = 0;
float pi = 0;

int main(void)
{
	vInitClock();
	vInitDisplay();
	
	xTaskCreate(controllerTask, (const char *) "control_tsk", configMINIMAL_STACK_SIZE+150, NULL, 3, &controllertask);
	xTaskCreate(vPICalcLeibniz, (const char * ) "Leibniz_tsk", configMINIMAL_STACK_SIZE+30, NULL, 1, &LeibnizCalc);
	xTaskCreate(vPICalcNila, (const char * ) "Nila_tsk", configMINIMAL_STACK_SIZE+30, NULL, 1, &NilaCalc);
	
	evButtonState = xEventGroupCreate();
	xEventGroupSetBits(evButtonState,LEIBNIZ_METHOD);

	vDisplayClear();
	vDisplayWriteStringAtPos(0,0,"PI-Calc HS2023");
	
	vTaskStartScheduler();
	return 0;
}

void vPICalcLeibniz(void *pvParameters){
	(void) pvParameters;
	float pi4 = 1;
	uint32_t n = 3;
	for (;;)
	{

		if (xEventGroupGetBits(evButtonState) & EVSYSTEM_RESET)
		{
				pi4 = 1;
				n = 3;
				pi = 0;
		}
		while ((xEventGroupGetBits(evButtonState) & EVSYSTEM_START) && (xEventGroupGetBits(evButtonState) & LEIBNIZ_METHOD))
		{
			
			pi4 = pi4 - (1.0/n) + (1.0/(n+2));
			n += 4;
			pi = pi4*4;
		}
		

		
	vTaskDelay(20/portTICK_RATE_MS);
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
	
	uint8_t displaycounter = 50;
	uint8_t Calc_Mode = 0;
	

	initButtons();
	
	for(;;) {
		
		updateButtons();
		if (displaycounter == 0)
		{
			vDisplayClear();
			vDisplayWriteStringAtPos(0,0,"PI-Calc HS2023");
			switch(Calc_Mode){
				case 0:
					vDisplayWriteStringAtPos(1,0,"Mode: Leibniz" );
					break;
				case 1:
					vDisplayWriteStringAtPos(1,0,"Mode: Nilikantha");		
					break;
				default:
					vDisplayWriteStringAtPos(1,0,"Model: NOT SET");
					break;
			}
			char pistring[12];
			sprintf(&pistring[0], "%.6f", pi);
			vDisplayWriteStringAtPos(2,0,"PI:%s", pistring);
			vDisplayWriteStringAtPos(3,0,"Start Stop CHG RST");
			displaycounter = 50;
		}else{
			displaycounter--;
		}

		if(getButtonPress(BUTTON1) == SHORT_PRESSED) {

			xEventGroupClearBits(evButtonState, EVSYSTEM_CLEAR);	
			xEventGroupSetBits(evButtonState, EVSYSTEM_START);
			systemstate = (xEventGroupGetBits(evButtonState) & EVSTATUS_MASK); //Debug
			

		}
		if(getButtonPress(BUTTON2) == SHORT_PRESSED) {				
			xEventGroupClearBits(evButtonState, EVSYSTEM_CLEAR);
			xEventGroupSetBits(evButtonState, EVSYSTEM_STOP);
			systemstate = (xEventGroupGetBits(evButtonState) & EVSTATUS_MASK); //Debug
			
			
		}
		if(getButtonPress(BUTTON3) == SHORT_PRESSED) {
			if (Calc_Mode)
			{
				xEventGroupClearBits(evButtonState, NILA_METHOD);
				xEventGroupSetBits(evButtonState, LEIBNIZ_METHOD);
				Calc_Mode = 0; 
			}else{
				
				xEventGroupClearBits(evButtonState, LEIBNIZ_METHOD);
				xEventGroupSetBits(evButtonState, NILA_METHOD);
				Calc_Mode = 1;
			}
		}
		if(getButtonPress(BUTTON4) == SHORT_PRESSED) {			
			
			xEventGroupClearBits(evButtonState, EVSYSTEM_CLEAR);
			xEventGroupSetBits(evButtonState, EVSYSTEM_RESET);
			systemstate = (xEventGroupGetBits(evButtonState) & EVSTATUS_MASK); //Debug
			
			
		}
		

		vTaskDelay(10/portTICK_RATE_MS);
	}
}