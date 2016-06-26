This files in this folder and the subfolders have been created
or modified as part of the IoTBLocks project.
  Find out more:
  http://www.pratikpanda.com/iotblocks/SDcore
  Copyright (C) 2015 - 2016, Pratik Panda
________________________________________________________________________

Change Log:

 June 26, 2016: Initial release
                - Tested to read FAT32 formatted SD/SDHC cards reliably
                - Very basic functions, no optimization
                - Thread safety not implemented
________________________________________________________________________

Current issues:
- Some cards do not work with CS connected to ESP8266. Tie CS to ground.
- 2 different threads must not call file read/write functions together
- Pull-up needed on MISO for some cards to operate
_______________________________________________________________________
LICENSE for the software:

This program is free software: you can redistribute it and/or modify
it under the terms of version 3 of the GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program.  If not, see <http://www.gnu.org/licenses/>.
________________________________________________________________________
