/**
 * \file `src/phy.c`
 * 
 * `phy.c` defines functions for interacting with a generic PHY interface.
 *
 * \see `include/networking/phy.h` for detailed API specifications and
 * documentation comments.
 *
 * \author H Paterson <harley.paterson@postgrad.otago.ac.nz>
 * \copyright BSL-1.0
 * \date February 2021.
 */

#include <assert.h>
#include "networking/phy.h"

size_t phyRead(Phy phy, uint8_t* data, size_t length)
{
    assert(phy != NULL);
    assert(data != NULL);
    assert(length > 0);
    return phy->read(phy->context, data, length);
}

size_t phyWrite(Phy phy, uint8_t* data, size_t length)
{
    assert(phy != NULL);
    assert(data != NULL);
    assert(length > 0);
    return phy->write(phy->context, data, length);
}
