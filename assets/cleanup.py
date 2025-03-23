import os, argparse, subprocess
from scapy.all import ARP, send, Ether

parser = argparse.ArgumentParser(description="Cleanup the network configuration")
parser.add_argument("--interface", type=str, help="Network interface name", default="Wi-Fi")
parser.add_argument("--target_ip", type=str, help="Target IP address", default="")
parser.add_argument("--target_mac", type=str, help="Target MAC address", default="")
parser.add_argument("--gateway_ip", type=str, help="Gateway IP address", default="")
parser.add_argument("--gateway_mac", type=str, help="Gateway MAC address", default="")
args = parser.parse_args()

# Restore DHCP configuration


def set_ip_forwarding(enable: bool):
    """Enable or disable global IP forwarding in Windows."""
    value = "1" if enable else "0"
    command = f'reg add HKLM\\SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters /v IPEnableRouter /t REG_DWORD /d {value} /f'
    subprocess.run(command, shell=True, check=True)
    print(f"IP Forwarding {'Enabled' if enable else 'Disabled'} (Global Setting)")

# Example Usage
set_ip_forwarding(False)  # Enable IP forwarding



os.system(f'netsh interface ip set address name="{args.interface}" source=dhcp')
os.system('netsh advfirewall firewall delete rule name="Allow 8080" protocol=TCP localport=80')

packet = ARP(op=2, pdst=args.target_ip, hwdst=args.target_mac, psrc=args.gateway_ip, hwsrc=args.gateway_mac)
packet1 = ARP(op=2, pdst=args.gateway_ip, hwdst=args.gateway_mac, psrc=args.target_ip, hwsrc=args.target_mac)
send(packet, count=4, verbose=False)
send(packet1, count=4, verbose=False)