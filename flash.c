#include "Globals_Define.h"
#include "flash.h"

int *Flash_ptrA;
int *Flash_ptrB;
int *Flash_ptrC;
int *Flash_ptrD;

//#define FLASH_A		0x10C0
//#define FLASH_B		0x1080
//#define FLASH_C		0x1040
//#define FLASH_D		0x1000

/*  Function Definitions  */
unsigned char EraseFlashMemory (char Segment);
void writeSN(int LOTCODE_WW, int LOTCODE_YY,int SN_1, int SN_2);
void writeTest(int TEST_WW, int TEST_YY,int TEST_RESULT);
void writeCal(int VOLTS, int R6,int R7);

void writeCal(int VOLTS, int R6,int R7)
{
	int *Flash_ptrD;
	// Setup Clock @ 1MHz
	BCSCTL1 = CALBC1_1MHZ;                   			// Set DCO to 1MHz
	DCOCTL = CALDCO_1MHZ;

	FCTL3 = FWKEY;                  		// clear lock bit
	FCTL1 = FWKEY + WRT;            		// change to write mode
	Flash_ptrD = (int *) VOLTAGE_PTR;     	// VCC Supply Rail (mv)
	*Flash_ptrD = VOLTS;
	Flash_ptrD = (int *) RESISTOR_R6;     	// R6
	*Flash_ptrD = R6;

	Flash_ptrD = (int *) RESISTOR_R7;     	// R7
	*Flash_ptrD = R7;

	FCTL1 = FWKEY;                  // clear write bit
	FCTL3 = FWKEY + LOCK;           // set lock bit

	// Setup Clock @ 16MHz
	BCSCTL1 = CALBC1_16MHZ;                   			// Set DCO to 16MHz
	DCOCTL = CALDCO_16MHZ;
}
void writeThresholds(unsigned int LowWarn, unsigned int LowCritical,unsigned int HighCritical)
{
	int *Flash_ptrD;
	// Setup Clock @ 1MHz
	BCSCTL1 = CALBC1_1MHZ;                   			// Set DCO to 1MHz
	DCOCTL = CALDCO_1MHZ;

	FCTL3 = FWKEY;                  					// clear lock bit
	FCTL1 = FWKEY + WRT;            					// change to write mode
	Flash_ptrD = (int *) LowBatCritical_PTR;     		// low Voltage critical
	*Flash_ptrD = LowCritical;
	Flash_ptrD = (int *) LowBatWarn_PTR;     			// low Voltage warning
	*Flash_ptrD = LowWarn;

	Flash_ptrD = (int *) HighBatCritical_PTR;     		// High Voltage Critical
	*Flash_ptrD= HighCritical;

	FCTL1 = FWKEY;                  // clear write bit
	FCTL3 = FWKEY + LOCK;           // set lock bit

	// Setup Clock @ 16MHz
	BCSCTL1 = CALBC1_16MHZ;                   			// Set DCO to 16MHz
	DCOCTL = CALDCO_16MHZ;
}
void writeDelay(int Heartbeat, int OVER,int UNDER,int Running,int TurnOff, int HeartbeatDuration)
{
	int *Flash_ptrD;
	// Setup Clock @ 1MHz
	BCSCTL1 = CALBC1_1MHZ;                   			// Set DCO to 1MHz
	DCOCTL = CALDCO_1MHZ;

	FCTL3 = FWKEY;                  					// clear lock bit
	FCTL1 = FWKEY + WRT;            					// change to write mode
	Flash_ptrD = (int *) HeartBeatDelay_PTR;     		// Heart beat delay (seconds)
	*Flash_ptrD = Heartbeat;

	Flash_ptrD = (int *) OverVoltageDelay_PTR;     		// Over voltage turn off delay
	*Flash_ptrD = OVER;

	Flash_ptrD = (int *) UnderVoltageDelay_PTR;     	// Under voltage turn off delay
	*Flash_ptrD = UNDER;

	Flash_ptrD = (int *) IgnitionlessRunningVolt_PTR;     	// Under voltage turn off delay
	*Flash_ptrD = Running;

	Flash_ptrD = (int *) IgnitionlessTurnOffVolt_PTR;     	// Under voltage turn off delay
	*Flash_ptrD = TurnOff;

	Flash_ptrD = (int *) HeartbeatDuration_PTR;     	// heartBeat delay
	*Flash_ptrD = HeartbeatDuration;

	FCTL1 = FWKEY;                  // clear write bit
	FCTL3 = FWKEY + LOCK;           // set lock bit

	// Setup Clock @ 16MHz
	BCSCTL1 = CALBC1_16MHZ;                   			// Set DCO to 16MHz
	DCOCTL = CALDCO_16MHZ;
}
void writeTimer1(int val_0, int val_1,int val_2,int val_3,int val_4,int val_5,int val_6,int val_7)
{
	int *Flash_ptrD;
	// Setup Clock @ 1MHz
	BCSCTL1 = CALBC1_1MHZ;                   			// Set DCO to 1MHz
	DCOCTL = CALDCO_1MHZ;

	FCTL3 = FWKEY;                  		// clear lock bit
	FCTL1 = FWKEY + WRT;            		// change to write mode
	Flash_ptrD = (int *) Timer1_0_PTR;
	*Flash_ptrD = val_0;
	Flash_ptrD = (int *) Timer1_1_PTR;
	*Flash_ptrD = val_1;

	Flash_ptrD = (int *) Timer1_2_PTR;
	*Flash_ptrD = val_2;
	Flash_ptrD = (int *) Timer1_3_PTR;
	*Flash_ptrD = val_3;

	Flash_ptrD = (int *) Timer1_4_PTR;
	*Flash_ptrD = val_4;
	Flash_ptrD = (int *) Timer1_5_PTR;
	*Flash_ptrD = val_5;

	Flash_ptrD = (int *) Timer1_6_PTR;
	*Flash_ptrD = val_6;
	Flash_ptrD = (int *) Timer1_7_PTR;
	*Flash_ptrD = val_7;

	FCTL1 = FWKEY;                  // clear write bit
	FCTL3 = FWKEY + LOCK;           // set lock bit

	// Setup Clock @ 16MHz
	BCSCTL1 = CALBC1_16MHZ;                   			// Set DCO to 16MHz
	DCOCTL = CALDCO_16MHZ;
}
void writeTimer2(int val_0, int val_1,int val_2,int val_3,int val_4,int val_5,int val_6,int val_7)
{
	int *Flash_ptrC;
	// Setup Clock @ 1MHz
	BCSCTL1 = CALBC1_1MHZ;                   			// Set DCO to 1MHz
	DCOCTL = CALDCO_1MHZ;

	FCTL3 = FWKEY;                  		// clear lock bit
	FCTL1 = FWKEY + WRT;            		// change to write mode
	Flash_ptrC = (int *) Timer2_0_PTR;
	*Flash_ptrC = val_0;
	Flash_ptrC = (int *) Timer2_1_PTR;
	*Flash_ptrC = val_1;

	Flash_ptrC = (int *) Timer2_2_PTR;
	*Flash_ptrC = val_2;
	Flash_ptrC = (int *) Timer2_3_PTR;
	*Flash_ptrC = val_3;

	Flash_ptrC = (int *) Timer2_4_PTR;
	*Flash_ptrC = val_4;
	Flash_ptrC = (int *) Timer2_5_PTR;
	*Flash_ptrC = val_5;

	Flash_ptrC = (int *) Timer2_6_PTR;
	*Flash_ptrC = val_6;
	Flash_ptrC = (int *) Timer2_7_PTR;
	*Flash_ptrC = val_7;

	FCTL1 = FWKEY;                  // clear write bit
	FCTL3 = FWKEY + LOCK;           // set lock bit

	// Setup Clock @ 16MHz
	BCSCTL1 = CALBC1_16MHZ;                   			// Set DCO to 16MHz
	DCOCTL = CALDCO_16MHZ;
}
void writeTimer3(int val_0, int val_1,int val_2,int val_3)
{
	int *Flash_ptrD;
	// Setup Clock @ 1MHz
	BCSCTL1 = CALBC1_1MHZ;                   			// Set DCO to 1MHz
	DCOCTL = CALDCO_1MHZ;

	FCTL3 = FWKEY;                  		// clear lock bit
	FCTL1 = FWKEY + WRT;            		// change to write mode
	Flash_ptrD = (int *) Timer3_0_PTR;
	*Flash_ptrD = val_0;
	Flash_ptrD = (int *) Timer3_1_PTR;
	*Flash_ptrD = val_1;

	Flash_ptrD = (int *) Timer3_2_PTR;
	*Flash_ptrD = val_2;
	Flash_ptrD = (int *) Timer3_3_PTR;
	*Flash_ptrD = val_3;

	FCTL1 = FWKEY;                  // clear write bit
	FCTL3 = FWKEY + LOCK;           // set lock bit

	// Setup Clock @ 16MHz
	BCSCTL1 = CALBC1_16MHZ;                   			// Set DCO to 16MHz
	DCOCTL = CALDCO_16MHZ;
}
void writeTimer4(int val_0, int val_1,int val_2,int val_3)
{
	int *Flash_ptrC;
	// Setup Clock @ 1MHz
	BCSCTL1 = CALBC1_1MHZ;                   			// Set DCO to 1MHz
	DCOCTL = CALDCO_1MHZ;

	FCTL3 = FWKEY;                  		// clear lock bit
	FCTL1 = FWKEY + WRT;            		// change to write mode
	Flash_ptrC = (int *) Timer4_0_PTR;
	*Flash_ptrC = val_0;
	Flash_ptrC = (int *) Timer4_1_PTR;
	*Flash_ptrC = val_1;

	Flash_ptrC = (int *) Timer4_2_PTR;
	*Flash_ptrC = val_2;
	Flash_ptrC = (int *) Timer4_3_PTR;
	*Flash_ptrC = val_3;

	FCTL1 = FWKEY;                  // clear write bit
	FCTL3 = FWKEY + LOCK;           // set lock bit

	// Setup Clock @ 16MHz
	BCSCTL1 = CALBC1_16MHZ;                   			// Set DCO to 16MHz
	DCOCTL = CALDCO_16MHZ;
}
void writeTimer5(int val_0, int val_1,int val_2,int val_3)
{
	int *Flash_ptrC;
	// Setup Clock @ 1MHz
	BCSCTL1 = CALBC1_1MHZ;                   			// Set DCO to 1MHz
	DCOCTL = CALDCO_1MHZ;

	FCTL3 = FWKEY;                  		// clear lock bit
	FCTL1 = FWKEY + WRT;            		// change to write mode
	Flash_ptrC = (int *) Timer5_0_PTR;
	*Flash_ptrC = val_0;
	Flash_ptrC = (int *) Timer5_1_PTR;
	*Flash_ptrC = val_1;

	Flash_ptrC = (int *) Timer5_2_PTR;
	*Flash_ptrC = val_2;
	Flash_ptrC = (int *) Timer5_3_PTR;
	*Flash_ptrC = val_3;

	FCTL1 = FWKEY;                  // clear write bit
	FCTL3 = FWKEY + LOCK;           // set lock bit

	// Setup Clock @ 16MHz
	BCSCTL1 = CALBC1_16MHZ;                   			// Set DCO to 16MHz
	DCOCTL = CALDCO_16MHZ;
}
void writeSN(int LOTCODE_WW, int LOTCODE_YY,int SN_1, int SN_2)
{

	int *Flash_ptrD;
	// Setup Clock @ 1MHz
	BCSCTL1 = CALBC1_1MHZ;                   			// Set DCO to 1MHz
	DCOCTL = CALDCO_1MHZ;


	FCTL3 = FWKEY;                  		// clear lock bit
	FCTL1 = FWKEY + WRT;            		// change to write mode
	Flash_ptrD = (int *) SERIAL_2_PTR;     	// SN Lower 2 digits
	*Flash_ptrD = SN_2;
	Flash_ptrD = (int *) SERIAL_1_PTR;     	// SN Upper 2 digits
	*Flash_ptrD = SN_1;

	Flash_ptrD = (int *) WEEK_PTR;     		// LOT CODE WW
	*Flash_ptrD = LOTCODE_WW;
	Flash_ptrD = (int *) YEAR_PTR;     		// LOT CODE YY
	*Flash_ptrD = LOTCODE_YY;

	FCTL1 = FWKEY;                  // clear write bit
	FCTL3 = FWKEY + LOCK;           // set lock bit

	// Setup Clock @ 16MHz
	BCSCTL1 = CALBC1_16MHZ;                   			// Set DCO to 16MHz
	DCOCTL = CALDCO_16MHZ;
}
unsigned char EraseFlashMemory (char Segment)
{

	switch (Segment)// top of segment
	{
	case 'A':{Flash_ptrA = (int *)FLASH_A; break;}
	case 'B':{Flash_ptrB = (int *)FLASH_B; break;}
	case 'C':{Flash_ptrC = (int *)FLASH_C; break;}
	case 'D':{Flash_ptrD = (int *)FLASH_D; break;}
	default: {return 1;}
	}

	    FCTL2 = FWKEY + FSSEL_1 + (FN1);    // use MCLK/3
	    FCTL1 = FWKEY + ERASE;          	// set erase bit
	    FCTL3 = FWKEY;                  	// clear lock bit

	    // dummy write to initiate erase
    switch (Segment)
    	{
    	case 'A':{*Flash_ptrA = 0x00; break;}
    	case 'B':{*Flash_ptrB = 0x00; break;}
    	case 'C':{*Flash_ptrC = 0x00; break;}
    	case 'D':{*Flash_ptrD = 0x00; break;}
    	default: {return 1;}
    	}
    FCTL3 = FWKEY + LOCK;           // set lock bit

    return 0;
}
void writeTest(int TEST_WW, int TEST_YY,int TEST_RESULT)
{
	int *Flash_ptrD;
	// Setup Clock @ 1MHz
	BCSCTL1 = CALBC1_1MHZ;                   		// Set DCO to 1MHz
	DCOCTL = CALDCO_1MHZ;

	FCTL3 = FWKEY;                  				// clear lock bit
	FCTL1 = FWKEY + WRT;            				// change to write mode

	Flash_ptrD = (int *) TEST_WW_PTR;     			// TEST WEEK PTR
	*Flash_ptrD = TEST_WW;
	Flash_ptrD = (int *) TEST_YY_PTR;     			// TEST YEAR PTR
	*Flash_ptrD = TEST_YY;
	Flash_ptrD = (int *) TEST_RS_PTR;     			// TEST RESULT PTR
	*Flash_ptrD = TEST_RESULT;

	FCTL1 = FWKEY;                  				// clear write bit
	FCTL3 = FWKEY + LOCK;           				// set lock bit

	// Setup Clock @ 16MHz
	BCSCTL1 = CALBC1_16MHZ;                   		// Set DCO to 16MHz
	DCOCTL = CALDCO_16MHZ;
}
void writePulseDivider (int val_0)
{
		int *Flash_ptrC;
		// Setup Clock @ 1MHz
		BCSCTL1 = CALBC1_1MHZ;                   		// Set DCO to 1MHz
		DCOCTL = CALDCO_1MHZ;

		FCTL3 = FWKEY;                  				// clear lock bit
		FCTL1 = FWKEY + WRT;            				// change to write mode

		Flash_ptrC = (int *) RemoteStartPulseDivider_PTR;
		if (val_0 != 0)									// can't have a divide by zero!
		{
			*Flash_ptrC = val_0;						// divisor
		}

		FCTL1 = FWKEY;                  				// clear write bit
		FCTL3 = FWKEY + LOCK;           				// set lock bit

		// Setup Clock @ 16MHz
		BCSCTL1 = CALBC1_16MHZ;                   		// Set DCO to 16MHz
		DCOCTL = CALDCO_16MHZ;
}
