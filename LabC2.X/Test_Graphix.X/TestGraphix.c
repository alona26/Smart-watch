/*
 * File:   LabC2.c
 * Author: Amit
 *
 * Created on April 11, 2021, 16:17 AM
 */

#include <stdlib.h>
#include <stdio.h>

#include "System/system.h"
#include "System/delay.h"
#include "oledDriver/oledC.h"
#include "oledDriver/oledC_colors.h"
#include "oledDriver/oledC_shapes.h"

/*
                         Main application
 */
int main(void)
{
    uint8_t x, y;
    // initialize the system
    SYSTEM_Initialize();

    oledC_setBackground(OLEDC_COLOR_SKYBLUE);
    
    //Main loop
    while(1)
    {
      oledC_clearScreen();

      for (x=0 ; x<=94 ; ++x)
        oledC_DrawLine(47, 47, x, 0, 1, OLEDC_COLOR_DARKRED);

      for (x=0 ; x<=94 ; ++x)
        oledC_DrawLine(47, 47, x, 94, 1, OLEDC_COLOR_DARKGREEN);

      for (y=0 ; y<94 ; ++y)
        oledC_DrawLine(47, 47, 0, y, 1, OLEDC_COLOR_DARKVIOLET);

      for (y=0 ; y<94 ; ++y)
        oledC_DrawLine(47, 47, 94, y, 1, OLEDC_COLOR_DARKBLUE);

      DELAY_milliseconds(5000);
    }
  return 1;
}
/**
 End of File
*/

