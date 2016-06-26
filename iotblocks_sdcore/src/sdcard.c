/*
 * File	: sdcard.c
 * This file has been created as part of the IoTBLocks project
 * Find out more:
 * http://www.pratikpanda.com/iotblocks/SDcore
 * Copyright (C) 2015 - 2016, Pratik Panda
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of version 3 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "spi_register.h"
#include "esp_common.h"
#include "sdcard.h"

// Bits in ESP8266 SPI_USER
#define ESP_CMD_EN    (1<<31)
#define ESP_ADDR_EN   (1<<30)
#define ESP_DUMMY_EN  (1<<29)
#define ESP_DATA_EN   (1<<27)
#define ESP_MISO_EN   (1<<28)
#define ESP_MOSI_EN   (1<<27)
#define ESP_DUPLEX_EN (1<<0)

#define GO_IDLE_STATE             0
#define ALL_SEND_CID        			1
#define SEND_CSD          				9
#define SEND_CID          				10
#define STOP_TRANSMISSION         12
#define SEND_STATUS               13
#define READ_MULTIPLE_BLOCKS      18
#define WRITE_MULTIPLE_BLOCKS     25
#define APP_CMD		     	          55

#define SEND_IF_COND		          8
#define SET_BLOCK_LEN            16
#define ERASE_BLOCK_START_ADDR   32
#define ERASE_BLOCK_END_ADDR     33
#define ERASE_SELECTED_BLOCKS    38
#define SD_SEND_OP_COND	    	   41   //ACMD41
#define READ_OCR                 58
#define CRC_ON_OFF               59

unsigned char is_SDHC;

// Initialize physical interface for SD card
void sdcard_phy_init (void)
{
  // Init HSPI GPIO
  WRITE_PERI_REG(PERIPHS_IO_MUX, 0x105);
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, 2);
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, 2);
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, 2);
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, 2);
}

// Init SPI interface to work at 500 kHz frequency, needed
// for initialization phase
void sdcard_low_speed_en (void)
{
  // Wait for last send to finish
  while (READ_PERI_REG (SPI_CMD(1)) & (1<<18));

// MSB first, always
  CLEAR_PERI_REG_MASK(SPI_CTRL(1), SPI_WR_BIT_ORDER);
  CLEAR_PERI_REG_MASK(SPI_CTRL(1), SPI_RD_BIT_ORDER);

  // Master mode, slave mode OFF
  CLEAR_PERI_REG_MASK(SPI_SLAVE(1), SPI_SLAVE_MODE);

  // Configure clock divider to 320kHz @ 80MHz CPU clock
  CLEAR_PERI_REG_MASK(SPI_CLOCK(1), SPI_CLK_EQU_SYSCLK);
  WRITE_PERI_REG(SPI_CLOCK(1), (5<<18) |(41 << 12) |(20 << 6) |(41));

  // Normal SPI bus
  WRITE_PERI_REG(SPI_CTRL2(1), (1 << 18));
  WRITE_PERI_REG(SPI_CTRL(1), 0x00000000);
}

// High speed SPI enable, needed for normal SD operation
// for testing puposes, use 2MHz, which is easy to scope
// This speed should work with pretty much any SD card
void sdcard_high_speed_en (void)
{
  // Wait for last send to finish
  while (READ_PERI_REG (SPI_CMD(1)) & (1<<18));

  // MSB first, always
    CLEAR_PERI_REG_MASK(SPI_CTRL(1), SPI_WR_BIT_ORDER);
    CLEAR_PERI_REG_MASK(SPI_CTRL(1), SPI_RD_BIT_ORDER);

    // Master mode, slave mode OFF
    CLEAR_PERI_REG_MASK(SPI_SLAVE(1), SPI_SLAVE_MODE);

    // Configure clock divider to 8MHz @ 80MHz CPU clock
    CLEAR_PERI_REG_MASK(SPI_CLOCK(1), SPI_CLK_EQU_SYSCLK);
    WRITE_PERI_REG(SPI_CLOCK(1), (9 << 12) |(4 << 6) |(9));
    // Normal SPI bus
    WRITE_PERI_REG(SPI_CTRL2(1), (1 << 18));
    WRITE_PERI_REG(SPI_CTRL(1), 0x00000000);
}

// Produces about 20MHz SPI clock and very high speed data transfer
// rates if HSPI buffer used properly. However, not all cards will
// be able to run at such high speeds.
// If this causes read/write failure, consider using high_speed
// NOTE: Use this speed with PCB designs only. Breadboards usually
// are not suitable for this speed (data corruption possible)
void sdcard_extreme_speed_en (void)
{
  // Wait for last send to finish
  while (READ_PERI_REG (SPI_CMD(1)) & (1<<18));

  // MSB first, always
    CLEAR_PERI_REG_MASK(SPI_CTRL(1), SPI_WR_BIT_ORDER);
    CLEAR_PERI_REG_MASK(SPI_CTRL(1), SPI_RD_BIT_ORDER);

    // Master mode, slave mode OFF
    CLEAR_PERI_REG_MASK(SPI_SLAVE(1), SPI_SLAVE_MODE);

    // Configure clock divider to 20MHz @ 80MHz CPU clock
    CLEAR_PERI_REG_MASK(SPI_CLOCK(1), SPI_CLK_EQU_SYSCLK);
    WRITE_PERI_REG(SPI_CLOCK(1), (3 << 12) |(1 << 6) |(3));

    // Normal SPI bus
    WRITE_PERI_REG(SPI_CTRL2(1), (1 << 18));
    WRITE_PERI_REG(SPI_CTRL(1), 0x00000000);
}

// Full duplex routine to send data to the SD catd and
// also receive data at the same time. Good alternative
// for holding MOSI line high when receiving data
unsigned char sdcard_tx (unsigned char tx)
{
  // Wait for last send to finish
  while (READ_PERI_REG (SPI_CMD(1)) & (1<<18));

  // Enable buffer transmit and full duplex mode
  WRITE_PERI_REG (SPI_USER(1), ESP_MOSI_EN|ESP_DUPLEX_EN);

  // Enable 8-bit transmit and receive
  WRITE_PERI_REG (SPI_USER1(1), (7 << 17)|(7<<8) );

  // Disable all CS except CS0
  WRITE_PERI_REG (SPI_PIN(1), 0x06);

  // Load byte to buffer
  WRITE_PERI_REG (SPI_W0(1), tx);

  // Send the data out
  WRITE_PERI_REG (SPI_CMD(1), (1<<18));

  // Finished?
  while (READ_PERI_REG (SPI_CMD(1)) & (1<<18));

  // Return received byte
  return (unsigned char)(READ_PERI_REG (SPI_W0(1)));
}

// Send command to SD card and return the response.
// In case of multi-byte response, this only returns the
// first not-0xFF byte returned by card.
unsigned char SD_sendCommand (unsigned char cmd, unsigned long arg)
{
	unsigned char response, response2, retry=0;

  // Byte address for SD and block address for SDHC
  if(is_SDHC == 0)
  if(cmd == READ_MULTIPLE_BLOCKS||cmd == WRITE_MULTIPLE_BLOCKS)
  	arg = arg << 9;

  sdcard_tx(cmd | 0x40); //send command, first two bits always '01'

  sdcard_tx(arg>>24);
  sdcard_tx(arg>>16);
  sdcard_tx(arg>>8);
  sdcard_tx(arg);

  // CRC must be correct for some CMD0 and CMD8
  if(cmd == SEND_IF_COND)    sdcard_tx(0x87);
  else                  	   sdcard_tx(0x95);

  // Wait till time out
  while((response = sdcard_tx(0xFF)) == 0xFF)
   if(retry++ == 0xFF) break;

  sdcard_tx (0xFF); // Extra clocks after every transaction

return response;
}

// initialization compatible with SDIO v 2.0 specs
signed char initialize_sd(void)
{
unsigned char i, response, SD_v = 2;
unsigned char attempts, retry;

  sdcard_phy_init();
  sdcard_low_speed_en();

  // Over 74 dummy clock cycles needed
  for(i=0;i<10;i++)
      sdcard_tx(0xFF);

  do
  {
   response = SD_sendCommand(GO_IDLE_STATE, 0);
   if(++attempts>200)
   	  return -1;
  } while(response != 1);

  sdcard_tx (0xFF);
  sdcard_tx (0xFF);
  sdcard_tx (0xFF);

  attempts = 0;
  do
  {
   if(++attempts > 200)
     {
	  SD_v = 1;
	  break;
     }
    response = SD_sendCommand(SEND_IF_COND,0x000001AA);
  } while(response != 0x01);

  // Receive the 4 byte response pattern
  sdcard_tx (0xFF);
  sdcard_tx (0xFF);
  sdcard_tx (0xFF);
  sdcard_tx (0xFF);

  // Bring card to active state now
  attempts = 0;
  do
  {
    response = SD_sendCommand(APP_CMD,0);
    response = SD_sendCommand(SD_SEND_OP_COND,0x40000000); //ACMD41
    if(++attempts > 250)  return -1;  // Times out if no response
   } while(response != 0);

   // Check whether card is SDHC
  is_SDHC = 0;
  attempts = 0;
  if (SD_v == 2)
  {
   do
   {
    sdcard_tx(READ_OCR | 0x40);

    sdcard_tx(0);
    sdcard_tx(0);
    sdcard_tx(0);
    sdcard_tx(0);
    sdcard_tx(0x95);

    retry = 0;
    while((response = sdcard_tx(0xFF)) == 0xFF)
     if(retry++ == 0xFF) break;

     if(response == 0x00)
     {
      response = sdcard_tx(0xFF) & 0x40;
      if(response == 0x40) is_SDHC = 1;
      else is_SDHC = 0;

      sdcard_tx (0xFF);
      sdcard_tx (0xFF);
      sdcard_tx (0xFF);
    	break;
     }
  sdcard_tx (0xFF);
	 retry++;
  }while(++attempts < 200);
}
   sdcard_tx (0xFF);

//SD_sendCommand(SET_BLOCK_LEN, 512); //set block size to 512; default size is 512
  sdcard_high_speed_en();
  sdcard_tx (0xFF);
  sdcard_tx (0xFF);
return 0;
}

// Read blocks to SD card. Note that block size MUST be 512
// Also, this routine is not optimized at all. It can probably
// be almost 5 times faster than right now.
unsigned char read_blocks_sd (unsigned char *buffer, unsigned int startBlock, unsigned int totalBlocks)
{
unsigned char response;
unsigned short i, retry = 0;

response = SD_sendCommand(READ_MULTIPLE_BLOCKS, startBlock);

if(response != 0x00) return response;

i=0;
while( totalBlocks )
{
  retry = 0;
  while(sdcard_tx (0xFF) != 0xFE) // startBlock token
  if(retry++ == 0xFFFF)    // Long timeout
    return 1;

  for( ; i<512; i++)
    *buffer++ = sdcard_tx (0xFF);


  sdcard_tx (0xFF); //Ignore CRC
  sdcard_tx (0xFF);

  sdcard_tx(0xFF);

  totalBlocks--;
}

SD_sendCommand(STOP_TRANSMISSION, 0); //Stop transmission
sdcard_tx(0xFF);

return 0;
}

// Write blocks to SD card. Note that block size MUST be 512
// Also, this routine is not optimized at all. It can probably
// be 5 Times faster than right now.
unsigned char write_blocks_sd(const unsigned char *buff, unsigned long startBlock, unsigned long totalBlocks)
{
unsigned char response;
unsigned int i, retry=0;

response = SD_sendCommand(WRITE_MULTIPLE_BLOCKS, startBlock);

if(response != 0x00) return response;

while( totalBlocks )
{
   i=0;

   sdcard_tx(0xFC); //Send start block token

   for(i=0; i<512; i++) // Send 512 bytes
     sdcard_tx( *buff++);

   sdcard_tx(0xFF); // Dummy CRC, CRC must be turned off!
   sdcard_tx(0xFF);

   response = sdcard_tx (0xFF);
   if( (response & 0x1F) != 0x05)
   {
      return response;
   }

   while(!sdcard_tx (0xFF))
     if(retry++ == 0xFFFF)
        return 1;

   sdcard_tx (0xFF);
   totalBlocks--;
}

sdcard_tx(0xFD); // Stop transmission

retry = 0;
while(!sdcard_tx (0xFF)) // Wait till write complete
   if(retry++ == 0xFFFF)
      return 1;

sdcard_tx(0xFF);

while(!sdcard_tx (0xFF)) // Wait till write completion
   if(retry++ == 0xFFFF) return 1;

return 0;
}
