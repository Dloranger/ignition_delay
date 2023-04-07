// UART
unsigned char UART_Configure (unsigned char BAUDRATE);
unsigned char UART_Splash (void);
unsigned char uartSend( unsigned char *pucData, unsigned char ucLength );
void SendTestResult(unsigned char result);
