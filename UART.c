#include "Globals_Define.h"
#include "UART.h"



volatile unsigned char TXBUFF [256];
volatile unsigned char TX_PTR;

unsigned char uartSend(unsigned char *pucData, unsigned char ucLength)
{
	int i;
  for(i=ucLength;i>0;i--)
  {
    // Wait for TX buffer to be ready for new data
    while(!(UC0IFG & UCA0TXIFG));

    // Push data to TX buffer
    UCA0TXBUF = *pucData;

    // Update variables
    pucData++;
  }
  return 0;
}
unsigned char UART_Splash (void)
{
	unsigned char StartUpSplash[47] =
	{
			0x0D,0x0A,
			'S','E','R','I','A','L',' ','N','U','M','B','E','R',':','#','#','#','#','#','#','#','#',0x0D,0x0A,
			'P','R','O','U','D','L','Y',' ','M','A','D','E',' ','I','N',' ','U','S','A',0x0D,0x0A,
	};
	int *WEEK = (int *) WEEK_PTR;
	int *YEAR = (int *) YEAR_PTR;
	int *SERIAL_1 = (int *) SERIAL_1_PTR;
	int *SERIAL_2 = (int *) SERIAL_2_PTR;

	if ((*(int *) WEEK) != 0xffff)
	{
	StartUpSplash[16] = (*(int *) WEEK)>>8;
	StartUpSplash[17] = (*(int *) WEEK) & 0xff;
	StartUpSplash[18] = (*(int *)YEAR)>>8;
	StartUpSplash[19] = (*(int *) YEAR) & 0xff;
	StartUpSplash[20] = (*(int *) SERIAL_1)>>8;
	StartUpSplash[21] = (*(int *) SERIAL_1) & 0xff;
	StartUpSplash[22] = (*(int *) SERIAL_2)>>8;
	StartUpSplash[23] = (*(int *) SERIAL_2) & 0xff;
	}
	uartSend(StartUpSplash,sizeof(StartUpSplash));
	return 0;
}
unsigned char UART_Configure (unsigned char BAUDRATE)
{
	UCA0CTL1 |= UCSWRST;						// put state machine in reset
	UART_PSEL |= UART_TX_PIN + UART_RX_PIN;
	UCA0CTL1 = UCSSEL_2;                      // SMCLK
switch (BAUDRATE)
{
	case 0x01:// 9600 from 1Mhz
	{
		UCA0BR0 = 104;
		UCA0BR1 = 0;
		UCA0MCTL = UCBRS_1;
		break;
	}
	case 0x02:// 9600 from 2Mhz
	{
		UCA0BR0 = 0xDA;
		UCA0BR1 = 0x0;
		UCA0MCTL = UCBRS_6;
		break;
	}
	case 0x04: // 9600 from 4Mhz
	{
		UCA0BR0 = 0xA0;
		UCA0BR1 = 0x1;
		UCA0MCTL = UCBRS_6;
		break;
	}
	case 0x06:// 9600 from 6Mhz
	{
		UCA0BR0 = 0x7B;
		UCA0BR1 = 0x2;
		UCA0MCTL = UCBRS_3;
		break;
	}
	case 0x08:// 9600 from 8Mhz
	{
		UCA0BR0 = 0x41;
		UCA0BR1 = 0x03;
		UCA0MCTL = UCBRS_2;
		break;
	}
	case 0x0A:// 9600 from 10Mhz
	{
		UCA0BR0 = 0x79;
		UCA0BR1 = 0x4;
		UCA0MCTL = UCBRS_7;
		break;
	}
	case 0x0C: // 9600 from 12Mhz
	{
		UCA0BR0 = 0xE2;
		UCA0BR1 = 0x4;
		UCA0MCTL = 0;
		break;
	}
	case  0x10:// 9600 from 16Mhz
	{
		UCA0BR0 = 0x82;
		UCA0BR1 = 0x6;
		UCA0MCTL = UCBRS_6;
		break;
	}
	case 0x20:// 16 MHz->115200
	{
		UCA0BR0 = 0x8A;
		UCA0BR1 = 0x00;
		UCA0MCTL = UCBRS_7;
		break;
	}
default:
	{
		break;
	}
}
	P1SEL2 = UART_TX_PIN + UART_RX_PIN; 		// P1.1 = RXD, P1.2=TXD
	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
	return 0;
}
