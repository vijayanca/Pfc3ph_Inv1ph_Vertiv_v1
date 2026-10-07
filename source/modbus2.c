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

extern void UpdateBits(void);
extern void Update_DoBits(N16 strt, N16 flag);
extern void BitsUpdate(N16 strt, N16 flag, int *A);
extern void update_datablock_DI(N16 st_adr, N16 ncoils, N8 *buf);
extern void update_datablock_DO(N16 st_adr, N16 ncoils, N8 *buf);
Uint16  pi_com1(void);
extern N16 DObitAddress[];
#define NUM_DOBITS  8   // entries in DObitAddress[] (modbus.c)


void ModbusProcessing1(void)
	{
		
		if ( frame_complete1 == TRUE)
		    {
		        UpdateBits();
		        pi_com1();
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

#define CMD_DEFAULT 999

Uint16  pi_com1(void)
{
    int i = 0, _i = 0, Func = 0;
    Uint16  _adr1 = 0, _val1 = 0;
    long ltemp = 0;
    long ltemp1 = 0;


    Uint16	tx_count=0;
    Uint16 crc_rxd=0,crc_act=0,picom=0,ret;
    float32_t *fptr;
    float32_t float_val = 0;

	rxbuffer1 = &rx_buffer1[0];
	frame_complete1 = FALSE;
	
	if(*(rxbuffer1) == address1)
	{
	 	crc_act = TableCRC(rxbuffer1,chars_rxd1-1);
	    crc_rxd = (( (int) *(rxbuffer1+chars_rxd1) << 8) | (unsigned char) *(rxbuffer1+chars_rxd1-1));
		if (crc_rxd==crc_act)       // checking CRC
	    {                               // add routine for service the functions

		    st_addr1 = ((int)*(rxbuffer1+2)<<8) | *(rxbuffer1+3);
            nregs1 =   ((int)*(rxbuffer1+4)<<8) | *(rxbuffer1+5);
            Func = *(rxbuffer1+1);
           switch(Func)
           {
	       case MB_READDO:

	                    if((nregs1 == 0) || (st_addr1 >= NPARAMS) || (nregs1 > (NPARAMS - st_addr1)))
	                    {
	                        *(rxbuffer1+1) = 0x81;
	                        *(rxbuffer1+2)=2;
	                        tx_count=5;
	                        break;
	                    }

	                        nBytes = (nregs1 + 7)>>3;
	                        *(rxbuffer1+2)= (unsigned char) nBytes;
	                        update_datablock_DO(st_addr1, nregs1, rxbuffer1);
	                        tx_count= *(rxbuffer1+2) + 5;
	                    break;

	           case MB_READDI:
	               if((nregs1 == 0) || (st_addr1 >= NPARAMS) || (nregs1 > (NPARAMS - st_addr1)))
	               {
	                   *(rxbuffer1+1) = 0x82;
	                   *(rxbuffer1+2)=2;
	                   tx_count=5;
	                   break;
	               }

	                   nBytes = (nregs1 + 7)>>3;
	                   *(rxbuffer1+2)= (unsigned char) nBytes;
	                   update_datablock_DI(st_addr1, nregs1, rxbuffer1);
	                   tx_count= *(rxbuffer1+2) + 5;
	               break;

	           case MB_SETDO:
	                st_addr1 = ( (Uint16) *(rxbuffer1+2)<<8) | *(rxbuffer1+3);
	                nregs1 =   ((int)*(rxbuffer1+4)<<8) | *(rxbuffer1+5);
	                if((st_addr1>99) && (st_addr1 < (100 + NUM_DOBITS)))
	                            {
	                                DOParams[st_addr1] = nregs1 & 0x0001;
	                                UpdateOneInt(DObitAddress[st_addr1-100],nregs1 & 0x0001);
	                            }
	                tx_count= 8;
	               break;
	           case MB_SETDOM:
	                 st_addr1 = ( (Uint16) *(rxbuffer1+2)<<8) | *(rxbuffer1+3);
	                 nregs1 =   ((int)*(rxbuffer1+4)<<8) | *(rxbuffer1+5);

	                 if((st_addr1>99) && (st_addr1<110))
	                                {
	                                    if(*(rxbuffer1+6) == 1)
	                                    {
	                                      Update_DoBits(st_addr1, *(rxbuffer1+7));
                                          tx_count= 8;
	                                    }
	                                    else if(*(rxbuffer1+6) == 2)
	                                    {
	                                        Update_DoBits(st_addr1, *(rxbuffer1+7));
                                            Update_DoBits(st_addr1+8, *(rxbuffer1+8));
                                            tx_count= 8;
	                                    }

	                                }
	                 else
	                 {
	                       *(rxbuffer1+1) = 0x8F;
	                        *(rxbuffer1+2)=3;
	                        tx_count=5;
	                 }
	               break;

           	case MB_READREG:

             	st_addr1 = ((int)*(rxbuffer1+2)<<8) | *(rxbuffer1+3);
             	nregs1 =   ((int)*(rxbuffer1+4)<<8) | *(rxbuffer1+5);

                if(st_addr1 > 0x1fff) //23c0
                     {
                         sendLog(st_addr1-0x2000, MB_PORT_B);
                         return(picom);
                     }

                if(st_addr1 > 0xfff)
     			{
             		snapShot(st_addr1, nregs1, MB_PORT_B);
             		return(picom);
     			}
                                           
             	if( ((nregs1 + st_addr1) > INT_RANGE_RMAX) || (nregs1>100))
             	{
					*(rxbuffer1+1) = 0x83;
					*(rxbuffer1+2)=2;
					tx_count=5;
					break;
				}
				
	
					*(rxbuffer1+2)= (unsigned char) nregs1*2;

					if(st_addr1<INT_RANGE_RMIN) update_datablock(st_addr1, nregs1,rxbuffer1);
					else             update_datablock_Int(st_addr1, nregs1,rxbuffer1);

					tx_count=nregs1*2+5;

				break;
				
			case MB_SETREG: 

				st_addr1 = ( (Uint16) *(rxbuffer1+2)<<8) | *(rxbuffer1+3);
				nregs1 =   ((int)*(rxbuffer1+4)<<8) | *(rxbuffer1+5);
				
				if(st_addr1 == 999) { specialCommnad = CMD_DEFAULT; tx_count = 8; break; }

				ret = 1;    // out of range -> exception reply

				if((st_addr1>=INT_RANGE_WMIN) && (st_addr1 < (INT_RANGE_WMIN + NUMWPARAMS1)))
				    {
				        ret = UpdateOneInt(st_addr1 - 500,nregs1);
				    }
                                                  
					if(ret)
					{
						*(rxbuffer1+1) = 0x90;
						*(rxbuffer1+2)=3;
						tx_count=5;	
					}
					else if(ret == 0)
					{
						tx_count=8;
					}  
			        
				break;
            case MB_SETREGM:

                st_addr1 = ( (N16) *(rxbuffer1+2)<<8) | *(rxbuffer1+3);
                nregs1 =   ((int)*(rxbuffer1+4)<<8) | *(rxbuffer1+5);
                if(st_addr1 == (BASE_RTC + 500))
                {
                    st_addr1 = BASE_RTC;
                    ArrayParamsI[BASE_RTC + 0] = ( (N16) *(rxbuffer1+7)<<8) | *(rxbuffer1+8);
                    ArrayParamsI[BASE_RTC + 1] = ( (N16) *(rxbuffer1+9)<<8) | *(rxbuffer1+10);
                    ArrayParamsI[BASE_RTC + 2] = ( (N16) *(rxbuffer1+11)<<8) | *(rxbuffer1+12);
                    ArrayParamsI[BASE_RTC + 3] = ( (N16) *(rxbuffer1+13)<<8) | *(rxbuffer1+14);
                    ArrayParamsI[BASE_RTC + 4] = ( (N16) *(rxbuffer1+15)<<8) | *(rxbuffer1+16);
                    ArrayParamsI[BASE_RTC + 5] = ( (N16) *(rxbuffer1+17)<<8) | *(rxbuffer1+18);
                    ArrayParamsI[BASE_RTC + 6] = ( (N16) *(rxbuffer1+19)<<8) | *(rxbuffer1+20);

#if RTC_PRESENT
                    SetTime(&ArrayParamsI[BASE_RTC]);
#endif

                    tx_count=8;
                    break;
                }
                if(st_addr1 == SERIAL_START)
                {
                    ArrayParamsI[SERIAL_START + 0] = ( (N16) *(rxbuffer1+7)<<8) | *(rxbuffer1+8);
                    ArrayParamsI[SERIAL_START + 1] = ( (N16) *(rxbuffer1+9)<<8) | *(rxbuffer1+10);
                    ArrayParamsI[SERIAL_START + 2] = ( (N16) *(rxbuffer1+11)<<8) | *(rxbuffer1+12);
                    ArrayParamsI[SERIAL_START + 3] = ( (N16) *(rxbuffer1+13)<<8) | *(rxbuffer1+14);
                    ArrayParamsI[SERIAL_START + 4] = ( (N16) *(rxbuffer1+15)<<8) | *(rxbuffer1+16);
                    ArrayParamsI[SERIAL_START + 5] = ( (N16) *(rxbuffer1+17)<<8) | *(rxbuffer1+18);
                    ArrayParamsI[SERIAL_START + 6] = ( (N16) *(rxbuffer1+19)<<8) | *(rxbuffer1+20);
                    ArrayParamsI[SERIAL_START + 7] = ( (N16) *(rxbuffer1+21)<<8) | *(rxbuffer1+22);
                    ArrayParamsI[SERIAL_START + 8] = ( (N16) *(rxbuffer1+23)<<8) | *(rxbuffer1+24);
                    ArrayParamsI[SERIAL_START + 9] = ( (N16) *(rxbuffer1+25)<<8) | *(rxbuffer1+26);

                  //  updateSerial();

                    tx_count=8;
                    break;
                }

                if((st_addr1 >= INT_RANGE_WMIN) && (st_addr1 <= INT_RANGE_WMAX))
                    {
                        for(_i=0;_i<nregs1;_i++)
                              {
                                     _adr1  = st_addr1 +_i - 500;
                                     _val1 = rx_buffer1[7+_i+_i]; _val1 <<= 8; _val1 |= rx_buffer1[8+_i+_i];
                                    UpdateOneInt(_adr1, _val1);
                                    //ArrayParamsI[st_addr1 +_i - 500] =  (Uint16) (*(rxbuffer1+7+_i+_i))<<8 | *(rxbuffer1+8+_i+_i);
                              }
                        tx_count=8;
                    }
                else if((st_addr1 >= FLOAT_RANGE_WMIN) && (st_addr1 <= FLOAT_RANGE_WMAX))
                    {
                        if( nregs1 == 2 )
                        {
                            _adr1 = st_addr1/2;
                            ltemp1 = rx_buffer1[7];
                            ltemp = ltemp1 << 24;
                            ltemp1 = rx_buffer1[8];
                            ltemp |= (ltemp1 << 16);
                            ltemp1 = rx_buffer1[9];
                            ltemp |= (ltemp1 << 8);
                            ltemp1 = rx_buffer1[10];
                            ltemp |= ltemp1;

//                            ltemp = (long) ((long)*(rxbuffer1+7) <<24) | ((long)*(rxbuffer1+8) <<16) | (*(rxbuffer1+9)<<8) | *(rxbuffer1+10);
                            fptr = (float *) &ltemp;
                            float_val = *fptr;

                            ret = UpdateOne(st_addr1/2,float_val);
                            tx_count=8;
                        }
                    }
                else
                {
                    *(rxbuffer1+1) = 0x90;
                    *(rxbuffer1+2)=1;
                    tx_count=5;
                }
                break;
    		default:                       // other than 3/5/16
				*(rxbuffer1+1) |= 0x80;
				*(rxbuffer1+2) = 1;
				tx_count=5;
			}        //Switch case OVER

			picom=0;
			if(tx_count>0)
		 	{
		 		crc_act = TableCRC(rxbuffer1,tx_count-2);
				
				*(rxbuffer1+tx_count-1) =  crc_act >>8;
				*(rxbuffer1+tx_count-2)= crc_act & 0xff;
				
				EnableTX1();    // RS485 #1 (SCIB) transmit, GPIO40
				for(i=0;i<tx_count;i++)
				{
					scib_xmit( (char) *(rxbuffer1+i));
				}
				DisableTX1();   // scib_xmit waited for TXEMPTY
				live_frame1    = FALSE;
			}

		}          //if CRC ok condition ends here
	}          //if ADDRESS ends here
	return(picom);
}
//End of File
