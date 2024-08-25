#!/usr/bin/env python3
"""
License: MIT License
Copyright (c) 2023 Miel Donkers

Very simple HTTP server in python for logging requests
Usage::
    ./server.py [<port>]
"""
from http.server import BaseHTTPRequestHandler, HTTPServer
import logging, json
from pathlib import Path
import glob

def copy_presets (j):
    for _preset in j:
        preset = j [_preset]
        filename = Path.home() / "amprack" / "presets" / preset ["name"]
        print (f'writing {filename}')
        with open(filename, 'w') as f:
            json.dump(preset, f, ensure_ascii=False, indent=4)

def get_presets ():
    presets = dict ()
    i = 0
    for preset in glob.glob (str (Path.home() / "amprack" / "presets") + "/*"):
        f = open (preset)
        presets [i] = f.read ()
        f.close ()
        i += 1

    return presets
    
class S(BaseHTTPRequestHandler):
    def _set_response(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()

    def do_GET(self):
        logging.info("GET request,\nPath: %s\nHeaders:\n%s\n", str(self.path), str(self.headers))
        self._set_response()
        self.wfile.write("GET request for {}".format(self.path).encode('utf-8'))

    def do_POST(self):
        content_length = int(self.headers['Content-Length']) # <--- Gets the size of data
        post_data = self.rfile.read(content_length) # <--- Gets the data itself
        # ~ logging.info("POST request,\nPath: %s\nHeaders:\n%s\n\nBody:\n%s\n",
                # ~ str(self.path), str(self.headers), post_data.decode('utf-8'))

        j = json.loads (post_data)
        my_presets = get_presets ()
        copy_presets (j)
        
        self._set_response()
        self.wfile.write(str.encode (json.dumps (my_presets)))
        exit ()

def run(server_class=HTTPServer, handler_class=S, port=8080):
    logging.basicConfig(level=logging.INFO)
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    logging.info('Starting httpd...\n')
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
    logging.info('Stopping httpd...\n')

if __name__ == '__main__':
    from sys import argv

    if len(argv) == 2:
        run(port=int(argv[1]))
    else:
        run()
