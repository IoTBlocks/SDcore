/*
 * File	: sdcard.h
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

#ifndef _SD_CARD_H_
  #define _SD_CARD_H_

// Please do not directly access the HSPI module with user code
// That may cause problems in RTOS based applications and
// may result in card corruption.
signed char initialize_sd(void);
unsigned char write_blocks_sd(const unsigned char *buff, unsigned long startBlock, unsigned long totalBlocks);
unsigned char read_blocks_sd (unsigned char *buffer, unsigned int startBlock, unsigned int totalBlocks);

#endif
