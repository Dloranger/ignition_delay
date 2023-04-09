#include <msp430G2553.h>
#define selfProgram 0x01
#define ENABLED		0x01
#define DISABLED 	0x00
#define HW_REV	0x1C	// 1 Channel
//#define HW_REV	0x2C	// 2 Channel - not released
//#define HW_REV	0x3C	// 3 Channel - not released
//#define HW_REV	0x5C	// 5 Channel - not released
//#define HW_REV	0xFC	// 16 Channel (Dash controller) - not released

//Semaphores for interrupt activities+++++++++++++
extern unsigned char Semaphores;
extern unsigned const char TimerSemaphore;
extern unsigned const char CommandProcessSemaphore;
extern unsigned const char SystemSelfTest;

extern unsigned char StatusBits;

extern unsigned const char InvalidCommand;

/******************************* Safe start option *****************************/
//#define SafeStart 0x01

/******************************* UART SIGNALS **********************************/

#define UART_PSEL 			P1SEL
//#define BAUDRATE 			0x01			//SMCLK = 01MHZ, 9600 Baud
//#define BAUDRATE 			0x02			//SMCLK = 02MHZ, 9600 Baud
//#define BAUDRATE 			0x04			//SMCLK = 04MHZ, 9600 Baud
//#define BAUDRATE 			0x06			//SMCLK = 06MHZ, 9600 Baud
//#define BAUDRATE 			0x08			//SMCLK = 08MHZ, 9600 Baud
//#define BAUDRATE 			0x0A			//SMCLK = 10MHZ, 9600 Baud
//#define BAUDRATE 			0x0C			//SMCLK = 12MHZ, 9600 Baud
//#define BAUDRATE 			0x10			//SMCLK = 16MHZ, 9600 Baud
//#define BAUDRATE 			0x20			//SMCLK = 16MHZ, 115200 Baud

extern volatile unsigned char UART_PSEL;
extern volatile unsigned char UCA0BR0;
extern volatile unsigned char UCA0BR1;
extern volatile unsigned char UCA0CTL1;
extern volatile unsigned char UCA0MCTL;
extern volatile unsigned char UCA0TXBUF;

/************************* FLASH BLOCK D block of 64 bytes ******************************/
#define FLASH_D						0x1000
// Serial Number (8-chars)
#define WEEK_PTR  					FLASH_D
#define YEAR_PTR 					FLASH_D+1
#define SERIAL_1_PTR 				FLASH_D+2
#define SERIAL_2_PTR				FLASH_D+3
// Calibration Settings (6-BYTES)
#define VOLTAGE_PTR					FLASH_D+4
#define	RESISTOR_R7					FLASH_D+5
#define RESISTOR_R6					FLASH_D+6

//Test Data (3-INT)
#define TEST_WW_PTR					FLASH_D+7
#define TEST_YY_PTR					FLASH_D+8
#define TEST_RS_PTR					FLASH_D+9

// MISC settings (12-BYTES)
#define HeartBeatDelay_PTR 			FLASH_D+10		//number of seconds between heart beat
#define OverVoltageDelay_PTR 		FLASH_D+11		//number of seconds to delay on over voltage condition
#define UnderVoltageDelay_PTR 		FLASH_D+12		//number of seconds to delay on under voltage condition
#define LowBatCritical_PTR			FLASH_D+13		//mV
#define LowBatWarn_PTR				FLASH_D+14		//mV
#define HighBatCritical_PTR			FLASH_D+15		//mV
#define IgnitionlessTurnOffVolt_PTR	FLASH_D+16		//mV
#define IgnitionlessRunningVolt_PTR FLASH_D+17		//mv
#define HeartbeatDuration_PTR		FLASH_D+18		//milliSeconds

//Timer3 Settings (8-BYTES)
#define Timer3_0_PTR				FLASH_D+19		//Seconds
#define Timer3_1_PTR				FLASH_D+20		//Seconds
#define Timer3_2_PTR				FLASH_D+21		//Seconds
#define Timer3_3_PTR				FLASH_D+22		//Seconds

//Timer1 Settings (16-BYTES)
#define Timer1_0_PTR				FLASH_D+24		//Seconds
#define Timer1_1_PTR				FLASH_D+25		//Seconds
#define Timer1_2_PTR				FLASH_D+26		//Seconds
#define Timer1_3_PTR				FLASH_D+27		//Seconds
#define Timer1_4_PTR				FLASH_D+28		//Seconds
#define Timer1_5_PTR				FLASH_D+29		//Seconds
#define Timer1_6_PTR				FLASH_D+30		//Seconds
#define Timer1_7_PTR				FLASH_D+31		//Seconds



/************************* FLASH BLOCK C block of 64 bytes ******************************/
#define FLASH_C						0x1040

//Timer2 Settings (16-BYTES)
#define Timer2_0_PTR				FLASH_C			//Seconds
#define Timer2_1_PTR				FLASH_C+1		//Seconds
#define Timer2_2_PTR				FLASH_C+2		//Seconds
#define Timer2_3_PTR				FLASH_C+3		//Seconds
#define Timer2_4_PTR				FLASH_C+4		//Seconds
#define Timer2_5_PTR				FLASH_C+5		//Seconds
#define Timer2_6_PTR				FLASH_C+6		//Seconds
#define Timer2_7_PTR				FLASH_C+7		//Seconds

//Timer4 Settings (8-BYTES)
#define Timer4_0_PTR				FLASH_C+8		//Seconds
#define Timer4_1_PTR				FLASH_C+9		//Seconds
#define Timer4_2_PTR				FLASH_C+10		//Seconds
#define Timer4_3_PTR				FLASH_C+11		//Seconds
//Timer5 Settings (-BYTES)
#define Timer5_0_PTR				FLASH_C+12		//Seconds
#define Timer5_1_PTR				FLASH_C+13		//Seconds
#define Timer5_2_PTR				FLASH_C+14		//Seconds
#define Timer5_3_PTR				FLASH_C+15		//Seconds

#define RemoteStartPulseDivider_PTR		FLASH_C+16		//1sec/value


/************************* FLASH BLOCK B block of 64 bytes ******************************/
#define FLASH_B						0x1080

/************************* FLASH BLOCK A block of 64 bytes ******************************/
#define FLASH_A						0x10C0
//Be very careful using this memory, it has special requirements for data formatting

/******************************* GLOBAL SIGNALS **********************************/
extern volatile unsigned char IE2;
extern volatile unsigned char IFG2;

/*********************************************** Pin Definitions ********************/
#if(HW_REV == 0x5C)  // 5 CH Timer
//PORT 3
//Pin 3.0 D
//#define		RED_LED_PIN				BIT4
//#define		RED_LED_DIR				P3DIR
//#define		RED_LED_OUT				P3OUT
//#define		RED_LED_REN				P3REN
//#define		RED_LED_IN				P3IN
//#define		RED_LEDResEnable		ENABLED

//Pin 3.1 D
#define		IGNITION_PIN			BIT1
#define		IGNITION_DIR			P3DIR
#define		IGNITION_OUT			P3OUT
#define		IGNITION_REN			P3REN
#define		IGNITION_IN				P3IN
#define		IGNITIONResEnable		DISABLED

//Pin 3.2 D
#define		OutputDriver4_PIN		BIT2
#define		OutputDriver4_DIR		P3DIR
#define		OutputDriver4_REN		P3REN
#define		OutputDriver4_OUT		P3OUT
#define		OutputDriver4ResEnable	ENABLED

//Pin 3.3 D
#define		OutputDriver5_PIN		BIT3
#define		OutputDriver5_DIR		P3DIR
#define		OutputDriver5_REN		P3REN
#define		OutputDriver5_OUT		P3OUT
#define		OutputDriver5ResEnable	ENABLED

//Pin 3.4 D
#define		TIMER3a_PIN				BIT4
#define		TIMER3a_DIR				P3DIR
#define		TIMER3a_OUT				P3OUT
#define		TIMER3a_REN				P3REN
#define		TIMER3a_IN				P3IN
#define		TIMER3aResEnable		ENABLED


//Pin 3.5 D
#define		TIMER3b_PIN				BIT5
#define		TIMER3b_DIR				P3DIR
#define		TIMER3b_OUT				P3OUT
#define		TIMER3b_REN				P3REN
#define		TIMER3b_IN				P3IN
#define		TIMER3bResEnable		ENABLED


//Pin 3.6 D
#define		TIMER4a_PIN				BIT4
#define		TIMER4a_DIR				P3DIR
#define		TIMER4a_OUT				P3OUT
#define		TIMER4a_REN				P3REN
#define		TIMER4a_IN				P3IN
#define		TIMER4aResEnable		ENABLED

//Pin 3.7 D
#define		TIMER4b_PIN				BIT5
#define		TIMER4b_DIR				P3DIR
#define		TIMER4b_OUT				P3OUT
#define		TIMER4b_REN				P3REN
#define		TIMER4b_IN				P3IN
#define		TIMER4bResEnable		ENABLED

//PORT 2
//Pin 2.0 D
// unused
//#define		LowBatAlarm_PIN			BIT0
//#define		LowBatAlarm_DIR			P2DIR
//#define		LowBatAlarm_REN			P2REN
//#define		LowBatAlarm_OUT			P2OUT
//#define		LowBatAlarmResEnable	DISABLED

//Pin 2.1 D
//#define		GRN_LED_PIN				BIT5
//#define		GRN_LED_DIR				P3DIR
//#define		GRN_LED_OUT				P3OUT
//#define		GRN_LED_REN				P3REN
//#define		GRN_LED_IN				P3IN
//#define		GRN_LEDResEnable		DISABLED


//Pin 2.2 D
#define		TIMER1a_PIN				BIT2
#define		TIMER1a_DIR				P2DIR
#define		TIMER1a_OUT				P2OUT
#define		TIMER1a_REN				P2REN
#define		TIMER1a_IN				P2IN
#define		TIMER1aResEnable		ENABLED

//Pin 2.3 D
#define		TIMER1b_PIN				BIT3
#define		TIMER1b_DIR				P2DIR
#define		TIMER1b_OUT				P2OUT
#define		TIMER1b_REN				P2REN
#define		TIMER1b_IN				P2IN
#define		TIMER1bResEnable		ENABLED
//Pin 2.4 D
#define		TIMER2a_PIN				BIT4
#define		TIMER2a_DIR				P2DIR
#define		TIMER2a_OUT				P2OUT
#define		TIMER2a_REN				P2REN
#define		TIMER2a_IN				P2IN
#define		TIMER2aResEnable		ENABLED
//Pin 2.5 D
#define		TIMER2b_PIN				BIT5
#define		TIMER2b_DIR				P2DIR
#define		TIMER2b_OUT				P2OUT
#define		TIMER2b_REN				P2REN
#define		TIMER2b_IN				P2IN
#define		TIMER2bResEnable		ENABLED

//Pin 2.6 D
#define		VoltageMonitorEnable_PIN		BIT6
#define		VoltageMonitorEnable_DIR		P2DIR
#define		VoltageMonitorEnable_OUT		P2OUT
#define		VoltageMonitorEnable_REN		P2REN
#define		VoltageMonitorEnable_IN			P2IN
#define		VoltageMonitorEnableResEnable	ENABLED

//Pin 2.7 D
#define		IgnitionSenseEnable_PIN			BIT7
#define		IgnitionSenseEnable_DIR			P2DIR
#define		IgnitionSenseEnable_OUT			P2OUT
#define		IgnitionSenseEnable_REN			P2REN
#define		IgnitionSenseEnable_IN			P2IN
#define		IgnitionSenseEnableResEnable	ENABLED


//PORT 1
//Pin 1.0 D
#define		BAT_SENSE_PIN			BIT0

//Pin 1.1 UART D
#define UART_RX_PIN 				BIT1

//Pin 1.2 UART D
#define UART_TX_PIN 				BIT2

//Pin 1.3 D
#define		OutputDriver1_PIN		BIT3
#define		OutputDriver1_DIR		P1DIR
#define		OutputDriver1_REN		P1REN
#define		OutputDriver1_OUT		P1OUT
#define		OutputDriver1ResEnable	ENABLED

//Pin 1.4 D
#define		OutputDriver2_PIN		BIT4
#define		OutputDriver2_DIR		P1DIR
#define		OutputDriver2_REN		P1REN
#define		OutputDriver2_OUT		P1OUT
#define		OutputDriver2ResEnable	ENABLED

//Pin 1.5 D
#define		OutputDriver3_PIN		BIT5
#define		OutputDriver3_DIR		P1DIR
#define		OutputDriver3_REN		P1REN
#define		OutputDriver3_OUT		P1OUT
#define		OutputDriver3ResEnable	ENABLED

//Pin 1.6
#define		TIMER5a_PIN				BIT6
#define		TIMER5a_DIR				P1DIR
#define		TIMER5a_OUT				P1OUT
#define		TIMER5a_REN				P1REN
#define		TIMER5a_IN				P1IN
#define		TIMER5aResEnable		ENABLED

//Pin 1.7
#define		TIMER5b_PIN				BIT7
#define		TIMER5b_DIR				P1DIR
#define		TIMER5b_OUT				P1OUT
#define		TIMER5b_REN				P1REN
#define		TIMER5b_IN				P1IN
#define		TIMER5bResEnable		ENABLED

#elif(HW_REV == 0x3C)  // 3 CH Timer
//PORT 3
//Pin 3.0 D
#define		TIMER3b_PIN				BIT0
#define		TIMER3b_DIR				P3DIR
#define		TIMER3b_OUT				P3OUT
#define		TIMER3b_REN				P3REN
#define		TIMER3b_IN				P3IN
#define		TIMER3bResEnable		ENABLED

//Pin 3.1 D
#define		TIMER3a_PIN				BIT1
#define		TIMER3a_DIR				P3DIR
#define		TIMER3a_OUT				P3OUT
#define		TIMER3a_REN				P3REN
#define		TIMER3a_IN				P3IN
#define		TIMER3aResEnable		ENABLED

//Pin 3.2 D
#define		TIMER1b_PIN				BIT2
#define		TIMER1b_DIR				P3DIR
#define		TIMER1b_OUT				P3OUT
#define		TIMER1b_REN				P3REN
#define		TIMER1b_IN				P3IN
#define		TIMER1bResEnable		ENABLED

//Pin 3.3 D
#define		TIMER1a_PIN				BIT3
#define		TIMER1a_DIR				P3DIR
#define		TIMER1a_OUT				P3OUT
#define		TIMER1a_REN				P3REN
#define		TIMER1a_IN				P3IN
#define		TIMER1aResEnable		ENABLED

//Pin 3.4 D
//#define		RED_LED_PIN				BIT4
//#define		RED_LED_DIR				P3DIR
//#define		RED_LED_OUT				P3OUT
//#define		RED_LED_REN				P3REN
//#define		RED_LED_IN				P3IN
//#define		RED_LEDResEnable		ENABLED

//Pin 3.5 D
//#define		GRN_LED_PIN				BIT5
//#define		GRN_LED_DIR				P3DIR
//#define		GRN_LED_OUT				P3OUT
//#define		GRN_LED_REN				P3REN
//#define		GRN_LED_IN				P3IN
//#define		GRN_LEDResEnable		DISABLED

//Pin 3.6 D
#define		IGNITION_PIN			BIT6
#define		IGNITION_DIR			P3DIR
#define		IGNITION_OUT			P3OUT
#define		IGNITION_REN			P3REN
#define		IGNITION_IN				P3IN
#define		IGNITIONResEnable		DISABLED

//Pin 3.7 D
#define		OutputDriver3_PIN		BIT7
#define		OutputDriver3_DIR		P3DIR
#define		OutputDriver3_REN		P3REN
#define		OutputDriver3_OUT		P3OUT
#define		OutputDriver3ResEnable	ENABLED

//PORT 2
//Pin 2.0
//#define		LowBatAlarm_PIN			BIT0
//#define		LowBatAlarm_DIR			P2DIR
//#define		LowBatAlarm_REN			P2REN
//#define		LowBatAlarm_OUT			P2OUT
//#define		LowBatAlarmResEnable	DISABLED

//Pin 2.1 D
#define		TIMER2b_PIN				BIT1
#define		TIMER2b_DIR				P2DIR
#define		TIMER2b_OUT				P2OUT
#define		TIMER2b_REN				P2REN
#define		TIMER2b_IN				P2IN
#define		TIMER2bResEnable		ENABLED

//Pin 2.2 D
#define		TIMER2a_PIN				BIT2
#define		TIMER2a_DIR				P2DIR
#define		TIMER2a_OUT				P2OUT
#define		TIMER2a_REN				P2REN
#define		TIMER2a_IN				P2IN
#define		TIMER2aResEnable		ENABLED
//Pin 2.3

//Pin 2.4

//Pin 2.5
//#define		TIMER1c_PIN				BIT5
//#define		TIMER1c_DIR				P2DIR
//#define		TIMER1c_OUT				P2OUT
//#define		TIMER1c_REN				P2REN
//#define		TIMER1c_IN				P2IN
//#define		TIMER1cResEnable		ENABLED

//Pin 2.6
// Not Used

//Pin 2.7
//Not Used


//PORT 1
//Pin 1.0
#define		BAT_SENSE_PIN			BIT0

//Pin 1.1 UART
#define UART_RX_PIN 				BIT1

//Pin 1.2 UART
#define UART_TX_PIN 				BIT2

//Pin 1.3
// Not Used

//Pin 1.4 D
#define		VoltageMonitorEnable_PIN		BIT4
#define		VoltageMonitorEnable_DIR		P1DIR
#define		VoltageMonitorEnable_OUT		P1OUT
#define		VoltageMonitorEnable_REN		P1REN
#define		VoltageMonitorEnable_IN			P1IN
#define		VoltageMonitorEnableResEnable	ENABLED

//Pin 1.5 D
#define		IgnitionSenseEnable_PIN			BIT5
#define		IgnitionSenseEnable_DIR			P1DIR
#define		IgnitionSenseEnable_OUT			P1OUT
#define		IgnitionSenseEnable_REN			P1REN
#define		IgnitionSenseEnable_IN			P1IN
#define		IgnitionSenseEnableResEnable	ENABLED

//Pin 1.6 D
#define		OutputDriver2_PIN		BIT6
#define		OutputDriver2_DIR		P1DIR
#define		OutputDriver2_REN		P1REN
#define		OutputDriver2_OUT		P1OUT
#define		OutputDriver2ResEnable	ENABLED
//Pin 1.7 D
#define		OutputDriver1_PIN		BIT7
#define		OutputDriver1_DIR		P1DIR
#define		OutputDriver1_REN		P1REN
#define		OutputDriver1_OUT		P1OUT
#define		OutputDriver1ResEnable	ENABLED

#elif(HW_REV == 0x1C)	// 1 CH Timer
//Pin 2.0
#define		RED_LED_PIN				BIT0
#define		RED_LED_DIR				P2DIR
#define		RED_LED_OUT				P2OUT
#define		RED_LED_REN				P2REN
#define		RED_LED_IN				P2IN
#define		RED_LEDResEnable		DISABLED

//Pin 2.1
#define		GRN_LED_PIN				BIT1
#define		GRN_LED_DIR				P2DIR
#define		GRN_LED_OUT				P2OUT
#define		GRN_LED_REN				P2REN
#define		GRN_LED_IN				P2IN
#define		GRN_LEDResEnable		DISABLED

//Pin 2.2 (Test Point)
//#define		OutputDriver2_PIN		BIT2
//#define		OutputDriver2_DIR		P2DIR
//#define		OutputDriver2_REN		P2REN
//#define		OutputDriver2_OUT		P2OUT
//#define		OutputDriver2ResEnable	DISABLED

//Pin 2.3
#define		IGNITION_PIN			BIT3
#define		IGNITION_DIR			P2DIR
#define		IGNITION_OUT			P2OUT
#define		IGNITION_REN			P2REN
#define		IGNITION_IN				P2IN
#define		IGNITIONResEnable		DISABLED

//Pin 2.4
#define		LowBatAlarm_PIN			BIT4
#define		LowBatAlarm_DIR			P2DIR
#define		LowBatAlarm_REN			P2REN
#define		LowBatAlarm_OUT			P2OUT
#define		LowBatAlarmResEnable	DISABLED

//Pin 2.5
#define		TIMER1a_PIN				BIT5
#define		TIMER1a_DIR				P2DIR
#define		TIMER1a_OUT				P2OUT
#define		TIMER1a_REN				P2REN
#define		TIMER1a_IN				P2IN
#define		TIMER1aResEnable		ENABLED

//Pin 2.6
#define		TIMER1b_PIN				BIT6
#define		TIMER1b_DIR				P2DIR
#define		TIMER1b_OUT				P2OUT
#define		TIMER1b_REN				P2REN
#define		TIMER1b_IN				P2IN
#define		TIMER1bResEnable		ENABLED

//Pin 2.7
#define		TIMER1c_PIN				BIT7
#define		TIMER1c_DIR				P2DIR
#define		TIMER1c_OUT				P2OUT
#define		TIMER1c_REN				P2REN
#define		TIMER1c_IN				P2IN
#define		TIMER1cResEnable		ENABLED

//PORT 1
//Pin 1.0
#define		BAT_SENSE_PIN			BIT0

//Pin 1.1 UART
#define UART_RX_PIN 				BIT1

//Pin 1.2 UART
#define UART_TX_PIN 				BIT2

//Pin 1.3 (Test Point)
//#define		TIMER2a_PIN				BIT3
//#define		TIMER2a_DIR				P1DIR
//#define		TIMER2a_OUT				P1OUT
//#define		TIMER2a_REN				P1REN
//#define		TIMER2a_IN				P1IN
//#define		TIMER2aResEnable		ENABLED
//Pin 1.4 (Test Point)
//#define		TIMER2b_PIN				BIT4
//#define		TIMER2b_DIR				P1DIR
//#define		TIMER2b_OUT				P1OUT
//#define		TIMER2b_REN				P1REN
//#define		TIMER2b_IN				P1IN
//#define		TIMER2bResEnable		ENABLED
//Pin 1.5
#define		OutputDriver1_PIN				0x20
#define		OutputDriver1_DIR				P1DIR
#define		OutputDriver1_REN				P1REN
#define		OutputDriver1_OUT				P1OUT
#define		OutputDriver1ResEnable			DISABLED

//Pin 1.6
#define		VoltageMonitorEnable_PIN		0x40
#define		VoltageMonitorEnable_DIR		P1DIR
#define		VoltageMonitorEnable_OUT		P1OUT
#define		VoltageMonitorEnable_REN		P1REN
#define		VoltageMonitorEnable_IN			P1IN
#define		VoltageMonitorEnableResEnable	ENABLED
//Pin 1.7
#define		IgnitionSenseEnable_PIN			BIT7
#define		IgnitionSenseEnable_DIR			P1DIR
#define		IgnitionSenseEnable_OUT			P1OUT
#define		IgnitionSenseEnable_REN			P1REN
#define		IgnitionSenseEnable_IN			P1IN
#define		IgnitionSenseEnableResEnable	ENABLED

#elif(HW_REV == 0x2C)   // 2 CH Timer
#define selfProgram 1;
//Port 3
//Pin 3.0
//Pin 3.0
#define     TIMER2a_PIN             BIT0
#define     TIMER2a_DIR             P3DIR
#define     TIMER2a_OUT             P3OUT
#define     TIMER2a_REN             P3REN
#define     TIMER2a_IN              P3IN
#define     TIMER2aResEnable        TIMER2a_PIN

//Pin 3.1
#define     TIMER2b_PIN             BIT1
#define     TIMER2b_DIR             P3DIR
#define     TIMER2b_OUT             P3OUT
#define     TIMER2b_REN             P3REN
#define     TIMER2b_IN              P3IN
#define     TIMER2bResEnable        TIMER2b_PIN

//Pin 3.2
#define     TIMER2c_PIN             BIT2
#define     TIMER2c_DIR             P3DIR
#define     TIMER2c_OUT             P3OUT
#define     TIMER2c_REN             P3REN
#define     TIMER2c_IN              P3IN
#define     TIMER2cResEnable        TIMER2c_PIN

//Pin 3.3 <-> TP9
//Not used

//Pin 3.4
//Not used

//Pin 3.5
//Not used

//Pin 3.6
//Not used

//Pin 3.7
//Not used

//Port 2
//Pin 2.0
#define     RED_LED_PIN             BIT0
#define     RED_LED_DIR             P2DIR
#define     RED_LED_OUT             P2OUT
#define     RED_LED_REN             P2REN
#define     RED_LED_IN              P2IN
#define     RED_LEDResEnable        0

//Pin 2.1
#define     GRN_LED_PIN             BIT1
#define     GRN_LED_DIR             P2DIR
#define     GRN_LED_OUT             P2OUT
#define     GRN_LED_REN             P2REN
#define     GRN_LED_IN              P2IN
#define     GRN_LEDResEnable        0

//Pin 2.2 <-> TP8
//Not used

//Pin 2.3
#define     IGNITION_PIN            BIT3
#define     IGNITION_DIR            P2DIR
#define     IGNITION_OUT            P2OUT
#define     IGNITION_REN            P2REN
#define     IGNITION_IN             P2IN
#define     IGNITIONResEnable       0

//Pin 2.4
#define     LowBatAlarm_PIN         BIT4
#define     LowBatAlarm_DIR         P2DIR
#define     LowBatAlarm_REN         P2REN
#define     LowBatAlarm_OUT         P2OUT
#define     LowBatAlarmResEnable    0

//Pin 2.5
#define     TIMER1a_PIN             BIT5
#define     TIMER1a_DIR             P2DIR
#define     TIMER1a_OUT             P2OUT
#define     TIMER1a_REN             P2REN
#define     TIMER1a_IN              P2IN
#define     TIMER1aResEnable        TIMER1a_PIN

//Pin 2.6
#define     TIMER1b_PIN             BIT6
#define     TIMER1b_DIR             P2DIR
#define     TIMER1b_OUT             P2OUT
#define     TIMER1b_REN             P2REN
#define     TIMER1b_IN              P2IN
#define     TIMER1bResEnable        TIMER1b_PIN

//Pin 2.7
#define     TIMER1c_PIN             BIT7
#define     TIMER1c_DIR             P2DIR
#define     TIMER1c_OUT             P2OUT
#define     TIMER1c_REN             P2REN
#define     TIMER1c_IN              P2IN
#define     TIMER1cResEnable        TIMER1c_PIN

//PORT 1
//Pin 1.0
#define     BAT_SENSE_PIN           BIT0

//Pin 1.1 UART
#define UART_RX_PIN                 BIT1

//Pin 1.2 UART
#define UART_TX_PIN                 BIT2

//Pin 1.3 <-> TP7
//Not used

//Pin 1.4
#define     OutputDriver2_PIN               BIT4
#define     OutputDriver2_DIR               P1DIR
#define     OutputDriver2_REN               P1REN
#define     OutputDriver2_OUT               P1OUT
#define     OutputDriver2ResEnable          0

//Pin 1.5
#define     OutputDriver1_PIN               BIT5
#define     OutputDriver1_DIR               P1DIR
#define     OutputDriver1_REN               P1REN
#define     OutputDriver1_OUT               P1OUT
#define     OutputDriver1ResEnable          0

//Pin 1.6
#define     VoltageMonitorEnable_PIN        BIT6
#define     VoltageMonitorEnable_DIR        P1DIR
#define     VoltageMonitorEnable_OUT        P1OUT
#define     VoltageMonitorEnable_REN        P1REN
#define     VoltageMonitorEnable_IN         P1IN
#define     VoltageMonitorEnableResEnable   VoltageMonitorEnable_PIN

//Pin 1.7
#define     IgnitionSenseEnable_PIN         BIT7
#define     IgnitionSenseEnable_DIR         P1DIR
#define     IgnitionSenseEnable_OUT         P1OUT
#define     IgnitionSenseEnable_REN         P1REN
#define     IgnitionSenseEnable_IN          P1IN
#define     IgnitionSenseEnableResEnable    IgnitionSenseEnable_PIN

#elif(HW_REV == 0xFC)
//PORT 1
//Pin 1.0
#define		BAT_SENSE_PIN			BIT0

//Pin 1.1 UART
#define UART_RX_PIN 				BIT1

//Pin 1.2 UART
#define UART_TX_PIN 				BIT2


//Pin 2.0
#define		TIMER1a_PIN				BIT0
#define		TIMER1a_DIR				P2DIR
#define		TIMER1a_OUT				P2OUT
#define		TIMER1a_REN				P2REN
#define		TIMER1a_IN				P2IN
#define		TIMER1aResEnable		ENABLED
//Pin 2.1
#define		TIMER1b_PIN				BIT1
#define		TIMER1b_DIR				P2DIR
#define		TIMER1b_OUT				P2OUT
#define		TIMER1b_REN				P2REN
#define		TIMER1b_IN				P2IN
#define		TIMER1bResEnable		ENABLED
//Pin 2.2
#define		TIMER1c_PIN				BIT2
#define		TIMER1c_DIR				P2DIR
#define		TIMER1c_OUT				P2OUT
#define		TIMER1c_REN				P2REN
#define		TIMER1c_IN				P2IN
#define		TIMER1cResEnable		ENABLED
//Pin 2.3
#define		IgnitionSenseEnable_PIN			BIT3
#define		IgnitionSenseEnable_DIR			P2DIR
#define		IgnitionSenseEnable_OUT			P2OUT
#define		IgnitionSenseEnable_REN			P2REN
#define		IgnitionSenseEnable_IN			P2IN
#define		IgnitionSenseEnableResEnable	ENABLED
//Pin 2.4
#define		SPI_CS_PIN						BIT4
#define		SPI_CS_DIR						P2DIR
#define		SPI_CS_OUT						P2OUT
#define		SPI_CS_REN						P2REN
#define		SPI_CS_IN						P2IN
#define		SPI_CS_ResEnable				ENABLED
//Pin 2.5
#define		VoltageMonitorEnable_PIN		BIT5
#define		VoltageMonitorEnable_DIR		P2DIR
#define		VoltageMonitorEnable_OUT		P2OUT
#define		VoltageMonitorEnable_REN		P2REN
#define		VoltageMonitorEnable_IN			P2IN
#define		VoltageMonitorEnableResEnable	ENABLED
//Pin 2.6
#define		IGNITION_PIN			BIT6
#define		IGNITION_DIR			P2DIR
#define		IGNITION_OUT			P2OUT
#define		IGNITION_REN			P2REN
#define		IGNITION_IN				P2IN
#define		IGNITIONResEnable		DISABLED







#else
#error "INVALID HW REVISION DETECTED!"
#endif

#if (HW_REV == 0xFC)
#define IODIRA      0x00
#define IODIRB      0x01
#define IPOLA       0x02
#define IPOLB       0x03
#define GPINTENA    0x04
#define GPINTENB    0x05
#define DEFVALA     0x06
#define DEFVALB     0x07
#define INTCONA     0x08
#define INTCONB     0x09
#define IOCONA      0x0A
#define IOCONB      0x0B
#define GPPUA       0x0C
#define GPPUB       0x0D
#define INTFA       0x0E
#define INTFB       0x0F
#define INTCAPA     0x10
#define INTCAPB     0x11
#define GPIOA       0x12
#define GPIOB       0x13
#define OLATA       0x14
#define OLATB       0x15
#define MCP23S17write    0x40
#define MCP23S17read     0x41
#endif

