# Daytime Client-Server (TCP)

A small TCP implementation of the **Daytime protocol** (RFC 867) in C. The server sends the current date and time to every client that connects, and the client prints it.

## Files

| File | Description |
|------|-------------|
| `daytimeserver.c` | Iterative server. Listens on TCP port 13 and handles one client at a time |
| `daytimetcpcli.c` | Client. Connects to a server, peeks at the incoming data, then prints it |

## Build

```bash
gcc daytimeserver.c -o daytimeserver
gcc daytimetcpcli.c -o daytimetcpcli
```

## Usage

### 1. Start the server

The server takes no arguments and always listens on port 13. Port 13 is privileged, so run it with `sudo`:

```bash
sudo ./daytimeserver
```

```
Daytime server listening on TCP port 13...
```

### 2. Run the client

The client needs **exactly 2 arguments**: the server address and the port (or service name).

```bash
./daytimetcpcli <hostname or IP address> <service or port#>
```

Example:

```bash
./daytimetcpcli 127.0.0.1 13
```

Output:

```
connected to 127.0.0.1
24 bytes from PEEK, 24 bytes pending
2026-10-01 14:30:05 IST
```

Running the client with any other number of arguments prints a usage message and exits.

## How it works

**Server:** creates a TCP socket, binds it to port 13 on all interfaces, and loops forever. For each client it accepts the connection, writes the local time as `YYYY-MM-DD HH:MM:SS TZ`, and closes the connection.

**Client:** resolves the address with `getaddrinfo` (IPv4 or IPv6), connects, and prints the server's IP. It then uses `recv(..., MSG_PEEK)` and `ioctl(FIONREAD)` to show how many bytes are waiting before reading and printing them. It stops when the server closes the connection.

## Notes

- Start the server before the client.
- To test on one machine, use `127.0.0.1` (or `localhost`) as the address.
- To reach the server from another machine, use the server's IP and make sure port 13 is allowed through its firewall.
- The server only listens on IPv4. Port 13 is hardcoded in `daytimeserver.c`, so to use a different port (such as 1313 to avoid `sudo`), change it there, recompile, and pass the same port to the client.

## Author

[Parikshit910](https://github.com/Parikshit910)