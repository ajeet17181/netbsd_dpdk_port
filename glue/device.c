#include <special_includes/sys/types.h>
#include <special_includes/sys/param.h>
#include <special_includes/sys/mbuf.h>
#include <special_includes/sys/malloc.h>
#include <special_includes/sys/sockio.h>
#include <lib/libkern/libkern.h>
#include <netbsd/net/if.h>
#include <netbsd/net/if_arp.h>
#include <netbsd/net/bpf.h>
#include <netbsd/net/if_ether.h>
#include <netbsd/netinet/in.h>

#define IFCAP_TXCSUM (IFCAP_CSUM_IPv4_Tx|IFCAP_CSUM_TCPv4_Tx|IFCAP_CSUM_UDPv4_Tx|IFCAP_CSUM_TCPv6_Tx|IFCAP_CSUM_UDPv6_Tx)
#define IFCAP_RXCSUM (IFCAP_CSUM_IPv4_Rx|IFCAP_CSUM_TCPv4_Rx|IFCAP_CSUM_UDPv4_Rx|IFCAP_CSUM_TCPv6_Rx|IFCAP_CSUM_UDPv6_Rx)
#define IFCAP_HWCSUM (IFCAP_TXCSUM|IFCAP_RXCSUM)
#define IFCAP_LRO 0

void configure_if_addr(struct ifnet *ifp,unsigned int ip_addr,unsigned int mask)
{
    struct ifreq ifr;
    struct sockaddr *sa = ifreq_getaddr(0,&ifr);
    struct sockaddr_in *sin = (struct sockaddr_in *)sa; 
    int rc;

    sin->sin_family = AF_INET;
    sin->sin_addr.s_addr = ip_addr;
    if((rc = in_control(NULL/*socket*/, SIOCSIFADDR, &ifr, ifp))) {
        printf("cannot configure IP Address on interface %d\n",rc);
        return;
    }
    sin->sin_addr.s_addr = ip_addr;
    if((rc = in_control(NULL/*socket*/, SIOCSIFNETMASK, &ifr, ifp))) {
        printf("cannot configure IP Address on interface %d\n",rc);
        return;
    }
}

static int dpdk_if_init(struct ifnet *ifp)
{
printf("%s %d\n",__FILE__,__LINE__);
	return 0;
}

static void dpdk_ifstop(struct ifnet *ifp, int disable)
{
}

static int dpdk_ioctl(struct ifnet *ifp, u_long cmd, void *arg)
{
	return 0;
}

static void dpdk_if_start(struct ifnet *ifp)
{
printf("%s %d\n",__FILE__,__LINE__);
}

static int dpdk_mq_start(struct ifnet *ifp, struct mbuf *m)
{
printf("%s %d\n",__FILE__,__LINE__);
	return 0;
}

static void dpdk_qflush(struct ifnet *ifp)
{
}

struct ifnet *createInterface(int instance)
{
    struct ifnet *ifp = malloc(sizeof(struct ifnet),M_NETADDR,0);
    char ifname[IFNAMSIZ];
    char macaddr[6];

    sprintf(ifname,"dpdk%d",instance);
    strlcpy(ifp->if_xname, ifname, IFNAMSIZ);
    ifp->if_mtu = ETHERMTU;
    ifp->if_baudrate = 1000000000;
    ifp->if_init = dpdk_if_init;
    ifp->if_stop = dpdk_ifstop;
//    ifp->if_softc = adapter;
    ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST;
    ifp->if_ioctl = dpdk_ioctl;
    ifp->if_start = dpdk_if_start;
#if __FreeBSD_version >= 800000
    ifp->if_transmit = dpdk_mq_start;
    ifp->if_qflush = dpdk_qflush;
#endif
//    ifp->if_snd.ifq_maxlen = adapter->num_tx_desc - 2;

    if_attach(ifp);

    ether_ifattach(ifp, macaddr);
//    ether_set_ifflags_cb(ec, ixgbe_ifflags_cb);

//	ifp->if_hdrlen = sizeof(struct ether_vlan_header);

    ifp->if_capabilities |= IFCAP_HWCSUM | IFCAP_TSOv4;
    ifp->if_capenable = 0;

    /* Don't enable LRO by default */
    ifp->if_capabilities |= IFCAP_LRO;	

    return ifp;
}