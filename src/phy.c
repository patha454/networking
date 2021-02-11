/**
 * \file include\networking\phy.h
 *
 * `phy.h` defines an interface to a simulated physical layer.
 *
 * \author H Paterson <harley.paterson@postgrad.otago.ac.nz>
 * \copyright BSL-1.0
 * \date February 2021
 */

#include <assert.h>
#include <stdlib.h>

/**
 * Maximum number of simulated clients that can be connected to a simulated
 * PHY.
 */
#define MAX_PHY_ENDPOINTS 256

struct Phy
{
    
    /** Number of client endpoints attached to the PHY. */
    uint clients;
};

/** Handle to a PHY simulator. */
typedef struct Phy* Phy;

/**
 * Allocate a new PHY.
 *
 * \return Handle to the new PHY, or NULL on failure.
 */
Phy phyAlloc()
{
    Phy phy = calloc(1, sizeof(*phy));
    if (phy == NULL) {
        return NULL;
    }
}

/**
 * Free an existing PHY.
 *
 * \note `phyFree` will not perform any teardown work.
 *
 * \param phy The PHY to destroy.
 */
void phyFree(Phy phy)
{
    free(phy);
}

int main(void)
{
    Phy phy = phyAlloc();
    assert(phy != NULL);
    phyFree(phy);
    return EXIT_SUCCESS;
}