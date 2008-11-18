/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *      Defines and prototypes for LCD_driver.c
 *
 * \par Application note:
 *      AVR065: LCD Driver for the STK502
 *
 * \par Documentation:
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Name: RELEASE_1_1 $
 * $Revision: 1.3 $
 * $RCSfile: LCD_driver.h,v $
 * $Date: 2006/02/16 18:14:01 $  \n
 ******************************************************************************/

/**************************************************************/
// Definitions
/**************************************************************/
#define LCD_INITIAL_CONTRAST 0x0F
#define LCD_TIMER_SEED		15
#define LCD_REGISTER_COUNT  20

/**************************************************************/
//MACROS
/**************************************************************/
#define LCD_SET_COLON(active) LCD_displayData[8]=active; //!< active =[TRUE;FALSE]
#define pLCDREG (*(unsigned char *)(0xEC))                // DEVICE SPECIFIC!!! (ATmega169(P))
#define LCD_CONTRAST_LEVEL(level) LCDCCR=(0x0F & level);  // DEVICE SPECIFIC!!! (ATmega169(P))

/**************************************************************/
// Global functions
/**************************************************************/
void LCD_Init (void);
void LCD_WriteDigit(unsigned char input, unsigned char digit);
void LCD_AllSegments(unsigned char);

/**************************************************************/
// Global variables
/**************************************************************/
union _LCD_status{
  unsigned char allFields;
  struct{
    volatile unsigned char updateRequired : 1; //!< Indicates that the LCD_displayData should be latched
                                      //! to the LCD Data Registers.C an be used to block LCD
                                      //! updating, while updating LCD_displayData.
    volatile unsigned char updateComplete : 1; //!< Indicates that the LCD_displayData has been latched
                                      //! to the LCD Data Registers. Can be used to determine
                                      //! if LCD_displayData is ready for new data.
    unsigned char unused : 6;
  };
};
extern union _LCD_status LCD_status;

extern unsigned char LCD_timer;           //!< Determine the delay to next LCD update
extern unsigned char LCD_displayData[LCD_REGISTER_COUNT];
