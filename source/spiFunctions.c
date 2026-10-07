
/*spiFunctions

 Created on: Jun 2, 2013
     Author: VCA
*/
#define CCEXTERN    extern
#define INIT0
#define INITARRAY0

#include "f28x_project.h"
#include "../ccVariables.h"
#include "pin_map.h"


void spi_init(void);
#define Nop()    __asm(" NOP")

unsigned int count = 0;
unsigned int epr_byte;

unsigned int dum_read = 0;

static Uint16 eeprom_read_status(void);
static void eeprom_write_enable(void);
static Uint16 eeprom_wait_write_complete(void);
static Uint16 eeprom_wait_wel(void);
#define WaitForDataByte()   \
        spi_timeout = 0; \
        while((SpiaRegs.SPIFFRX.bit.RXFFST == 0x0) && (spi_timeout < SPI_TIME))

#define DISABLE_SPI()       GpioDataRegs.GPBSET.bit.GPIO57 = 1
#define EEPROM_CS_LOW()     GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1
#define EEPROM_CS_HIGH()    GpioDataRegs.GPBSET.bit.GPIO57 = 1
#define SPI_WRITE8(x)       (SpiaRegs.SPITXBUF = ((Uint16)(x) << 8))
#define SPI_READ8()         (SpiaRegs.SPIRXBUF & 0x00FF)

//EEPROM Functions
/************************************************************************
* EEPROM Commands
************************************************************************/
    #define EEPROM_PAGE_SIZE    (unsigned)128
    #define EEPROM_CMD_READ     (unsigned)0x0300
    #define EEPROM_CMD_WRITE    (unsigned)0x0200
    #define EEPROM_CMD_WRDI     (unsigned)0x0400
    #define EEPROM_CMD_WREN     (unsigned)0x0600
    #define EEPROM_CMD_RDSR     (unsigned)0x0500
    #define EEPROM_CMD_WRSR     (unsigned)0x0100
    #define EEPROM_CMD_PERASE   (unsigned)0x0100

   // #define EEPROM_CMD_PERASE   (unsigned)0x4200
    #define EEPROM_CMD_SERASE   (unsigned)0xD800
    #define EEPROM_CMD_CERASE   (unsigned)0xC700

N32 download_staddr = 0;
N16 epr_byte;


Uint16 eeprom_read_words(Uint16 addr, Uint16 *buf, Uint16 words)
{
    Uint16 i;
    Uint16 msb, lsb;
    volatile Uint16 dummy;

    spi_error = 0;

    EEPROM_CS_LOW();

    SpiaRegs.SPITXBUF = 0x0300;           // READ
    while(SpiaRegs.SPIFFRX.bit.RXFFST == 0);
    dummy = SpiaRegs.SPIRXBUF;

    SpiaRegs.SPITXBUF = (addr & 0xFF00);  // address high
    while(SpiaRegs.SPIFFRX.bit.RXFFST == 0);
    dummy = SpiaRegs.SPIRXBUF;

    SpiaRegs.SPITXBUF = ((addr & 0x00FF) << 8); // address low
    while(SpiaRegs.SPIFFRX.bit.RXFFST == 0);
    dummy = SpiaRegs.SPIRXBUF;

    for(i = 0; i < words; i++)
    {
        SpiaRegs.SPITXBUF = 0x0000;
        while(SpiaRegs.SPIFFRX.bit.RXFFST == 0);
        msb = SpiaRegs.SPIRXBUF & 0x00FF;

        SpiaRegs.SPITXBUF = 0x0000;
        while(SpiaRegs.SPIFFRX.bit.RXFFST == 0);
        lsb = SpiaRegs.SPIRXBUF & 0x00FF;

        buf[i] = (msb << 8) | lsb;
    }

    EEPROM_CS_HIGH();

    return 0;
}


Uint16 eeprom_write_words(Uint16 addr, Uint16 *buf, Uint16 words)
{
    Uint16 i;
    volatile Uint16 dummy;

    // optional: reject page crossing
    if(((addr & 0x007F) + (words * 2)) > 128)
        return 1;   // page overflow

    eeprom_write_enable();
    if(!eeprom_wait_wel())
        return 2;

    EEPROM_CS_LOW();

    SpiaRegs.SPITXBUF = 0x0200;           // WRITE
    while(SpiaRegs.SPIFFRX.bit.RXFFST == 0);
    dummy = SpiaRegs.SPIRXBUF;

    SpiaRegs.SPITXBUF = (addr & 0xFF00);  // address high
    while(SpiaRegs.SPIFFRX.bit.RXFFST == 0);
    dummy = SpiaRegs.SPIRXBUF;

    SpiaRegs.SPITXBUF = ((addr & 0x00FF) << 8); // address low
    while(SpiaRegs.SPIFFRX.bit.RXFFST == 0);
    dummy = SpiaRegs.SPIRXBUF;

    for(i = 0; i < words; i++)
    {
        SpiaRegs.SPITXBUF = (buf[i] & 0xFF00);         // data MSB
        while(SpiaRegs.SPIFFRX.bit.RXFFST == 0);
        dummy = SpiaRegs.SPIRXBUF;

        SpiaRegs.SPITXBUF = ((buf[i] & 0x00FF) << 8);  // data LSB
        while(SpiaRegs.SPIFFRX.bit.RXFFST == 0);
        dummy = SpiaRegs.SPIRXBUF;
    }

    EEPROM_CS_HIGH();   // very important: end write command here

    if(!eeprom_wait_write_complete())
        return 3;

    return 0;
}

static void eeprom_write_enable(void)
{
    volatile Uint16 dummy;

    EEPROM_CS_LOW();
    SpiaRegs.SPITXBUF = 0x0600;   // WREN
    while(SpiaRegs.SPIFFRX.bit.RXFFST == 0);
    dummy = SpiaRegs.SPIRXBUF;
    EEPROM_CS_HIGH();
}


static Uint16 eeprom_read_status(void)
{
    Uint16 rx;

    EEPROM_CS_LOW();

    SpiaRegs.SPITXBUF = 0x0500;   // RDSR
    while(SpiaRegs.SPIFFRX.bit.RXFFST == 0);
    rx = SpiaRegs.SPIRXBUF;       // dummy

    SpiaRegs.SPITXBUF = 0x0000;   // clock out status
    while(SpiaRegs.SPIFFRX.bit.RXFFST == 0);
    rx = SpiaRegs.SPIRXBUF & 0x00FF;

    EEPROM_CS_HIGH();
    return rx;
}




static Uint16 eeprom_wait_wel(void)
{
    Uint16 t = 1000;
    while(t--)
    {
        if(eeprom_read_status() & 0x02)   // WEL bit
            return 1;
    }
    return 0;
}

static Uint16 eeprom_wait_write_complete(void)
{
    Uint16 t = 20000;
    while(t--)
    {
        if((eeprom_read_status() & 0x01) == 0)   // WIP = 0
            return 1;
    }
    return 0;
}


/*
void spi_init(void)
    {
    GPIO_setPinConfig(GPIO_8_SPIA_SIMO);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_SPISIMOA, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(DEVICE_GPIO_PIN_SPISIMOA, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(GPIO_10_SPIA_SOMI);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_SPISOMIA, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(DEVICE_GPIO_PIN_SPISOMIA, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(GPIO_9_SPIA_CLK);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_SPICLKA, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(DEVICE_GPIO_PIN_SPICLKA, GPIO_QUAL_ASYNC);

        // Initialize SPI FIFO registers
    SpiaRegs.SPIFFTX.all=0xE040;
    SpiaRegs.SPIFFRX.all=0x204f;
    SpiaRegs.SPIFFCT.all=0x0;

    SpiaRegs.SPICCR.all =0x47;    // Reset on, rising edge, 8-bit char bits
    SpiaRegs.SPICTL.all =0x06;    // Enable master mode, normal phase,
    SpiaRegs.SPIBRR.all  = 0x000b; //baud rate

    SpiaRegs.SPICCR.all =0x0C7;   // Relinquish SPI from Reset // Bit 4 loop back mode disabled
    DELAY_US(100);
    EEPROM_CS_HIGH();
  }

void WriteRecordPointer(void) {

    if (recordWriteTimer == 0)
    {
        spiWriteBuffer[0] = RecordNumber;
        eeprom_write_words(EP_PACKET_NUM,spiWriteBuffer, 2);
        DELAY_US(2000);

        old_RecordNumber = RecordNumber;
    }

}
*/
N16     eeprom_write_word(Uint16 addr, N16 val)
{

    N8 dummy_rd;
  //  N16 count = 0;

    eeprom_write_enable();
    EEPROM_CS_LOW();

    SpiaRegs.SPITXBUF = EEPROM_CMD_WRITE;
    WaitForDataByte();
    dummy_rd = SpiaRegs.SPIRXBUF;

    SpiaRegs.SPITXBUF = addr & 0xff00;
    WaitForDataByte();
    dummy_rd = SpiaRegs.SPIRXBUF ;//& 0xff;

    SpiaRegs.SPITXBUF = addr << 8;
    WaitForDataByte();
    dummy_rd = SpiaRegs.SPIRXBUF ;

         SpiaRegs.SPITXBUF = val & 0xff00;
         WaitForDataByte();
         dummy_rd = SpiaRegs.SPIRXBUF;

         SpiaRegs.SPITXBUF = val << 8;
         WaitForDataByte();
         dummy_rd = SpiaRegs.SPIRXBUF;

    if(spi_timeout >= SPI_TIME)
       {
        dummy_rd = TRUE;
        spi_error = dummy_rd;
       }
    EEPROM_CS_HIGH();

    return(spi_error);
}

N16  eeprom_read_word(Uint16 addr)
    {
        N16 dummy_rd;
        N16 count = 0;
        N16 lsb, msb;

        EEPROM_CS_LOW();

        SpiaRegs.SPITXBUF = EEPROM_CMD_READ;
        WaitForDataByte();
        dummy_rd = SpiaRegs.SPIRXBUF ;//& 0xff;

        SpiaRegs.SPITXBUF = addr & 0xff00;
        WaitForDataByte();
        dummy_rd = SpiaRegs.SPIRXBUF ;//& 0xff;

        SpiaRegs.SPITXBUF = addr << 8;
        WaitForDataByte();
        dummy_rd = SpiaRegs.SPIRXBUF ;//& 0xff;

            SpiaRegs.SPITXBUF = 0x00;   //dummy write to read the result
            WaitForDataByte();
            msb = SpiaRegs.SPIRXBUF & 0xff;

            SpiaRegs.SPITXBUF = 0x00;   //dummy write to read the result
            WaitForDataByte();            lsb = SpiaRegs.SPIRXBUF & 0xff;
            count = lsb | (msb<< 8);

        if(spi_timeout >= SPI_TIME)
           {
                dummy_rd = TRUE;
                spi_error = dummy_rd;
           }

        EEPROM_CS_HIGH();
        return(count);

    }

//===========================================================================
// No more.
//===========================================================================

