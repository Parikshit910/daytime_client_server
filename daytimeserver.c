#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//THIS IS ITERATIVE SERVER since it iterates through each client every time
int main(void)
{
    int listenfd;
    int connfd;

    struct sockaddr_in servaddr;

    char timebuf[128];

    time_t now;
    struct tm *tm_info;


    listenfd = socket(AF_INET, SOCK_STREAM, 0);
/*
struct sockaddr_in {        // Total: 16 bytes
    sa_family_t    sin_family; // 2 bytes (AF_INET)
    in_port_t      sin_port;   // 2 bytes
    struct in_addr sin_addr;   // 4 bytes (32-bit IPv4 address)
    unsigned char  sin_zero[8]; // 8 bytes padding
};
the only correct argument for sin_family is AF_INET, its not a general struct that you can use on different addrtesses
for say IPv6 you will need

struct sockaddr_in6 {       // Total: 28 bytes
    sa_family_t     sin6_family;   // 2 bytes (AF_INET6)
    in_port_t       sin6_port;     // 2 bytes
    uint32_t        sin6_flowinfo; // 4 bytes
    struct in6_addr sin6_addr;     // 16 bytes (128-bit IPv6 address)
    uint32_t        sin6_scope_id; // 4 bytes
};
*/
    if (listenfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /*
     * 2. Fill in the server address structure.
     */
    memset(&servaddr, 0, sizeof(servaddr));
// we zero the structure before filling the fields we care about
    servaddr.sin_family = AF_INET;//<- again IPv4 address
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//so values like INADDR_ANY or digits like 13 are stored in little endian format
    servaddr.sin_port = htons(13);//but Network BYTE are alway stored according to Big endian format so this function like htonl ,  htons,  ntohs, ntohl will  convert the arguments first into big endian format and thenn store it
//TCP port 13 is the standard port for the Daytime Protocol (defined in RFC 867).
    /*
     * 3. Bind the socket to port 13.
     */
/*So now we have a socket called 'listenfd' and we have struct called servaddr which we have filled by all the
necessary info that the socket needs like address and all of the  description now we call the function
'bind()' and pass both the socket and the address desciption, i.e., the start ptr of address desciption and the total 
size so the function  knows how far it needs to see from the starting point*/
    if (bind(listenfd,
             (struct sockaddr *)&servaddr,
             sizeof(servaddr)) < 0) {
        perror("bind");
        close(listenfd);
        exit(EXIT_FAILURE);
    }

    /*
     * 4. Tell the kernel that this socket
     *    should accept incoming connections.
     */
    if (listen(listenfd, 10) < 0) {
        perror("listen");
        close(listenfd);
        exit(EXIT_FAILURE);
    }

    printf("Daytime server listening on TCP port 13...\n");

    /*
     * 5. Keep accepting clients forever.
     */
    for (;;) {

        /*
         * 6. Accept one incoming TCP connection.
         */
        connfd = accept(listenfd, NULL, NULL);

        if (connfd < 0) {
            perror("accept");
            continue;
        }

        /*
         * 7. Get the current time.
         */
        now = time(NULL);
        tm_info = localtime(&now);

        /*
         * 8. Convert the time into a human-readable string.
         */
        strftime(timebuf,
                 sizeof(timebuf),
                 "%Y-%m-%d %H:%M:%S %Z\n",
                 tm_info);

        /*
         * 9. Send the time to the connected client.
         */
        write(connfd, timebuf, strlen(timebuf));

        /*
         * 10. Close this client's connection.
         */
        close(connfd);
    }

    /*
     * We never reach here because of the infinite loop.
     */
    close(listenfd);

    return 0;
}