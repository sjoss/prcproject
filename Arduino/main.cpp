/*
Modified by Amy
version 0.4
 
Description:
  Update the library and sketch to compatible with IDE V1.0 and earlier
  Improve the key detection function to be compatible with the Arduino Leonardo
  Improve the waitOK function
  Move the bit map array to the main code
 
Any suggestions are welcome.
E-mail: Lauran.pan@gmail.com
 
Editor     : Amy from DFRobot
Date       : 20.12.2012
 
 
*/
 #include "SPI.h"
#include "LCD4884.h"
#include "Pcr.h"
#include "MenuParam.h"


 
char* PCR_STOP  = "PCR : STOP";
char* PCR_START  = "PCR : START";
char* pcrState;
Pcr pcrProcess;
int  adc_key_val[5] ={
  50, 200, 400, 600, 800 };
 
// debounce counters
byte button_count[NUM_KEYS];
// button status - pressed/released
byte button_status[NUM_KEYS];
// button on flags for user program 
byte button_flag[NUM_KEYS];
 char get_key(unsigned int input)
{
  char k;
 
  for (k = 0; k < NUM_KEYS; k++)
  {
    if (input < adc_key_val[k])
    {
 
      return k;
    }
  }
 
  if (k >= NUM_KEYS)
    k = -1;     // No valid key pressed
 
  return k;
}
 char* startStopPcr(){
  char* test = "Start PCR";
  if (pcrState == PCR_START){
    test = "Stop PCR";
  }
 return test;
} 
char* pcrCurrentState(){
   return "look state"; 
}
 
 
 
 
// menu definition
char* (*menu_items[NUM_MENU_ITEM])(void)={
  startStopPcr,
  pcrCurrentState
};
 
void runPCR()
{
  lcd.LCD_write_string(10, 1, "run PCR", MENU_NORMAL);
  pcrState = PCR_START;
  //launch pcr
  pcrProcess.run();
   
}
void stopPCR()
{
  lcd.LCD_write_string(10, 1, "stop PCR", MENU_NORMAL);
  pcrState = PCR_STOP;
   pcrProcess.stop();
 //stop pcr => fan start to 30°c 
}
void update_adc_key(){
  int adc_key_in;
  char key_in;
  byte i;
 
  adc_key_in = analogRead(0);
  key_in = get_key(adc_key_in);
  for(i=0; i<NUM_KEYS; i++)
  {
    if(key_in==i)  //one key is pressed 
    { 
      if(button_count[i]<DEBOUNCE_MAX)
      {
        button_count[i]++;
        if(button_count[i]>DEBOUNCE_ON)
        {
          if(button_status[i] == 0)
          {
            button_flag[i] = 1;
            button_status[i] = 1; //button debounced to 'pressed' status
          }
 
        }
      }
 
    }
    else // no button pressed
    {
      if (button_count[i] >0)
      {  
        button_flag[i] = 0; 
        button_count[i]--;
        if(button_count[i]<DEBOUNCE_OFF){
          button_status[i]=0;   //button debounced to 'released' status
        }
      }
    }
  }
}



void waitfor_OKkey(){
  byte i;
  byte key = 0xFE;
  for(byte i=0; i<NUM_KEYS; i++){
    button_count[i]=0;
    button_status[i]=0;
    button_flag[i]=0;
  }
  update_adc_key();
  while (key!= CENTER_KEY){
    for(i=0; i<NUM_KEYS; i++){
      if(button_flag[i] !=0){
        button_flag[i]=0;  // reset button flag
        if(i== CENTER_KEY) 
        {
          key=CENTER_KEY; 
        }
      }
      update_adc_key();
    }
  }
}

void  controlPCR(){
   if (pcrState == PCR_START){
    stopPCR();
  }else{
     runPCR();
  }
  lcd.LCD_write_string(38, 5, "OK", MENU_HIGHLIGHT );
  waitfor_OKkey();
}

// waiting for center key press

void lookPcrState(){
  lcd.LCD_write_string(38, 5, "OK", MENU_HIGHLIGHT );
  waitfor_OKkey();
}

void (*menu_funcs[NUM_MENU_ITEM])(void) = {
  controlPCR,
  lookPcrState
};
 
char current_menu_item;
 


 
void init_MENU(void){
 
  byte i;
 
  lcd.LCD_clear();
  lcd.LCD_write_string(MENU_X, MENU_Y, (menu_items[0])(), MENU_HIGHLIGHT );
 
  for (i=1; i<NUM_MENU_ITEM; i++){
    lcd.LCD_write_string(MENU_X, MENU_Y+i, (menu_items[i])(), MENU_NORMAL);
  }
 
 
}


void setup()
{
 pcrState = PCR_STOP;
  // reset button arrays
  for(byte i=0; i<NUM_KEYS; i++){
    button_count[i]=0;
    button_status[i]=0;
    button_flag[i]=0;
  }
 
  lcd.LCD_init();
  lcd.LCD_clear();
 
  //menu initialization
  init_MENU();
  current_menu_item = 0;    
 
  lcd.backlight(ON);//Turn on the backlight
  //lcd.backlight(OFF); // Turn off the backlight  
}
 
unsigned long buttonFlasher = 0;
 



/* loop */
 
void loop()
{
  byte i;
  lcd.LCD_write_string(MENU_X, MENU_Y-2, pcrState, MENU_NORMAL);
  lcd.LCD_write_string(MENU_X, MENU_Y-1, "", MENU_NORMAL);
  for(i=0; i<NUM_KEYS; i++){
    if(button_flag[i] !=0){
 
      button_flag[i]=0;  // reset button flag
      switch(i){
 
      case UP_KEY:
        // current item to normal display
        lcd.LCD_write_string(MENU_X, MENU_Y + current_menu_item, (menu_items[current_menu_item])(), MENU_NORMAL );
        current_menu_item -=1;
        if(current_menu_item <0)  current_menu_item = NUM_MENU_ITEM -1;
        // next item to highlight display
        lcd.LCD_write_string(MENU_X, MENU_Y + current_menu_item, (menu_items[current_menu_item])(), MENU_HIGHLIGHT );
        break;
      case DOWN_KEY:
        // current item to normal display
        lcd.LCD_write_string(MENU_X, MENU_Y + current_menu_item, (menu_items[current_menu_item])(), MENU_NORMAL );
        current_menu_item +=1;
        if(current_menu_item >(NUM_MENU_ITEM-1))  current_menu_item = 0;
        // next item to highlight display
        lcd.LCD_write_string(MENU_X, MENU_Y + current_menu_item, (menu_items[current_menu_item])(), MENU_HIGHLIGHT );
        break;
      case LEFT_KEY:
        init_MENU();
        current_menu_item = 0;
        break;   
      case CENTER_KEY:
        lcd.LCD_clear();
        (*menu_funcs[current_menu_item])();
        //(*menu_funcs[0])();        
       // Serial.println("hello");
        lcd.LCD_clear();
        init_MENU();
        current_menu_item = 0;         
        break;  
      }
 
    }
  }
  if(millis() - buttonFlasher > 5){
    update_adc_key();
    buttonFlasher = millis();
   }    
}

/* menu functions */
 

 


 

 
