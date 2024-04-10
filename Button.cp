#line 1 "E:/ATP/V2/Button - Copy (29)_/Button.c"

sbit LCD_RS at LATB4_bit;
sbit LCD_EN at LATB5_bit;
sbit LCD_D4 at LATB0_bit;
sbit LCD_D5 at LATB1_bit;
sbit LCD_D6 at LATB2_bit;
sbit LCD_D7 at LATB3_bit;

sbit LCD_RS_Direction at TRISB4_bit;
sbit LCD_EN_Direction at TRISB5_bit;
sbit LCD_D4_Direction at TRISB0_bit;
sbit LCD_D5_Direction at TRISB1_bit;
sbit LCD_D6_Direction at TRISB2_bit;
sbit LCD_D7_Direction at TRISB3_bit;

char atc1[] = "AT";
char atc2[] = "ATE0";

char atc3[50];
char atc4[] = "AT#SGACT=1,1";
char atc5[100];
char atc6[] = "AT#FTPTO=5000";
char atc7[] = "AT#FTPTYPE=0";
char atc8[50];
char atc9[] = "AT#GPRS=0";
char atc10[21] = "AT#FTPAPP=";
char atc11[] = "ATH";
char atc12[] = "AT#SHDN";
char atc13[] = "AT#WAKE=0";
char atc14[] = "AT$GPSD=2";
char atc15[] = "AT$GPSACP";
char atc16[] = "AT#SELINT=2";
char atc17[50];
char atd[] = "ATD0533527819;";
char atc18[] = "AT#SH=1";
char atc19[] = "AT#SCFG=1,1,300,90,600,50";
char atc20[] = "AT#SO=1";
char atc21[] = "AT&K0";
char atc22[] = "AT#SLED=2";
char atc23[] = "AT#SLEDSAV";
char plaka[20];

char read[1024];
char readGPS[200];
int ind = 0;
int indGPS = 2;
short error_counter = 0;
short no_carrier_counter = 0;
short flag_NO_CRRIER = 1;
short flag_for_NO_CARRIER_check = 0;
short Flag_For_EEPROM_CONFIG = 0;
int eeprom_ind = 0;
int eeprom_location = 0;
int tr = 0, tr1 = 0, tr2 = 0, tr3 = 0, tr4 = 0, tr5 = 0, tr6 = 0;
short f1=1,f2=0,f3=0,f4=0,f5=0,f6=0;
short CNTforConcatanate = 0;
short FlagforGPSData = 0;

void interrupt()
{
 if( PIR3.RC2IF )
 {
 readGPS[0] = 'A';
 readGPS[1] = 'T';
 readGPS[indGPS] = UART2_Read();
 indGPS = indGPS + 1;
 if(readGPS[2]=='$' && readGPS[indGPS-2]==13 && readGPS[indGPS-1]==10)
 {
 while(plaka[CNTforConcatanate]!='\0')
 {
 readGPS[indGPS-2] = plaka[CNTforConcatanate];
 CNTforConcatanate = CNTforConcatanate + 1;
 indGPS = indGPS + 1;
 readGPS[indGPS]='\0';
 }
 indGPS = 2;
 CNTforConcatanate = 0;
 FlagforGPSData = 1;
 }
 PIR3.RC2IF=0;
 }

 if( PIR1.RC1IF )
 {
 read[ind] = UART1_Read();
 ind++;
 if(read[0]!='M' && read[1]!='C' && read[2]!='O' && read[3]!='N' && read[4]!='F' && read[ind-2]==13 && read[ind-1]==10)
 {
 read[ind-2]='\0';
 ind = 0;
 }
 if(read[0]=='M' && read[1]=='C' && read[2]=='O' && read[3]=='N' && read[4]=='F' && read[ind-2]==13 && read[ind-1]==10)
 {
 PORTB.F6 = 1;
 read[ind-2] = '\0';
 ind = 0;
 Flag_For_EEPROM_CONFIG = 1;
 }
 PIR1.RC1IF=0;
 }
}

void send_atc(char *s)
{
 while(*s)
 {
 UART1_Write(*s++);
 }
 UART1_Write(0x0D);
 delay_ms(1000);
}

short OK_Response_AT(char *atc)
{
 Lcd_Cmd(_LCD_CLEAR);
 LCD_Out(1,1,atc);
 send_atc(atc);
 LCD_Out(2,1,read);
 while(1)
 {
 if(read[0] == 'E' && read[1] == 'R' && read[2] == 'R' && read[3] == 'O' && read[4] == 'R')
 {
 send_atc(atc);
 LCD_Out(1,1,atc);
 LCD_Out(2,1,read);
 Delay_ms(200);
 Lcd_Cmd(_LCD_CLEAR);
 LCD_Out(1,1,"ERROR!");
 error_counter = error_counter+1;
 if(error_counter == 5)
 {
 error_counter = 0;
 return 1;
 }
 }

 if(read[0] == 'N' && read[1] == 'O' && read[2] == ' ' && read[3] == 'C' && read[4] == 'A'
 && read[5] == 'R' && read[6] == 'R' && read[7] == 'I' && read[8] == 'E' && read[9] == 'R')
 {
 LCD_Out(1,1,atc);
 LCD_Out(2,1,read);
 Lcd_Cmd(_LCD_CLEAR);
 LCD_Out(1,1,"NO CARRIER!");
 Delay_ms(5000);
 Lcd_Cmd(_LCD_CLEAR);
 read[0] = '\0';
 return 1;
 }

 if(read[0] == 'O' && read[1] == 'K')
 {
 LCD_Out(2,1,read);
 Delay_ms(200);
 Lcd_Cmd(_LCD_CLEAR);
 LCD_Out(1,1,"OK!");
 read[0] = '\0';
 ind = 0;
 return 0;
 }

 if(read[0] == 'C' && read[1] == 'O' && read[2] == 'N' && read[3] == 'N'
 && read[4] == 'E' && read[5] == 'C' && read[6] == 'T')
 {
 LCD_Out(2,1,read);
 Delay_ms(200);
 Lcd_Cmd(_LCD_CLEAR);
 LCD_Out(1,1,"CONNECTED!");
 read[0] = '\0';
 ind = 0;
 return 0;
 }
 }
}

void port_init()
{
 ANSELC = 0;
 ANSELD = 0;
 PORTB = 0;
 TRISB = 3;
 PORTD = 0;
}

void interrupt_init()
{
 INTCON.GIE = 1;
 INTCON.PEIE = 1;
 PIE1.RC1IE = 1;
 PIE1.TX1IE = 0;
 PIE3.RC2IE = 1;
 Delay_ms(100);
}

void LCD_init_()
{
 Delay_ms(100);
 Lcd_Init();
 Lcd_Cmd(_LCD_CLEAR);
 Lcd_Cmd(_LCD_CURSOR_OFF);
 Delay_ms(100);
}

void UART_init()
{
 UART1_Init(19200);
 delay_ms(100);
 UART2_Init(4800);
 delay_ms(1000);
}

void main()
{
 port_init();

 interrupt_init();

 LCD_init_();

 UART_init();

 while(PORTB.F7 == 1)
 {
 if(Flag_For_EEPROM_CONFIG == 1)
 {
 while(read[eeprom_ind+5] != '\0')
 {
 EEPROM_Write(eeprom_ind, read[eeprom_ind+5]);
 eeprom_ind++;
 EEPROM_Write(eeprom_ind, '\0');
 }
 }
 }

 while(PORTB.F7 == 0)
 {
 while(EEPROM_Read(tr) != '\0')
 {
 if(EEPROM_Read(tr) != '~' && f1 == 1)
 {
 atc3[tr1] = EEPROM_Read(tr);
 atc3[tr1+1] = '\0';
 f2=0,f3=0,f4=0,f5=0,f6=0;
 tr1 = tr1 + 1;
 if(EEPROM_Read(tr+1) == '~')
 f1=0,f2=1;
 }

 else if(EEPROM_Read(tr) != '~' && f2 == 1)
 {
 atc5[tr2] = EEPROM_Read(tr);
 atc5[tr2+1] = '\0';
 f1=0,f3=0,f4=0,f5=0,f6=0;
 tr2 = tr2 + 1;
 if(EEPROM_Read(tr+1) == '~')
 f2=0,f3=1;
 }

 else if(EEPROM_Read(tr) != '~' && f3 == 1)
 {
 atc8[tr3] = EEPROM_Read(tr);
 atc8[tr3] = '\0';
 f1=0,f2=0,f4=0,f5=0,f6=0;
 tr3 = tr3 + 1;
 if(EEPROM_Read(tr+1) == '~')
 f3=0,f4=1;
 }

 else if(EEPROM_Read(tr) != '~' && f4 == 1)
 {
 atc10[tr4] = EEPROM_Read(tr);
 atc10[tr4+1] = '\0';
 f1=0,f2=0,f3=0,f5=0,f6=0;
 tr4 = tr4 + 1;
 if(EEPROM_Read(tr+1) == '~')
 f4=0,f5=1;
 }

 else if(EEPROM_Read(tr) != '~' && f5 == 1)
 {
 atc17[tr5] = EEPROM_Read(tr);
 atc17[tr5+1] = '\0';
 f1=0,f2=0,f3=0,f4=0,f6=0;
 tr5 = tr5 + 1;
 if(EEPROM_Read(tr+1) == '~')
 f5=0,f6=1;
 }

 else if(EEPROM_Read(tr) != '~' && f6 == 1)
 {
 plaka[tr6] = EEPROM_Read(tr);
 plaka[tr6+1] = '\0';
 f1=0,f2=0,f3=0,f4=0,f5=0;
 tr6 = tr6 + 1;
 if(EEPROM_Read(tr+1) == '~')
 f6=0;
 }
 tr = tr + 1;
 }
 delay_ms(100);
 break;
 }

 CREN_RC1STA_bit = 0;
 delay_ms(10);
 CREN_RC1STA_bit = 1;

 DELAY_MS(30000);
 while(1)
 {
 while(flag_NO_CRRIER)
 {
 port_init();
 interrupt_init();
 LCD_init_();
 UART_init();
 if(OK_Response_AT(atc2))
 break;
 CREN_RC1STA_bit = 0;
 delay_ms(10);
 CREN_RC1STA_bit = 1;

 if(OK_Response_AT(atc1))
 break;

 CREN_RC1STA_bit = 0;
 delay_ms(10);
 CREN_RC1STA_bit = 1;

 if(OK_Response_AT(atc11))
 break;
 if(OK_Response_AT(atc21))
 break;
 if(OK_Response_AT(atc22))
 break;
 if(OK_Response_AT(atc23))
 break;
 if(OK_Response_AT(atc18))
 break;
 if(OK_Response_AT(atc9))
 break;
 if(OK_Response_AT(atc3))
 break;
 if(OK_Response_AT(atc4))
 break;
 if(OK_Response_AT(atc19))
 break;
 if(OK_Response_AT(atc17))
 break;
 flag_NO_CRRIER = 0;
 flag_for_NO_CARRIER_check = 1;
 }

 while(flag_for_NO_CARRIER_check)
 {
 if(readGPS[5] == 'R' && readGPS[6] == 'M' && readGPS[7] == 'C' && FlagforGPSData == 1)
 {
 send_atc(readGPS);
 FlagforGPSData = 0;
 delay_ms(7000);
 }
 if(RC1STA.B1 == 1)
 {
 RC1STA.B4 = 0;
 delay_ms(10);
 RC1STA.B4 = 1;
 UART1_Init(19200);
 }

 if(RC2STA.B1 == 1 || PORTD.F6 == 0)
 {
 RC2STA.B4 = 0;
 delay_ms(10);
 RC2STA.B4 = 1;
 UART2_Init(4800);
 }

 if(read[0] == 'E' && read[1] == 'R' && read[2] == 'R' && read[3] == 'O' && read[4] == 'R')
 {
 flag_NO_CRRIER = 1;
 flag_for_NO_CARRIER_check = 0;
 }
 }
 }
}
