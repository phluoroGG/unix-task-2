/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2022.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 60-2 */

/* id_echo_sv.c

   This program implements a daemon that provides the UDP "echo" service. It
   reads datagrams and then sends copies back to the originating address.

   NOTE: this program must be run under a root login, in order to allow the
   "echo" port (7) to be bound. Alternatively, for test purposes, you can edit
   id_echo.h and replace the SERVICE name with a suitable unreserved port
   number (e.g., "51000"), and make a corresponding change in the client.

   See also id_echo_cl.c.
*/
#include <syslog.h>
#include "id_echo.h"
#include "become_daemon.c"

int
main(int argc, char *argv[])
{
    int sfd;
    ssize_t numRead;
    socklen_t len;
    struct sockaddr_storage claddr;
    char buf[BUF_SIZE];
    char addrStr[IS_ADDR_STR_LEN];

    if (becomeDaemon(0) == -1) {
    	printf("Error in function becomeDaemon\n");
    	exit(EXIT_FAILURE);
    }

    sfd = inetBind(SERVICE, SOCK_DGRAM, NULL);
    if (sfd == -1)
        exit(EXIT_FAILURE);

    /* Receive datagrams and return copies to senders */

    for (;;) {
        len = sizeof(struct sockaddr_storage);
        numRead = recvfrom(sfd, buf, BUF_SIZE, 0,
                           (struct sockaddr *) &claddr, &len);
        if (numRead == -1)
            exit(EXIT_FAILURE);

        sendto(sfd, buf, numRead, 0, (struct sockaddr *) &claddr, len);
    }
}
