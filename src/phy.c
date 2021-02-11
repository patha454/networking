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
#include <sys/socket.h>
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

    /** File descriptors for client sockets. */
    int sockets[MAX_PHY_ENDPOINTS];
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
    return phy;
}

/**
 * Configure a newly allocated PHY for use.
 *
 * \param phy The Phy to configure.
 */
void phyConfigure(Phy phy)
{
    
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

/**
 * Connect a socket endpoint to the virtual PHY.
 *
 * \param phy The PHY to add a new connection to.
 * \return File descriptor to read and write to the PHY, or -1 on error.
 */
int phyConnect(Phy phy)
{
    assert(phy != NULL);
    if (phy->clients == MAX_PHY_ENDPOINTS) {
        return -1;
    }
    int endpoints[2] = { 0 };
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, endpoints) == -1) {
        return -1;
    } 
    phy->sockets[phy->clients++] = endpoints[0];
    return endpoints[1];
}

int main(void)
{
    Phy phy = phyAlloc();
    assert(phy != NULL);
    int fd = phyConnect(phy);
    assert(fd != -1);
    phyFree(phy);
    return EXIT_SUCCESS;
}