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

void UpdateBits(void);
void Update_DoBits(N16 strt, N16 flag);
void BitsUpdate(N16 strt, N16 flag, int *A);
void update_datablock_DI(N16 st_adr, N16 ncoils, N8 *buf);
void update_datablock_DO(N16 st_adr, N16 ncoils, N8 *buf);


N16 DObitAddress[] = {100, 101, 102, 104, 107, 116, 122, 118};
#define NUM_DOBITS  (sizeof(DObitAddress)/sizeof(DObitAddress[0]))

// Send a frame on the port the request came in on.
// Common Controller v2: both ports are RS-485 and drive their enable line
// (port A: GPIO34, port B: GPIO40). scix_xmit waits for TXEMPTY per character.
static void port_xmit(N16 portNumber, N8 *buf, N16 n)
{
    N16 ix;

    if(portNumber == MB_PORT_B)
    {
        EnableTX1();
        for(ix=0;ix<n;ix++)
            scib_xmit(buf[ix]);
        DisableTX1();
    }
    else
    {
        EnableTX();
        for(ix=0;ix<n;ix++)
            scia_xmit(buf[ix]);
        DisableTX();
    }
}

void    sendLog(N16 nPkt, N16 portNumber)
{
    N16 epAddr = 0, ix = 0, crc_act = 0;
    N8 *buf = (portNumber == MB_PORT_B) ? rx_buffer1 : rx_buffer;

    if(nPkt == 5000)    //erase Log
    {
        //EraseEventLog();
        return;
    }

    epAddr = (nPkt << 6) + LOGSTART;

    eeprom_read_words(epAddr,spiReadBuffer, RECSIZE);

    buf[2] = (RECSIZE + RECSIZE);

    for(ix=0;ix < RECSIZE;ix++)
    {
        buf[ix*2+3] = (spiReadBuffer[ix] >> 8);
        buf[ix*2+4] = (spiReadBuffer[ix] & 0xff);
    }

    crc_act = TableCRC(buf,(RECSIZE + RECSIZE + 3));

     buf[(RECSIZE+RECSIZE+4)] =  crc_act >>8;
     buf[(RECSIZE+RECSIZE+3)]= crc_act & 0xff;
     port_xmit(portNumber, buf, RECSIZE+RECSIZE+5);
}



void ModbusProcessing(void)
	{
		
		if ( frame_complete == TRUE)
		    {
		        pi_com();
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

#define CMD_DEFAULT 999
Uint16  _adr1 = 0, _val1 = 0;
float32_t float_val = 0;
long ltemp = 0;
long ltemp1 = 0;

Uint16  pi_com(void)
{
    int _i = 0, Func = 0;
    Uint16	tx_count=0;

    Uint16 crc_rxd=0,crc_act=0,picom=0,ret;
    float32_t *fptr;


	rxbuffer = &rx_buffer[0];
	frame_complete = FALSE;
	
	if(*(rxbuffer) == address)
	{
	 	crc_act = TableCRC(rxbuffer,chars_rxd-1);
	    crc_rxd = (( (int) *(rxbuffer+chars_rxd) << 8) | (unsigned char) *(rxbuffer+chars_rxd-1));
		if (crc_rxd==crc_act)       // checking CRC
	    {                               // add routine for service the functions

		    st_addr = ((int)*(rxbuffer+2)<<8) | *(rxbuffer+3);
            nregs =   ((int)*(rxbuffer+4)<<8) | *(rxbuffer+5);
            Func = *(rxbuffer+1);
            UpdateBits();
           switch(Func)
           {
	       case MB_READDO:

	                    if((nregs == 0) || (st_addr >= NPARAMS) || (nregs > (NPARAMS - st_addr)))
	                    {
	                        *(rxbuffer+1) = 0x81;
	                        *(rxbuffer+2)=2;
	                        tx_count=5;
	                        break;
	                    }

	                        nBytes = (nregs + 7)>>3;
	                        *(rxbuffer+2)= (unsigned char) nBytes;
	                        update_datablock_DO(st_addr, nregs, rxbuffer);
	                        tx_count= *(rxbuffer+2) + 5;
	                    break;

	           case MB_READDI:
	               if((nregs == 0) || (st_addr >= NPARAMS) || (nregs > (NPARAMS - st_addr)))
	               {
	                   *(rxbuffer+1) = 0x82;
	                   *(rxbuffer+2)=2;
	                   tx_count=5;
	                   break;
	               }

	                   nBytes = (nregs + 7)>>3;
	                   *(rxbuffer+2)= (unsigned char) nBytes;
	                   update_datablock_DI(st_addr, nregs, rxbuffer);
	                   tx_count= *(rxbuffer+2) + 5;
	               break;

	           case MB_SETDO:
	                st_addr = ( (Uint16) *(rxbuffer+2)<<8) | *(rxbuffer+3);
	                nregs =   ((int)*(rxbuffer+4)<<8) | *(rxbuffer+5);
	                if((st_addr>99) && (st_addr < (100 + NUM_DOBITS)))
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
	                                    if(*(rxbuffer+6) == 1)
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
	                       *(rxbuffer+1) = 0x8F;
	                        *(rxbuffer+2)=3;
	                        tx_count=5;
	                 }
	               break;

           	case MB_READREG:

             	st_addr = ((int)*(rxbuffer+2)<<8) | *(rxbuffer+3);
             	nregs =   ((int)*(rxbuffer+4)<<8) | *(rxbuffer+5);

                if(st_addr > 0x1fff) //23c0
                     {
                         sendLog(st_addr-0x2000, MB_PORT_A);
                         return(picom);
                     }

                if(st_addr > 0xfff)
     			{
             		snapShot(st_addr, nregs, MB_PORT_A);
             		return(picom);
     			}
                                           
             	if( ((nregs + st_addr) > 900) || (nregs>100))
             	{
					*(rxbuffer+1) = 0x83;
					*(rxbuffer+2)=2;
					tx_count=5;
					break;
				}
				
//#define   INT_RANGE_WMIN 700
//#define   INT_RANGE_WMAX 899
//#define   FLOAT_RANGE_WMIN 200
//#define   FLOAT_RANGE_WMAX 450
//
//#define   INT_RANGE_RMIN 500
//#define   INT_RANGE_RMAX 899
//#define   FLOAT_RANGE_RMIN 200
//#define   FLOAT_RANGE_RMAX 450

					*(rxbuffer+2)= (unsigned char) nregs*2;

					if(st_addr<INT_RANGE_RMIN) update_datablock(st_addr, nregs,rxbuffer);
					else update_datablock_Int(st_addr, nregs,rxbuffer);

					tx_count=nregs*2+5;

				break;
				
			case MB_SETREG: 

				st_addr = ( (Uint16) *(rxbuffer+2)<<8) | *(rxbuffer+3);
				nregs =   ((int)*(rxbuffer+4)<<8) | *(rxbuffer+5);
				
				if(st_addr == 999) { specialCommnad = CMD_DEFAULT; tx_count = 8; break; }

				ret = 1;    // out of range -> exception reply

				if((st_addr>=INT_RANGE_WMIN) && (st_addr < (INT_RANGE_WMIN + NUMWPARAMS1)))
				    {
				        ret = UpdateOneInt(st_addr - 500,nregs);
				        tx_count=8;
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
                if(st_addr == (BASE_RTC + 500))
                {
                    st_addr = BASE_RTC;
                    ArrayParamsI[BASE_RTC + 0] = ( (N16) *(rxbuffer+7)<<8) | *(rxbuffer+8);
                    ArrayParamsI[BASE_RTC + 1] = ( (N16) *(rxbuffer+9)<<8) | *(rxbuffer+10);
                    ArrayParamsI[BASE_RTC + 2] = ( (N16) *(rxbuffer+11)<<8) | *(rxbuffer+12);
                    ArrayParamsI[BASE_RTC + 3] = ( (N16) *(rxbuffer+13)<<8) | *(rxbuffer+14);
                    ArrayParamsI[BASE_RTC + 4] = ( (N16) *(rxbuffer+15)<<8) | *(rxbuffer+16);
                    ArrayParamsI[BASE_RTC + 5] = ( (N16) *(rxbuffer+17)<<8) | *(rxbuffer+18);
                    ArrayParamsI[BASE_RTC + 6] = ( (N16) *(rxbuffer+19)<<8) | *(rxbuffer+20);

#if RTC_PRESENT
                    SetTime(&ArrayParamsI[BASE_RTC]);
#endif

                    tx_count=8;
                    break;
                }
//                if(st_addr == SERIAL_START + 500)
//                {
//                    ArrayParamsI[SERIAL_START + 0] = ( (N16) *(rxbuffer+7)<<8) | *(rxbuffer+8);
//                    ArrayParamsI[SERIAL_START + 1] = ( (N16) *(rxbuffer+9)<<8) | *(rxbuffer+10);
//                    ArrayParamsI[SERIAL_START + 2] = ( (N16) *(rxbuffer+11)<<8) | *(rxbuffer+12);
//                    ArrayParamsI[SERIAL_START + 3] = ( (N16) *(rxbuffer+13)<<8) | *(rxbuffer+14);
//                    ArrayParamsI[SERIAL_START + 4] = ( (N16) *(rxbuffer+15)<<8) | *(rxbuffer+16);
//                    ArrayParamsI[SERIAL_START + 5] = ( (N16) *(rxbuffer+17)<<8) | *(rxbuffer+18);
//                    ArrayParamsI[SERIAL_START + 6] = ( (N16) *(rxbuffer+19)<<8) | *(rxbuffer+20);
//                    ArrayParamsI[SERIAL_START + 7] = ( (N16) *(rxbuffer+21)<<8) | *(rxbuffer+22);
//                    ArrayParamsI[SERIAL_START + 8] = ( (N16) *(rxbuffer+23)<<8) | *(rxbuffer+24);
//                    ArrayParamsI[SERIAL_START + 9] = ( (N16) *(rxbuffer+25)<<8) | *(rxbuffer+26);
//
//                  //  updateSerial();
//
//                    tx_count=8;
//                    break;
               // }

                if((st_addr >= INT_RANGE_WMIN) && (st_addr <= INT_RANGE_WMAX))
                    {
                        for(_i=0;_i<nregs;_i++)
                              {
                                     _adr1  = st_addr +_i - 500;
                                     _val1 = rx_buffer[7+_i+_i]; _val1 <<= 8; _val1 |= rx_buffer[8+_i+_i];
                                    UpdateOneInt(_adr1, _val1);
                                    //ArrayParamsI[st_addr +_i - 500] =  (Uint16) (*(rxbuffer+7+_i+_i))<<8 | *(rxbuffer+8+_i+_i);
                              }
                        tx_count=8;
                    }
                else if((st_addr >= FLOAT_RANGE_WMIN) && (st_addr <= FLOAT_RANGE_WMAX))
                    {
                        if( nregs == 2 )
                        {
                            _adr1 = st_addr/2;
                            ltemp1 = rx_buffer[7];
                            ltemp = ltemp1 << 24;
                            ltemp1 = rx_buffer[8];
                            ltemp |= (ltemp1 << 16);
                            ltemp1 = rx_buffer[9];
                            ltemp |= (ltemp1 << 8);
                            ltemp1 = rx_buffer[10];
                            ltemp |= ltemp1;

//                            ltemp = (long) ((long)*(rxbuffer+7) <<24) | ((long)*(rxbuffer+8) <<16) | (*(rxbuffer+9)<<8) | *(rxbuffer+10);
                            fptr = (float *) &ltemp;
                            float_val = *fptr;

                            ret = UpdateOne(st_addr/2,float_val);
                            tx_count=8;
                        }
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
				
				EnableTX();
				for(_i=0;_i<tx_count;_i++)
				{
					scia_xmit( (char) *(rxbuffer+_i));
				}
				DisableTX();
				live_frame     = FALSE;
			}

		}          //if CRC ok condition ends here
	}          //if ADDRESS ends here

	return(picom);
}

#define     plot0       lpf_vInpR.u
#define     plot1       lpf_vInpY.u
#define     plot2       lpf_vInpB.u
#define     plot3       lpf_iInpR.y
#define     plot4       lpf_iInpY.y
#define     plot5       lpf_iInpB.y

#define     plot6       lpf_vdc.y
#define     plot7       IdcFback
#define     plot8       idc1Fbk
#define     plot9       ibatFbk
#define     plot10      pid_idc.Out

#define     plot11      RP1
#define     plot12      RN1
#define     plot13      YP1
#define     plot14      YN1
#define     plot15      BP1
#define     plot16      BN1

#define     plot17      vOutput //vol_Bph
#define     plot18      pllGrid.theta_0.sin_val   //
#define     plot19      pllGrid.theta.sin_val

#define     plot21      RP2
#define     plot22      RN2
#define     plot23      YP2
#define     plot24      YN2
#define     plot25      BP2
#define     plot26      BN2


void snapShot(N16 st_addr, N16 nreg, N16 portNumber)
{
	int i;
	long dat1;
	long *nptr;
	int k = 0;
	int txCount = 0;
	N16 crc1 = 0;
	N16 increment = 1;

switch(st_addr & 0xFF)
	{
		case 0:
			waveptr =  &plot0;
			break;
		case 1:
			waveptr =  &plot1;
			break;
		case 2:
			waveptr =  &plot2;
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
            if(bridge2) waveptr =  &plot21;
            break;
        case 12:
            waveptr =  &plot12;
            if(bridge2) waveptr =  &plot22;
            break;
        case 13:
            waveptr =  &plot13;
            if(bridge2) waveptr =  &plot23;
            break;
        case 14:
            waveptr =  &plot14;
            if(bridge2) waveptr =  &plot24;
            break;
        case 15:
            waveptr =  &plot15;
            if(bridge2) waveptr =  &plot25;
            break;
        case 16:
            waveptr =  &plot16;
            if(bridge2) waveptr =  &plot26;
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

	tx_buffer[0] = (portNumber == MB_PORT_B) ? address1 : address;
	tx_buffer[1] = 0x3;

	nptr = (long *)&waveBuffer;
	k = 3;


	if(nreg == 100)
	{
	    txCount = 205;
	    increment = 2;
	}
	else
    {
        txCount = 405;
        increment = 1;
    }
	tx_buffer[2] = (txCount - 5) & 0xff;   // byte count field is 8 bits on the wire


    for(i=0;i < SNAP_SIZE;i+=increment)
    {
        dat1 = *nptr;
        tx_buffer[k++] = (dat1>>24) & 0xff;
        tx_buffer[k++] = (dat1>>16) & 0xff;
        tx_buffer[k++] = (dat1>>8) & 0xff;
        tx_buffer[k++] = (dat1 & 0xff) & 0xff;
        nptr++;
    }

	crc1 = TableCRC(tx_buffer,txCount-2);
	tx_buffer[txCount-1] =   crc1 >>8;
	tx_buffer[txCount-2] =   crc1 & 0xff;
	port_xmit(portNumber, tx_buffer, txCount);
}

void update_datablock_DO(N16 st_adr, N16 ncoils, N8 *buf)
{
    Uint16 regCount = 0, posNow = 0;
    Uint16 byte1 = 0, t1 = 0;
    Uint16 nbytes = (ncoils + 7) >> 3;

     for(regCount = 0;regCount < nbytes;regCount++)
         {
             byte1 = 0;
             for(t1=0;t1<8;t1++)
             {
                 posNow = regCount*8 + t1;
                 if(posNow >= ncoils) break;    // unused bits in last byte must be 0
                 if(DOParams[st_adr + posNow]) byte1 |= (1 << t1);
             }
             *(buf + regCount + 3) = (N8) byte1;
          } //end of for LOOP
 }


void update_datablock_DI(N16 st_adr, N16 ncoils, N8 *buf)
{

    Uint16 regCount = 0, posNow = 0;
    Uint16 byte1 = 0, t1 = 0;
    Uint16 nbytes = (ncoils + 7) >> 3;

     for(regCount = 0;regCount < nbytes;regCount++)
         {
             byte1 = 0;
             for(t1=0;t1<8;t1++)
             {
                 posNow = regCount*8 + t1;
                 if(posNow >= ncoils) break;    // unused bits in last byte must be 0
                 if(DIParams[st_adr + posNow]) byte1 |= (1 << t1);
             }
             *(buf + regCount + 3) = (N8) byte1;
          }

 }


void update_datablock_Int(N16 st_adr, N16 nreg, N8 *rxbuf)
    {

        Uint16 offset;
        Uint16 regCount;
        Uint16 TempValue = 0;

        for(regCount=st_adr;regCount < st_adr + nreg;regCount++)
            {
                offset = (regCount - st_adr) << 1;
                offset += 3;    //offset for data bytes
                TempValue = ArrayParamsI[regCount-500];

                *(rxbuf + offset++) = TempValue >> 8;
                *(rxbuf + offset)   = TempValue & 0xff;

             } //end of for LOOP
    }

void update_datablock(N16 st_adr, N16 nreg, N8 *rxbuf )
    {

        Uint16 offset;
        Uint16 regCount;
        Uint16 nregs_half;
        Uint16 regStart;
        long val = 0;
        long *nptr = 0;

        nregs_half = nreg >>1;
        regStart = st_adr /2;

        for(regCount = 0;regCount < nregs_half;regCount++)
        	{
	        	offset = 3 + (regCount << 2);
	        	nptr = (long *) &ArrayParamsF[regStart + regCount];
	        	val = *nptr;

	        	*(rxbuf + offset++) = val >>24;
		        *(rxbuf + offset++)   = val >>16;
                *(rxbuf + offset++) = val >>8;
                *(rxbuf + offset)   = val & 0xff;

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
static const WORD wCRCTable[] = {
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
   0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040 };

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
    BitsUpdate(80, AlarmFlag6 , DOParams);
    BitsUpdate(96, AlarmFlag7 , DOParams);
    BitsUpdate(112, AlarmFlag8 , DOParams);

//    UnitOnCommand_Bit       = UnitOnCommand ;
//    AcceptReset_Bit         = AcceptReset   ;
//    TestMode_Bit            = TestMode      ;
//    emShutdown_Bit          = emShutdown    ;
//    UpdateEEPROM_Bit        = UpdateEEPROM  ;
//    PFSign_Bit              = PFSign        ;
//    RevPolarity_Bit         = RevPolarity   ;

    BitsUpdate(0, 0x1111, DIParams);
    BitsUpdate(1, 0x2222, DIParams);
    BitsUpdate(2, 0x4444, DIParams);
    BitsUpdate(3, 0x8888, DIParams);
    BitsUpdate(4, 0x5555, DIParams);

}

//End of File
