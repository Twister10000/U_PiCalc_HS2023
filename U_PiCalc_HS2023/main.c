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

#define EVBUTTONS_S1	1<<0
#define EVBUTTONS_S2	1<<1
#define EVBUTTONS_S3	1<<2
#define EVBUTTONS_S4	1<<3
#define EVBUTTONS_L1	1<<4
#define EVBUTTONS_L2	1<<5
#define EVBUTTONS_L3	1<<6
#define EVBUTTONS_L4	1<<7
#define EVBUTTONS_CLEAR	0xFF

void vUserInterface(void *pvParameters);
void vPICalcLeibniz(void *pvParameters);
void vPICalcNila(void *pvParameters);

TaskHandle_t UserInt;
TaskHandle_t LeibnizCalc;
TaskHandle_t NilaCalc;

void controllerTask(void* pvParameters);

int main(void)
{
	vInitClock();
	vInitDisplay();
	
	xTaskCreate( controllerTask, (const char *) "control_tsk", configMINIMAL_STACK_SIZE+150, NULL, 3, NULL);
	xTaskCreate(vUserInterface, (const char *) "UserInt_tsk", configMINIMAL_STACK_SIZE+30, NULL, 2, UserInt);
	xTaskCreate(vPICalcLeibniz, (const char * ) "Leibniz_tsk", configMINIMAL_STACK_SIZE+30, NULL, 1, LeibnizCalc);
	xTaskCreate(vPICalcNila, (const char * ) "Nila_tsk", configMINIMAL_STACK_SIZE+30, NULL, 1, NilaCalc);

	vDisplayClear();
	vDisplayWriteStringAtPos(0,0,"PI-Calc HS2023");
	
	vTaskStartScheduler();
	return 0;
}
void vUserInterface(void *pvParameters){
	
	vTaskDelay(500/portTICK_RATE_MS);
}

void vPICalcLeibniz(void *pvParameters){
	
	vTaskDelay(500/portTICK_RATE_MS);
}

void vPICalcNila(void *pvParameters){
	
	vTaskDelay(500/portTICK_RATE_MS);
}	

void controllerTask(void* pvParameters) {
	initButtons();
	for(;;) {
		updateButtons();
		if(getButtonPress(BUTTON1) == SHORT_PRESSED) {
			char pistring[12];
			sprintf(&pistring[0], "PI: %.8f", M_PI);
			vDisplayWriteStringAtPos(1,0, "%s", pistring);
		}
		if(getButtonPress(BUTTON2) == SHORT_PRESSED) {
			
		}
		if(getButtonPress(BUTTON3) == SHORT_PRESSED) {
			
		}
		if(getButtonPress(BUTTON4) == SHORT_PRESSED) {
			
		}
		if(getButtonPress(BUTTON1) == LONG_PRESSED) {
			
		}
		if(getButtonPress(BUTTON2) == LONG_PRESSED) {
			
		}
		if(getButtonPress(BUTTON3) == LONG_PRESSED) {
			
		}
		if(getButtonPress(BUTTON4) == LONG_PRESSED) {
			
		}
		vTaskDelay(10/portTICK_RATE_MS);
	}
}