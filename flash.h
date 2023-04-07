unsigned char EraseFlashMemory (char Segment);
void writeSN(int LOTCODE_WW, int LOTCODE_YY,int SN_1, int SN_2);
void writeTest(int TEST_WW, int TEST_YY,int TEST_RESULT);
void writeCal(int VOLTS, int R6,int R7);
void writeDelay(int Heartbeat, int OVER,int UNDER, int Running, int TurnOff, int HeatbeatDuration);
void writeThresholds(unsigned int LowWarn,unsigned int LowCritical,unsigned int HighCritical);
void writeTimer1(int val_0, int val_1,int val_2,int val_3,int val_4,int val_5,int val_6,int val_7);
void writeTimer2(int val_0, int val_1,int val_2,int val_3,int val_4,int val_5,int val_6,int val_7);
void writeTimer3(int val_0, int val_1,int val_2,int val_3);
void writeTimer4(int val_0, int val_1,int val_2,int val_3);
void writeTimer5(int val_0, int val_1,int val_2,int val_3);
void writePulseDivider(int val_0);
