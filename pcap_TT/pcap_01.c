#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>


#define PCAP_CNT_MAX 0
#define PCAP_SNAPSHOT 1024
#define PCAP_TIMEOUT 100

       void packet_view(unsigned char *, const struct pcap_pkthdr *, const unsigned char *);

       int main(int argc, char *argv[]) {
               char *dev;
               char errbuf[PCAP_ERRBUF_SIZE];
               bpf_u_int32 net;
               bpf_u_int32 netmask;
               struct in_addr net_addr, mask_addr;
               pcap_t *pd;
               struct bpf_program fcode;

               if(!(dev = pcap_lookupdev(errbuf))) {
                       perror(errbuf);
                       exit(1);
               }

               if(pcap_lookupnet(dev, &net, &netmask, errbuf) < 0) {
                       perror(errbuf);
                       exit(1);
               }

               net_addr.s_addr = net;
               mask_addr.s_addr = netmask;

               printf("Device : %s\n", dev);
               printf("Net Address : %s\n", inet_ntoa(net_addr));
               printf("Netmask : %s\n", inet_ntoa(mask_addr));
               printf("========================================================================\n");

               if((pd = pcap_open_live(dev, PCAP_SNAPSHOT, 1, PCAP_TIMEOUT, errbuf)) == NULL) {
                       perror(errbuf);
                       exit(1);
               }

               if(pcap_loop(pd, PCAP_CNT_MAX, packet_view, 0) < 0) {
                       perror(pcap_geterr(pd));
                       exit(1);
               }

               pcap_close(pd);

               return 1;
       }

       void packet_view(unsigned char *user, const struct pcap_pkthdr *h, const unsigned char *p) {
               int len = 0;
               struct ip *iph;
               struct ether_header *etherh;
               unsigned short e_type;
               struct tcphdr *tcph;

               etherh = (struct ether_header *)p;

               //MAC 받아오기
               printf("MAC \n");
               printf("SrcMac : ");
               for (int i = 0; i < 6; i++) {
                   if(i == 5) {
                       printf("%02X\n",etherh->ether_shost[i]);
                   }else {
                       printf("%02X:",etherh->ether_shost[i]);
                   }
               }

               printf("DstMac : ");
               for (int i = 0; i < 6; i++) {
                   if(i == 5) {
                       printf("%02X\n",etherh->ether_dhost[i]);
                   }else {
                       printf("%02X:",etherh->ether_dhost[i]);
                   }
               }

               e_type = ntohs(etherh->ether_type);

               //IP 받아오기
               if( e_type == ETHERTYPE_IP ) {
                   printf("IP \n");
                   p += sizeof(struct ether_header);
                   iph = (struct ip *) p;
                   printf("Src Address : %s\n", inet_ntoa(iph->ip_src));
                   printf("Dst Address : %s\n", inet_ntoa(iph->ip_dst));
               }else if( e_type == ETHERTYPE_ARP ) {
                   printf("arp \n");
               }else if( e_type == ETHERTYPE_REVARP ) {
                   printf("rarp \n");
               }else {
                   printf("no such type -> type : %d\n", e_type);
               }

               //PORT 받아오기
               printf("PORT \n");
               if (iph->ip_p == IPPROTO_TCP)
                       {
                           tcph = (struct tcphdr *)(p + iph->ip_hl * 4);
                           printf("Src Port : %d\n" , ntohs(tcph->source));
                           printf("Dst Port : %d\n" , ntohs(tcph->dest));
                       }

               //Packet 받아오기
               printf("PACKET\n");
               while(len < h->len) {
                       printf("%02x ", *(p++));
                       if(!(++len % 16))
                               printf("\n");
               }
               printf("\n========================================================================\n");
               return ;
       }
