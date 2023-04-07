#include "Globals_Define.h"
#include "UART.h"
#include "flash.h"

volatile unsigned char Pointer = 0;

volatile unsigned char CommandString[40];

unsigned char CommandProcess(void);
unsigned char pointerIncrement(void);
unsigned char ECHO = 0;
unsigned char pointerIncrement(void)
{
	Pointer = Pointer +1;
	return Pointer;
}
unsigned char CommandProcess(void)
{
	static unsigned char LineEnd [2] =  {0x0A,0x0D};

	// CHECK FOR RESET REQUEST
	if ((CommandString[1] == 'R' )&
			(CommandString[2] == 'S') &
			(CommandString[3] == 'T')
	)
	{
		uartSend(LineEnd,sizeof(LineEnd));
		// Start WDT
		WDTCTL &= ~WDTHOLD;
		// force a WDT timeout to reset
		while (1);
	} //RESET

	// CHECK FOR "w_sn:" REQUEST
	if ((CommandString[1] == 'w')&
			(CommandString[2] == '_')&
			(CommandString[3] == 's')&
			(CommandString[4] == 'n')&
			(CommandString[5] == ':'))
	{
		writeSN(((CommandString[6]<<8) + CommandString[7]),	// WW
				((CommandString[8]<<8) + CommandString[9]),	// YY
				((CommandString[10]<<8) + CommandString[11]), // SN_1
				((CommandString[12]<<8) + CommandString[13]));// SN_2
		uartSend(LineEnd,sizeof(LineEnd));
		// Clear Command Buffer
		CommandString[0] = 0;
		return 0;
	}// w_sn:

	// CHECK FOR "w_test:" REQUEST
	if ((CommandString[1] == 'w')&
			(CommandString[2] == '_')&
			(CommandString[3] == 't')&
			(CommandString[4] == 'e')&
			(CommandString[5] == 's')&
			(CommandString[6] == 't')&
			(CommandString[7] == ':'))
	{
		writeTest(((CommandString[8]<<8) + CommandString[9]),
				((CommandString[10]<<8) + CommandString[11]),
				((CommandString[12]<<8) + CommandString[13]));
		// Clear Command Buffer
		CommandString[0] = 0;
		uartSend(LineEnd,sizeof(LineEnd));
		return 0;
	}// w_test:

	// CHECK FOR "r_cal" REQUEST
	if ((CommandString[1] == 'r')&
			(CommandString[2] == '_')&
			(CommandString[3] == 'c')&
			(CommandString[4] == 'a')&
			(CommandString[5] == 'l'))
	{
		unsigned char data[6] = {'#','#','#','#','#','#'};
		data[0] = (*((int *) VOLTAGE_PTR)) & 0xFF;
		data[1] = (*((int *) VOLTAGE_PTR))>>8;
		data[2] = (*((int *) RESISTOR_R6)) & 0xFF;
		data[3] = (*((int *) RESISTOR_R6))>>8;
		data[4] = (*((int *) RESISTOR_R7)) & 0xFF;
		data[5] = (*((int *) RESISTOR_R7))>>8;
		uartSend(data,sizeof(data));
		// Clear Command Buffer
		CommandString[0] = 0;
		return 0;
	}// r_cal

	// CHECK FOR "w_cal:" REQUEST
	if ((CommandString[1] == 'w')&
			(CommandString[2] == '_')&
			(CommandString[3] == 'c')&
			(CommandString[4] == 'a')&
			(CommandString[5] == 'l')&
			(CommandString[6] == ':'))
	{
		writeCal(((CommandString[7]<<8) + CommandString[8]),	//voltage (mv)
				((CommandString[9]<<8) + CommandString[10]),	//R6
				((CommandString[11]<<8) + CommandString[12]));	//R7
		// Clear Command Buffer
		CommandString[0] = 0;
		return 0;
	}// w_cal:

	// CHECK FOR "r_del:" REQUEST
	if ((CommandString[1] == 'r')&
			(CommandString[2] == '_')&
			(CommandString[3] == 'd')&
			(CommandString[4] == 'e')&
			(CommandString[5] == 'l'))
	{
		unsigned char data[12] = {'#','#','#','#','#','#','#','#','#','#','#','#'};
		data[0] = (*((int *) HeartBeatDelay_PTR)) & 0xFF;
		data[1] = (*((int *) HeartBeatDelay_PTR))>>8;
		data[2] = (*((int *) OverVoltageDelay_PTR)) & 0xFF;
		data[3] = (*((int *) OverVoltageDelay_PTR))>>8;
		data[4] = (*((int *) UnderVoltageDelay_PTR)) & 0xFF;
		data[5] = (*((int *) UnderVoltageDelay_PTR))>>8;
		data[6] = (*((int *) IgnitionlessRunningVolt_PTR)) & 0xFF;
		data[7] = (*((int *) IgnitionlessRunningVolt_PTR))>>8;
		data[8] = (*((int *) IgnitionlessTurnOffVolt_PTR)) & 0xFF;
		data[9] = (*((int *) IgnitionlessTurnOffVolt_PTR))>>8;
		data[10] = (*((int *) HeartbeatDuration_PTR)) & 0xFF;
		data[11] = (*((int *) HeartbeatDuration_PTR))>>8;
		uartSend(data,sizeof(data));
		// Clear Command Buffer
		CommandString[0] = 0;
		return 0;
	}//r_del

	// CHECK FOR "w_del:" REQUEST
	if ((CommandString[1] == 'w')&
			(CommandString[2] == '_')&
			(CommandString[3] == 'd')&
			(CommandString[4] == 'e')&
			(CommandString[5] == 'l')&
			(CommandString[6] == ':'))
	{
		writeDelay(((CommandString[7]<<8) + CommandString[8]),		//Heartbeat
				((CommandString[9]<<8) + CommandString[10]),		//OverVoltage
				((CommandString[11]<<8) + CommandString[12]),		//Undervoltage
				((CommandString[13]<<8) + CommandString[14]),		// Running Voltage
				((CommandString[15]<<8) + CommandString[16]),		// Turn Off Voltage
				((CommandString[17]<<8) + CommandString[18]));		// Heart Beat duration
		// Clear Command Buffer
		CommandString[0] = 0;
		return 0;
	}// w_del:

	// CHECK FOR "r_thr:" REQUEST
	if ((CommandString[1] == 'r')&
			(CommandString[2] == '_')&
			(CommandString[3] == 't')&
			(CommandString[4] == 'h')&
			(CommandString[5] == 'r'))
	{
		unsigned char data[6] = {'#','#','#','#','#','#'};
		data[0] = (*((int *) LowBatWarn_PTR)) & 0xFF;
		data[1] = (*((int *) LowBatWarn_PTR))>>8;
		data[2] = (*((int *) LowBatCritical_PTR)) & 0xFF;
		data[3] = (*((int *) LowBatCritical_PTR))>>8;
		data[4] = (*((int *) HighBatCritical_PTR)) & 0xFF;
		data[5] = (*((int *) HighBatCritical_PTR))>>8;
		uartSend(data,sizeof(data));
		// Clear Command Buffer
		CommandString[0] = 0;
		return 0;

	}// r_thr

	// CHECK FOR "w_thr:" REQUEST
	if ((CommandString[1] == 'w')&
			(CommandString[2] == '_')&
			(CommandString[3] == 't')&
			(CommandString[4] == 'h')&
			(CommandString[5] == 'r')&
			(CommandString[6] == ':'))
	{
		writeThresholds(((CommandString[7]<<8) + CommandString[8]),	//Low Warning
				((CommandString[9]<<8) + CommandString[10]),	//Low Critical
				((CommandString[11]<<8) + CommandString[12]));	//High Critical
		// Clear Command Buffer
		CommandString[0] = 0;
		return 0;
	}// w_thr:

	// CHECK FOR "r_t#" REQUEST
	if ((CommandString[1] == 'r')&
			(CommandString[2] == '_')&
			(CommandString[3] == 't'))
	{
		unsigned char data[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		switch (CommandString[4])
		{
		default:
		{
			// Clear Command Buffer
			CommandString[0] = 0;
			return 1;
		}
		case '1':
		{
			data[0] = (*((int *) Timer1_0_PTR))>>8;
			data[1] = (*((int *) Timer1_0_PTR)) & 0xFF;
			data[2] = (*((int *) Timer1_1_PTR))>>8;
			data[3] = (*((int *) Timer1_1_PTR)) & 0xFF;
			data[4] = (*((int *) Timer1_2_PTR))>>8;
			data[5] = (*((int *) Timer1_2_PTR)) & 0xFF;
			data[6] = (*((int *) Timer1_3_PTR))>>8;
			data[7] = (*((int *) Timer1_3_PTR)) & 0xFF;
			data[8] = (*((int *) Timer1_4_PTR))>>8;
			data[9] = (*((int *) Timer1_4_PTR)) & 0xFF;
			data[10] = (*((int *) Timer1_5_PTR))>>8;
			data[11] = (*((int *) Timer1_5_PTR)) & 0xFF;
			data[12] = (*((int *) Timer1_6_PTR))>>8;
			data[13] = (*((int *) Timer1_6_PTR)) & 0xFF;
			data[14] = (*((int *) Timer1_7_PTR))>>8;
			data[15] = (*((int *) Timer1_7_PTR)) & 0xFF;
			uartSend(data,sizeof(data));
			// Clear Command Buffer
			CommandString[0] = 0;
			return 0;
		}//case 1
		case '2':
		{
			data[0] = (*((int *) Timer2_0_PTR))>>8;
			data[1] = (*((int *) Timer2_0_PTR)) & 0xFF;
			data[2] = (*((int *) Timer2_1_PTR))>>8;
			data[3] = (*((int *) Timer2_1_PTR)) & 0xFF;
			data[4] = (*((int *) Timer2_2_PTR))>>8;
			data[5] = (*((int *) Timer2_2_PTR)) & 0xFF;
			data[6] = (*((int *) Timer2_3_PTR))>>8;
			data[7] = (*((int *) Timer2_3_PTR)) & 0xFF;
			data[8] = (*((int *) Timer2_4_PTR))>>8;
			data[9] = (*((int *) Timer2_4_PTR)) & 0xFF;
			data[10] = (*((int *) Timer2_5_PTR))>>8;
			data[11] = (*((int *) Timer2_5_PTR)) & 0xFF;
			data[12] = (*((int *) Timer2_6_PTR))>>8;
			data[13] = (*((int *) Timer2_6_PTR)) & 0xFF;
			data[14] = (*((int *) Timer2_7_PTR))>>8;
			data[15] = (*((int *) Timer2_7_PTR)) & 0xFF;
			uartSend(data,sizeof(data));
			// Clear Command Buffer
			CommandString[0] = 0;
			return 0;
		}// case 2
		case '3':
		{
			data[0] = (*((int *) Timer3_0_PTR))>>8;
			data[1] = (*((int *) Timer3_0_PTR)) & 0xFF;
			data[2] = (*((int *) Timer3_1_PTR))>>8;
			data[3] = (*((int *) Timer3_1_PTR)) & 0xFF;
			data[4] = (*((int *) Timer3_2_PTR))>>8;
			data[5] = (*((int *) Timer3_2_PTR)) & 0xFF;
			data[6] = (*((int *) Timer3_3_PTR))>>8;
			data[7] = (*((int *) Timer3_3_PTR)) & 0xFF;
			uartSend(data,sizeof(data));
			// Clear Command Buffer
			CommandString[0] = 0;
			return 0;
		}// case 4
		case '4':
		{
			data[0] = (*((int *) Timer4_0_PTR))>>8;
			data[1] = (*((int *) Timer4_0_PTR)) & 0xFF;
			data[2] = (*((int *) Timer4_1_PTR))>>8;
			data[3] = (*((int *) Timer4_1_PTR)) & 0xFF;
			data[4] = (*((int *) Timer4_2_PTR))>>8;
			data[5] = (*((int *) Timer4_2_PTR)) & 0xFF;
			data[6] = (*((int *) Timer4_3_PTR))>>8;
			data[7] = (*((int *) Timer4_3_PTR)) & 0xFF;
			uartSend(data,sizeof(data));
			// Clear Command Buffer
			CommandString[0] = 0;
			return 0;
		}// case 4
		case '5':
		{
			data[0] = (*((int *) Timer5_0_PTR))>>8;
			data[1] = (*((int *) Timer5_0_PTR)) & 0xFF;
			data[2] = (*((int *) Timer5_1_PTR))>>8;
			data[3] = (*((int *) Timer5_1_PTR)) & 0xFF;
			data[4] = (*((int *) Timer5_2_PTR))>>8;
			data[5] = (*((int *) Timer5_2_PTR)) & 0xFF;
			data[6] = (*((int *) Timer5_3_PTR))>>8;
			data[7] = (*((int *) Timer5_3_PTR)) & 0xFF;
			uartSend(data,sizeof(data));
			// Clear Command Buffer
			CommandString[0] = 0;
			return 0;
		}// case 5
		}// switch
	}

	// CHECK for "w_pd:" request
	if ((CommandString[1] =='w')&
		(CommandString[2] == '_')&
		(CommandString[3] == 'p')&
		(CommandString[4] == 't'))
		{
			// CommandString[5] is the ":"
			writePulseDivider((CommandString[6]));
			// Clear Command Buffer
			CommandString[0] = 0;
		}
	// CHECK FOR "w_t#:" REQUEST
	if ((CommandString[1] == 'w')&
			(CommandString[2] == '_')&
			(CommandString[3] == 't'))
	{
		switch (CommandString[4])
		{
		case '1':
		{
			// CommandString[5] is the ":"
			writeTimer1(
					((CommandString[6]<<8) + CommandString[7]),//Timer1_0
					((CommandString[8]<<8) + CommandString[9]),//Timer1_1
					((CommandString[10]<<8) + CommandString[11]),//Timer1_2
					((CommandString[12]<<8) + CommandString[13]),//Timer1_3
					((CommandString[14]<<8) + CommandString[15]),//Timer1_4
					((CommandString[16]<<8) + CommandString[17]),//Timer1_5
					((CommandString[18]<<8) + CommandString[19]),//Timer1_6
					((CommandString[20]<<8) + CommandString[21])//Timer1_7
			);
			break;
		}
		case '2':
		{
			// CommandString[5] is the ":"
			writeTimer2(
					((CommandString[6]<<8) + CommandString[7]),//TimerN_0
					((CommandString[8]<<8) + CommandString[9]),//TimerN_1
					((CommandString[10]<<8) + CommandString[11]),//TimerN_2
					((CommandString[12]<<8) + CommandString[13]),//TimerN_3
					((CommandString[14]<<8) + CommandString[15]),//TimerN_4
					((CommandString[16]<<8) + CommandString[17]),//TimerN_5
					((CommandString[18]<<8) + CommandString[19]),//TimerN_6
					((CommandString[20]<<8) + CommandString[21])//TimerN_7
			);
			break;
		}
		case '3':
		{
			// CommandString[5] is the ":"
			writeTimer3(
					((CommandString[6]<<8) + CommandString[7]),//TimerN_0
					((CommandString[8]<<8) + CommandString[9]),//TimerN_1
					((CommandString[10]<<8) + CommandString[11]),//TimerN_2
					((CommandString[12]<<8) + CommandString[13])//TimerN_3
			);
			break;
		}
		case '4':
		{
			// CommandString[5] is the ":"
			writeTimer4(
					((CommandString[6]<<8) + CommandString[7]),//TimerN_0
					((CommandString[8]<<8) + CommandString[9]),//TimerN_1
					((CommandString[10]<<8) + CommandString[11]),//TimerN_2
					((CommandString[12]<<8) + CommandString[13])//TimerN_3
			);
			break;
		}
		case '5':
		{
			// CommandString[5] is the ":"
			writeTimer5(
					((CommandString[6]<<8) + CommandString[7]),//TimerN_0
					((CommandString[8]<<8) + CommandString[9]),//TimerN_1
					((CommandString[10]<<8) + CommandString[11]),//TimerN_2
					((CommandString[12]<<8) + CommandString[13])//TimerN_3
			);
			break;
		}
		default:
		{
			break;
		}
		}
		// Clear Command Buffer
		CommandString[0] = 0;
		return 0;
	}// w_t#:

	// CHECK FOR "ECHO:" REQUEST
	if ((CommandString[1] == 'E')&
			(CommandString[2] == 'C')&
			(CommandString[3] == 'H')&
			(CommandString[4] == 'O'))
	{
		ECHO ^= 0x01;
		// Clear Command Buffer
		CommandString[0] = 0;
		return 0;
	}// ECHO:

	uartSend(LineEnd,sizeof(LineEnd));
	// Clear Command Buffer
	CommandString[0] = 0;

	return 1;
}



