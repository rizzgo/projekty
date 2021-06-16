
/*
    Hlavičkový súbor projektu IPK - Sniffer paketov
    Richard Seipel - xseipe00
    25.4.2021  
 */

#include <iostream>
#include <cstring>
#include <getopt.h>
#include <pcap.h>
#include <ctype.h>
#include <iomanip>
#include <arpa/inet.h>
#include <netinet/ether.h>

#define ETHR_STRING_LEN 18

typedef struct {
    char *interface = NULL;
    int port = -1;
    bool tcp = false;
    bool udp = false;
    bool arp = false;
    bool icmp = false;
    int num = 1;
} arguments_t;

typedef struct {
    char source_mac[ETHR_STRING_LEN];
    char destination_mac[ETHR_STRING_LEN];
    char source[INET_ADDRSTRLEN] = "";
    char destination[INET_ADDRSTRLEN] = "";
    char source6[INET6_ADDRSTRLEN] = "";
    char destination6[INET6_ADDRSTRLEN] = "";
    u_short source_port;
    u_short destination_port;
    bool write_port = false;
    bool write_mac = false;
    bool write_ip = false;
    bool write_ip6 = false;
} header_data_t;


void print_interfaces();
void get_time(const struct pcap_pkthdr *packet_header, char *formatted_time);

void print_packet_header(const struct pcap_pkthdr *packet_header, header_data_t *ip_data);
void print_packet(const struct pcap_pkthdr *packet_header, const u_char *packet);