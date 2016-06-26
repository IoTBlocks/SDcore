#!/bin/bash

export SDK_PATH=/home/esp8266/Desktop/ESP8266_RTOS_SDK
export BIN_PATH=LATEST_BIN

echo "Custom gen_misc.sh version 20150911"
echo "==================================="

    echo "SDK_PATH:"
    echo "$SDK_PATH"
    echo ""

    echo "BIN_PATH:"
    echo "$BIN_PATH"
    echo ""

echo "Please check SDK_PATH & BIN_PATH, enter (Q) to quit:"
read input
if [[ $input == q ]] && [[ $input == Q ]]; then
    exit
fi
echo ""

boot=none
echo "boot mode: $boot"
echo ""
app=0
echo "Generate eagle.flas.bin + eagle.irom0text.bin"
echo "App: $app"
echo ""


    spi_speed=40
echo "spi speed: $spi_speed MHz"
echo ""

    spi_mode=QIO
echo "spi mode: $spi_mode"
echo ""


    spi_size_map=4
    echo "spi size: 4096KB"
echo ""

echo "start..."
echo ""

make clean

make BOOT=$boot APP=$app SPI_SPEED=$spi_speed SPI_MODE=$spi_mode SPI_SIZE_MAP=$spi_size_map
