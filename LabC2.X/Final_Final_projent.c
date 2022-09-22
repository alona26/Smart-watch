/*
 * File:   Final_Project.c
 * Author: Alona Rozner
 * Created on April 11, 2021, 16:17 AM
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "System/system.h"
#include "System/delay.h"
#include "oledDriver/oledC.h"
#include "oledDriver/oledC_colors.h"
#include "oledDriver/oledC_shapes.h"
//Global Varible
int repaint = 0; // update the display
int second, minute, hour, day, month;
int prev_second= 0, prev_minute, prev_hour , prev_day, prev_month;
int mode = 0;
int AmPm = 1;
int changeAmPm = 1; // if change am/pm
char Time[]="00:00:00";
char TempTime[]="00:00:00";
char TempDate[]="01/01";

int falg_time_S1 = 1;
char NewTime[]="00:00:00";
char AlarmTime[]="00:00:00";
char NewDate[]="01/01";
char Date[]="01/01";
int mode_set_time;
int alarm_active = 0;
int mode_set;
int mode12or24 = 0;
unsigned char analog_clock[60][2] = {{47, 7}, {51, 7}, {55, 8}, {59, 9}, 
{63, 10}, {67, 12}, {71, 15}, {74, 17}, {77, 20}, {79, 23}, {82, 27}, {84, 31},
{85, 35},{86, 39},{87, 43},{87, 47},{87, 51}, {86, 55}, {85, 59}, {84, 63}, 
{82, 67}, {79, 71}, {77, 74}, {74, 77}, {71, 79}, {67, 82}, {63, 84}, 
{59, 85}, {55, 86}, {51, 87},{47, 87}, {43, 87}, {39, 86}, {35, 85}, {31, 84},
{27, 82}, {23, 79}, {20, 77}, {17, 74}, {15, 71}, {12, 67}, {10, 63}, 
{9, 59}, {8, 55}, {7, 51},{7, 47}, {7, 43}, {8, 39}, {9, 35}, {10, 31}, 
{12, 27}, {15, 23}, {17, 20}, {20, 17}, {23, 15}, {27, 12}, {31, 10}, {35, 9}, 
{39, 8}, {43, 7}};
uint16_t color;
//Functions Declaretion
void Analog_face();
void IncreaseSecond();
void Menu();
void Digital_face();
void interval();


bool S1_Pressed(void){
    if (!PORTAbits.RA11){
       LATAbits.LATA8=1;//LED1 ON
       if (falg_time_S1){ // long press
        DELAY_milliseconds(1200);
       }
       while(PORTAbits.RA11); // until unclick
       LATAbits.LATA8=0;//LED1 OFF
        return true;
    }
     return false;
}
bool S2_Pressed(void){
    if (!PORTAbits.RA12){
       LATAbits.LATA9=1;
       
       while(PORTAbits.RA12); // until unclick
       LATAbits.LATA9=0;
        return true;
    }
     return false;
}
void __attribute__((interrupt,no_auto_psv))_T1Interrupt(void)//THE INTERRUPT FUNCTION
//I USED NO_AUTO_PSV BECUSE I GOT WARNING IN THE OUTPUT TERMINAL 
{    
        IncreaseSecond();   
        IFS0bits.T1IF=0;//INTERRUPT FINSHED    
}
// print small watch to oled
void small_watch(){
            if(prev_second != second){
                oledC_DrawRectangle(82, 0, 98 , 10,  OLEDC_COLOR_BLACK);
                oledC_DrawString(48, 0, 1, 1, (uint8_t *)NewTime, OLEDC_COLOR_WHITE);
                oledC_DrawString(48, 0, 1, 1, (uint8_t *)NewTime, OLEDC_COLOR_WHITE);
            
            }
            if( prev_minute != minute){
                oledC_DrawRectangle(65, 0, 78 , 10,  OLEDC_COLOR_BLACK);
                oledC_DrawString(48, 0, 1, 1, (uint8_t *)NewTime, OLEDC_COLOR_WHITE);
            }
            
            if(  prev_hour != hour){
                oledC_DrawRectangle(47, 0, 59 , 10,  OLEDC_COLOR_BLACK);
                oledC_DrawString(48, 0, 1, 1, (uint8_t *)NewTime, OLEDC_COLOR_WHITE);
            }
}
// if alarm is on draw icon of alram
void Icon_alarm(){
    if (alarm_active ){
    oledC_DrawCircle(5,5,2, OLEDC_COLOR_YELLOW);
    oledC_DrawRing(5,5,5,2,OLEDC_COLOR_YELLOW);
    }
    else {
    oledC_DrawCircle(5,5,2, OLEDC_COLOR_BLACK);
    oledC_DrawRing(5,5,5,2,OLEDC_COLOR_BLACK);
    }
}
// bit bang function
void oledC_BitBangCommand(unsigned char cmd) {
    LATCbits.LATC3 = 0;
    LATCbits.LATC9 = 0;
    uint8_t var;
    bool bit_data;
    int i;
    for(i = 7;i >= 0;i--)
    {
        DELAY_microseconds(1);
        LATBbits.LATB15 = 0;
        var = 0b1;
        bit_data = (cmd >> i) & var;
        LATBbits.LATB14 = bit_data;
        DELAY_microseconds(1);
        LATBbits.LATB15 = 1;
    }

    LATCbits.LATC9 = 1; 

}
void IncreaseSecond(){
    memcpy(Time, NewTime, strlen(NewTime));
    memcpy(Date, NewDate, strlen(NewDate));
    prev_second = second = NewTime[7]-'0'+((NewTime[6]-'0')*10);
    prev_minute = minute;
    minute= NewTime[4]-'0'+((NewTime[3]-'0')*10);
    prev_hour = hour;
    hour = NewTime[1]-'0'+((NewTime[0]-'0')*10);
    day = NewDate[1]-'0'+((NewDate[0]-'0')*10);
    month = NewDate[4]-'0'+((NewDate[3]-'0')*10);
    second += 1; // Increase one second
    //Validate second, minute, hour, day, month.
    if(second == 60){
        second = 0;
        minute += 1;
    }
    if(minute == 60){
        minute = 0;
        hour +=1;
    }
    if(hour == 24 ){ 
        hour = 0;
        day += 1;
    }
    if(month == 2 && day == 28){
        day = 1;
        month += 1;
    }
    if(month != 2 && day == 31){
        day = 1;
        month += 1;
    }
    if(month == 13){
        day = 1;
        month = 1;
    }
        if(hour < 13){ //AM
        changeAmPm = 1;
        }
        if(hour >= 13) { //PM
            changeAmPm = 0;
        }
    interval();
    //Convert to string fo Digital face.
    NewTime[0] = hour / 10 +'0';
    NewTime[1] = hour % 10 +'0';
    NewTime[3] = minute / 10 +'0';
    NewTime[4] = minute % 10 +'0';
    NewTime[6] = second / 10 +'0';
    NewTime[7] = second % 10 +'0';
    NewDate[0] = day / 10 +'0';
    NewDate[1] = day % 10 +'0';
    NewDate[3] = month / 10 +'0';
    NewDate[4] = month % 10 +'0';
    if(alarm_active){
        if ( strcmp(AlarmTime, NewTime) == 0)
        {
        __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
        RPOR7bits.RP14R = 0; //RB14->SPI MOSI
        RPOR7bits.RP15R = 0; //RB15->SPI Clock
        __builtin_write_OSCCONL(OSCCON | 0x40); // lock PPS
        TRISBbits.TRISB14 = 0;
        TRISBbits.TRISB15 = 0;
        TRISCbits.TRISC3 =  0;
        TRISCbits.TRISC9 =  0;

        alarm_active = 0;
        int i;
        for(i = 0; i < 10; i++){ //Flashing fo 20 seconds.
             oledC_BitBangCommand(OLEDC_CMD_SET_DISPLAY_MODE_INVERSE);
            DELAY_milliseconds(1000);
             oledC_BitBangCommand(OLEDC_CMD_SET_DISPLAY_MODE_ON);
            DELAY_milliseconds(1000);
            }
        TRISBbits.TRISB14 = 0;
        TRISBbits.TRISB15 = 0;
        TRISCbits.TRISC9 =  0;
        SYSTEM_Initialize();
        User_initialize();
        oledC_DrawRectangle(35, 24, 60,54 , color);
        oledC_DrawString(4, 30, 2, 2, (uint8_t *)NewTime, OLEDC_COLOR_WHITE);
        oledC_DrawRectangle(0, 24, 28,54 , color);
        oledC_DrawString(4, 30, 2, 2, (uint8_t *)NewTime, OLEDC_COLOR_WHITE);
        }
    }
    repaint = 1;
}
void User_initialize(){
    color = OLEDC_COLOR_BLACK;
    TRISAbits.TRISA11=1;//s1 is input
    TRISAbits.TRISA12=1;//s2 is input
    TRISAbits.TRISA8=0;//LED 1 OUTPUT
    TRISAbits.TRISA9=0;//LED 2 OUTPUT
    T1CONbits.TON=1;                 //SET THE TON BIT TO 1 TO ENABLE THE TIMER
    T1CONbits.TSIDL=1;
    T1CONbits.TCS=0;
    T1CONbits.TCKPS=0b11;                 //CHOOSING THE TIMER PRESCALER RATIO 
    PR1=16000;                              //CHOOSE THE LIMT FOR THE TIMER 1
    IFS0bits.T1IF=0;                        //THE FLAG 
    IPC0bits.T1IP=0b001;                    //PRIORTY
    IEC0bits.T1IE=1;                        //ENABLE 
    INTCON2bits.GIE=1;                      //GLOBAL ENABLE FOR THE PROGRAM               
    //Potentiometer ON
    ANSCbits.ANSC0 = 1 ;
    AD1CON2bits.PVCFG = 0x00 ;
    AD1CON2bits.NVCFG0 = 0x0 ; 
    AD1CON3bits.ADCS = 0xFF ;
    AD1CON1bits.SSRC = 0x0 ;
    AD1CON3bits.SAMC = 0b10000 ;
    AD1CON1bits.FORM = 0b00 ;
    AD1CON2bits.SMPI = 0x0 ;
    AD1CON1bits.MODE12 = 0 ;
    AD1CON1bits.ADON = 1 ;
    AD1CHS = 8 ;//CHANEL
}
void Draw_AmPm(){
    char* AmPmItems[] = { "PM", "AM" };
    if(mode12or24){
        if(AmPm != changeAmPm){
            oledC_DrawString(2, 88, 1, 1, (uint8_t *)AmPmItems[AmPm], 
                    OLEDC_COLOR_BLACK);
            oledC_DrawString(2, 88, 1, 1, (uint8_t *)AmPmItems[changeAmPm], 
                    OLEDC_COLOR_WHITE);
            AmPm = changeAmPm;

        }
        else{
        oledC_DrawString(2, 88, 1, 1, (uint8_t *)AmPmItems[changeAmPm], 
            OLEDC_COLOR_WHITE);
        }
    }
    else{
    oledC_DrawString(2, 88, 1, 1, (uint8_t *)AmPmItems[changeAmPm], 
                OLEDC_COLOR_BLACK);
    }
}
void DrawDate(){

    if(prev_day != day ){
        oledC_DrawRectangle(68, 88, 78,96 , OLEDC_COLOR_BLACK); // Delete the prev day
        oledC_DrawString(68, 88, 1, 1, (uint8_t *)NewDate, OLEDC_COLOR_WHITE);
    }

    if( prev_month != month ){
        oledC_DrawRectangle(86, 88, 96,96 , OLEDC_COLOR_BLACK);// Delete the prev month

        oledC_DrawString(68, 88, 1, 1, (uint8_t *)NewDate, OLEDC_COLOR_WHITE);
    }
}
// print analog face to oled
void Analog_face(){
    oledC_DrawCircle(47, 47,47, color);
    while(1){
        int center = 47;
        int i;
            for (i = 0; i < 60; i++)
            {
                if (i % 5 == 0)
                {
                    oledC_DrawLine(center + (analog_clock[i][0] - 47)*0.9, 
                            center + (analog_clock[i][1] - center)*0.9, 
                            analog_clock[i][0], analog_clock[i][1], 1, 
                            OLEDC_COLOR_WHITE);
                }
            }
        Draw_AmPm();
        DrawDate();
        Icon_alarm();
        oledC_DrawLine(center,center, 
                center + (analog_clock[prev_second][0] - center)*0.8, 
                center + (analog_clock[prev_second][1] - center)*0.8, 1, color);
        if (prev_minute != minute)
            oledC_DrawLine(center,center, 
                    center + (analog_clock[prev_minute][0] - center)*0.6, 
                    center + (analog_clock[prev_minute][1] - center)*0.6, 2, color);
        if (prev_hour != hour)
            oledC_DrawLine(center,center, 
                    center + (analog_clock[(prev_hour % 12) * 5 + prev_minute / 12][0] - center)*0.4, 
                    center + (analog_clock[(prev_hour % 12) * 5 + prev_minute / 12][1] - center)*0.4, 
                    4, color);
        oledC_DrawLine(center,center, 
                center + (analog_clock[second][0] - center)*0.8, 
                center + (analog_clock[second][1] - 47)*0.8, 1, 
                OLEDC_COLOR_RED);
        oledC_DrawLine(center,center, 
                center + (analog_clock[minute][0] - center)*0.6, 
                center + (analog_clock[minute][1] - center)*0.6, 2, 
                OLEDC_COLOR_WHITE);
        oledC_DrawLine(center,center, 
                center + (analog_clock[(hour % 12) * 5 + minute / 12][0] - center)*0.4, 
                center + (analog_clock[(hour % 12) * 5 + minute / 12][1] - center)*0.4, 
                4, OLEDC_COLOR_WHITE);
        if(S1_Pressed()){
            Menu();
        }
        // Shotdown Alarm
        if(S2_Pressed()){
            alarm_active = 0;
            Icon_alarm();
        }
    }
}
// print menu to oled
void DrawMenu(){
    small_watch();
    char* menuItems[] = { "Display Mode", "12H/24H Interval", "Set Time", 
    "Set Date","Themes", "Alarm" , "Exit"};
    oledC_setBackground(OLEDC_COLOR_BLACK);
    oledC_DrawString(8, 12, 1, 1, (uint8_t *)menuItems[0], OLEDC_COLOR_WHITE);
    oledC_DrawString(6, 27, 1, 1, (uint8_t *)menuItems[1], OLEDC_COLOR_WHITE);
    oledC_DrawString(8, 42, 1, 1, (uint8_t *)menuItems[2], OLEDC_COLOR_WHITE);
    oledC_DrawString(8,57, 1, 1, (uint8_t *)menuItems[3], OLEDC_COLOR_WHITE);
    oledC_DrawString(8,72, 1, 1, (uint8_t *)menuItems[4], OLEDC_COLOR_WHITE);
    oledC_DrawString(8, 87,1, 1, (uint8_t *)menuItems[5], OLEDC_COLOR_WHITE);
    oledC_DrawString(70, 87,1, 1, (uint8_t *)menuItems[6], OLEDC_COLOR_WHITE);

}
// get the potentiometer value
int getPotentiometerValue()
{
    AD1CON1bits.SAMP = 1 ;  // A to D          
    int i = 0;
    for (i = 0 ; i < 1000 ; i++) ; 
    AD1CON1bits.SAMP = 0 ;   
    for (i = 0 ; i < 1000 ; i++) ; 
    while (!AD1CON1bits.DONE) ;  
    return ADC1BUF0 ;
}
// print set dat mode
void DrawSetDate(){
        oledC_clearScreen();
        oledC_DrawString(10, 30, 2, 2, (uint8_t *)TempDate, OLEDC_COLOR_WHITE);
}
void SetDate(){
     // S1- locate where we want to change the values
    // S2- Go back to menu
    // Potentiometer- change the value
    int count = 1; 
    int day = 0;
    int month=0;
    int value = 0;
    int number_of_days_in_month = 33;
    DrawSetDate();
    while(1){
        if(S2_Pressed()){
            if (count >= 2 ){
                count = 1;
            }
            else{
                count += 1;
            }
            
            oledC_clearScreen();
            DrawSetDate();
        }
        switch(count){
            case 1:
                if( (month % 10) == 2){
                    number_of_days_in_month = 36;
                }
                if( month % 10 != 2){
                    number_of_days_in_month = 33;
                }
                value = getPotentiometerValue()/number_of_days_in_month;
                oledC_DrawLine(10 , 50, 30 ,50 , 1 , OLEDC_COLOR_WHITE); // space = 20 , space between lines=15
                if( value != getPotentiometerValue()/number_of_days_in_month){
                    value = getPotentiometerValue()/number_of_days_in_month;
                    day = value;
                    TempDate[0] = day / 10 +'0';
                    TempDate[1] = day % 10 +'0';
                    
                    DrawSetDate();
                }
                break;
            case 2:
                value = getPotentiometerValue()/85;
                oledC_DrawLine(43, 50,63 ,50 , 1 , OLEDC_COLOR_WHITE); // space = 20 , space between lines=15
                if( value != getPotentiometerValue()/85){
                    value = getPotentiometerValue()/85;
                    month = value;
                    TempDate[3] = month / 10 +'0';
                    TempDate[4] = month % 10 +'0';
                    DrawSetDate();
                }
                break;
        }
        if(S1_Pressed()){
            strcpy(NewDate,TempDate);
            strcpy(TempDate, "01/01");
            Menu();
        }
    }
}
// print set time to oled
void DrawSetTime(){
        oledC_clearScreen();
        oledC_DrawString(4, 30, 2, 2, (uint8_t *)TempTime, OLEDC_COLOR_WHITE);   
}

void Alarm(){
 // S1- locate where we want to change the values
    // S2- Go back to menu
    // Potentiometer- change the value
    int X_place = 5;
    int count = 1; 
    int temp_value = 0;
        oledC_clearScreen();
        oledC_DrawString(4, 30, 2, 2, (uint8_t *)TempTime, OLEDC_COLOR_WHITE); 
    while(1){
        if(S2_Pressed()){
            if (count >= 3 ){
                count = 1;
            }
            else{
                count += 1;
            }
            
        oledC_clearScreen();
        oledC_DrawString(4, 30, 2, 2, (uint8_t *)TempTime, OLEDC_COLOR_WHITE);
        }
        switch(count){
            case 1:
                temp_value = getPotentiometerValue()/42;
                oledC_DrawLine(X_place , 50, X_place + 20 ,50 , 1 , OLEDC_COLOR_WHITE); // space = 20 , space between lines=15
                if( temp_value != getPotentiometerValue()/42){
                    temp_value = getPotentiometerValue()/42;
                    hour = temp_value;
                    TempTime[0] = hour / 10 +'0';
                    TempTime[1] = hour % 10 +'0';
                    oledC_clearScreen();
        oledC_DrawString(4, 30, 2, 2, (uint8_t *)TempTime, OLEDC_COLOR_WHITE);
                }
                break;
            case 2:
                temp_value = getPotentiometerValue()/17;
                oledC_DrawLine(X_place + (1*32), 50, X_place + 20 + (1*32) ,50 , 1 , OLEDC_COLOR_WHITE); // space = 20 , space between lines=15
                if( temp_value != getPotentiometerValue()/17){
                    temp_value = getPotentiometerValue()/17;
                    minute = temp_value;
                    TempTime[3] = minute / 10 +'0';
                    TempTime[4] = minute % 10 +'0';
                   oledC_clearScreen();
        oledC_DrawString(4, 30, 2, 2, (uint8_t *)TempTime, OLEDC_COLOR_WHITE);
                }
                break;
            case 3:
                temp_value = getPotentiometerValue()/17;
                oledC_DrawLine(X_place + (2*32), 50, X_place + 20 + (2*32) ,50 , 1 , OLEDC_COLOR_WHITE); // space = 20 , space between lines=15
                if( temp_value != getPotentiometerValue()/17){
                    temp_value = getPotentiometerValue()/17;
                    second = temp_value;
                    TempTime[6] = second / 10 +'0';
                    TempTime[7] = second % 10 +'0';
                   oledC_clearScreen();
                    oledC_DrawString(4, 30, 2, 2, (uint8_t *)TempTime, OLEDC_COLOR_WHITE);
                }  
                break;
        }
        if(S1_Pressed()){
            strcpy(AlarmTime, TempTime);
            strcpy(TempTime, "00:00:00");

            Menu();
        } 
    }
}

void SetTime(){
 // S1- locate where we want to change the values
    // S2- Go back to menu
    // Potentiometer- change the value
    int X_place = 5;
    int count = 1; 
    int temp_value = 0;
    DrawSetTime();
    while(1){
        if(S2_Pressed()){
            if (count >= 3 ){
                count = 1;
            }
            else{
                count += 1;
            }
            
            oledC_clearScreen();
            DrawSetTime();
        }
        switch(count){
            case 1:
                temp_value = getPotentiometerValue()/42;
                oledC_DrawLine(X_place , 50, X_place + 20 ,50 ,
                        1 , OLEDC_COLOR_WHITE); // space = 20 , space between lines=15
                if( temp_value != getPotentiometerValue()/42){
                    temp_value = getPotentiometerValue()/42;
                    temp_value %= 23;
                    hour = temp_value;
                    TempTime[0] = hour / 10 +'0';
                    TempTime[1] = hour % 10 +'0';
                    DrawSetTime();
                }
                break;
            case 2:
                temp_value = getPotentiometerValue()/17;
                oledC_DrawLine(X_place + (1*32), 50, X_place + 20 + (1*32) ,50 ,
                        1 , OLEDC_COLOR_WHITE); // space = 20 , space between lines=15
                if( temp_value != getPotentiometerValue()/17){
                    temp_value = getPotentiometerValue()/17;
//                    temp_value %= 59;
                    minute = temp_value;
                    TempTime[3] = minute / 10 +'0';
                    TempTime[4] = minute % 10 +'0';
                    DrawSetTime();
                }
                break;
            case 3:
                temp_value = getPotentiometerValue()/17;
                oledC_DrawLine(X_place + (2*32), 50, X_place + 20 + (2*32) ,50 , 
                        1 , OLEDC_COLOR_WHITE); // space = 20 , space between lines=15
                if( temp_value != getPotentiometerValue()/17){
                    temp_value = getPotentiometerValue()/17;
                    second = temp_value;
                    TempTime[6] = second / 10 +'0';
                    TempTime[7] = second % 10 +'0';
                    DrawSetTime();
                }  
                break;
        }
        if(S1_Pressed()){
            strcpy(NewTime, TempTime);
            strcpy(TempTime, "00:00:00");
            Menu();
        } 
    }
}
void Digital_face(){        
    oledC_DrawRectangle(0, 24, 95,54 , color);

    while(1){
        if (repaint){
            
            repaint = 0;

            if(prev_second != second){
                oledC_DrawRectangle(68, 24, 95,54 , color);

                oledC_DrawString(4, 30, 2, 2, (uint8_t *)NewTime, OLEDC_COLOR_WHITE);
            
            }
            if( prev_minute != minute){
                oledC_DrawRectangle(35, 24, 60,54 , color);
                oledC_DrawString(4, 30, 2, 2, (uint8_t *)NewTime, OLEDC_COLOR_WHITE);
            }
            if(  prev_hour != hour){
                oledC_DrawRectangle(0, 24, 28,54 , color);
                oledC_DrawString(4, 30, 2, 2, (uint8_t *)NewTime, OLEDC_COLOR_WHITE);
            }
            DrawDate();
            Draw_AmPm();
            Icon_alarm();
            
        }
        if(S1_Pressed()){
            Menu();
        }
        // Shotdown Alarm
        if(S2_Pressed()){
            alarm_active = 0;
            Icon_alarm();
        }
    }
}
void interval(){
    if(mode12or24){
        if(hour == 0){
            hour = 12;

        }
        if(hour > 13){
            hour -= 12;
        }
        if (alarm_active && AlarmTime[1]-'0'+((NewTime[0]-'0')*10) == 0){
                AlarmTime[0] = hour / 10 +'0';
                AlarmTime[1] = hour % 10 +'0';

        }
        if (alarm_active && AlarmTime[1]-'0'+((NewTime[0]-'0')*10) > 13){
            AlarmTime[0] = (AlarmTime[1]-'0'+((AlarmTime[0]-'0')*10)-12) / 10 +'0';
            AlarmTime[1] = (AlarmTime[1]-'0'+((AlarmTime[0]-'0')*10)-12) % 10 +'0';

        }
    }
    else{
        if(changeAmPm == 1){
            if(hour == 12){
                hour = 0;
            }
        }
         if(changeAmPm == 0){
            hour += 12;
        }
        if (alarm_active && AlarmTime[1]-'0'+((NewTime[0]-'0')*10) == 0){
            AlarmTime[0] = hour / 10 +'0';
            AlarmTime[1] = hour % 10 +'0';

        }
        if (alarm_active && AlarmTime[1]-'0'+((NewTime[0]-'0')*10) > 13){
            AlarmTime[0] = (AlarmTime[1]-'0'+((AlarmTime[0]-'0')*10)+12) / 10 +'0';
            AlarmTime[1] = (AlarmTime[1]-'0'+((AlarmTime[0]-'0')*10)+12) % 10 +'0';

        }
    }
    
}
void screenCover () {
    char* the_theme[]= {"The Theme"};
    uint16_t colors[] = { OLEDC_COLOR_BLACK, OLEDC_COLOR_BLUE, OLEDC_COLOR_PURPLE, 
    OLEDC_COLOR_DARKGREEN, OLEDC_COLOR_RED };
    int counter = 0 ;
    oledC_clearScreen();
    oledC_DrawString(23, 35, 1, 2, (uint8_t *)the_theme[0], OLEDC_COLOR_WHITE); 

    while (1){
        
        if(S2_Pressed()){
            oledC_clearScreen();
            if (counter > 4){
                counter = 0;
            }
            else {counter ++;}
            oledC_DrawCircle(47, 47,47 ,colors[counter]);
            oledC_DrawString(23, 35, 1, 2,(uint8_t *)the_theme[0], OLEDC_COLOR_WHITE); 
            color = colors[counter];
            
        } 
        if(S1_Pressed()){
            Menu();
        }
        
    }
}
void Menu(){
    char* selected[] = { ">" };
    if(S1_Pressed()){
        int count_menu_option=1;
        falg_time_S1 = 0;
        oledC_clearScreen();
        DrawMenu();
        oledC_DrawString(1, 12,1, 1, (uint8_t *)selected[0], OLEDC_COLOR_WHITE);
        while(1){
            small_watch();
            if(S2_Pressed())
            {
                DrawMenu();
                if(count_menu_option == 6){
                oledC_DrawString(63, 87,1, 1,
                    (uint8_t *)selected[0], OLEDC_COLOR_WHITE);

                }
                else{
                oledC_DrawString(1, 12+(count_menu_option*15),1, 1,
                        (uint8_t *)selected[0], OLEDC_COLOR_WHITE);
                }
                count_menu_option += 1;
                
               if(count_menu_option > 6){
                count_menu_option=0;
               }
            }
            if(S1_Pressed()){
                switch (count_menu_option){
                  case 1:
                    if(mode){mode = 0;}
                    else {mode = 1;}
                      if (mode){
                        oledC_clearScreen();
                        falg_time_S1 = 1;
                        Analog_face();
                      }
                      else {
                        oledC_clearScreen();
                        falg_time_S1 = 1;
                        Digital_face();
                      }
                      break;
                  case 2:
                      mode12or24 = !mode12or24;
                      break;
                 case 3:
                     falg_time_S1 = 1;
                     SetTime();
                      break;
                 case 4:
                     falg_time_S1 = 1;
                     SetDate();
                      break;
                case 5:
                    falg_time_S1 = 1;
                     screenCover();
                      break;
                case 6:
                    falg_time_S1 = 1;
                    alarm_active = 1;
                    Alarm();
                    break;
                    
                case 0:
                    if (mode){
                        oledC_clearScreen();
                        falg_time_S1 = 1;
                        Analog_face();
                      }
                      else {
                        oledC_clearScreen();
                        falg_time_S1 = 1;
                        Digital_face();
                      }

                }
            }
        }
    }
}
int main(void){
    // initialize the system
    SYSTEM_Initialize();
    User_initialize();
    oledC_clearScreen();
    while(1){
        if(S1_Pressed()){
            Menu();
        }
        else{
            Digital_face();
        }
    }
}
