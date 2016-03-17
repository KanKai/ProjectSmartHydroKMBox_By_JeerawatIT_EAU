## Local build configuration
## Parameters configured here will override default and ENV values.
## Uncomment and change examples:

#Add your source directories here separated by space
MODULES = app

#SPIFFY = /Users/admin/tmp/spiffy/build/spiffy
#SPIFFY = /opt/Sming/Sming/spiffy/spiffy

## ESP_HOME sets the path where ESP tools and SDK are located.
## Windows:
# ESP_HOME = c:/Espressif

## MacOS / Linux:
#ESP_HOME = /opt/esp-open-sdk

## SMING_HOME sets the path where Sming framework is located.
## Windows:
# SMING_HOME = c:/tools/sming/Sming 

# MacOS / Linux
# SMING_HOME = /opt/sming/Sming

## COM port parameter is reqruied to flash firmware correctly.
## Windows: 
COM_PORT = COM3

# MacOS / Linux:
#COM_PORT = /dev/tty.usbserial-A50285BI
#COM_PORT = /dev/tty.SLAB_USBtoUART
# Com port speed
COM_SPEED = 230400
#COM_SPEED	= 115200
#COM_SPEED	= 921600

#DISABLE_SPIFFS = 1
# SPIFFs Location
SPIFF_FILES = web/build
SPI_SIZE = 1M 
SPI_MODE = dio