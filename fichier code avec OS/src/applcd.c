/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    applcd.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "applcd.h"
#include "Mc32DriverLcd.h"
#include "semphr.h"
#include "queue.h"
#include "checksum.h"
#include <stdio.h>
#include <math.h>

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APPLCD_DATA applcdData;

QueueHandle_t queueTx  = NULL; //déclaration 

#define MSG_BUF_SIZE 9
// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************



// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APPLCD_Initialize ( void )

  Remarks:
    See prototype in applcd.h.
 */

void APPLCD_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    applcdData.state = APPLCD_STATE_INIT;

    
}


/******************************************************************************
  Function:
    void APPLCD_Tasks ( void )

  Remarks:
    See prototype in applcd.h.
 */

void APPLCD_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( applcdData.state )
    {
        /* Application's initial state. */
        case APPLCD_STATE_INIT:
        {
            //init LCD + affichage message
            lcd_init();
            lcd_bl_on();
            printf_lcd("EMSY3 TP5 FreeRTOS");
            lcd_gotoxy(1,2);
            printf_lcd("<LFO & KGR>");
            
            //Initialisation 
            queueTx  = xQueueCreate( 18, MSG_BUF_SIZE);
            
            applcdData.state = APPLCD_STATE_SERVICE_TASKS;

            break;
        }

        case APPLCD_STATE_SERVICE_TASKS:
        {
            BSP_LEDToggle(BSP_LED_0);
            //BSP_LEDOff(BSP_LED_2); //debug
            uint8_t retVal = 0; //par défaut pas de message reçu complet
            char msgBuf[9];
            //Réception d'un élément 
            if (xQueueReceive( queueTx, msgBuf, portMAX_DELAY )){
                //décodage...
                if ( msgBuf[0]==0x02 && msgBuf[1]=='1' && msgBuf[8]==0x03) //message de température ? (contrôle début de trame, type et longueur)   
                {
                    if (CheckChecksum((int8_t*)&msgBuf[1], 5, (int8_t*)&msgBuf[6]))//contrôle checksum
                    {
                        applcdData.newTemp[0] = msgBuf[2];
                        applcdData.newTemp[1] = msgBuf[3];
                        applcdData.newTemp[2] = msgBuf[4];
                        applcdData.newTemp[3] = msgBuf[5];
                        applcdData.newTemp[4] = 0; //fin de chaine
                        retVal = 1;                            
                    }
                    else
                    {
                        applcdData.nbErrors++;
                        retVal = 3;     
                    }               
                }
                else if (msgBuf[5]==0x03 && msgBuf[0]==0x02 && msgBuf[1]=='2') //message de caractère ? (contrôle début de trame,type et longueur) 
                {
                    if (CheckChecksum((int8_t*)&msgBuf[1], 2, (int8_t*)&msgBuf[3]))//contrôle checksum
                    {
                        applcdData.newChar = msgBuf[2];
                        retVal = 2;                            
                    }
                    else
                    {
                        applcdData.nbErrors++;
                        retVal = 3;     
                    }                 
                }
                else
                {
                    applcdData.nbErrors++;
                    retVal = 3;    
                } 
                
                switch (retVal)  
                {
                    case 1: //reçu une température
                    {
                        lcd_gotoxy(1,3);
                        printf_lcd("t: ");
                        printf_lcd("%s", applcdData.newTemp); 
                        lcd_gotoxy(1,4);
                        printf_lcd(" OK / %4d erreurs", applcdData.nbErrors);
                        break;
                    } 
                    case 2: //reçu un caractère
                    {
                        lcd_gotoxy(10,3);
                        printf_lcd("/ car: %c", applcdData.newChar);
                        lcd_gotoxy(1,4);
                        printf_lcd(" OK / %4d erreurs", applcdData.nbErrors);
                        break;
                    }  
                    case 3: //erreur
                    {
                        lcd_gotoxy(1,4);
                        printf_lcd("NOK / %4d erreurs", applcdData.nbErrors);
                        break;
                    }
                    default:
                    {
                        lcd_gotoxy(1,3);
                        printf_lcd(";-;");
                        //rien reçu
                        break;
                    }
                }
            }
            
            //BSP_LEDOn(BSP_LED_2); //debug
            break;
        }

        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

 void APP_SendCharToApp(int8_t c)
{
    int8_t frame[9];
    
    //formatte trame
    frame[0] = 2;   //début de trame
    frame[1] = '2'; //type de trame
    frame[2] = c;
    ComputeChecksum(&frame[1], 2, &frame[3]);   //remplit checksum (frame[3] et [4])
    frame[5] = 3;   //fin de trame
    frame[6] = 0;   //padding pour trame totale 9 car.
    frame[7] = 0;   //padding pour trame totale 9 car.
    frame[8] = 0;   //padding pour trame totale 9 car.
         
    //Placement d'un élément dans la queue 
    xQueueSendFromISR( queueTx , frame, 0U );

}
 
 void APP_SendTempToApp(float fTemp)
{
    int8_t frame[9];
    char sTempUp[3];
    char sTempDown[2];
//    sprintf(sTemp, "%4.1f", (double)fTemp);
//    sprintf(sTemp, "%4d", (uint8_t)fTemp);
//    snprintf(sTemp,sizeof(sTemp), "%4.1f", (double)fTemp);
    
    int16_t iTempUp = (int16_t)fTemp;
    int8_t iTempDown = abs((int8_t)((fTemp - (float)iTempUp)*10.0));
    sprintf(sTempUp, "%2d", iTempUp);
    sprintf(sTempDown, "%d", iTempDown);
    
    
    //formatte trame
    frame[0] = 2;   //début de trame
    frame[1] = '1'; //type de trame
    frame[2] = sTempUp[0];
    frame[3] = sTempUp[1];
    frame[4] = '.';
    frame[5] = sTempDown[0];
    ComputeChecksum(&frame[1], 5, &frame[6]);   //remplit checksum (frame[3] et [4])
    frame[8] = 3;   //fin de trame
    
    //Placement d'un élément dans la queue 
    xQueueSend( queueTx , frame, 0U );
}

/*******************************************************************************
 End of File
 */
