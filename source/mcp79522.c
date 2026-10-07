/*spiFunctions

 Created on: Feb 2, 2016
     Author: VCA
*/
#define CCEXTERN    extern
#define INIT0
#define INITARRAY0

#include "f28x_project.h"
#include "../ccVariables.h"
#include "pin_map.h"
#include "gpio.h"
#include "string.h"

#define	DWORD	unsigned long
#define WaitForDataByte()           spi_timeout = 0;    \
                                    while((SpiaRegs.SPIFFRX.bit.RXFFST == 0x0) && (spi_timeout < SPI_TIME))

#define	SPI_RTCC_SRWRITE	0x100	//Write STATUS register
#define	SPI_RTCC_EEWRITE	0x200	//Write data to EE memory array beginning at selected address
#define	SPI_RTCC_EEREAD		0x300	//Read data from EE memory array beginning at selected address
#define	SPI_RTCC_EEWRDI		0x400	//Reset the write enable latch (disable write operations)
#define	SPI_RTCC_SRREAD		0x500	//Read STATUS register
#define	SPI_RTCC_EEWREN		0x600	//Set the write enable latch (enable write operations)

#define	SPI_RTCC_WRITE		0x1200	//Write RTCC_/SRAM data to memory array beginning at selected address
#define	SPI_RTCC_READ		0x1300	//Read RTCC_/SRAM array beginning at selected address
#define	SPI_RTCC_UNLOCK		0x1400	//Unlock ID Locations
#define	SPI_RTCC_IDWRITE	0x3200	//Write to the ID Locations
#define	SPI_RTCC_IDREAD		0x3300	//Read the ID Locations
#define	SPI_RTCC_CLRRAM		0x5400

//------------------------------------------------------------------------

//------------------------------------------------------------------------
#define NULL    0
#define POS_MSEC	0
#define POS_SEC		1
#define POS_MIN		2
#define POS_HR		3
#define POS_DOW		4
#define POS_DATE	5
#define POS_MONTH	6
#define POS_YEAR	7

#define IDLOC			10
#define E_PARSTART		0x10
//#define E_SERSTART		0xD0
//#define	RECORDNUMLOC	0x20
void RTCCWriteArray(BYTE addr, BYTE* vData, BYTE wLen);


//date_time sys_datetime;



void WriteRecordPointer(N16 nrecord)
{
	N8 rArray[4]={0};

	rArray[0] = nrecord >> 8;
	rArray[1] = nrecord & 0xff;
	RTCCWriteArray(RECORDNUMLOC, rArray, 2);
	return;
}	


N16 ReadRecordPointer(void)
{
	N16 tmp = 0;
	N8 rArray[4]={0};
	
	RTCCReadArray((BYTE) RECORDNUMLOC, rArray,2);
	tmp = rArray[0]; tmp <<= 8;
	tmp |= rArray[1];
	return(tmp);
}	


void ReadTime(void)
{
	N8 tmpBuf[12] = {0};

#if !RTC_PRESENT
	return;     // no RTC: keep RTCSecond..RTCYear as set (compile time / Modbus)
#endif
		RTCCReadArray(0,tmpBuf,10);
		
		RTCSecond = RTC.Second = ((tmpBuf[POS_SEC] & 0x70) >> 4) * 10 + (tmpBuf[POS_SEC] & 0x0F);
		RTCMinute = RTC.Minute = ((tmpBuf[POS_MIN] & 0x70) >> 4) * 10 + (tmpBuf[POS_MIN] & 0x0F);
		RTCHour = RTC.Hour= ((tmpBuf[POS_HR] & 0x30) >> 4) * 10 + (tmpBuf[POS_HR] & 0x0F);
		RTCDay = tmpBuf[POS_DOW]; // & 0xF0) >> 4) * 10 + (tmpBuf[POS_HR] & 0x0F);
		RTCDate = RTC.Date = ((tmpBuf[POS_DATE] & 0x30) >> 4) * 10 + (tmpBuf[POS_DATE] & 0x0F);
		RTCMonth = RTC.Month = ((tmpBuf[POS_MONTH] & 0x10) >> 4) * 10 + (tmpBuf[POS_MONTH] & 0x0F);
		RTCYear = RTC.Year = (tmpBuf[POS_YEAR] >> 4) * 10 + (tmpBuf[POS_YEAR] & 0x0F);
	return; //rtcc_buff ; // return the read result
}


void	EnableRTCClock(void)
	{
		N8 tmpBuf[4] = {0};
		char dt[12];
		char tt[12];
		
		tmpBuf[0]=0x0;
		tmpBuf[1]=0x35;
		tmpBuf[2]=0x1;
		tmpBuf[3]=0x1;

		RTCCWriteArray(0x1,tmpBuf,4);
		delay_ms(10);

		tmpBuf[0]=0x81;
		tmpBuf[1]=0x35;
		RTCCWriteArray(0x1,(N8 *)tmpBuf,4);
		delay_ms(10);

		strcpy(dt, __DATE__); //compiled Date
		strcpy(tt, __TIME__); //compiled Date

    	RTCSecond	= asc2Dec(tt[6],tt[7]);
    	RTCMinute	= asc2Dec(tt[3],tt[4]);
    	RTCHour		= asc2Dec(tt[0],tt[1]);
    	RTCDay 		= 0;
    	RTCDate		= asc2Dec(dt[4],dt[5]);
    	RTCMonth	= findMonth(dt[0],dt[1],dt[2]);
    	RTCYear		= asc2Dec(dt[9],dt[10]);
  		SetTime(&RTCSecond);
	}

void SetTime(int *buf)
 {
	N8 tmpBuf[10] = {0};
	N8 i = 0;
	N16 x = 0;
	
	
	for(i=1;i<8;i++)
	{
		x = *(buf+i-1) & 0xff;
		x = ((x/10) << 4) + x%10;
		tmpBuf[i] = (N8) x;
	}
	tmpBuf[1] |= 0x80;
	tmpBuf[4] |= 0x8;
	Nop();
	Nop();
        for(i=1;i<10;i++)
        {
            RTCCWriteArray(i,&tmpBuf[i],1);
            delay_ms(2);
        }	
 }


void RTCCReadArray(BYTE dwAddress, BYTE *vData, BYTE wLength)
{
    volatile BYTE Dummy;
    volatile BYTE count=0;

    if(vData == NULL || wLength == 0)
        return;
#if !RTC_PRESENT
    for(count=0;count<wLength;count++) *vData++ = 0;    // no RTC on this board
    return;
#endif
    Nop();
    Nop();
    ENABLE_RTC();

    // Send READ opcode
	SpiaRegs.SPITXBUF = SPI_RTCC_READ;//rtc read
	WaitForDataByte();
	Dummy = SpiaRegs.SPIRXBUF ; //& 0xff;

    // Send address
	SpiaRegs.SPITXBUF = ((N16)dwAddress << 8);
	WaitForDataByte();
	Dummy = SpiaRegs.SPIRXBUF ; //& 0xff;

    // Read data
   // while(wLength--)
    for(count=0;count<wLength;count++)
    {
    	SpiaRegs.SPITXBUF = 0;
    	WaitForDataByte();
    	*vData++ = SpiaRegs.SPIRXBUF & 0xff;
    	Nop();
    	Nop();
    }
    DISABLE_RTC();
}

void	RTCwriteByte(BYTE addr, BYTE val)
{
	 N16 dt = 0;
	 N16 _tmp;

#if !RTC_PRESENT
	return;     // no RTC on this board
#endif
	ENABLE_RTC();

    	SpiaRegs.SPITXBUF = SPI_RTCC_WRITE;//rtc write
    	WaitForDataByte();
    	_tmp = SpiaRegs.SPIRXBUF;

    // Send address
    	dt = addr; dt <<= 8;
    	SpiaRegs.SPITXBUF = (dt & 0xff00);
    	WaitForDataByte();
    	_tmp = SpiaRegs.SPIRXBUF;

    	dt = val; dt <<= 8;
    	SpiaRegs.SPITXBUF = (dt & 0xff00);
		WaitForDataByte();
		_tmp = SpiaRegs.SPIRXBUF;
	    DISABLE_RTC();

		dt = _tmp;

}

void RTCCWriteArray(BYTE addr, BYTE* vData, BYTE wLen)
{
    volatile BYTE Dummy;
    N8 _c = 0;
    N16 dt = 0;


    if(vData == NULL || wLen == 0)
        return;
#if !RTC_PRESENT
    return;     // no RTC on this board
#endif

   	ENABLE_RTC();

    	SpiaRegs.SPITXBUF = SPI_RTCC_WRITE;//rtc write
    	WaitForDataByte();
    	Dummy = SpiaRegs.SPIRXBUF;

    // Send address
    	SpiaRegs.SPITXBUF = ( (N16) addr<< 8);
    	WaitForDataByte();
    	Dummy = SpiaRegs.SPIRXBUF;
	for(_c=0;_c<=wLen;_c++)
//	while(wLen--)
        {
			dt = *vData++;
			SpiaRegs.SPITXBUF = (dt << 8) & 0xff00;
			WaitForDataByte();
	        Dummy = SpiaRegs.SPIRXBUF;
	        Nop();
	        Nop();
		}

    // Restore SPI state
    DISABLE_RTC();
}

N8	ReadRTCCStatus(void)
	{
	N8 Dummy1;

#if !RTC_PRESENT
    return 0;   // no RTC on this board
#endif
    ENABLE_RTC(); // Activate chip select

  		SpiaRegs.SPITXBUF = SPI_RTCC_SRREAD;    /*  read Status  */
  		WaitForDataByte();
    	Dummy1 = SpiaRegs.SPIRXBUF;
    	SpiaRegs.SPITXBUF = 0;
    	WaitForDataByte();
    	Dummy1 = SpiaRegs.SPIRXBUF;
    	DISABLE_RTC();
    	Nop();
    	Nop();

	return Dummy1;		
	}


void eepromReadArray(BYTE addr, BYTE *vData, BYTE wLength)
{
    volatile BYTE Dummy;

    // Ignore operations when the destination is NULL or nothing to read
    if(vData == NULL || wLength == 0) return;
#if !RTC_PRESENT
    while(wLength--) *vData++ = 0;  // no RTC (and no RTC EEPROM) on this board
    return;
#endif
    ENABLE_RTC();

    // Send READ opcode
    SpiaRegs.SPITXBUF = SPI_RTCC_EEREAD;//rtc read
    WaitForDataByte();
    Dummy = SpiaRegs.SPIRXBUF;

    // Send address
    SpiaRegs.SPITXBUF = ( (N16) addr<< 8);;
    WaitForDataByte();
    Dummy = SpiaRegs.SPIRXBUF;

    // Read data
    while(wLength--)
    {
        SpiaRegs.SPITXBUF = 0;
        WaitForDataByte();
        *vData++ = SpiaRegs.SPIRXBUF;
    }

    // Deactivate chip select
    DISABLE_RTC();

}

void eepromWriteArray(BYTE addr, BYTE *vData, WORD wLen)
{
    volatile BYTE Dummy;
    N8	_count = 0;
    N16 dt = 0;

	// Do nothing if no data to process
	if(wLen == 0u)
		return;
#if !RTC_PRESENT
	return;     // no RTC (and no RTC EEPROM) on this board
#endif
	RTCCWriteEnable();
	ENABLE_RTC();

    SpiaRegs.SPITXBUF = SPI_RTCC_EEWRITE;//rtc write
    WaitForDataByte();
    Dummy = SpiaRegs.SPIRXBUF;

    // Send address
    SpiaRegs.SPITXBUF = ( (N16) addr<< 8);
    WaitForDataByte();
    Dummy = SpiaRegs.SPIRXBUF;

	while(wLen--)
        {
			dt = *vData++;
			SpiaRegs.SPITXBUF = (dt << 8);
			WaitForDataByte();
	        Dummy = SpiaRegs.SPIRXBUF;
	        _count++;
		}
		Nop();
		Nop();
    // Restore SPI state
    DISABLE_RTC();
    RTCCWriteDisable();
}


void RTCCWriteEnable(void)
	{
		BYTE dummy;

#if !RTC_PRESENT
		return;     // no RTC on this board
#endif
		ENABLE_RTC();

	    // Send instruction
	    SpiaRegs.SPITXBUF = SPI_RTCC_EEWREN;
	    WaitForDataByte();
	    dummy = SpiaRegs.SPIRXBUF;
    // Deactivate chip select
    DISABLE_RTC();
    if(dummy) dummy = 0;
	}
	
void RTCCWriteDisable(void)
	{
		BYTE dummy;
#if !RTC_PRESENT
		return;     // no RTC on this board
#endif
		ENABLE_RTC();

	    SpiaRegs.SPITXBUF = SPI_RTCC_EEWRDI;
	    WaitForDataByte();
	    dummy = SpiaRegs.SPIRXBUF;
    // Deactivate chip select
    DISABLE_RTC();
    if(dummy) dummy = 0;

	}
