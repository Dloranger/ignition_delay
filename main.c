#include "Globals_Define.h"
#include "UART.h"
#include "CommandHandler.h"
#include "stdlib.h"
#include "flash.h"
#include "stdio.h"
#include "stdint.h"

const unsigned long HBDelayCorrectionFactor = 20;
const unsigned long TimerDelayCorrectionFactor = 20;
const unsigned long LowBatAlarmCorrectionFactor = 20;
float LowBatAlarmDelay = 0.5;  // Alarm on timing (secs) when LowBatAlarmMode == 1, default is 0.1
const unsigned long LowBatAlarmDelay2 = 60;// Alarm on timing (secs) when LowBatAlarmMode == 1, default is 60
const unsigned char	LowBatAlarmMode = 1;  // 0 is constant on for and LED or etc, 1 is for a remote start pulse.

/******************************* External devices **************************/
void ConfigureSPIBUS(void);
void ConfigureMCP23S17_Addr_0(void);
void ConfigureMCP23S17_Addr_1(void);
void ConfigureMCP23S17 (void);
uint8_t readMCP23S17 (uint8_t Address, uint8_t Register);
void writeMCP23S17 (uint8_t Address, uint8_t Register, uint8_t value);

/******************************* Status SIGNALS ****************************/
unsigned char	LowBatAlarmState =0;
unsigned int	TIMEOUT;						// Seconds
unsigned int	OVER_VOLTAGE_DELAY;				// Seconds
unsigned int	UNDER_VOLTAGE_DELAY;			// Seconds
unsigned int	IgnitionlessTurnOffVolt;		// mVolts
unsigned int	IgnitionlessRunningVolt;		// mVolts
unsigned int	LowBatWarn; 					// mVolts
unsigned int	LowBatCritical;					// mVolts
unsigned int	HighBatCritical;				// mVolts

extern unsigned char CommandString[];
extern unsigned char ECHO;
unsigned char Ignitionless;

// Timer
void Timer1 (void);
void Timer2 (void);
void Timer3 (void);
void Timer4 (void);
void Timer5 (void);
void Configure_HW_TimerA0(void);
void Configure_HW_TimerA1(void);
void ConfigureTimer1(void);
void ConfigureTimer2(void);
void ConfigureTimer3(void);
void ConfigureTimer4(void);
void ConfigureTimer5(void);
void HeartBeat(void);
void CheckBatteryStatus(void);
void RED_LED(void);
void GRN_LED(void);

/******************************* Self Test  *****************************/
unsigned char ConfigureAndSelfTest(void);
unsigned int	StarterDropVolt;				// mVolts
unsigned int	HeartBeatDelay;					// Seconds
unsigned char 	Heartbeat;

/*********************************************** GLOBAL VARIABLES ********************/
unsigned long	LowBatAlarmCounter	= 0;
unsigned int 	TimerCount 			= 0;
unsigned long 	CountDown1 			= 0;
unsigned long 	CountDown2 			= 0;
unsigned long 	CountDown3 			= 0;
unsigned long 	CountDown4 			= 0;
unsigned long 	CountDown5 			= 0;
float		 	BatteryLevel;
unsigned int 	BatteryLevelOld1;

unsigned long 	OverVoltageCount 	= 0;
unsigned long	UnderVoltageCount 	= 0;
float 			BatteryScaling		= 0;

unsigned int	R6;
unsigned int	R7;
//float			Supply=0.1;

unsigned int	RED_BLINK;

const unsigned long	TimerCounter 		= 25000; // ~= 250KHz/N
unsigned long 	OverVoltageDelay;

unsigned char BatteryStatus		= 0;
unsigned char BatteryStatusOld	= 0;
// BIT0 = Ignitionless enabled & running threshold reached
// BIT1 = Ignitionless enabled & Starter drop threshold reached
// BIT2 = Ignition less enabled & charging rate threshold reached
// BIT3	= Battery is below low warning threshold
// BIT4 = Battery is below low critical threshold
// BIT5 = Battery is above high critical threshold

int main(void)
{
    // Kill Watch Dog Timer
    WDTCTL = WDTPW + WDTHOLD;                           // Stop WDT

	//Port3 does not exist in 20 pin TSSOP, but compiler expects it to be handled
	P3DIR = 0x00;
	P3OUT = 0x00;
	P2DIR = 0x00;
	P2OUT = 0x00;
	P1DIR = 0x00;
	P1OUT = 0x00;
	ConfigureAndSelfTest();
	ADC10CTL0 |= ENC + ADC10SC;             	// ADC Sampling and conversion start
	//LowBatAlarmCounter = 50;

	// Wait until either the ignition-less or physical ignition signals are detected
	// to start normal functionality.  Basically prevent false starts when initial
	// power is applied.  Vehicle has to be started to start the timer up
	while (!(BatteryStatus & 0x01) & !((IGNITION_IN & IGNITION_PIN)==IGNITION_PIN))
	{
		ADC10CTL0 |= ENC + ADC10SC;             	// ADC Sampling and conversion start
		CheckBatteryStatus();
		//LowBatAlarmCounter--;
		__bis_SR_register(LPM0_bits + GIE);			// Enter low power mode with interrupts
	}
	BatteryLevel = 12.4;
	// write battery level to OUTput

	for(;;)  // Run forever
	{
		if (LowBatAlarmCounter)
		{
			LowBatAlarmCounter--;
		}
		CheckBatteryStatus();
		RED_LED();
		GRN_LED();
		if ((IGNITION_IN & IGNITION_PIN)  == IGNITION_PIN)
		{
			// Clear voltage counters due to ignition pin
			UnderVoltageCount = 0;
			OverVoltageCount = 0;
			// Configure Timers
#ifdef SPI_CS_OUT
			ConfigureTimer1();
#endif
#ifdef OutputDriver1_OUT
			ConfigureTimer1();
#endif
#ifdef OutputDriver2_OUT
			ConfigureTimer2();
#endif
#ifdef OutputDriver3_OUT
			ConfigureTimer3();
#endif
#ifdef OutputDriver4_OUT
			ConfigureTimer4();
#endif
#ifdef OutputDriver5_OUT
			ConfigureTimer5();
#endif
			// Turn OUTPUTs on immediately
#ifdef OutputDriver1_OUT
			OutputDriver1_OUT |= OutputDriver1_PIN;
#endif
#ifdef OutputDriver2_OUT
			OutputDriver2_OUT |= OutputDriver2_PIN;
#endif
#ifdef OutputDriver3_OUT
			OutputDriver3_OUT |= OutputDriver3_PIN;
#endif
#ifdef OutputDriver4_OUT
			OutputDriver4_OUT |= OutputDriver4_PIN;
#endif
#ifdef OutputDriver5_OUT
			OutputDriver5_OUT |= OutputDriver5_PIN;
#endif
#ifdef SPI_CS_OUT
	    	writeMCP23S17(1, GPIOA, readMCP23S17(0, GPIOA));
	    	writeMCP23S17(1, GPIOB, readMCP23S17(0, GPIOB));
#endif
		}
		else
		{
			switch (BatteryStatus)
			{
			default: // continue to next case
			case 0x00: // battery is normal, no ignition sense signal
			{
			    LowBatAlarmCounter=0;
				// RESET Low Battery Alarm
#ifdef LowBatAlarm_DIR
				if (!LowBatAlarmCounter)
				{
					LowBatAlarm_OUT &= ~LowBatAlarm_PIN;
				}
#endif
				break;
			}
			case 0x01: // continue to next case
			case 0x02: // continue to next case
			case 0x04: // Ignition sense signal(s) detected
			{
			    LowBatAlarmCounter=0;
				UnderVoltageCount = 0;
				// RESET Low Battery Alarm
#ifdef LowBatAlarm_DIR
				if (!LowBatAlarmCounter)
				{
					LowBatAlarm_OUT &= ~LowBatAlarm_PIN;
				}
#endif
				//Check if Ignition Sense is enabled
				if ((IgnitionSenseEnable_IN & IgnitionSenseEnable_PIN) == IgnitionSenseEnable_PIN) // ignition sense enabled
				{
					// Set values into count down timers, Turn OUTputs ON
#ifdef OutputDriver1_OUT
					ConfigureTimer1();
					OutputDriver1_OUT |= OutputDriver1_PIN;
#endif
					// Timer 2 if present
#ifdef OutputDriver2_OUT
					ConfigureTimer2();
					OutputDriver2_OUT |= OutputDriver2_PIN;
#endif
					// Timer 3 if present
#ifdef OutputDriver3_OUT
					ConfigureTimer3();
					OutputDriver3_OUT |= OutputDriver3_PIN;
#endif
					// Timer 4 if present
#ifdef OutputDriver4_OUT
					ConfigureTimer4();
					OutputDriver4_OUT |= OutputDriver4_PIN;
#endif
					// Timer 5 if present
#ifdef OutputDriver5_OUT
					ConfigureTimer5();
					OutputDriver5_OUT |= OutputDriver5_PIN;
#endif
					// Panel Controller
#ifdef SPI_CS_OUT
					ConfigureTimer1();
			    	writeMCP23S17(1, GPIOA, readMCP23S17(0, GPIOA));
			    	writeMCP23S17(1, GPIOB, readMCP23S17(0, GPIOB));
#endif
					//RESET Over Voltage Counter
					OverVoltageCount = 0;
				}
				break;
			}
			case 0x08: // Battery is below low warning threshold, Enable Low Voltage warning
			{
				//RESET Over Voltage Counter
				OverVoltageCount = 0;
				// SET Low Battery Alarm
#ifdef LowBatAlarm_DIR
				if (LowBatAlarmMode==0)
				{
				LowBatAlarm_OUT |= LowBatAlarm_PIN;
				LowBatAlarmCounter = LowBatAlarmDelay * LowBatAlarmCorrectionFactor;
				}
				else
				{
					switch (LowBatAlarmState)
					{
					case 0://Set Timer value
						LowBatAlarm_OUT |= LowBatAlarm_PIN;
						LowBatAlarmCounter = LowBatAlarmDelay * LowBatAlarmCorrectionFactor;
						LowBatAlarmState = 1;
						break;
					case 1: //Count Down Timer
						if (!LowBatAlarmCounter)
						{
							LowBatAlarm_OUT &= ~LowBatAlarm_PIN;
							LowBatAlarmCounter = LowBatAlarmDelay2 * LowBatAlarmCorrectionFactor;
							LowBatAlarmState = 2;

						}
						break;
					case 2: //Enforce lowBatAlarm signal delay
					{
						if (!LowBatAlarmCounter)
						{
							LowBatAlarmState = 0;
						}
					}
					break;
					default: LowBatAlarmState = 0;
					}//Switch
				}//Else
#endif
				// RESET under voltage count (above critical level)
				UnderVoltageCount = 0;
				break;
			}
			case 0x10: // Battery is below low critical threshold
			{
#ifdef LowBatAlarm_DIR
				if (LowBatAlarmMode==0)
				{
				LowBatAlarm_OUT |= LowBatAlarm_PIN;
				LowBatAlarmCounter = LowBatAlarmDelay * LowBatAlarmCorrectionFactor;
				}
				else
				{
					switch (LowBatAlarmState)
					{
					case 0://Set Timer value
						LowBatAlarm_OUT |= LowBatAlarm_PIN;
						LowBatAlarmCounter = LowBatAlarmDelay * LowBatAlarmCorrectionFactor;
						LowBatAlarmState = 1;
						break;
					case 1: //Count Down Timer
						if (!LowBatAlarmCounter)
						{
							LowBatAlarm_OUT &= ~LowBatAlarm_PIN;
							LowBatAlarmCounter = LowBatAlarmDelay2;
							LowBatAlarmState = 2;
							break;
						}
					case 2: //Enforce lowBatAlarm signal delay
					{
						if (!LowBatAlarmCounter)
						{
							LowBatAlarmState = 0;
						}
					}
					default: LowBatAlarmState = 0;
					}//Switch
				}//Else
#endif
				//RESET Over Voltage Counter
				OverVoltageCount = 0;
				// Test for count expiration
				if (UnderVoltageCount++ >= (* ((int *) UnderVoltageDelay_PTR))*TimerDelayCorrectionFactor)
				{
					// Ensure we don't roll over the counter
					UnderVoltageCount = (* ((int *) UnderVoltageDelay_PTR))*TimerDelayCorrectionFactor;
					//Count exceeded, Kill OUTputs
					CountDown1 = 0;
					CountDown2 = 0;
					CountDown3 = 0;
					CountDown4 = 0;
					CountDown5 = 0;
				}
				break;
			}
			case 0x20: // Battery is above high critical threshold
			{
			    LowBatAlarmCounter=0;
				// Test for count expiration
				if (OverVoltageCount++ >= (* ((int *) OverVoltageDelay_PTR))*TimerDelayCorrectionFactor)
				{
					OverVoltageCount = (* ((int *) OverVoltageDelay_PTR))*TimerDelayCorrectionFactor;  // ensure we don't roll over due to ongoing incrementing
					//Count exceeded, Kill OUTputs
					CountDown1 = 0;
					CountDown2 = 0;
					CountDown3 = 0;
					CountDown4 = 0;
					CountDown5 = 0;
					//GRN_LED_OUT &= ~ GRN_LED_PIN;		// Ensure Green LED is OFF
				}
				break;
			}
			}  // Close Switch structure
		}
		// Process Timers
		Timer1();
		__delay_cycles (16000);
		Timer2();
		__delay_cycles (16000);
		Timer3();
		__delay_cycles (15000);
		Timer4();
		__delay_cycles (16000);
		Timer5();
		if (CountDown1|CountDown2|CountDown3|CountDown4|CountDown5)
		{
			Heartbeat = 0;
		}
		else
		{
			Heartbeat++;
		}

		//if ((ECHO))
		//{
			// write battery level to OUTput
		//	unsigned char BatPrint[13];
		//	unsigned int batV = BatteryLevel;
		//	unsigned int batmV = (BatteryLevel -batV)*100;
		//	sprintf(BatPrint,"VOLTS:%02d.%02d\r\n",batV, batmV);
		//	uartSend (BatPrint, sizeof(BatPrint));
		//}
		ADC10CTL0 |= ENC + ADC10SC;             	// ADC Sampling and conversion start
		__bis_SR_register(LPM0_bits + GIE);			// Enter low power mode with interrupts
	}
}

void RED_LED(void)
{
#ifdef RED_LED_OUT
	const unsigned int fastBlink = 0x1FFF;
	const unsigned int slowBlink = 0xAFFF;
	// Define RED LED based on BatteryStatus values
	switch (BatteryStatus)
	{
	case 0x00:	  	// battery is normal
	{
		RED_BLINK = 0;  // Turn off Timer
		TA1CCR0 = RED_BLINK;
		RED_LED_OUT &= ~ RED_LED_PIN;	// Turn off OUTPUT
		break;
	}
	case 0x01:
	case 0x02:
	case 0x04:		// Ignition Sense Detected
	{
		break;
	}
	case 0x08:		// Battery is below low warning threshold, 	Blink RED LED Slow
	{
		RED_BLINK = slowBlink;  			// Turn ON Timer
		TA1CCR0 = RED_BLINK;
		break;
	}
	case 0x10:		// Battery is below low critical threshold, Blink RED LED FAST
	{
		RED_BLINK = fastBlink;  			// Turn ON Timer
		TA1CCR0 = RED_BLINK;
		break;
	}
	case 0x20:		// Battery is above high critical threshold, RED LED ON
	{
		RED_BLINK = 0;  			// Turn OFF Timer
		TA1CCR0 = RED_BLINK;
		RED_LED_OUT |= RED_LED_PIN;
		break;
	}
	default:
	{
		break;
	}

	}// End switch
#endif
}
void GRN_LED(void)
{
#ifdef GRN_LED_OUT
	switch (BatteryStatus)
	{
	case 0:
	{
		if (IGNITION_IN & IGNITION_PIN)
		{
			GRN_LED_OUT |= GRN_LED_PIN;
		}
		else
		{
			// Check for Timing
			if (CountDown1|CountDown2)
			{
				// Blink Green LED to indicate TIMING
				GRN_LED_OUT ^= GRN_LED_PIN;
			}
			else
			{
				HeartBeat();
			}
		}
		break;
	}// Case 0
	case 0x01:
	case 0x02:
	case 0x04:
	{
		GRN_LED_OUT |= GRN_LED_PIN;
		break;
	}// case 1/2/4
	case 0x08:
	{
		break;
	}
	case 0x10:
	{
		if (IGNITION_IN & IGNITION_PIN)
		{
			GRN_LED_OUT |= GRN_LED_PIN;
		}
		else
		{
			GRN_LED_OUT &= ~GRN_LED_PIN;
		}
		break;
	}// case 0x10
	case 0x20:
	{
		if (IGNITION_IN & IGNITION_PIN)
		{
			GRN_LED_OUT |= GRN_LED_PIN;
		}
		else
		{
			GRN_LED_OUT &= ~GRN_LED_PIN;
		}
		break;
	}// case 0x20
	}// Switch
#endif
}
void Configure_HW_TimerA0()
{
	TA0CCTL0 = CCIE;                           									// TA1CCR0 interrupt enabled
	TA0CCR0 = TimerCounter;
	TimerCount = TimerCounter;
	TA0CTL  = TASSEL_2;													// SMCLK
	TA0CTL |= MC_1;														// Count up mode
	TA0CTL |= ID_3;                  									// SMCLK /8 (2MHZ/8)

}
void Configure_HW_TimerA1()
{
	TA1CCTL0 = CCIE;                           									// TA1CCR0 interrupt enabled
	TA1CCR0 = 0x0;						  										// Disabled to start with
	TA1CTL = TASSEL_2;															// SMCLK
	TA1CTL |= MC_1;                  											// Count up mode
	TA1CTL |= ID_3;																// SMCLK /8 (2MHz/8)
}
void HeartBeat(void)
{
#ifdef GRN_LED_OUT
	int i;
	if (Heartbeat >= HeartBeatDelay)
	{
		Heartbeat = 0;

		GRN_LED_OUT |= GRN_LED_PIN;	 			// GRN LED ON
		// leave on requested length
		for (i=(* ((int *) HeartbeatDuration_PTR));i>0;i--)
		{
			__delay_cycles (15888); // value found empirically
		}
		GRN_LED_OUT &= ~GRN_LED_PIN;			// GRN LED OFF
	}
#endif
}

void CheckBatteryStatus(void)
{
	unsigned int volts;
	long int battery;

	battery = ADC10MEM;
	BatteryLevelOld1 = BatteryLevel;
	volts = *((int *) VOLTAGE_PTR);
	battery = battery * volts;
	// account for adc scaling
	BatteryLevel = battery >>10;
	// account for voltage divider
	BatteryLevel /= BatteryScaling;
	// account for mV scaling
	BatteryLevel /= 1000;
	// fix timer autostarting at powerup
	if (BatteryLevelOld1 == 0)
	{
		BatteryLevelOld1 = battery;
	}

	int BatteryLevelmV = BatteryLevel *1000;
	BatteryStatusOld = BatteryStatus;
	BatteryStatus = 0;
	// See if ignition-less option is enabled
	if ((IgnitionSenseEnable_IN & IgnitionSenseEnable_PIN) == IgnitionSenseEnable_PIN)
	{
		// Check for running voltage
		if ((BatteryLevelmV > (* ((int *) IgnitionlessRunningVolt_PTR)))|	// reached turn-on voltage
				((BatteryLevelmV >= (* ((int *) IgnitionlessTurnOffVolt_PTR))) & (BatteryStatusOld & BIT0))) // above turnoff, already running,
		{
			BatteryStatus |= BIT0;
		}
	}
	// Check for Battery Monitor Issues
	if ((VoltageMonitorEnable_IN & VoltageMonitorEnable_PIN) == VoltageMonitorEnable_PIN)  // check for battery monitor enabled
	{
		if ((BatteryLevelmV < * ((int *) LowBatWarn_PTR)) & (BatteryLevelmV > * ((int *) LowBatCritical_PTR)))
		{
			OverVoltageCount = 0;
			UnderVoltageCount = 0;
			BatteryStatus |= BIT3;
		}
		else
		{
			if (BatteryLevelmV <= (* ((int *) LowBatCritical_PTR)))
			{
				OverVoltageCount = 0;
				BatteryStatus |= BIT4;
				//UnderVoltageCount++;
			}
			else
			{
				if (BatteryLevelmV >= (* ((int *) HighBatCritical_PTR)))
				{
					UnderVoltageCount = 0;
					// if ignitionless is active or Ignition is detected
					if ((BatteryStatus & 0x07)|
							((IGNITION_IN & IGNITION_PIN) == IGNITION_PIN))
					{
						OverVoltageCount = 0;
					}
					else
					{
						BatteryStatus |= BIT5;
						/*if (OverVoltageCount > * ((int *) OverVoltageDelay_PTR*TimerDelayCorrectionFactor) )
						{
							OverVoltageCount = * ((int *) OverVoltageDelay_PTR*TimerDelayCorrectionFactor);// prevent rollover
						}
						else
						{
							OverVoltageCount++;
						}*/
					}

				}
			}
		}
	}

}
void ConfigureTimer1(void)
{
	int setting = 9;
#if ((HW_REV==0x1C)|(HW_REV==0x2C))
	setting = ((TIMER1c_IN & TIMER1c_PIN)+(TIMER1b_IN & TIMER1b_PIN)+(TIMER1a_IN & TIMER1a_PIN))>>5;
#elif HW_REV==0x3C
	setting = ((TIMER1b_IN & TIMER1b_PIN)+(TIMER1a_IN & TIMER1a_PIN))>>2;
#elif HW_REV==0x5C
	setting = ((TIMER1b_IN & TIMER1b_PIN)+(TIMER1a_IN & TIMER1a_PIN))>>2;
#elif HW_REV==0xFC
	setting = ((TIMER1c_IN & TIMER1c_PIN)+(TIMER1b_IN & TIMER1b_PIN)+(TIMER1a_IN & TIMER1a_PIN));
#endif
	switch (setting)
	{
	case 0x00:CountDown1 = (*((int *) Timer1_0_PTR)*TimerDelayCorrectionFactor); break;
	case 0x01:CountDown1 = (*((int *) Timer1_1_PTR)*TimerDelayCorrectionFactor); break;
	case 0x02:CountDown1 = (*((int *) Timer1_2_PTR)*TimerDelayCorrectionFactor); break;
	case 0x03:CountDown1 = (*((int *) Timer1_3_PTR)*TimerDelayCorrectionFactor); break;
	case 0x04:CountDown1 = (*((int *) Timer1_4_PTR)*TimerDelayCorrectionFactor); break;
	case 0x05:CountDown1 = (*((int *) Timer1_5_PTR)*TimerDelayCorrectionFactor); break;
	case 0x06:CountDown1 = (*((int *) Timer1_6_PTR)*TimerDelayCorrectionFactor); break;
	case 0x07:CountDown1 = (*((int *) Timer1_7_PTR)*TimerDelayCorrectionFactor); break;
	default: CountDown1 = 0;break;
	}
}
void ConfigureTimer2(void)
{
	int setting = 9;
#if ((HW_REV==0x2C))
    setting = ((TIMER2c_IN & TIMER2c_PIN)+(TIMER2b_IN & TIMER2b_PIN)+(TIMER2a_IN & TIMER2a_PIN));
#elif HW_REV==0x3C
	setting = ((TIMER2b_IN & TIMER2b_PIN)+(TIMER2a_IN & TIMER2a_PIN))>>1;
#elif HW_REV==0x5c
	setting = ((TIMER2b_IN & TIMER2b_PIN)+(TIMER2a_IN & TIMER2a_PIN))>>4;
#endif

	switch (setting)
	{
	case 0x00:CountDown2 = (*((int *) Timer2_0_PTR)*TimerDelayCorrectionFactor); break;
	case 0x01:CountDown2 = (*((int *) Timer2_1_PTR)*TimerDelayCorrectionFactor); break;
	case 0x02:CountDown2 = (*((int *) Timer2_2_PTR)*TimerDelayCorrectionFactor); break;
	case 0x03:CountDown2 = (*((int *) Timer2_3_PTR)*TimerDelayCorrectionFactor); break;
    case 0x04:CountDown2 = (*((int *) Timer2_4_PTR)*TimerDelayCorrectionFactor); break;
    case 0x05:CountDown2 = (*((int *) Timer2_5_PTR)*TimerDelayCorrectionFactor); break;
    case 0x06:CountDown2 = (*((int *) Timer2_6_PTR)*TimerDelayCorrectionFactor); break;
    case 0x07:CountDown2 = (*((int *) Timer2_7_PTR)*TimerDelayCorrectionFactor); break;
	default: CountDown2 = 0;break;
	}
}
void ConfigureTimer3(void)
{
	int setting = 9;
#if HW_REV==0x3C
	setting = ((TIMER3b_IN & TIMER3b_PIN)+(TIMER3a_IN & TIMER3a_PIN));
#elif HW_REV==0x5C
	setting = ((TIMER3b_IN & TIMER3b_PIN)+(TIMER3a_IN & TIMER3a_PIN))>>4;
#endif

	switch (setting)
	{
	case 0x00:CountDown3 = (*((int *) Timer3_0_PTR)*TimerDelayCorrectionFactor); break;
	case 0x01:CountDown3 = (*((int *) Timer3_1_PTR)*TimerDelayCorrectionFactor); break;
	case 0x02:CountDown3 = (*((int *) Timer3_2_PTR)*TimerDelayCorrectionFactor); break;
	case 0x03:CountDown3 = (*((int *) Timer3_3_PTR)*TimerDelayCorrectionFactor); break;
	default: CountDown3 = 0;break;
	}
}
void ConfigureTimer4(void)
{
#if HW_REV==0x5C

	int setting = 9;
	setting = ((TIMER4b_IN & TIMER4b_PIN)+(TIMER4a_IN & TIMER4a_PIN))>>4;

	switch (setting)
	{
	case 0x00:CountDown4 = (*((int *) Timer4_0_PTR)*TimerDelayCorrectionFactor); break;
	case 0x01:CountDown4 = (*((int *) Timer4_1_PTR)*TimerDelayCorrectionFactor); break;
	case 0x02:CountDown4 = (*((int *) Timer4_2_PTR)*TimerDelayCorrectionFactor); break;
	case 0x03:CountDown4 = (*((int *) Timer4_3_PTR)*TimerDelayCorrectionFactor); break;
	default: CountDown4 = 0;break;
	}
#endif
}
void ConfigureTimer5(void)
{
#if HW_REV==0x5C
	int setting = 9;
	setting = ((TIMER5b_IN & TIMER5b_PIN)+(TIMER5a_IN & TIMER5a_PIN))>>6;
switch (setting)
	{
	case 0x00:CountDown5 = (*((int *) Timer5_0_PTR)*TimerDelayCorrectionFactor); break;
	case 0x01:CountDown5 = (*((int *) Timer5_1_PTR)*TimerDelayCorrectionFactor); break;
	case 0x02:CountDown5 = (*((int *) Timer5_2_PTR)*TimerDelayCorrectionFactor); break;
	case 0x03:CountDown5 = (*((int *) Timer5_3_PTR)*TimerDelayCorrectionFactor); break;
	default: CountDown5 = 0;break;
	}
#endif
}
void Timer1(void)
{
	// Handle Timing
	if (CountDown1 == 0)
	{
#ifdef OutputDriver1_OUT
		OutputDriver1_OUT &= ~OutputDriver1_PIN;													// Turn OFF OUTPUT
#endif
#ifdef SPI_CS_OUT
    	writeMCP23S17(1, GPIOA, 0x00);										// Turn OFF OUTPUT
    	writeMCP23S17(1, GPIOB, 0x00);										// Turn OFF OUTPUT
#endif
	}
	else
	{
#ifdef OutputDriver1_OUT
		//P1DIR |= BIT5;
		OutputDriver1_OUT |= OutputDriver1_PIN;												// Turn ON OUTPUT
#endif
#ifdef SPI_CS_OUT
    	writeMCP23S17(1, GPIOA, readMCP23S17(0, GPIOA));					// Turn ON OUTPUT
    	writeMCP23S17(1, GPIOB, readMCP23S17(0, GPIOB));					// Turn ON OUTPUT
#endif
		CountDown1--;
	}
}
void Timer2(void)
{
#ifdef OutputDriver2_OUT
	// Handle Timing
	if (CountDown2 == 0)
	{
		OutputDriver2_OUT &= ~OutputDriver2_PIN;													// Turn OFF OUTPUT
	}
	else
	{
		CountDown2--;
		OutputDriver2_OUT |= OutputDriver2_PIN;												// Turn ON OUTPUT
	}
#endif
}
void Timer3(void)
{
#ifdef OutputDriver3_OUT
	// Handle Timing
	if (CountDown3 == 0)
	{
		OutputDriver3_OUT &= ~OutputDriver3_PIN;													// Turn OFF OUTPUT
	}
	else
	{
		CountDown3--;
		OutputDriver3_OUT |= OutputDriver3_PIN;												// Turn ON OUTPUT
	}
#endif
}
void Timer4(void)
{
#ifdef OutputDriver4_OUT
	// Handle Timing
	if (CountDown4 == 0)
	{
		OutputDriver4_OUT &= ~OutputDriver4_PIN;													// Turn OFF OUTPUT
	}
	else
	{
		CountDown4--;
		OutputDriver4_OUT |= OutputDriver4_PIN;												// Turn ON OUTPUT
	}
#endif
}
void Timer5(void)
{
#ifdef OutputDriver5_OUT
	// Handle Timing
	if (CountDown5 == 0)
	{
		OutputDriver5_OUT &= ~OutputDriver5_PIN;													// Turn OFF OUTPUT
	}
	else
	{
		CountDown5--;
		OutputDriver5_OUT |= OutputDriver5_PIN;												// Turn ON OUTPUT
	}
#endif
}

unsigned char ConfigureAndSelfTest(void)
{


	// Configure Ports
	P1SEL = 0;
	P1SEL |= BAT_SENSE_PIN;	         					// A0 as ADC
	P2SEL = 0;
	P2SEL2 = 0;

	// Setup Clocks @ 16MHz
	BCSCTL1 = CALBC1_16MHZ;                   			// Set DCO to 16MHz
	DCOCTL = CALDCO_16MHZ;

	// Set SM clock to 4MHz
	BCSCTL2 |=  DIVS_2;									// SMCLK = DCO/4 (4 MHZ)

	// Setup Timer
	Configure_HW_TimerA0();
	Configure_HW_TimerA1();

	//Configure UART & Display Splash message on UART

	//UART_Configure(0x04);								// Configure the port @ 16MHZ
	//UART_Splash();										// Send Welcome Message

	//	UART_Configure(0x01);								// Configure the port @ 2MHZ
	//Configure ADC
	ADC10CTL0 |= ADC10SHT_3;// slow Sampling speed
	ADC10CTL0 |= ADC10ON;	// turn ADC on
	ADC10CTL0 |= ADC10IE; 	// interrupt enabled
	//
	ADC10CTL1 |= INCH_0;    // Channel 0                   						// input A0
	ADC10AE0  |= 0x01;       // Channel 0                  						// PA.0 ADC option select
	ADC10CTL0 |= ENC + ADC10SC;             									// Sampling and conversion start

	/********************************** CONFIGURE PORT PINS ******************/
	//Configure Low Battery Alarm
#ifdef LowBatAlarm_DIR
	//Configure Low Bat Alarm
	LowBatAlarm_OUT	&= ~LowBatAlarm_PIN;					//Start with OUTput off
	LowBatAlarm_DIR |= LowBatAlarm_PIN;					//Configure as OUTput

	if (LowBatAlarmResEnable)
	{
		LowBatAlarm_REN |= LowBatAlarm_PIN;
	}
#endif
	//Configure Output Driver 1
#ifdef OutputDriver1_DIR
	OutputDriver1_DIR |= OutputDriver1_PIN;				//Configure as OUTput
	OutputDriver1_OUT &= ~OutputDriver1_PIN;			//Start with OUTput off
	if (OutputDriver1ResEnable)
	{
		OutputDriver1_REN |= OutputDriver1_PIN;
	}
#endif
	//Configure Output Driver 2
#ifdef	OutputDriver2_DIR
	OutputDriver2_DIR|= OutputDriver2_PIN;				//Configure as OUTput
	OutputDriver2_OUT &= ~OutputDriver2_PIN;			//Start with OUTput off
	if (OutputDriver2ResEnable)
	{
		OutputDriver2_REN |= OutputDriver2_PIN;
	}
#endif
	//Configure Output Driver 3
#ifdef	OutputDriver3_DIR
	OutputDriver3_DIR|= OutputDriver3_PIN;				//Configure as OUTput
	OutputDriver3_OUT &= ~OutputDriver3_PIN;			//Start with OUTput off
	if (OutputDriver3ResEnable)
	{
		OutputDriver3_REN |= OutputDriver3_PIN;
	}
#endif
	//Configure Output Driver 4
#ifdef	OutputDriver4_DIR
	OutputDriver4_DIR|= OutputDriver4_PIN;				//Configure as OUTput
	OutputDriver4_OUT &= ~OutputDriver4_PIN;			//Start with OUTput off
	if (OutputDriver4ResEnable)
	{
		OutputDriver4_REN |= OutputDriver4_PIN;
	}
#endif
	//Configure Output Driver 5
#ifdef	OutputDriver5_DIR
	OutputDriver5_DIR|= OutputDriver5_PIN;				//Configure as OUTput
	OutputDriver5_OUT &= ~OutputDriver5_PIN;			//Start with OUTput off
	if (OutputDriver5ResEnable)
	{
		OutputDriver5_REN |= OutputDriver5_PIN;
	}
#endif
	//Configure Voltage Monitor Switch
#ifdef VoltageMonitorEnable_DIR
	VoltageMonitorEnable_DIR&= ~VoltageMonitorEnable_PIN;		//Configure as input
	VoltageMonitorEnable_OUT &= ~VoltageMonitorEnable_PIN;		//Pull Down Resistor as pin is input
	if (VoltageMonitorEnableResEnable)
	{
		VoltageMonitorEnable_REN |= VoltageMonitorEnable_PIN;	//Enable Pull Resistor
	}
#endif
	//Configure timer 1a
#ifdef TIMER1a_DIR
	TIMER1a_DIR &= ~TIMER1a_PIN;						//Configure as input
#if HW_REV ==0xFC
	TIMER1a_OUT |= TIMER1a_PIN;							//Pull up Resistor as pin is input
#else
	TIMER1a_OUT &= ~TIMER1a_PIN;						//Pull Down Resistor as pin is input
#endif
	if (TIMER1aResEnable)
	{
		TIMER1a_REN |= TIMER1a_PIN;						//Enable Pull Resistor
	}
#endif
	//Configure timer 1b
#ifdef TIMER1b_DIR
	TIMER1b_DIR &= ~TIMER1b_PIN;						//Configure as input
#if HW_REV ==0xFC
	TIMER1b_OUT |= TIMER1b_PIN;							//Pull up Resistor as pin is input
#else
	TIMER1b_OUT &= ~TIMER1b_PIN;						//Pull Down Resistor as pin is input
#endif
	if (TIMER1bResEnable)
	{
		TIMER1b_REN |= TIMER1b_PIN;						//Enable Pull Resistor
	}
#endif
	//Configure timer 1c
#ifdef TIMER1c_DIR
	TIMER1c_DIR &= ~TIMER1c_PIN;						//Configure as OUTput
#if HW_REV ==0xFC
	TIMER1c_OUT |= TIMER1c_PIN;							//Pull up Resistor as pin is input
#else
	TIMER1c_OUT &= ~TIMER1c_PIN;						//Pull Down Resistor as pin is input
#endif
	if (TIMER1cResEnable)
	{
		TIMER1c_REN |= TIMER1c_PIN;						//Enable Pull Resistor
	}
#endif
	//Configure timer 2a
#ifdef TIMER2a_DIR
	TIMER2a_DIR &= ~TIMER2a_PIN;						//Configure as input
	TIMER2a_OUT &= ~TIMER2a_PIN;						//Pull Down Resistor as pin is input
	if (TIMER2aResEnable)
	{
		TIMER2a_REN |= TIMER2a_PIN;						//Enable Pull Resistor
	}
#endif
	//Configure timer 2b
#ifdef TIMER2b_DIR
	TIMER2b_DIR &= ~TIMER2b_PIN;						//Configure as input
	TIMER2b_OUT &= ~TIMER2b_PIN;						//Pull Down Resistor as pin is input
	if (TIMER2bResEnable)
	{
		TIMER2b_REN |= TIMER2b_PIN;						//Enable Pull Resistor
	}
#endif
	//Configure timer 3a
#ifdef TIMER3a_DIR
	TIMER3a_DIR &= ~TIMER3a_PIN;						//Configure as input
	TIMER3a_OUT &= ~TIMER3a_PIN;						//Pull Down Resistor as pin is input
	if (TIMER3aResEnable)
	{
		TIMER3a_REN |= TIMER3a_PIN;						//Enable Pull Resistor
	}
#endif
	//Configure timer 3b
#ifdef TIMER3b_DIR
	TIMER3b_DIR &= ~TIMER3b_PIN;						//Configure as input
	TIMER3b_OUT &= ~TIMER3b_PIN;						//Pull Down Resistor as pin is input
	if (TIMER3bResEnable)
	{
		TIMER3b_REN |= TIMER3b_PIN;						//Enable Pull Resistor
	}
#endif
	//Configure timer 4a
#ifdef TIMER4a_DIR
	TIMER4a_DIR &= ~TIMER4a_PIN;						//Configure as input
	TIMER4a_OUT &= ~TIMER4a_PIN;						//Pull Down Resistor as pin is input
	if (TIMER4aResEnable)
	{
		TIMER4a_REN |= TIMER4a_PIN;						//Enable Pull Resistor
	}
#endif
	//Configure timer 4b
#ifdef TIMER4b_DIR
	TIMER4b_DIR &= ~TIMER4b_PIN;						//Configure as input
	TIMER4b_OUT &= ~TIMER4b_PIN;						//Pull Down Resistor as pin is input
	if (TIMER4bResEnable)
	{
		TIMER4b_REN |= TIMER4b_PIN;						//Enable Pull Resistor
	}
#endif
	//Configure timer 5a
#ifdef TIMER5a_DIR
	TIMER5a_DIR &= ~TIMER5a_PIN;						//Configure as input
	TIMER5a_OUT &= ~TIMER5a_PIN;						//Pull Down Resistor as pin is input
	if (TIMER5aResEnable)
	{
		TIMER5a_REN |= TIMER5a_PIN;						//Enable Pull Resistor
	}
#endif
	//Configure timer 5b
#ifdef TIMER5b_DIR
	TIMER5b_DIR &= ~TIMER5b_PIN;						//Configure as input
	TIMER5b_OUT &= ~TIMER5b_PIN;						//Pull Down Resistor as pin is input
	if (TIMER5bResEnable)
	{
		TIMER5b_REN |= TIMER5b_PIN;						//Enable Pull Resistor
	}
#endif
	//Configure SPI BUS
#ifdef SPI_CS_DIR
	ConfigureSPIBUS();
	ConfigureMCP23S17();
	writeCal((0x0CBC),	//voltage (mv)
					(0x2710),	//R6
					(0x03e8));	//R7
#endif
	//Configure RED LED
#ifdef RED_LED_DIR
	RED_LED_DIR |= RED_LED_PIN;							//Configure as OUTput
	//RED_LED_OUT &= ~RED_LED_PIN;						//Start with OUTput off
	if (RED_LEDResEnable)
	{
		RED_LED_REN |= RED_LED_PIN;						//Enable Pull Resistor
	}
#endif
	//Configure GREEN LED
#ifdef GRN_LED_DIR
	GRN_LED_DIR |= GRN_LED_PIN;							//Configure as OUTput
	GRN_LED_OUT &= ~GRN_LED_PIN;						//Start with OUTput off
	if (GRN_LEDResEnable)
	{
		GRN_LED_REN |= GRN_LED_PIN;						//Enable Pull Resistor
	}
#endif
	//Configure IGNITION INPUT
#ifdef IGNITION_DIR
	IGNITION_DIR &= ~IGNITION_PIN;						//Configure as input
	IGNITION_OUT &= ~IGNITION_PIN;						//Pull Down Resistor as pin is input
	if (IGNITIONResEnable)
	{
		IGNITION_REN |= IGNITION_PIN;					//Enable Pull Resistor
	}
#endif
	//Configure Ignition Sense Switch
#ifdef IgnitionSenseEnable_DIR
	IgnitionSenseEnable_DIR &= ~IgnitionSenseEnable_PIN;			//Configure as input
	if (HW_REV != 0xFC)
	{
	IgnitionSenseEnable_OUT &= ~IgnitionSenseEnable_PIN;		//Pull Down Resistor as pin is input
	}
	else
	{
		IgnitionSenseEnable_OUT |= IgnitionSenseEnable_PIN;		//Pull up Resistor as pin is input
	}
	if (IgnitionSenseEnableResEnable)
	{
		IgnitionSenseEnable_REN |= IgnitionSenseEnable_PIN; 	//Enable Pull Resistor
	}
#endif

#ifdef selfProgram


    if (0xFFFF == *((int *) RESISTOR_R7))
            {
                //Program Voltage divider values
                //Setup the battery sense circuit
                writeCal(3300, 10000,1500);

                //Setup delays
                writeDelay(
                        (2),      //Heartbeat (sec)
                        (3),  //OverVoltage (sec)
                        (3),  //Undervoltage (sec)
                        (13500),  // Sense turn on Voltage (mV)
                        (13000),  // Sense turn Off Voltage (mv)
                        (200));   // Heart Beat duration (sec)

                //Setup thresholds
                writeThresholds(
                        (11450), //Low Warning
                        (11000),    //Low Critical
                        (18000));  //High Critical

                // Setup Timers
                writeTimer1(
                        (1),//Timer1_0
                        (10),//Timer1_1
                        (300),//Timer1_2
                        (900),//Timer1_3
                        (1800),//Timer1_4
                        (3600),//Timer1_5
                        (7200),//Timer1_6
                        (14400));//Timer1_7

                writeTimer2(
                        (1),//TimerN_0
                        (10),//TimerN_1
                        (300),//TimerN_2
                        (900),//TimerN_3
                        (1800),//TimerN_4
                        (3600),//TimerN_5
                        (7200),//TimerN_6
                        (14400));//TimerN_7

                //Remote start delays
                //LowBatAlarmDelay = 1/1;
            }
	// Calculate Voltage Divider
	R6 		= *((int *) RESISTOR_R6);
	R7 		= *((int *) RESISTOR_R7);
	BatteryScaling = R7;
	BatteryScaling /= (R6 + R7);

	// Configure Heart Beat Delay
	HeartBeatDelay = (*((int *) HeartBeatDelay_PTR))*HBDelayCorrectionFactor;

	//Configure Voltage thresholds
	LowBatWarn = *((int *) LowBatWarn_PTR);
	LowBatCritical = *((int *) LowBatCritical_PTR);
	HighBatCritical = *((int *) HighBatCritical_PTR);
	IgnitionlessTurnOffVolt = *((int *)IgnitionlessTurnOffVolt_PTR);
	IgnitionlessRunningVolt = *((int *)IgnitionlessRunningVolt_PTR);

	//update remote start delay values
	if (*((int *) RemoteStartPulseDivider_PTR) == 0xFFFF)  // value is unprogrammed
		// Do nothing, leave scaler alone at 1 second (default)
	{;}
	else
	{
		LowBatAlarmDelay = 1/(*((int *) RemoteStartPulseDivider_PTR));
	}
#endif
	return 0;
}


uint8_t readMCP23S17 (uint8_t Address, uint8_t Register)
{
#ifdef SPI_CS_PIN
	uint8_t value;
	SPI_CS_OUT &= ~SPI_CS_PIN;                         // clear !CS to start
	while (!(IFG2 & UCB0TXIFG));            // USCI_B0 TX buffer ready?
	UCB0TXBUF = MCP23S17read+(Address<<1);  // Read command
	while (!(IFG2 & UCB0TXIFG));            // USCI_B0 TX buffer ready?
	UCB0TXBUF = Register;                   // register address for GPIO bank A
	while (!(IFG2 & UCB0TXIFG));            // USCI_B0 TX buffer ready?
	UCB0TXBUF = 0x00;                       // Dummy write
	while (!(IFG2 & UCB0TXIFG));            // USCI_B0 TX buffer ready?
	__delay_cycles(100);                    // Short delay
	value = UCB0RXBUF;
	SPI_CS_OUT |= SPI_CS_PIN;                          // set !CS
	return value;
#endif
	return 0;
}
void writeMCP23S17 (uint8_t Address, uint8_t Register, uint8_t value)
{
#ifdef SPI_CS_PIN

	SPI_CS_OUT &= ~SPI_CS_PIN;                         // clear !CS to start
	while (!(IFG2 & UCB0TXIFG));            // USCI_B0 TX buffer ready?
	UCB0TXBUF = MCP23S17write+(Address<<1);   // Write command
	while (!(IFG2 & UCB0TXIFG));            // USCI_B0 TX buffer ready?
	UCB0TXBUF = Register;                      // register address for GPIO bank B
	while (!(IFG2 & UCB0TXIFG));            // USCI_B0 TX buffer ready?
	UCB0TXBUF = value;                      // Set outputs;
	while (!(IFG2 & UCB0TXIFG));            // USCI_B0 TX buffer ready?
	__delay_cycles(100);                    // Short delay
	SPI_CS_OUT |= SPI_CS_PIN;                          // set !CS
#endif
}
void ConfigureSPIBUS (void)
{
#ifdef SPI_CS_PIN
	// Configure SPI Bus
	    SPI_CS_OUT |= SPI_CS_PIN;                   // set !CS to start
	    SPI_CS_DIR |= SPI_CS_PIN;                   // P2.4 output (!CS)
	    P1SEL |= BIT5 + BIT6 + BIT7;                // Set pins to USCI mode
	    P1SEL2 |= BIT5 + BIT6+BIT7;                 // Set pins to USCI mode

	    // SPI Control Register 0;
	    UCB0CTL0 = 0;                               // ensure clean state to start with
	    UCB0CTL1 = 0;                               // ensure clean state to start with
	    UCB0CTL1 |= UCSWRST;                        // Put state machine into reset
	    UCB0CTL0 &= ~UCCKPL;                        // Clock is inactive LOW
	    UCB0CTL0 |= UCCKPH;                         // Data is changed on second edge
	    UCB0CTL0 |= UCMSB;                          // MSB goes first
	    UCB0CTL0 |= UCMST;                          // Master Mode

	    // SPI Control Register 1
	    UCB0CTL1 |= BIT7;                           // Use SMCLK for clock source
	    UCB0CTL1 &= ~ UCSWRST;                      // Release SM from reset mode
#endif
}
void ConfigureMCP23S17 (void)
{
#ifdef SPI_CS_PIN
	{
	//Configure MCP23S17_1 Port A/B for output
	writeMCP23S17(1,IOCONA,0x08);   // Write (Address = 0, Reg = IOCONA, Value = 0xFF)
	    							// Set IO Bank A Dir to all outputs (0 = output)
	writeMCP23S17(1,IOCONB,0x08);   // Write (Address = 0, Reg = IOCONB, Value = 0xFF)
	        						// Set IO Bank A Dir to all outputs (0 = output)

	writeMCP23S17(0,IOCONA,0x08);   // Write (Address = 0, Reg = IOCONA, Value = 0x08)
	 	 	 	 	 	 	 	 	// Configure MCP23S17 to use address pins
	writeMCP23S17(0,IODIRA,0xFF);   // Write (Address = 0, Reg = IODIRA, Value = 0xFF)
    								// Set IO Bank A Dir to all inputs (0 = output)
	writeMCP23S17(0,IODIRB,0xFF);   // Write (Address = 0, Reg = IODIRB, Value = 0xFF)
        							// Set IO Bank A Dir to all inputs (0 = output)
	writeMCP23S17(0,GPPUA, 0xFF);   // Write (Address = 0, Reg = GPPUA,  Value = 0xFF)
    								// Set IO Bank A Pull Up resistors to all on
	writeMCP23S17(0,GPPUB, 0xFF);   // Write (Address = 0, Reg = GPPUB,  Value = 0xFF)
        							// Set IO Bank B Pull Up resistors to all on

    //Configure MCP23S17_1 Port A/B for output
	writeMCP23S17(1,IODIRA,0x00);   // Write (Address = 0, Reg = IODIRA, Value = 0xFF)
    								// Set IO Bank A Dir to all outputs (0 = output)
	writeMCP23S17(1,IODIRB,0x00);   // Write (Address = 0, Reg = IODIRB, Value = 0xFF)
        							// Set IO Bank A Dir to all outputs (0 = output)
#endif
}

#pragma vector=USCIAB0RX_VECTOR 												// UART Receiver
__interrupt void USCI0RX_ISR(void)
{


	// Clear interrupt Flag
	//IFG1 &= ~UCA0RXIFG ;

	//Store Character
	CommandString[0]++;														// Increment String size {byte 0}
	CommandString[CommandString[0]]= UCA0RXBUF;									// Store character received
	// ECHO back received character
	//UCA0TXBUF = temp;
	// Look for <CR&LF>
	if ((CommandString[CommandString[0]] == 10) & (CommandString[CommandString[0]-1] == 13))
	{
		CommandProcess();
	}

}
#pragma vector=USCIAB0TX_VECTOR 												// UART Transmitter
__interrupt void USCI0TX_ISR(void)
{
}
#pragma vector=TIMER0_A0_VECTOR 												// Timer0 A0 (Used for timing seconds)
__interrupt void Timer_A0 (void)
{
	// Clear interrupt flag
	//TA0CCTL1 &= ~CCIFG;
	//TA0CCR0 += TimerCounter;
	//Semaphores |= SystemTimer;
	__bic_SR_register_on_exit(LPM1_bits);
}
#pragma vector=TIMER0_A1_VECTOR 												// Timer0 A1 not used
__interrupt void Timer_A1 (void)
{
	;
}
#pragma vector=TIMER1_A0_VECTOR    												// RED LED
__interrupt void Timer1_A0(void)
{
	// Clear interrupt flag
	TA1CCTL1 &= ~CCIFG;
	TA1CCR0 += RED_BLINK;
#ifdef RED_LED_OUT
	RED_LED_OUT ^= RED_LED_PIN;
#endif
}
#pragma vector=TIMER1_A1_VECTOR    												// Timer1 A1 not used
__interrupt void Timer1_A1(void)
{
	;
}
#pragma vector=ADC10_VECTOR														// ADC10 ISR
__interrupt void ADC10_ISR(void)
{
	// Clear interrupt flag
	ADC10CTL0 &= ~ ADC10IFG;
	CheckBatteryStatus();
}
#pragma vector=PORT2_VECTOR 													// PORT2 ISR
__interrupt void P2_isr(void)
{
	;
}
#pragma vector=PORT1_VECTOR 													// PORT1 ISR
__interrupt void P1_isr(void)
{
	;																				// Not Used
}
#pragma vector=WDT_VECTOR
__interrupt void wdt_vector(void)
{
	;
}
#pragma vector = COMPARATORA_VECTOR
__interrupt void COMPA_ISR(void)
{
	;
}
#pragma vector=NMI_VECTOR
__interrupt void nmi_ (void)
{
	;
}
