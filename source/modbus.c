/*
 * MODBUS COMMUNICATION FUNCTIONS
 * USE RS-485 LINK
 */

#define	CCEXTERN	extern	
#define	INIT0
#define	INITARRAY0

#include "f28x_project.h"
#include "../ccVariables.h"
#include "inc/hw_memmap.h"
#include "sci.h"
#include "SVGEN_CLA.h"
void UpdateBits(void);
void Update_DoBits(N16 strt, N16 flag);
void BitsUpdate(N16 strt, N16 flag, int *A);
void update_datablock_DO(void);
void update_datablock_DI(void);

extern SVGEN_CLA svgen_dq1;

N16 DObitAddress[] = {100, 101, 104, 105, 112, 117, 123, 126, 127};

// Two Modbus ports with the same register map:
//   MB_PORT_A = SCIA, RS485 #2 (TXEN GPIO34)
//   MB_PORT_B = SCIB, RS485 #1 (TXEN GPIO40), GUI port
// pi_com() selects the port; the reply helpers below use rxbuffer and mb_putc().
#define MB_PORT_A   0
#define MB_PORT_B   1
static N16 mbPort = MB_PORT_A;

static void mb_putc(int c)
{
    if(mbPort == MB_PORT_B) scib_xmit(c);
    else scia_xmit(c);
}
#define MB_TX_ON()  { if(mbPort == MB_PORT_B) { EnableTX1(); } else { EnableTX(); } }
#define MB_TX_OFF() { if(mbPort == MB_PORT_B) { DisableTX1(); } else { DisableTX(); } }

void    sendLog(N16 nPkt)
{
    N16 epAddr = 0, ix = 0, crc_act = 0;

    if(nPkt == 5000)    //erase Log
    {
        EraseEventLog();
        return;
    }

    epAddr = (nPkt << 6) + LOGSTART;

    eeprom_read(epAddr,RECSIZE);

    rxbuffer[2] = (RECSIZE + RECSIZE);

    for(ix=0;ix < RECSIZE;ix++)
    {
        rxbuffer[ix*2+3] = (spiReadBuffer[ix] >> 8);
        rxbuffer[ix*2+4] = (spiReadBuffer[ix] & 0xff);
    }

    crc_act = TableCRC(rxbuffer,(RECSIZE + RECSIZE + 3));

     rxbuffer[(RECSIZE+RECSIZE+4)] =  crc_act >>8;
     rxbuffer[(RECSIZE+RECSIZE+3)]= crc_act & 0xff;
     MB_TX_ON();
     MB_TX_ON();
     MB_TX_ON();
     for(ix=0;ix<(RECSIZE+RECSIZE+5);ix++)
         mb_putc(rxbuffer[ix]);
     MB_TX_OFF();
}



void ModbusProcessing(void)
	{
		
		if ( frame_complete == TRUE)
		    {
		        UpdateBits();
		        pi_com(MB_PORT_A);
		    }

	    if((interchar_silent_timer >= INTERCHARDELAY) && (live_frame == TRUE))
	    {
		    if(char_waiting > 6)
		    {
	        	chars_rxd = char_waiting - 1;
	        	char_waiting = 0;
	        	frame_complete = TRUE;
	     	}
	     	live_frame = FALSE;
	    }

	 }

// SCIB (RS485 #1) framing, same as ModbusProcessing()
void ModbusProcessing1(void)
	{
		if ( frame_complete1 == TRUE)
		    {
		        UpdateBits();
		        pi_com(MB_PORT_B);
		    }

	    if((interchar_silent_timer1 >= INTERCHARDELAY) && (live_frame1 == TRUE))
	    {
		    if(char_waiting1 > 6)
		    {
	        	chars_rxd1 = char_waiting1 - 1;
	        	char_waiting1 = 0;
	        	frame_complete1 = TRUE;
	     	}
	     	live_frame1 = FALSE;
	    }
	 }


Uint16  pi_com(N16 port)
{
    int i, Func = 0;
    long ltemp = 0;
    Uint16	tx_count=0;
    Uint16 crc_rxd=0,crc_act=0,picom=0,ret;
    float32_t *fptr;
    float32_t float_val = 0;
    Uint16  nrx;                // received bytes - 1 (CRC high byte index)
    Uint16  myAddress;

    mbPort = port;
    if(port == MB_PORT_B)
    {
        rxbuffer = &rx_buffer1[0];
        frame_complete1 = FALSE;
        nrx = chars_rxd1;
        myAddress = address1;
    }
    else
    {
        rxbuffer = &rx_buffer[0];
        frame_complete = FALSE;
        nrx = chars_rxd;
        myAddress = address;
    }

	if(*(rxbuffer) == myAddress)
	{
	 	crc_act = TableCRC(rxbuffer,nrx-1);
	    crc_rxd = (( (int) *(rxbuffer+nrx) << 8) | (unsigned char) *(rxbuffer+nrx-1));
		if (crc_rxd==crc_act)       // checking CRC
	    {                               // add routine for service the functions

		    st_addr = ((int)*(rxbuffer+2)<<8) | *(rxbuffer+3);
            nregs =   ((int)*(rxbuffer+4)<<8) | *(rxbuffer+5);
            Func = *(rxbuffer+1);
           switch(Func)
           {
	       case MB_READDO:

	                    if((nregs + st_addr) > 200)
	                    {
	                        *(rxbuffer+1) = 0x81;
	                        *(rxbuffer+2)=2;
	                        tx_count=5;
	                        break;
	                    }

	                        nBytes = (nregs + 7)>>3;
	                        *(rxbuffer+2)= (unsigned char) nBytes;
	                        update_datablock_DO();
	                        tx_count= *(rxbuffer+2) + 5;
	                    break;

	           case MB_READDI:
	               if((nregs + st_addr) > 200)
	               {
	                   *(rxbuffer+1) = 0x81;
	                   *(rxbuffer+2)=2;
	                   tx_count=5;
	                   break;
	               }

	                   nBytes = (nregs + 7)>>3;
	                   *(rxbuffer+2)= (unsigned char) nBytes;
	                   update_datablock_DI();
	                   tx_count= *(rxbuffer+2) + 5;
	               break;

	           case MB_SETDO:
	                st_addr = ( (Uint16) *(rxbuffer+2)<<8) | *(rxbuffer+3);
	                nregs =   ((int)*(rxbuffer+4)<<8) | *(rxbuffer+5);
	                if((st_addr>99) && (st_addr<110))
	                            {
	                                DOParams[st_addr] = nregs & 0x0001;
	                                UpdateOneInt(DObitAddress[st_addr-100],nregs & 0x0001);
	                            }
	                tx_count= 8;
	               break;
	           case MB_SETDOM:

//	               Slave Address 11
//	               Function 0F
//	               Coil Address Hi 00
//	               Coil Address Lo 13
//	               Quantity of Coils Hi 00
//	               Quantity of Coils Lo 0A
//	               Byte Count 02
//	               Force Data Hi (Coils 27-20) CD
//	               Force Data Lo (Coils 29-28) 01
//	               Error Check (LRC or CRC) ��

	                 st_addr = ( (Uint16) *(rxbuffer+2)<<8) | *(rxbuffer+3);
	                 nregs =   ((int)*(rxbuffer+4)<<8) | *(rxbuffer+5);

	                 if((st_addr>99) && (st_addr<110))
	                                {
	                                    if(*(rxbuffer+6))
	                                    {
	                                      Update_DoBits(st_addr, *(rxbuffer+7));
                                          tx_count= 8;
	                                    }
	                                    else if(*(rxbuffer+6) == 2)
	                                    {
	                                        Update_DoBits(st_addr, *(rxbuffer+7));
                                            Update_DoBits(st_addr+8, *(rxbuffer+8));
                                            tx_count= 8;
	                                    }

	                                }
	                 else
	                 {
	                       *(rxbuffer+1) = 0x90;
	                        *(rxbuffer+2)=3;
	                        tx_count=5;
	                 }
	               break;

           	case MB_READREG:

             	st_addr = ((int)*(rxbuffer+2)<<8) | *(rxbuffer+3);
             	nregs =   ((int)*(rxbuffer+4)<<8) | *(rxbuffer+5);

                if(st_addr > 0x1fff) //23c0
                     {
                         sendLog(st_addr-0x2000);
                         return(picom);
                     }

                if(st_addr > 0xfff)
     			{
             		snapShot(st_addr);
             		return(picom);
     			}
                                           
             	if( ((nregs + st_addr) > 700) || (nregs>100))
             	{
					*(rxbuffer+1) = 0x83;
					*(rxbuffer+2)=2;
					tx_count=5;
					break;
				}
				
	
					*(rxbuffer+2)= (unsigned char) nregs*2;

					if(st_addr<500) update_datablock();
					else update_datablock_Int();

					tx_count=nregs*2+5;

				break;
				
			case MB_SETREG: 

				st_addr = ( (Uint16) *(rxbuffer+2)<<8) | *(rxbuffer+3);
				nregs =   ((int)*(rxbuffer+4)<<8) | *(rxbuffer+5);
				
			    if ((st_addr == 11) && nregs) {
			        RESET_FAULTS();

			    }

				if(st_addr == 999) { specialCommnad = CMD_DEFAULT; tx_count = 8; break; }

				if((st_addr>599) && (st_addr<700))
				    {
				        ret = UpdateOneInt(st_addr - 500,nregs);
				        Nop();
				        Nop();
				    }
                                                  
					if(ret)
					{
						*(rxbuffer+1) = 0x90;
						*(rxbuffer+2)=3;
						tx_count=5;	
					}
					else if(ret == 0)
					{
						tx_count=8;
					}  
			        
				break;
            case MB_SETREGM:

                st_addr = ( (N16) *(rxbuffer+2)<<8) | *(rxbuffer+3);
                nregs =   ((int)*(rxbuffer+4)<<8) | *(rxbuffer+5);
                     if(st_addr == BASE_RTC)
                     {
                         ArrayParamsI[BASE_RTC + 0] = ( (N16) *(rxbuffer+7)<<8) | *(rxbuffer+8);
                         ArrayParamsI[BASE_RTC + 1] = ( (N16) *(rxbuffer+9)<<8) | *(rxbuffer+10);
                         ArrayParamsI[BASE_RTC + 2] = ( (N16) *(rxbuffer+11)<<8) | *(rxbuffer+12);
                         ArrayParamsI[BASE_RTC + 3] = ( (N16) *(rxbuffer+13)<<8) | *(rxbuffer+14);
                         ArrayParamsI[BASE_RTC + 4] = ( (N16) *(rxbuffer+15)<<8) | *(rxbuffer+16);
                         ArrayParamsI[BASE_RTC + 5] = ( (N16) *(rxbuffer+17)<<8) | *(rxbuffer+18);
                         ArrayParamsI[BASE_RTC + 6] = ( (N16) *(rxbuffer+19)<<8) | *(rxbuffer+20);

                         //SetTime(&ArrayParam[240]);

                         seconds = RTCSecond;
     //                    minutes = RTCMinute;
     //                    hours = RTCHour;
     //                    days = RTCDate;

                         tx_count=8;
                         break;
                     }
//                     if(st_addr == SERIAL_START)
//                     {
//                         ArrayParam[SERIAL_START + 0] = ( (N16) *(rxbuffer+7)<<8) | *(rxbuffer+8);
//                         ArrayParam[SERIAL_START + 1] = ( (N16) *(rxbuffer+9)<<8) | *(rxbuffer+10);
//                         ArrayParam[SERIAL_START + 2] = ( (N16) *(rxbuffer+11)<<8) | *(rxbuffer+12);
//                         ArrayParam[SERIAL_START + 3] = ( (N16) *(rxbuffer+13)<<8) | *(rxbuffer+14);
//                         ArrayParam[SERIAL_START + 4] = ( (N16) *(rxbuffer+15)<<8) | *(rxbuffer+16);
//                         ArrayParam[SERIAL_START + 5] = ( (N16) *(rxbuffer+17)<<8) | *(rxbuffer+18);
//                         ArrayParam[SERIAL_START + 6] = ( (N16) *(rxbuffer+19)<<8) | *(rxbuffer+20);
//                         ArrayParam[SERIAL_START + 7] = ( (N16) *(rxbuffer+21)<<8) | *(rxbuffer+22);
//                         ArrayParam[SERIAL_START + 8] = ( (N16) *(rxbuffer+23)<<8) | *(rxbuffer+24);
//                         ArrayParam[SERIAL_START + 9] = ( (N16) *(rxbuffer+25)<<8) | *(rxbuffer+26);
//
//                         updateSerial();
//
//                         tx_count=8;
//                         break;
//                     }


                if( nregs == 2 )
                {
                    ltemp = (long) ((long)*(rxbuffer+7) <<24) | ((long)*(rxbuffer+8) <<16) | (*(rxbuffer+9)<<8) | *(rxbuffer+10);
                    fptr = (float *) &ltemp;
                    float_val = *fptr;

                    ret = UpdateOne(st_addr/2,float_val);
                    tx_count=8;
                }
                else
                {
                    *(rxbuffer+1) = 0x90;
                    *(rxbuffer+2)=1;
                    tx_count=5;
                }
                break;

    		default:                       // other than 3/5/16
				*(rxbuffer+1) |= 0x80;
				*(rxbuffer+2) = 1;
				tx_count=5;
			}        //Switch case OVER

			picom=0;
			if(tx_count>0)
		 	{
		 		crc_act = TableCRC(rxbuffer,tx_count-2);
				
				*(rxbuffer+tx_count-1) =  crc_act >>8;
				*(rxbuffer+tx_count-2)= crc_act & 0xff;
				
				MB_TX_ON();
				for(i=0;i<tx_count;i++)
				{
					mb_putc( (char) *(rxbuffer+i));
				}
				MB_TX_OFF();
				if(port == MB_PORT_B) live_frame1 = FALSE;
				else live_frame = FALSE;
			}

		}          //if CRC ok condition ends here
	}          //if ADDRESS ends here
	return(picom);
}
#define     plot0       vOutR       // output voltages, common-mode removed
#define     plot1       vOutY
#define     plot2       vOutB
#define     plot3       lpf_ioutR.y
#define     plot4       lpf_ioutY.y
#define     plot5       lpf_ioutB.y
#define     plot6       lpf_vbypR.y
#define     plot7       lpf_vbypY.y
#define     plot8       lpf_vbypB.y
#define     plot9       vcontrol.refa
#define     plot10      vcontrol.refb
#define     plot11      vcontrol.refc
#define     plot12      vcontrol.alpha
#define     plot13      vcontrol.beta
#define     plot14      prH[0][0].out
#define     plot15      vcontrol.vol.lpf_de.y
#define     plot16      pllBypass.theta.sin_val
#define     plot17      pllBypass.theta_0.sin_val
#ifdef SHIFTED_FBACK
    #define     plot18      theta_fb.sin_val
#else
    #define plot18      theta_inv.cos_val
#endif
#define     plot19      theta_inv.sin_val

void snapShot(N16 st_addr)
{
	int i;
	long dat1;
	long *nptr;

switch(st_addr & 0xFF)
	{
		case 0:
			waveptr =  (float32_t *) &plot0;
			break;
		case 1:
			waveptr =  (float32_t *) &plot1;
			break;
		case 2:
			waveptr =  (float32_t *) &plot2;
			break;
		case 3:
			waveptr =  &plot3;
			break;
		case 4:
			waveptr =  &plot4;
			break;
		case 5:
			waveptr =  &plot5;
			break;
		case 6:
			waveptr =  &plot6;
			break;
		case 7:
			waveptr =  &plot7;
			break;
		case 8:
			waveptr =  &plot8;
			break;
		case 9:
			waveptr =  &plot9;
			break;
		case 10:
			waveptr =  &plot10;
			break;
        case 11:
            waveptr =  &plot11;
            break;
        case 12:
            waveptr =  &plot12;
            break;
        case 13:
            waveptr =  &plot13;
            break;
        case 14:
            waveptr =  &plot14;
            break;
        case 15:
            waveptr =  &plot15;
            break;
        case 16:
            waveptr =  &plot16;
            break;
        case 17:
            waveptr =  &plot17;
            break;
        case 18:
            waveptr =  &plot18;
            break;
        case 19:
            waveptr =  &plot19;
            break;

	}
	snap_end = 0;
	while(!snap_end);		//wait for data capture

	MB_TX_ON();
	mb_putc((mbPort == MB_PORT_B) ? address1 : address);
	mb_putc(0x3);
	mb_putc(0x00);
	nptr = (long *)&waveBuffer;

		for(i=0;i < SNAP_SIZE;i++)
		{
			dat1 = *nptr;
			mb_putc(dat1>>24);
			mb_putc(dat1>>16);
			mb_putc(dat1>>8);
			mb_putc(dat1 & 0xff);
			nptr++;
		}

		mb_putc(0x01);
		mb_putc(0x02); //dummy CRC, no CRC check at receiving end
		MB_TX_OFF();
}

void update_datablock_DO(void)
{
    Uint16 regCount = 0, posNow = 0;
    Uint16 byte1 = 0, t1 = 0;


     for(regCount = 0;regCount < nBytes;regCount++)
         {
             byte1 = 0;
             for(t1=0;t1<8;t1++)
             {
                 posNow = regCount*8 + t1;
                 if(DOParams[st_addr + posNow]) byte1 |= (1 << t1);
             }
             *(rxbuffer + regCount + 3) = (N8) byte1;
          } //end of for LOOP
 }

void update_datablock_DI(void)
{

    Uint16 regCount = 0, posNow = 0;
    Uint16 byte1 = 0, t1 = 0;


     for(regCount = 0;regCount < nBytes;regCount++)
         {
             byte1 = 0;
             for(t1=0;t1<8;t1++)
             {
                 posNow = regCount*8 + t1;
                 if(DIParams[st_addr + posNow]) byte1 |= (1 << t1);
             }
             *(rxbuffer + regCount + 3) = (N8) byte1;
          }

 }


void update_datablock_Int(void)
    {

        Uint16 offset;
        Uint16 regCount;
        Uint16 TempValue = 0;


        for(regCount=st_addr;regCount < st_addr + nregs;regCount++)
            {
                offset = (regCount - st_addr) << 1;
                offset += 3;    //offset for data bytes
                TempValue = ArrayParamsI[regCount-500];

                *(rxbuffer + offset++) = TempValue >> 8;
                *(rxbuffer + offset)   = TempValue & 0xff;

             } //end of for LOOP
    }

void update_datablock(void)
    {

        Uint16 offset;
        Uint16 regCount;
        Uint16 nregs_half;
        Uint16 regStart;
        long val = 0;
        long *nptr = 0;

        nregs_half = nregs >>1;

        regStart = st_addr /2;

        for(regCount = 0;regCount < nregs_half;regCount++)
        	{
	        	offset = 3 + (regCount << 2);
	        	nptr = (long *) &ArrayParamsF[regStart + regCount];
	        	val = *nptr;

	        	*(rxbuffer + offset++) = val >>24;
		        *(rxbuffer + offset++)   = val >>16;
                *(rxbuffer + offset++) = val >>8;
                *(rxbuffer + offset)   = val & 0xff;

		     } //end of for LOOP
    }


unsigned char crc_buffer[100] = {};

N16 CRC16(N16 *dat, N16 nbytes)
{
   N16 i1 = 0;
   N16 nw = 0;

   int *x;

   nw = nbytes >>1;
   x = (int *)crc_buffer;

   for(i1=0;i1<nw;i1++)
   {
       *(x+i1) = *(dat+i1);
   }
   i1 = TableCRC (crc_buffer, nbytes);
   return(i1);

}

N16 TableCRC ( BYTE *nData, WORD wLength)
{
static const WORD wCRCTable[] =
{
   0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
   0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
   0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
   0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
   0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
   0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
   0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
   0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
   0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
   0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
   0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
   0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
   0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
   0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
   0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
   0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
   0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
   0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
   0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
   0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
   0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
   0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
   0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
   0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
   0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
   0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
   0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
   0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
   0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
   0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
   0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
   0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040
};

BYTE nTemp;
WORD wCRCWord = 0xFFFF;

   while (wLength--)
   {
      nTemp = *nData++ ^ wCRCWord;
      nTemp &= 0xff;
      wCRCWord >>= 8;
      wCRCWord  ^= wCRCTable[nTemp];
   }
   return (wCRCWord);
  // return ((wCRCWord >> 8) | (wCRCWord << 8));
} // end MODBUS_Table_LookUp_CRC function


void BitsUpdate(N16 strt, N16 flag, int *A)
{
    Uint16 k = 0;
        for(k=0;k<16;k++)
        {
            *(A + strt + k) = FALSE;
            if(flag & (1 << k)) *(A + strt + k) = TRUE;
            Nop();
            Nop();
        }
}

void Update_DoBits(N16 strt, N16 flag)
{
    Uint16 k = 0;
        for(k=0;k<8;k++)
        {
            DOParams[strt + k] = FALSE;
            if(flag & (1 << k)) DOParams[strt + k] = TRUE;

           // UpdateOneInt(DObitAddress[st_addr-100],nregs & 0x0001);
        }
}
void UpdateBits(void)
{
    BitsUpdate(0, AlarmFlag1, DOParams);
    BitsUpdate(16, AlarmFlag2, DOParams);
    BitsUpdate(32, AlarmFlag3, DOParams);
    BitsUpdate(48, AlarmFlag4, DOParams);
    BitsUpdate(64, AlarmFlag5 , DOParams);
    BitsUpdate(80, RelayFlag,  DOParams);
    BitsUpdate(96, CommStatusFlag , DOParams);

    UnitOnCommand_Bit       = UnitOnCommand ;
    AcceptReset_Bit         = AcceptReset   ;
    TestMode_Bit            = TestMode      ;
    Master_Bit              = Master        ;
    emShutdown_Bit          = EmergencyStop    ;
    autoBypassFlag_Bit      = autoBypassFlag;
    BypassAbscent_Bit       = BypassAbscent ;
    UpdateEEPROM_Bit        = UpdateEEPROM  ;
    PFSign_Bit              = PFSign        ;
    RevPolarity_Bit         = RevPolarity   ;

    BitsUpdate(0, DigitalInputFlag1, DIParams);

}

//End of File
