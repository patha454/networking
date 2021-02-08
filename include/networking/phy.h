/**
 * \file `include/networking/phy.h`
 *
 * `phy.h` declears types and functions used to create and interface to
 * (real or simulated) OSI physical layers.
 *
 * \author H Paterson <harley.paterson@postgrad.otago.ac.nz>
 * \copyright BSL-1.0
 * \date February 2021
 */

#include <stddef.h>
#include <stdint.h>

/**
 * Interface for a function which can write to a PHY.
 *
 * \param context Implementation specific context data for the PHY.
 * \param data Buffer to write data from.
 * \param length Maximum length of data to write.
 * \return The number of bytes actually written to the PHY.
 */
typedef size_t (*_PhyWrite)(void* context, uint8_t* data, size_t length);

/**
 * Interface for a function which can read from a PHY.
 *
 * \param context Implementation specific context data for the PHY.
 * \param data Buffer to read data into.
 * \param length Maximum length of data to read.
 * \return The number of bytes actually read from the PHY.
 */
typedef size_t (*_PhyRead)(void* context, uint8_t* data, size_t length);

/** Pointer for a PHY to store implementation specific context. */
typedef void* PhyContext;

/** Handle to a PHY implementation. */
typedef struct Phy* Phy;

/**
 * Interface used to control a PHY.
 */
struct Phy
{
    /** Pointer for the PHY to store implementation specifiec data. */
    PhyContext context;

    /** Fuction for writing to the PHY. */
    _PhyWrite write;

    /** Function used for reading from the PHY. */
    _PhyRead read;
};

/**
 * Read from a PHY.
 *
 * \param phy PHY to read from.
 * \param data Location to store data read from the PHY.
 * \param length Maximum length of data to read from the PHY.
 * \return Actual length of the data read from the PHY.
 */
size_t phyRead(Phy phy, uint8_t* data, size_t length);

/**
 * Write to a PHY.
 * 
 * \param phy PHY to write to.
 * \param data Data to write to the PHY.
 */
size_t phyWrite(Phy phy, uint8_t* data, size_t length);



