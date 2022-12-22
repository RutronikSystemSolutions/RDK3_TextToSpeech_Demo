/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the RDK3-TextToSpeech_Demo
*              Application for ModusToolbox.
*
* Related Document: See README.md
*
*
*  Created on: 2022-12-21
*  Company: Rutronik Elektronische Bauelemente GmbH
*  Address: Jonavos g. 30, Kaunas 44262, Lithuania
*  Author: GDR
*
*******************************************************************************
* (c) 2019-2021, Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* ("Software"), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries ("Cypress") and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software ("EULA").
*
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress's integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
*
* Rutronik Elektronische Bauelemente GmbH Disclaimer: The evaluation board
* including the software is for testing purposes only and,
* because it has limited functions and limited resilience, is not suitable
* for permanent use under real conditions. If the evaluation board is
* nevertheless used under real conditions, this is done at one’s responsibility;
* any liability of Rutronik is insofar excluded
*******************************************************************************/

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "spi_api.h"

/*First and last phrases of the TTS list*/
#define FIRST_PHRASE			0
#define LAST_PHRASE				5

/*Priority for button interrupts*/
#define BTN_IRQ_PRIORITY		5

/*Function prototypes used for this demo.*/
void handle_error(void);
void btn_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event);

cyhal_gpio_callback_data_t btn_data =
{
		.callback = btn_interrupt_handler,
		.callback_arg = NULL,
};

/*The variables for playing phrases*/
_Bool play_next = false;
int16_t current_phrase;


int main(void)
{
    cy_rslt_t result;
    int ecfg_result = -1;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /*Initialize LEDs*/
    result = cyhal_gpio_init( LED1, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS)
    {handle_error();}
    result = cyhal_gpio_init( LED2, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS)
    {handle_error();}
    result = cyhal_gpio_init( LED3, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS)
    {handle_error();}

    /*Indicate the beginning of the initialization*/
	cyhal_gpio_write((cyhal_gpio_t)LED1, CYBSP_LED_STATE_OFF);
	cyhal_gpio_write((cyhal_gpio_t)LED2, CYBSP_LED_STATE_OFF);
	cyhal_gpio_write((cyhal_gpio_t)LED3, CYBSP_LED_STATE_ON);

    /*Initialize Button*/
    result = cyhal_gpio_init(USER_BTN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, false);
    if (result != CY_RSLT_SUCCESS)
    {handle_error();}

    /*Register callback function */
    cyhal_gpio_register_callback(USER_BTN, &btn_data);

    /* Enable falling edge interrupt event */
    cyhal_gpio_enable_event(USER_BTN, CYHAL_GPIO_IRQ_FALL, BTN_IRQ_PRIORITY, true);
    __enable_irq();

    result = cy_retarget_io_init( KITPROG_TX, KITPROG_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS)
    {
        handle_error();
    }
    printf("\x1b[2J\x1b[;H");
    printf("RDK3 Text-to-Speech Demonstration.\r\n");

	/*Initialize Epson ASIC*/
	result = EPSON_Initialize();
	if(result != CY_RSLT_SUCCESS)
	{
		printf("Text-to-Speech Engine failed to initialize.\r\n");
		 CY_ASSERT(0);
	}

	/*Epson ASIC Hard-Reset Procedure*/
	GPIO_S1V30340_Reset(1);
	CyDelay(100);
	GPIO_S1V30340_Reset(0);
	CyDelay(100);
	GPIO_S1V30340_Reset(1);
	CyDelay(500);

	/*Set Epson ASIC mute signal(MUTE) to High(disable)*/
	GPIO_ControlMute(0);

	/*Set Epson ASIC standby signal(STBYEXIT) to Low(deassert)*/
	GPIO_ControlStandby(0);
	CyDelay(100);

	/*Configure Epson ASIC*/
	ecfg_result = S1V30340_Initialize_Audio_Config();
	while(ecfg_result != 0)
	{
		printf("S1V30340 SPI Initialization failed. \n\r");
		CyDelay(1000);
		ecfg_result = S1V30340_Initialize_Audio_Config();
	}
	printf("S1V30340 SPI Initialization succeeded. \n\r");

	/*Play the greeting*/
	current_phrase = FIRST_PHRASE;
	GPIO_ControlMute(1); /*Mute - OFF*/
	printf("Playing a phrase: %i. \n\r", current_phrase);
	S1V30340_Play_Specific_Audio(current_phrase);
	S1V30340_Wait_For_Termination();
	GPIO_ControlMute(0); /*Mute - ON*/

    /*Indicate the end of the initialization*/
	cyhal_gpio_write((cyhal_gpio_t)LED2, CYBSP_LED_STATE_ON);
	cyhal_gpio_write((cyhal_gpio_t)LED3, CYBSP_LED_STATE_OFF);

    for (;;)
    {
    	CyDelay(500);

    	if(play_next)
    	{
    		cyhal_gpio_write((cyhal_gpio_t)LED2, CYBSP_LED_STATE_OFF);
    		cyhal_gpio_write((cyhal_gpio_t)LED3, CYBSP_LED_STATE_ON);

    		play_next = false;
    		current_phrase++;
    		if(current_phrase > LAST_PHRASE)
    		{
    			current_phrase = FIRST_PHRASE;
    		}
    		printf("Playing a phrase: %i. \n\r", current_phrase);
    		GPIO_ControlMute(1); /*Mute - OFF*/
    		S1V30340_Play_Specific_Audio(current_phrase);
    		S1V30340_Wait_For_Termination();
    		GPIO_ControlMute(0); /*Mute - ON*/

    		cyhal_gpio_write((cyhal_gpio_t)LED2, CYBSP_LED_STATE_ON);
    		cyhal_gpio_write((cyhal_gpio_t)LED3, CYBSP_LED_STATE_OFF);
    	}
    }
}

/* Interrupt handler callback function */
void btn_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event)
{
	CY_UNUSED_PARAMETER(handler_arg);
    CY_UNUSED_PARAMETER(event);

    play_next = true;
}

void handle_error(void)
{
     /* Disable all interrupts. */
    __disable_irq();
    cyhal_gpio_write((cyhal_gpio_t)LED1, CYBSP_LED_STATE_OFF);
    cyhal_gpio_write((cyhal_gpio_t)LED2, CYBSP_LED_STATE_OFF);
    cyhal_gpio_write((cyhal_gpio_t)LED3, CYBSP_LED_STATE_ON);

    CY_ASSERT(0);
}

/* [] END OF FILE */
