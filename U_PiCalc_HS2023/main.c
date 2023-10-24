/*
 * U_PiCalc_HS2023.c
 *
 * Created: 3.10.2023:18:15:00
 * Author : Twister10000
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
#define LEIBNIZ_STATUS	0x8
#define NILA_STATUS		0x10
#define DISP_READ		0x20
#define CALC_STOP		0x40
#define CALC_RET		0x80
#define EVSTATUS_MASK	0xFF

/*DEFINES fertig*/

void controllerTask(void* pvParameters);
void vPICalcLeibniz(void *pvParameters);
void vPICalcNila(void *pvParameters);

TaskHandle_t LeibnizCalc;
TaskHandle_t NilaCalc;
TaskHandle_t controllertask;
EventGroupHandle_t evButtonState;

uint32_t systemstate = 0;
uint32_t starttime = 0;
uint32_t time = 0;

float pi = 0;

int main(void)
{
	vInitClock();
	vInitDisplay();
	
	xTaskCreate(controllerTask, (const char *) "control_tsk", configMINIMAL_STACK_SIZE+150, NULL, 3, &controllertask);
	xTaskCreate(vPICalcLeibniz, (const char * ) "Leibniz_tsk", configMINIMAL_STACK_SIZE+30, NULL, 1, &LeibnizCalc);
	xTaskCreate(vPICalcNila, (const char * ) "Nila_tsk", configMINIMAL_STACK_SIZE+30, NULL, 1, &NilaCalc);
	
	vTaskSuspend(NilaCalc);
	evButtonState = xEventGroupCreate();

	vDisplayClear();	
	vTaskStartScheduler();
	return 0;
}

void vPICalcLeibniz(void *pvParameters){
	(void) pvParameters;
	float pi4 = 1;
	uint32_t n = 3;
	for (;;)
	{
		systemstate = (xEventGroupGetBits(evButtonState) & EVSTATUS_MASK);
		
		switch(systemstate){
			case EVSYSTEM_RESET:
				pi4 = 1;
				n = 3;
				pi = 0;
				time = 0;
				xEventGroupClearBits(evButtonState, EVSTATUS_MASK);
				break;
			case EVSYSTEM_STOP:
				xEventGroupClearBits(evButtonState, EVSTATUS_MASK);
				break;
			case EVSYSTEM_START:
				starttime = xTaskGetTickCount();
				xEventGroupSetBits(evButtonState, LEIBNIZ_STATUS);
				xEventGroupClearBits (evButtonState, EVSYSTEM_START);
				break;	
			case LEIBNIZ_STATUS:
				pi4 = pi4 - (1.0/n) + (1.0/(n+2));
				n += 4;
				pi = pi4*4;
				time = xTaskGetTickCount() - starttime;
				if (pi > 3.141598 && pi < 3.141599)
				{
					xEventGroupClearBits(evButtonState, LEIBNIZ_STATUS);
	 			}
				break;
			case DISP_READ:
				xEventGroupClearBits(evButtonState, DISP_READ);		
				xEventGroupSetBits(evButtonState, CALC_STOP);	
				xEventGroupWaitBits(evButtonState, CALC_RET, pdTRUE, pdFALSE, portMAX_DELAY);				
				break;
			case LEIBNIZ_STATUS | DISP_READ:
				xEventGroupClearBits(evButtonState, DISP_READ);		
				xEventGroupSetBits(evButtonState, CALC_STOP);
				xEventGroupWaitBits(evButtonState, CALC_RET, pdTRUE, pdFALSE, portMAX_DELAY);
				break;
		}
	}
}

void vPICalcNila(void *pvParameters){
	
	(void) pvParameters;
	int8_t sign = 1;
	int32_t k = 3;
	for (;;)
	{
		systemstate = (xEventGroupGetBits(evButtonState) & EVSTATUS_MASK);
		
		switch(systemstate){
			case EVSYSTEM_RESET:
			k = 3;
			sign = 1;
			pi = 0;
			time = 0;
			xEventGroupClearBits(evButtonState, EVSTATUS_MASK);
			break;
			case EVSYSTEM_STOP:
			xEventGroupClearBits(evButtonState, EVSTATUS_MASK);
			break;
			case EVSYSTEM_START:
			starttime = xTaskGetTickCount();
			xEventGroupSetBits(evButtonState, NILA_STATUS);
			xEventGroupClearBits (evButtonState, EVSYSTEM_START);
			pi = 3;
			break;
			case NILA_STATUS:
			pi = pi + (sign * (4/(pow(k,3) - k)));
			sign = sign *(-1);
			k = k+2;
			time = xTaskGetTickCount() - starttime;
			if (pi > 3.141598 && pi < 3.141599)
			{
				xEventGroupClearBits(evButtonState, NILA_STATUS);
			}
			break;
		}
	}
}

void controllerTask(void* pvParameters) { //Button Task
	
	(void) pvParameters;
	uint8_t displaycounter = 50;
	uint8_t Calc_Mode = 0;
	initButtons();
	
	for(;;) {
		
		updateButtons();
		switch(displaycounter){
			case 0:
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
				char time_Calc [12];
				char pistring[12];
				xEventGroupSetBits(evButtonState, DISP_READ);
				xEventGroupWaitBits(evButtonState, CALC_STOP, pdTRUE, pdFALSE, portMAX_DELAY);
				sprintf(&pistring[0], "%.6f", pi);
				sprintf(&time_Calc[0], "%lu", time);
				vDisplayWriteStringAtPos(2,0,"PI:%s T%sms", pistring, time_Calc);
				vDisplayWriteStringAtPos(3,0,"Start Stop CHG RST");
				displaycounter = 50;
				xEventGroupSetBits(evButtonState, CALC_RET);
				break;
			default:
				displaycounter--;
				break;
		}

		if(getButtonPress(BUTTON1) == SHORT_PRESSED) {

			xEventGroupClearBits(evButtonState, EVSTATUS_MASK);	
			xEventGroupSetBits(evButtonState, EVSYSTEM_START);
		}
		if(getButtonPress(BUTTON2) == SHORT_PRESSED) {				
			xEventGroupClearBits(evButtonState, EVSTATUS_MASK);
			xEventGroupSetBits(evButtonState, EVSYSTEM_STOP);
		}
		if(getButtonPress(BUTTON3) == SHORT_PRESSED) {
			switch(Calc_Mode){
				case 1:
					vTaskSuspend(NilaCalc);
					vTaskResume(LeibnizCalc);
					xEventGroupClearBits(evButtonState, EVSTATUS_MASK);
					xEventGroupSetBits(evButtonState, EVSYSTEM_RESET);
					Calc_Mode = 0;
					break;
				case 0:
					vTaskSuspend(LeibnizCalc);
					vTaskResume(NilaCalc);
					xEventGroupClearBits(evButtonState, EVSTATUS_MASK);
					xEventGroupSetBits(evButtonState, EVSYSTEM_RESET);
					Calc_Mode = 1;
					break;
			}
		}
		if(getButtonPress(BUTTON4) == SHORT_PRESSED) {			
			
			xEventGroupClearBits(evButtonState, EVSTATUS_MASK);
			xEventGroupSetBits(evButtonState, EVSYSTEM_RESET);
		}
		vTaskDelay(10/portTICK_RATE_MS);
	}
}