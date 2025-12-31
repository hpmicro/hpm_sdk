# Network Split with Host Deep Sleep Example

This example shows how to use two powerful ESP-Hosted features together: **Network Split** and **Host Deep Sleep**. These features allow a host MCU to save power by sleeping while the slave device keeps the network connection alive and only wakes the host when necessary.

## What This Example Does

**Network Split** - Smart traffic routing:
- Host handles important traffic (ports 49152-61439)  
- Slave handles background traffic (ports 61440-65535)
- No unnecessary host wake-ups


**Host Deep Sleep** - Ultra-low power mode:
- Host can sleep while slave stays connected to WiFi
- Slave wakes host only for important traffic
- Wake-up can also be triggered by commands or MQTT messages

## Supported Platforms and Transports

### Supported Co-processors

| Co-Processors Supported | ESP32-C5 | ESP32-C6/C61 | ESP32 | ESP32-S2 | ESP32-S3 |
| :---------------------- | :------- | :----------- | :---- | :------- | :------- |

### Supported Host MCUs

| Hosts Supported | ESP32-P4 | ESP32-H2 | Any other MCU hosts |
| :-------------- | :------- | :------- | :------------------ |

### Supported Transport Interfaces

| Transport Interface | SDIO | SPI Full-Duplex | SPI Half-Duplex | UART |
| :------------------ | :--- | :-------------- | :-------------- | :--- |

## Example Hardware Used

This example is designed for the **ESP32-P4-Function-EV-Board** with its built-in ESP32-C6.

**Default setup:**
- **Host**: ESP32-P4 (handles important network traffic)
- **Slave**: ESP32-C6 (maintains WiFi and handles background traffic)  
- **Connection**: SDIO (pre-wired on the board)
- **Wake-up wire**: Already connected between GPIO2 (slave) and GPIO6 (host)

All listed items in default setup are customizable to suit specific use cases and requirements.


## Flash the Slave (ESP32-C6)

The slave needs ESP-Hosted firmware with Network Split and Host Power Save enabled.

With below additional configuration, Flash the slave using setup instructions, see: [Slave Example Guide](../../slave/README.md)


#### Slave Side Configuration:

Using `idf.py menuconfig`, Enable:

```
# Minimal slave config
Example Configuration
├── [*] Enable Network Split
└── [*] Allow host to power save
```

**Advanced slave settings** (optional):
```
Example Configuration
└── Network Split Configuration
    ├── Host Static Port Forwarding
    │   ├── TCP dst: 22,80,443,8080,8554
    │   └── UDP dst: 53,123
    ├── Port Ranges
    │   ├── Host: 49152–61439
    │   └── Slave: 61440–65535
    └── Host power save config
        ├── Allow host to enter deep sleep
        ├── Slave out: Host wakeup GPIO (2)
        └── Host Wakeup GPIO Level (High)
```

Continue to build and flash slave using [Slave Example Guide](../../slave/README.md)


## Flash the Host (ESP32-P4)

The host needs ESP-Hosted firmware with Network Split and Host Power Save configured.

**Essential host configuration:**
1. Set target: `idf.py set-target esp32p4`

2. Network split and Host power save is pre-configured in this example.
(Optionally) customise the configuration using `idf.py menuconfig`
```
Component config
└── ESP-Hosted config
    ├── [*] Enable Network Split
    │    └── Network Split Configuration
    │        └── Port Ranges
    │            ├── Host: 49152–61439
    │            └── Slave: 61440–65535
    └── [*] Enable Host Power Save
         └── Host Power Save Configuration
            [*] Allow host to enter deep sleep.
             └── Deep Sleep Configuration
                 ├── Host Wakeup GPIO (6)
                 └── Host wakeup GPIO active low
```

3. Build and flash: `idf.py build && idf.py -p <HOST_PORT> flash monitor`

## Testing the Example

### Basic Power Save Test
1. Wait for both devices to connect to WiFi
2. Connect to AP using command, `sta_connect <SSID> <Password>`
3. On host console: type `host-power-save` 
4. Host goes to sleep, slave stays connected
5. On slave console: type `wake-up-host` to wake the host

### Smart Wake-up Test
1. Put host to sleep using `host-power-save`
2. Send dummy TCP/UDP packet to the device IP in host ports (49152–61439)  → host wakes automatically
3. Send dummy TCP/UDP packet to the device IP in slave ports (61440-65535) → slave handles without waking host

##### Sample program to test Network based `Smart Wake-up Test`

<summary>C program to send UDP packet:</br></br>send_udp_pkt.c</summary>
<details>

```c
// SPDX-License-Identifier: Apache-2.0
// Copyright 2015-2025 Espressif Systems (Shanghai) PTE LTD

/* This is just sample program to send sample udp packet */

/*
====================================================================
                 send_udp_pkt.c
====================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP> <PORT>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);
    const char *msg = "Hello, UDP!";

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    // Non-blocking mode
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
    };
    inet_pton(AF_INET, ip, &addr.sin_addr);

    ssize_t sent = sendto(sock, msg, strlen(msg), 0,
                         (struct sockaddr *)&addr, sizeof(addr));

    if (sent < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            fprintf(stderr, "sendto would block, try again later\n");
        } else {
            perror("sendto");
        }
    } else {
        printf("Sent %zd bytes to %s:%d\n", sent, ip, port);
    }

    close(sock);
    return EXIT_SUCCESS;
}
```
</details>


<summary>Shell script to trigger wake-up: </br></br>send_udp_pkt.sh</summary>
<details>

```bash
#!/bin/bash

# Simple script to build and run send_udp_pkt.c

#====================================================================
#                 send_udp_pkt.sh
#====================================================================
# Check if IP address is provided
if [ $# -eq 0 ]; then
    echo "❌ Error: IP address is required!"
    echo "Usage: $0 <ip_address> [port]"
    echo "Example: $0 192.168.1.100 60000"
    exit 1
fi

IP_ADDRESS="$1"
DEFAULT_PORT="60000"

# Check if port is provided as second argument
if [ $# -ge 2 ]; then
    PORT="$2"
else
    PORT="$DEFAULT_PORT"
    echo "No port specified, using default port: $DEFAULT_PORT"
fi

echo "===== Network Split Host Wakeup with UDP Packet Tool ====="
echo "Target: $IP_ADDRESS:$PORT"
echo "Building and running network wakeup utility..."

# Compile the program
gcc -o send_udp_pkt send_udp_pkt.c -pthread

# Check if compilation succeeded
if [ $? -ne 0 ]; then
    echo "❌ Compilation failed!"
    exit 1
fi

echo "✅ Build successful!"
echo "Starting network wakeup utility..."
echo "-------------------------------------------"

# Run the program with IP address and port
./send_udp_pkt $IP_ADDRESS $PORT

# Capture result
RESULT=$?
if [ $RESULT -eq 0 ]; then
    echo "-------------------------------------------"
    echo "✅ Network wakeup packet sent successfully to $IP_ADDRESS:$PORT!"
else
    echo "-------------------------------------------"
    echo "❌ Network wakeup failed with code: $RESULT"
fi

exit $RESULT
```
</details>

Create both files in some temporary directory and run:
```bash
bash send_udp_pkt.sh <device_ip_address> 62000 # Send packet to slave port             --> ❌ No host wake up

bash send_udp_pkt.sh <device_ip_address> 123   # Send packet to host reserved udp port ==> ✅ host wake up
```

> [!TIP]
>
> You can design your packet routing & filtering as per application application use-case and deploy in slave firmware.
> Refer to the https://github.com/espressif/esp-hosted-mcu/blob/663d6631af6e7a6735755e2247ab60363fda87c8/slave/main/lwip_filter.c#L349

### Performance Test
1. Run iPerf server: `iperf -s -p 5001`
2. From another device: `iperf -c <device_ip> -p 5001`
3. Watch logs to see which device handles the traffic
For more details on iperf tests, refer [iperf test](README_iperf.md)

## Customizing for Other Hardware

#### Different Transport (e.g., SPI instead of SDIO)

**Slave side:**
```
Example Configuration
└── Bus Config in between Host and Co-processor
    └── Transport layer
        └── (X) SPI Full-duplex
```

**Host side:**
```
Component config
└── ESP-Hosted config
    └── Transport layer
        └── (X) SPI Full-duplex
```

Configure the specific GPIO pins for the chosen transport bus and Host wake up GPIO pin

#### Different MCU Combination
1. Change `idf.py set-target` to your MCU
2. Update GPIO pins in menuconfig to match your wiring
3. Make sure wake-up GPIO on host supports deep sleep wake-up

## Troubleshooting

**Host won't wake up:**
- Check GPIO wire connection between slave GPIO2 and host GPIO6
- Verify host GPIO6 supports RTC wake-up
- Check GPIO level settings match on both sides

**WiFi connection fails:**
- Double-check WiFi credentials in both devices
- Make sure both devices use the same WiFi settings

**Traffic not splitting correctly:**
- Check port range settings in slave menuconfig
- Monitor logs to see packet routing decisions
- Verify Network Split is enabled on slave

## References

- [Network Split Documentation](../../docs/feature_network_split.md)
- [Host Power Save Documentation](../../docs/feature_host_power_save.md)
- [ESP32-P4-Function-EV-Board Guide](../../docs/esp32_p4_function_ev_board.md)
- [Slave Example Guide](../../slave/README.md)
- [Main ESP-Hosted Documentation](../../README.md)


