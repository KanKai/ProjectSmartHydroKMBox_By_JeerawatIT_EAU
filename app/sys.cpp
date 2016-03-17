/*
 * sys.cpp
 *
 *  Created on: Jul 29, 2558 BE
 *      Author: admin
 */

#include <application.h>

uint32_t getChipId(void)
{
    return system_get_chip_id();
}

void restart()
{
    System.restart();
}




