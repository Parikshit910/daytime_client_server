#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAXLINE 4096

static void err_sys(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char **argv)
{
    int sockfd, n, npend;
    char recvline[MAXLINE + 1];
    socklen_t len;
    struct sockaddr_storage ss;
    struct addrinfo hints, *res, *rp;
    char host[NI_MAXHOST];
    int rc;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <hostname or IPaddress> <service or port#>\n", argv[0]);
        exit(1);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;      /* IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;    /* TCP */

    if ((rc = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
        exit(1);
    }

    sockfd = -1;
    for (rp = res; rp != NULL; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd < 0)
            continue;
        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;                      /* success */
        close(sockfd);
        sockfd = -1;
    }
    freeaddrinfo(res);

    if (sockfd < 0)
        err_sys("tcp_connect error");
    

    len = sizeof(ss);
    if (getpeername(sockfd, (struct sockaddr *)&ss, &len) < 0)
        err_sys("getpeername error");


    rc = getnameinfo((struct sockaddr *)&ss, len, host, sizeof(host),
                     NULL, 0, NI_NUMERICHOST);
    if (rc != 0) {
        fprintf(stderr, "getnameinfo error: %s\n", gai_strerror(rc));
        exit(1);
    }
    printf("connected to %s\n", host);

    for ( ; ; ) {
        if ((n = recv(sockfd, recvline, MAXLINE, MSG_PEEK)) < 0)
            err_sys("recv error");
        if (n == 0)
            break;                      /* server closed connection */

        if (ioctl(sockfd, FIONREAD, &npend) < 0)   /* check FIONREAD support */
            err_sys("ioctl error");
        printf("%d bytes from PEEK, %d bytes pending\n", n, npend);

        if ((n = read(sockfd, recvline, MAXLINE)) < 0)
            err_sys("read error");
        recvline[n] = 0;                
        fputs(recvline, stdout);
    }

    close(sockfd);
    exit(0);
}