/*
 * File	: user_main.c
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

#include "esp_common.h"
#include "ff.h"

unsigned char buf[1024];

void user_rf_pre_init(void)
{
}

void card_test (void* pvParameters)
{
  unsigned char res, valid_file;
  unsigned int count, i;
  FATFS fs;
  FIL f;

  for (i = 0; i<1024; i++)
    buf[i] = '0';

  printf ("\n\nSystem boot: OK");
    res = f_mount (&fs, "0:", 1);								// Mount disk
  		if(res != FR_OK)
  			printf ("\nMount error");
      else
        printf ("\nMount FS: OK");

      res = f_open (&f, "small.txt", FA_READ);					// Re-open file
    		if (res != FR_OK)
    			printf ("\nRead error: %d", res);
    		else
          printf ("\nRead file: OK");

while (1);
}


void user_init(void)
{
    printf("SDK version:%s\n", system_get_sdk_version());
    printf("\nChip ID: %d\n", system_get_chip_id());
    printf("\nFree heap: %d\n", system_get_free_heap_size());
    printf("\nIoTBlocks SDcore test\n\n");

    vTaskDelay (1000/portTICK_RATE_MS);

    xTaskCreate(card_test, (signed char *)"SDTask", 512, NULL, 5, NULL);

    vTaskDelay (100/portTICK_RATE_MS);
		printf("main() is over!\n");
}
