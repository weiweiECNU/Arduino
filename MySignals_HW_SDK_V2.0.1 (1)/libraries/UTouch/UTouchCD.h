// UTouchCD.h
// ----------
//
// Since there are slight deviations in all touch screens you should run a
// calibration on your display module. Run the UTouch_Calibration sketch
// that came with this library and follow the on-screen instructions to
// update this file.
//
// Remember that is you have multiple display modules they will probably 
// require different calibration data so you should run the calibration
// every time you switch to another module.
// You can, of course, store calibration data for all your modules here
// and comment out the ones you dont need at the moment.
//

// These calibration settings works with my ITDB02-3.2S.
// They MIGHT work on your display module, but you should run the
// calibration sketch anyway.


//VERSION 2 (NEW)
#define CAL_Y 0x035F40D0UL
#define CAL_X 0x0033CE96UL
#define CAL_S 0x000EF13FUL


/*
//VERSION 1 (OLD)
#define CAL_X 0x04120145UL
#define CAL_Y 0x005A0F0FUL
#define CAL_S 0x001400F0UL   //invierto coordenadas de x e y, ajusto a 320 y 240
*/


//#define CAL_X 0x01EA07A4UL
//#define CAL_Y 0x01EE87F3UL
