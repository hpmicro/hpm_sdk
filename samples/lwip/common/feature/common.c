#include "common.h"
#include "hpm_enet_phy_common.h"
#include "ethernetif.h"
#include "lwip.h"
#include "lwip/timeouts.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"

#if RGMII == 1
    #if defined __USE_DP83867
        #include "hpm_dp83867.h"
        #include "hpm_dp83867_regs.h"
    #elif defined __USE_RTL8211
        #include "hpm_rtl8211.h"
        #include "hpm_rtl8211_regs.h"
    #endif
#else
    #if defined __USE_DP83848
        #include "hpm_dp83848.h"
        #include "hpm_dp83848_regs.h"
    #elif defined  __USE_RTL8201
        #include "hpm_rtl8201.h"
        #include "hpm_rtl8201_regs.h"
    #else
        #error no specified Ethernet PHY !!!
    #endif
#endif

static enet_phy_status_t last_status;


#if LWIP_DHCP
static void dhcp_state(struct netif *netif)
{
    static uint8_t dhcp_last_state = DHCP_STATE_OFF;
    struct dhcp *dhcp = netif_dhcp_data(netif);

    if (enet_get_link_status() == false) {
        return;
    }

    if (dhcp == NULL) {
        dhcp_last_state = DHCP_STATE_OFF;
    } else if (dhcp_last_state != dhcp->state) {
        dhcp_last_state = dhcp->state;

        printf("DHCP state       : ");
        switch (dhcp_last_state) {
        case DHCP_STATE_OFF:
            printf("OFF");
            break;
        case DHCP_STATE_REQUESTING:
            printf("REQUESTING");
            break;
        case DHCP_STATE_INIT:
            printf("INIT");
            break;
        case DHCP_STATE_REBOOTING:
            printf("REBOOTING");
            break;
        case DHCP_STATE_REBINDING:
            printf("REBINDING");
            break;
        case DHCP_STATE_RENEWING:
            printf("RENEWING");
            break;
        case DHCP_STATE_SELECTING:
            printf("SELECTING");
            break;
        case DHCP_STATE_INFORMING:
            printf("INFORMING");
            break;
        case DHCP_STATE_CHECKING:
            printf("CHECKING");
            break;
        case DHCP_STATE_BOUND:
            printf("BOUND");
            break;
        case DHCP_STATE_BACKING_OFF:
            printf("BACKING_OFF");
            break;
        default:
            printf("%u", dhcp_last_state);
            assert(0);
            break;
        }
        printf("\r\n");

        if (dhcp_last_state == DHCP_STATE_BOUND) {
            printf("IPv4 Address     : %s\n", ipaddr_ntoa(&netif->ip_addr));
            printf("IPv4 Subnet mask : %s\n", ipaddr_ntoa(&netif->netmask));
            printf("IPv4 Gateway     : %s\n\n", ipaddr_ntoa(&netif->gw));
        }
    }
}
#endif

bool enet_get_link_status(void)
{
    return last_status.enet_phy_link;
}

void enet_self_adaptive_port_speed(void)
{
    enet_phy_status_t status;

    enet_line_speed_t line_speed[] = {enet_line_speed_10mbps, enet_line_speed_100mbps, enet_line_speed_1000mbps};
    char *speed_str[] = {"10Mbps", "100Mbps", "1000Mbps"};
    char *duplex_str[] = {"Half duplex", "Full duplex"};

#if RGMII == 1
    #if __USE_DP83867
        dp83867_get_phy_status(ENET, &status);
    #else
        rtl8211_get_phy_status(ENET, &status);
    #endif
#else
    #if __USE_DP83848
        dp83848_get_phy_status(ENET, &status);
    #else
        rtl8201_get_phy_status(ENET, &status);
    #endif
#endif

    if (memcmp(&last_status, &status, sizeof(enet_phy_status_t)) != 0) {
        memcpy(&last_status, &status, sizeof(enet_phy_status_t));
        if (status.enet_phy_link) {
            printf("Link Status: Up\n");
            printf("Link Speed:  %s\n", speed_str[status.enet_phy_speed]);
            printf("Link Duplex: %s\n", duplex_str[status.enet_phy_duplex]);
            enet_set_line_speed(ENET, line_speed[status.enet_phy_speed]);
            enet_set_duplex_mode(ENET, status.enet_phy_duplex);
        } else {
            printf("Link Status: Down\n");
        }
    }
}

void enet_services(struct netif *netif)
{
    #if LWIP_DHCP
    dhcp_start(netif);
    #endif
}

void enet_common_handler(struct netif *netif)
{
    ethernetif_input(netif);

    /* Handle all system timeouts for all core protocols */
    #if LWIP_TIMERS
    sys_check_timeouts();
    #endif

    /* show DHCP progress */
    #if LWIP_DHCP
    dhcp_state(netif);
    #endif
}