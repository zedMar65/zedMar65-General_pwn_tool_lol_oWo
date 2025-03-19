import os

# Network Interface
interface = "Wi-Fi"  # Replace with your actual network interface name

# First IP (gateway)
ip_1 = "192.168.1.100"
subnet_mask = "255.255.255.0"
gateway = "192.168.1.1"

# Second IP (target)
ip_2 = "192.168.1.101"

# Add the first IP (gateway IP)
os.system(f'netsh interface ip set address name="{interface}" static {ip_1} {subnet_mask} {gateway}')

# Add the second IP (target IP) to the same interface (this is the IP you'll "mask" as)
os.system(f'netsh interface ip add address name="{interface}" {ip_2} {subnet_mask}')

# Verify the IPs were added
os.system("ipconfig")
