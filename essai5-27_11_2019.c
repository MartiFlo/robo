/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Programme principal du projet roboflo
 *
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#define __MAIN_FILE__

#include <stdio.h>
#include <delays.h>
#include <spi.h>

#include "hardwareConfig.h"
#include "time.h"
#include "serial.h"
#include "bluetoothRN4x.h"
#include "drivers.h"
#include "analog.h"

#define FOREVER    1

#define BASE_DE_TEMPS   50
#define DUREE_CLIGNOTEMENT 500

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void clignotementLed (void)
{
  static uint8_t compteur = 1;

  if (--compteur == 0)
    {
      LED1 = ~LED1;
      compteur = DUREE_CLIGNOTEMENT / BASE_DE_TEMPS;
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void init (void)
{
    int i;
 
  // Initialisation led
  LED1_TRIS = 0;
  LED1 = 0;
  
  TRISAbits.TRISA0 = 1;  //Led suiveur de ligne input
  TRISAbits.TRISA1 = 1; 
  TRISAbits.TRISA2 = 1;
  TRISBbits.TRISB4 = 0;
  PORTBbits.RB4 = 1;
  analogSetup (3); //Nombre de port en analogique

  // Initialise SPI communication
  SDI_TRIS = 1; // SDI -> input
  SCK_TRIS = 0; // SCK -> output
  SDO_TRIS = 0; // SDO -> output
  SS_TRIS = 0; // SS -> output
  OpenSPI (SPI_FOSC_64, MODE_01, SMPMID);
  SPI_CHIP_DESELECT ();

//  // bruit moteur pour tester la communication avec le MCB
  for (i = 0; i < 100; i++)
    {
      LED1 = ~LED1;
      timeDelayMillisecond (50);
    }
  driveMotor (COMMAND_MOTOR_LEFT, 160, 0);
  timeDelayMillisecond (100);
  driveMotor (COMMAND_MOTOR_STOP, 0, 0);
  timeDelayMillisecond (100);
  driveMotor (COMMAND_MOTOR_RIGHT, 0, 160);
  timeDelayMillisecond (100);
  driveMotor (COMMAND_MOTOR_STOP, 0, 0);

  // Initialise USART and bluetooth
  TX_TRIS = 1;
  RX_TRIS = 1;
  bluetoothRN4xSetup ();
}
//--------------------------------------------------------------------------------------------------------------------------------------------
 int abs(int a){
      if(a<0){
          a=-a;
      }
      return a;
  }

void main (void)
{
  unsigned int compteur;
  unsigned int speed;
  unsigned int s;
  char c;
  int i;
  int mini1=2000, moy1=100, maxi1=0,mini2=2000, moy2=100, maxi2=0,mini3=2000, moy3=100, maxi3=0;
  unsigned int d,m,g;

  //init ();

  do
    {
      printf ((constStringPtr_t) "Appuyez sur la touche 's' pour démarrer\n");
      c = bluetoothRN4xReadChar ();
      timeDelayMillisecond (1000);
    } 
  while (c != 's');
  printf ((constStringPtr_t) "Start ok\n");
  
  // calibrage moy demarrage
  driveMotor (COMMAND_MOTOR_LEFT, 160, -160);
  for ( i = 0; i < 16; i++)
    {
        d = analogRead(0) ; //On lit le port 0
        m = analogRead(1) ;
        g = analogRead(2) ;
                
        maxi1 = (d + maxi1 + abs(d-maxi1) ) / 2;
        maxi2 = (m + maxi2 + abs(m-maxi2) ) / 2;
        maxi3 = (g + maxi3 + abs(g-maxi3) ) / 2;
        mini1 = (d + mini1 - abs(d-mini1) ) / 2;
        mini2 = (m + mini2 - abs(m-mini2) ) / 2;
        mini3 = (g + mini3 - abs(g-mini3) ) / 2;
        moy1= (mini1+maxi1)/2;
        moy2= (mini2+maxi2)/2;
        moy3= (mini3+maxi3)/2;
        timeDelayMillisecond (100);
    }
    printf((constStringPtr_t)"Valeur moyd = %u\n", moy1);
    printf((constStringPtr_t)"Valeur moym = %u\n", moy2);
    printf((constStringPtr_t)"Valeur moyg = %u\n", moy3);
    driveMotor (COMMAND_MOTOR_STOP, 0, 0);
      
  compteur = 0;
           
  // Boucle principale
  while (FOREVER)
    {
      timeDelayMillisecond (BASE_DE_TEMPS);
      clignotementLed ();
      c = bluetoothRN4xGetkey ();
      
      d = analogRead(0) ; //On lit le port 0
      m = analogRead(1) ;
      g = analogRead(2) ;
    
      if (m>moy2)
        {
            if (d<moy1)
            {
                driveMotor (COMMAND_MOTOR_FORWARD, 60, 80); //si capteur droit sort ligne alors pousse legerement plus a gauche
            }
            if (g<moy3) //capt de gauche plus sur ligne
            {
                driveMotor (COMMAND_MOTOR_FORWARD, 80, 60);
            }
            elif 
            {
              driveMotor (COMMAND_MOTOR_FORWARD, 80, 80); //3 capteurs pas possible sur ligne en meme tps si milieu pile au centre alors d et g sont passur ligne 
            }
      }

      else //si milieu sort de la ligne 
      {
          driveMotor (COMMAND_MOTOR_STOP, 0, 0);
      }  
  }
}
