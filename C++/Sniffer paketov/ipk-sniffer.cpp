
/*
    Programový súbor projektu IPK - Sniffer paketov
    Richard Seipel - xseipe00
    25.4.2021  
 */

#include "ipk-sniffer.h"

/*
    Zabezpečuje spracovanie a výpis informácii o pakete
 */
void print_packet_info(u_char *args, const struct pcap_pkthdr *packet_header, const u_char *packet) {

    #define ETHERNET_HEADER_LEN 14
    #define IPV4_PROTOCOL_OFFSET 9
    #define IPV4_SOURCE_OFFSET 12
    #define IPV4_DESTINATION_OFFSET 16
    #define IPV6_PROTOCOL_OFFSET 6
    #define IPV6_SOURCE_OFFSET 8
    #define IPV6_DESTINATION_OFFSET 24
    #define ARP_IPTYPE_OFFSET 2
    #define ARP_SOURCE_OFFSET 14
    #define ARP_DESTINATION_OFFSET 24
    #define ETHERTYPE_IP 0x0800
    #define ETHERTYPE_IPV6 0x86dd
    #define ETHERTYPE_ARP 0x0806
    #define PROTOCOL_TCP 6
    #define PROTOCOL_UDP 17
    #define PROTOCOL_ICMP 1
    #define PORT_LEN 2

    struct ether_header *ether_header = (struct ether_header *)packet;
    u_short ether_type = __builtin_bswap16(ether_header->ether_type);

    struct in_addr *ipv4_source;
    struct in_addr *ipv4_destination;

    struct ether_addr *mac_source;
    struct ether_addr *mac_destination;

    struct in6_addr *ipv6_source;
    struct in6_addr *ipv6_destination;
    
    header_data_t header_data;

	const u_char *ether_header_end; 

    ether_header_end = packet + ETHERNET_HEADER_LEN;


    if (ether_type == ETHERTYPE_ARP) {

        u_short arp_type = __builtin_bswap16(*(u_short*)(ether_header_end + ARP_IPTYPE_OFFSET)); 
        if (arp_type == ETHERTYPE_IP) {
            
            mac_source = (struct ether_addr*)(packet + ETHER_ADDR_LEN);
            mac_destination = (struct ether_addr*)(packet);
            header_data.write_mac = true;
        }
    } 
    else if (ether_type == ETHERTYPE_IP) {

        u_char length = (*(u_char *)ether_header_end & 0xf) * 4;
        u_char protocol = *(u_char *)(ether_header_end + IPV4_PROTOCOL_OFFSET); 
    
        ipv4_source = (struct in_addr*)(ether_header_end + IPV4_SOURCE_OFFSET);
        ipv4_destination = (struct in_addr*)(ether_header_end + IPV4_DESTINATION_OFFSET);
        header_data.write_ip = true;

        if (protocol == PROTOCOL_TCP || protocol == PROTOCOL_UDP) {
            const u_char *protocol_header = ether_header_end + length;
            header_data.source_port = __builtin_bswap16(*(u_short *)protocol_header);
            header_data.destination_port = __builtin_bswap16(*(u_short *)(protocol_header + PORT_LEN));
            header_data.write_port = true;

        }
    
    } else if (ether_type == ETHERTYPE_IPV6) {

        u_char length = 40;
        u_char protocol = *(u_char *)(ether_header_end + IPV6_PROTOCOL_OFFSET);

        ipv6_source = (struct in6_addr*)(ether_header_end + IPV6_SOURCE_OFFSET);
        ipv6_destination = (struct in6_addr*)(ether_header_end + IPV6_DESTINATION_OFFSET);
        header_data.write_ip6 = true;

        if (protocol == PROTOCOL_TCP || protocol == PROTOCOL_UDP) {
            const u_char *protocol_header = ether_header_end + length;
            header_data.source_port = __builtin_bswap16(*(u_short *)protocol_header);
            header_data.destination_port = __builtin_bswap16(*(u_short *)(protocol_header + PORT_LEN));
            header_data.write_port = true;

        }

    }

    if (header_data.write_mac) {
        strcpy(header_data.source_mac, ether_ntoa(mac_source));
        strcpy(header_data.destination_mac, ether_ntoa(mac_destination));
    }
    else if (header_data.write_ip) {
        inet_ntop(AF_INET, ipv4_source, header_data.source, sizeof(header_data.source));
        inet_ntop(AF_INET, ipv4_destination, header_data.destination, sizeof(header_data.destination));
    }
    else if (header_data.write_ip6) {
        inet_ntop(AF_INET6, ipv6_source, header_data.source6, sizeof(header_data.source6));
        inet_ntop(AF_INET6, ipv6_destination, header_data.destination6, sizeof(header_data.destination6));
    }
    
    print_packet_header(packet_header, &header_data);
    print_packet(packet_header, packet);

}

/* 
    Vypíše formátovaný čas do poľa zadaného ukazateľom formatted_time 
 */
void get_time(const struct pcap_pkthdr *packet_header, char *formatted_time) {

    struct tm * local = localtime(&packet_header->ts.tv_sec);

    char time[64];
    strftime(time, sizeof(time), "%FT%X", local);
    char zone[10];
    strftime(zone, sizeof(zone), "%z", local);

    std::string zone_string = zone;
    zone_string.insert(3, 1,':');

    sprintf(formatted_time, "%s.%ld%s", time, packet_header->ts.tv_usec, &zone_string[0]);
}

/* 
    Vypíše hlavičku paketu obsahujúcu čas, adresy, prípadné porty a dĺžku paketu v bytoch
 */
void print_packet_header(const struct pcap_pkthdr *packet_header, header_data_t *header_data) {

    char time[200];
    get_time(packet_header, time);
    
    std::cout << time << " ";
    if (header_data->write_ip)   std::cout << header_data->source;
    if (header_data->write_ip6)  std::cout << header_data->source6;
    if (header_data->write_mac)  std::cout << header_data->source_mac;
    if (header_data->write_port) std::cout << " : " <<  header_data->source_port;
    std::cout << " > ";
    if (header_data->write_ip)   std::cout << header_data->destination;
    if (header_data->write_ip6)  std::cout << header_data->destination6;
    if (header_data->write_mac)  std::cout << header_data->destination_mac;
    if (header_data->write_port) std::cout << " : " <<  header_data->destination_port;
    std::cout << ", length " << packet_header->len << " bytes\n";
} 

/* 
    Vypíše obsah paketu rozdelený po 16 bytoch v hexadecimálnom a ascii formáte 
 */
void print_packet(const struct pcap_pkthdr *packet_header, const u_char *packet) {

    if (packet_header->len > 0) {
        const u_char *temp_pointer = packet;
        int byte_count = 0;
        std::string buffer = "";
        
        while (byte_count < (int)packet_header->len) {
            if (byte_count%16 == 0) {
                printf("%s\n0x%04hhx: ", &buffer[0], byte_count);
                buffer = "";
            }   

            printf("%02hhx ", (unsigned int)*temp_pointer);
            if (isprint(*temp_pointer)) {
                buffer.push_back(*temp_pointer);
            }
            else buffer.push_back('.');
            byte_count++;
            temp_pointer++;     
        }
        printf("%s\n\n", &buffer[0]);
    }

}

/* 
    Spracuje argumenty programu a uloží výsledky do zadanej štruktúry arguments
 */
void parse_arguments(int argc, char *argv[], arguments_t *arguments) {

    #define ARP  3
    #define ICMP 4

    int c;

    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"interface",   required_argument,  0, 'i'  },
            {"tcp",         no_argument,        0, 't'  },
            {"udp",         no_argument,        0, 'u'  },
            {"arp",         no_argument,        0,  ARP },
            {"icmp",        no_argument,        0,  ICMP},
            { 0, 0, 0, 0 }
        };

        c = getopt_long(argc, argv, "i:p:tun:", long_options, &option_index);
        if (c == -1)
            break;


        char *num_string = NULL;
        char *port_string = NULL;
        char *ptr;

        switch (c) {
            case 0:
                break;

            case 'i':
                arguments->interface = optarg;
                break;

            case 'p':
                port_string = optarg;

                arguments->port = strtol(port_string, &ptr, 10);
                if (ptr == NULL) {
                    fprintf(stderr, "ERROR: Wrong port format\n");
                    exit(1);
                }
                break;

            case 't':
                arguments->tcp = true;
                break;

            case 'u':
                arguments->udp = true;
                break;
            
            case ARP:
                arguments->arp = true;
                break; 

            case ICMP:
                arguments->icmp = true;
                break;

            case 'n':
                num_string = optarg;

                arguments->num = strtol(num_string, &ptr, 10);
                if (ptr == NULL) {
                    fprintf(stderr, "ERROR: Wrong number of packets format\n");
                    exit(1);
                }
                break;

            case '?':
                if (arguments->interface) {
                    exit(1);
                }
                break;
                
            default:
                fprintf(stderr, "?? getopt returned character code 0%o ??\n", c);
                exit(1);
            }
    }

    if (!arguments->interface) {
        print_interfaces();
        exit(0);
    }

    if (optind < argc) {
        fprintf(stderr, "non-option argument elements: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        printf("\n");
        exit(1);
    }
}

/* 
    Vypíše dostupné rozhrania
 */
void print_interfaces() {
    pcap_if_t *alldevsp;
    char error_buffer[PCAP_ERRBUF_SIZE];
    if (pcap_findalldevs(&alldevsp, error_buffer)) {
        fprintf(stderr, "Error finding interfaces: %s\n", error_buffer);
        exit(1);
    }

    // write
    pcap_if_t *tmp_ptr = alldevsp;
    while (tmp_ptr != NULL) {
        std::cout << tmp_ptr->name << "\n";
        tmp_ptr = tmp_ptr->next;
    }
}

/* 
    Poskladá a vráti výraz pre fitrovanie paketov na základe argumentov programu
 */
char *get_filter(arguments_t *arguments) {
    
    std::string filter = "";
    std::string port = "";
    bool empty = true;

    if (arguments->port != -1) {
        port.append(std::to_string(arguments->port));
    }

    if (arguments->tcp) {
        if (!empty) {
            filter.append(" || ");
        }
        if (arguments->port != -1) {
            filter.append("tcp port " + port);
        }
        else {
            filter.append("tcp");
        }
        empty = false;
    }

    if (arguments->udp) {
        if (!empty) {
            filter.append(" || ");
        }
        if (arguments->port != -1) {
            filter.append("udp port " + port);
        }  
        else {
            filter.append("udp");
        }
        empty = false;
    }

    if (arguments->arp) {
        if (!empty) filter.append(" || ");
        filter.append("arp");
        empty = false;
    }

    if (arguments->icmp) {
        if (!empty) filter.append(" || ");
        filter.append("icmp || icmp6");
        empty = false;
    }

    char *filter_array = new char[filter.length() + 1];
    strcpy(filter_array, filter.c_str());
    return filter_array;
}

/* 
    Uvolní alokovanú pamäť pre výraz filtra  
 */
void delete_filter(char *filter_array) {
    delete[] filter_array;
}

/* 
    Spustí zachytávanie paketov s trvaním podľa zadaných argumentov
 */
int main(int argc, char *argv[]) {

    arguments_t arguments;
    pcap_t *p;
    char error_buffer[PCAP_ERRBUF_SIZE];

    struct bpf_program filter_program;
    char *filter;

    parse_arguments(argc, argv, &arguments);
    filter = get_filter(&arguments);

    if((p = pcap_create(arguments.interface, error_buffer)) == NULL) {
        fprintf(stderr, "pcap_create failed\n");
        exit(1);
    }

    pcap_set_promisc(p, true);
    pcap_set_timeout(p, 100);

    if(pcap_activate(p) < 0) {
        fprintf(stderr, "pcap_activate failed\n");
        exit(1);
    }
    
    if (pcap_compile(p, &filter_program, filter, 0, PCAP_NETMASK_UNKNOWN) == -1) {
        fprintf(stderr, "Bad filter - %s\n", pcap_geterr(p));
        return 2;
    }
    if (pcap_setfilter(p, &filter_program) == -1) {
        fprintf(stderr, "Error setting filter - %s\n", pcap_geterr(p));
        return 2;
    }
    
    if (pcap_loop(p, arguments.num, print_packet_info, NULL) == PCAP_ERROR) {
        fprintf(stderr, "pcap_loop error\n");
        exit(1);
    }

    delete_filter(filter);
    pcap_close(p);
    
    return 0;
}

/* 
    Informácie a prípadne konštrukcie kódu knižníc som čerpal z manuálových stránok knižníc (hlavne libpcap a getopts).
    https://www.tcpdump.org/pcap.html
    https://man7.org/linux/man-pages/man3/getopt.3.html

    Základné princípy sniffovania pomocou libpcap ako otvorenie pcap súboru, filtrovanie, loop alebo čítanie dát paketu
    som čerpal zo stránky: 
    https://www.devdungeon.com/content/using-libpcap-c
    Možno sa niekde nachádza aj kúsok kódu z tejto stránky, no vo väčšine situácii som používal vlastnú implementáciu riešení 
    pre potreby tohto projektu.
 */