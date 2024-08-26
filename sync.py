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
import gi
import concurrent.futures
import asyncio
from threading import Thread
import random
import requests

gi.require_version("Gtk", "3.0")
from gi.repository import Gtk
from urllib import request, parse

key = None
httpd = None
global port

def copy_presets (j):
    for _preset in j:
        preset = j [_preset]
        if (type (preset) is str):
            preset = json.loads (preset)

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
        exit ()

    def do_POST(self):
        content_length = int(self.headers['Content-Length']) # <--- Gets the size of data
        key_ = int(self.headers['key']) # <--- Gets the size of data
        print ("got key " + str (key_))
        global key, label
        if key_ != key:
            label.set_markup ('<span foreground="red" weight="bold" size="x-large">Key Mismatch</span>')
            self._set_response()
            httpd.server_close()
            exit ()

        post_data = self.rfile.read(content_length) # <--- Gets the data itself
        # ~ logging.info("POST request,\nPath: %s\nHeaders:\n%s\n\nBody:\n%s\n",
                # ~ str(self.path), str(self.headers), post_data.decode('utf-8'))

        j = json.loads (post_data)
        my_presets = get_presets ()
        copy_presets (j)
        
        self._set_response()
        self.wfile.write(str.encode (json.dumps (my_presets)))
        label.set_markup ('<span foreground="green" weight="bold" size="x-large">Synced Successfully</span>')
        httpd.server_close()
        exit ()

def run(server_class=HTTPServer, handler_class=S, _port=8081):
    global port
    logging.basicConfig(level=logging.INFO)
    server_address = ('', port)
    global httpd
    httpd = server_class(server_address, handler_class)
    logging.info('Starting httpd...\n')
    global label, button
    button.set_visible (True)
    label.set_markup (f'<big><b>{key}</b></big>')

    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
    logging.info('Stopping httpd...\n')


def do_exit (self):
    Gtk.main_quit ()
    global thread, port
    req =  request.Request("http://localhost:" + str (port)) # this will make the method "POST"
    resp = request.urlopen(req)

    # ~ thread.join (0)
    # ~ logging.info('Stopping httpd...\n')
    exit ()

def sync_remote (entry): 
    global thread, port
    payload = get_presets ()
    val = entry.get_text ().split (":")
    url = f"http://{val [0]}:{val [1]}"
    
    headers = {'key': val [2]}
    r = requests.post(url, data=json.dumps(payload), headers=headers)
    print(r.content)
    j = json.loads (r.content)
    
    copy_presets (j)
    label.set_markup ('<span foreground="green" weight="bold" size="x-large">Synced Successfully</span>')

def sync_remote_cb (self, ent):
    ent.activate()

if __name__ == '__main__':
    global thread
    global label, port
    from sys import argv
    if len(argv) == 2:
        port = int (argv [1])
    else:
        port = 8080
    
    win = Gtk.Window()
    win.set_size_request (400, 100)
    label = Gtk.Label ("Please wait ...")
    key = random.randint (1000, 9999)
    l2 = Gtk.Label ("Key:")
    box = Gtk.Box ()
    box2 = Gtk.Box ()
    box3 = Gtk.Box ()
    box2.set_orientation (1)
    win.add (box2)
    
    desc = Gtk.Label ("Sync Amp Rack Presets")
    desc.set_markup ('<span weight="bold" size="xx-large">Sync Amp Rack Presets</span>')
    
    import socket
    hostname = socket.getfqdn()

    box2.set_margin_start (20)
    box2.set_margin_end (20)

    grid = Gtk.Grid ()
    grid.attach (Gtk.Label ("IP:"), 0, 0, 1, 1)

    ip = Gtk.Label (str (socket.gethostbyname_ex(hostname)[2]).replace (",", " ").replace ("[", "").replace ("]","").replace ("'",""))
    grid.attach (ip, 1, 0, 2, 1)

    grid.attach (Gtk.Label ("Port:"), 0, 1, 1, 1)

    por = Gtk.Label (str (port))
    grid.attach (por, 1, 1, 1, 1)

    grid.attach (l2, 0, 2, 1, 1)
    grid.attach (label, 1, 2, 2, 1)

    box2.pack_start (desc, True, True, 10)
    box2.pack_start (Gtk.Label ("Use the following details to sync from another device"), True, True, 0)
    box2.pack_start (grid, True, True, 10)
    box2.pack_start (box, True, True, 10)
    # ~ box.pack_start (l2, True, True, 10)
    # ~ box.pack_start (label, True, True, 10)
    global button
    button = Gtk.Button ()
    button.set_label ("Close")
    button.set_halign (Gtk.Align.CENTER)
    box2.pack_start (box3, True, True, 10)
    box3.pack_start (button, False, False, 10)
    
    ent = Gtk.Entry ()
    ent.set_placeholder_text ("Sync with IP")
    
    btn = Gtk.Button ("Sync")
    grid.attach (Gtk.Label ("Enter details below to sync from this device"), 0, 3, 5, 1)
    grid.attach (ent, 0, 4, 4, 1)
    grid.attach (btn, 4, 4, 1, 1)
    grid.attach (Gtk.Label ("IP:PORT:KEY"), 0, 5, 4, 1)
    
    button.connect("clicked", do_exit)
    ent.connect ("activate", sync_remote)
    btn.connect ("clicked", sync_remote_cb, ent)
    win.connect("destroy", do_exit)
    win.connect("destroy", Gtk.main_quit)
    win.show_all()
    button.set_visible (False)
    Gtk.main_iteration_do (True)
    thread = Thread(target=run)
    thread.start()
    
    # ~ if len(argv) == 2:
        # ~ run(port=int(argv[1]))
    # ~ else:
        # ~ run()
        
    Gtk.main()
    exit ()
