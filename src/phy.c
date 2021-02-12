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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Maximum number of simulated clients that can be connected to a simulated
 * PHY.
 */
#define MAX_PHY_ENDPOINTS 256

/**
 * Maximum number of pending epoll events to return per scan.
 */
#define MAX_EPOLL_EVENTS 16

/**
 * Size, in bytes, of a buffer for copying from sockets.
 */
#define PHY_BUFFER_SIZE 4096

struct Phy
{
    
    /** Number of client endpoints attached to the PHY. */
    unsigned int clients;

    /** `epoll` instance used for monitoring connected sockets. */
    int epoll;

    /** File descriptors for client sockets. */
    int sockets[MAX_PHY_ENDPOINTS];

    /** Pending I/O events on the sockets. */
    struct epoll_event events[MAX_EPOLL_EVENTS];
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
    events.events = EPOLLIN;
    events.data.fd = endpoints[0];
    if (epoll_ctl(phy->epoll, EPOLL_CTL_ADD, endpoints[0], &events) == -1) {
        perror("Register epoll on FD");
        exit(EXIT_FAILURE);
    }
    phy->sockets[phy->clients++] = endpoints[0];
    return endpoints[1];
}

/**
 * Helper method for `phyPropagateFromSocket`. Write the full contets of a buffer to
 * a socket, or dies trying.
 *
 * \param socketFd Socket to write to.
 * \param buffer Buffer to write.
 * \param length Length of the `buffer` to write.
 */ 
void writeToSocket(int socketFd, uint8_t* buffer, size_t length)
{
    size_t sizeWritten = 0;
    ssize_t result = 0;
    while (sizeWritten < length) {
        printf("socket %d\nsizeWritten %ld\nlength %ld\n", socketFd, sizeWritten, length);
        result = write(socketFd, buffer + sizeWritten, length - sizeWritten);
        if (result == -1) {
            perror("Write to PHY socket");
            exit(EXIT_FAILURE);
        }
        sizeWritten += result;
    }
}

/**
 * Helper method for `phyPropagate`. Echos all output from one socket into all
 * other sockets.
 *
 * \param phy The phy to propagate over.
 * \param socketFd The socket to propagate from.
 */
void phyPropagateFromSocket(Phy phy, int socketFd)
{
    unsigned int i = 0;
    ssize_t sizeRead = 0;
    uint8_t* buffer = malloc(PHY_BUFFER_SIZE);
    if (buffer == NULL) {
        perror("Allocate propagation buffer.");
        exit(EXIT_FAILURE);
    }
    while (true) {
        sizeRead = read(socketFd, buffer, PHY_BUFFER_SIZE);
        if (sizeRead == 0) {
            break;
        }
        if (sizeRead == -1) {
            perror("Read from PHY socket");
            exit(EXIT_FAILURE);
        }
        printf("sizeRead: %ld\n", sizeRead);
        for (i = 0; i < phy->clients; i++) {
            if (phy->sockets[i] == socketFd) {
                continue;
            }
            writeToSocket(phy->sockets[i], buffer, sizeRead);
        }
    }
}

/**
 * Propagates data over the PHY, if any input data is pending from clients.
 *
 * \param Phy The phy to propagate data over.
 */
void phyPropagate(Phy phy)
{
    int pendingEvents = 0;
    int i = 0;
    pendingEvents = epoll_wait(phy->epoll, phy->events, MAX_EPOLL_EVENTS, 0);
    for (i = 0; i < pendingEvents; i++) {
        phyPropagateFromSocket(phy, phy->events[i].data.fd);
    }
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
    int fd1 = phyConnect(phy);
    int fd2 = phyConnect(phy);
    assert(fd1 != -1);
    assert(fd2 != -1);
    write(fd1, "Hello, network", 15);
    printf("Write to fd %d\n", fd1);
    phyPropagate(phy);
    phyFree(phy);
    return EXIT_SUCCESS;
}