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
#include <sys/epoll.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Maximum number of simulated clients that can be connected to a simulated
 * PHY.
 */
#define MAX_PHY_ENDPOINTS 256

struct Phy
{
    
    /** Number of client endpoints attached to the PHY. */
    unsigned int clients;

    /** `epoll` instance used for monitoring connected sockets. */
    int epoll;

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
    phy->epoll = epoll_create1(0);
    if (phy->epoll == -1) {
        perror("Creating epoll instance");
        exit(EXIT_FAILURE);
    }
}

/**
 * Connect a socket endpoint to the virtual PHY.
 *
 * \param phy The PHY to add a new connection to.
 * \return File descriptor to read and write to the PHY, or -1 on error.
 */
int phyConnect(Phy phy)
{
    struct epoll_event events = { 0 };
    events.events = EPOLLIN;
    events.data.fd = phy->clients;
    assert(phy != NULL);
    if (phy->clients == MAX_PHY_ENDPOINTS) {
        perror("Too many PHY clients");
        exit(EXIT_FAILURE);
    }
    int endpoints[2] = { 0 };
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, endpoints) == -1) {
        perror("Create PHY socketpair");
        exit(EXIT_FAILURE);
    } 
    if (epoll_ctl(phy->epoll, EPOLL_CTL_ADD, endpoints[0], &events) == -1) {
        perror("Register epoll on FD");
        exit(EXIT_FAILURE);
    }
    phy->sockets[phy->clients++] = endpoints[0];
    return endpoints[1];
}

/**
 * Propagates data over the PHY, if any input data is pending from clients.
 *
 * \param Phy The phy to propagate data over.
 */
void phyPropagate(Phy phy)
{
    return;
}

/**
 * Teardown an existing phy, closing any resources associated with it.
 *
 * \note `phyShutdown` will not release memory attached to the PHY.
 *
 * \see `phyFree`.
 *
 * \param phy The PHY to shutdown.
 */
void phyShutdown(Phy phy)
{
    unsigned int i = 0;
    close(phy->epoll);
    for (i = 0; i < phy->clients; i++) {
        close(phy->sockets[i]);
        phy->sockets[i] = 0;
    }
    phy->clients = 0;
    phy->epoll = 0;
}

/**
 * Free an existing PHY.
 *
 * \note `phyFree` will not perform any teardown work.
 *
 * \see `phyShutdown` to gracefully close the PHY before freeing.
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
    phyConfigure(phy);
    int fd = phyConnect(phy);
    assert(fd != -1);
    phyFree(phy);
    return EXIT_SUCCESS;
}