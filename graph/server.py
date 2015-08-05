#!/usr/bin/env python3
#-*- coding: utf8 -*-
 
from autobahn.asyncio.websocket import WebSocketServerProtocol, WebSocketServerFactory
import bottle
import threading
import pyinotify
import asyncio


WATCHFILE = "../metrics.txt"
WS_PORT=8081

@bottle.route('/<filename:path>')
def server_static(filename):
    return bottle.static_file(filename, root='.')

@bottle.route('/')
def server_static():
    return bottle.template("index.html", ws_port=WS_PORT)


class MetricsProtocol(WebSocketServerProtocol):
    def onConnect(self, request):
        self.factory.register(self)

    def onOpen(self):
        pass

    def onMessage(self, payload, isBinary):
        # Unused for now
        if isBinary:
            print("binary received: {0} bytes".format(len(payload)))
        else:
            print("text received: {0}".format(payload.decode('utf8')))

    def onClose(self, wasClean, code, reason):
        pass

    def connectionLost(self, reason):
        WebSocketServerProtocol.connectionLost(self, reason)
        self.factory.unregister(self)


class BroadcastServerFactory(WebSocketServerFactory):
    """Simple server broadcasting to all currently connected clients."""

    def __init__(self, url):
        WebSocketServerFactory.__init__(self, url)
        self.clients = []

    def register(self, client):
        if client not in self.clients:
            print("registered client {}".format(client.peer))
            self.clients.append(client)

    def unregister(self, client):
        if client in self.clients:
            print("unregistered client {}".format(client.peer))
            self.clients.remove(client)

    def broadcast(self, msg):
        #print("broadcasting message '{}'".format(msg))
        for c in self.clients:
            c.sendMessage(msg.encode('utf8'))


def monitoring_loop():
    wm = pyinotify.WatchManager()
    wm.add_watch(WATCHFILE, pyinotify.IN_MODIFY, rec=True)
    
    handler = FileEventHandler()
    notifier = pyinotify.Notifier(wm, handler)
    notifier.loop()


class FileEventHandler(pyinotify.ProcessEvent):
    def __init__(self, *args, **kwargs):
        super(FileEventHandler, self).__init__(*args, **kwargs)
        self.file = open(WATCHFILE)
        self.position = 0
        #self.watch_lines()

    def process_IN_MODIFY(self, event):
        self.watch_lines()

    def watch_lines(self):
        self.file.seek(self.position)
        remaining = self.file.read() # Read from position to EOF
        last_n = remaining.rfind('\n')
        if last_n >= 0:
            self.position += last_n + 1 # Advance position if a complete line is found
            
            new_lines = remaining[:last_n]
            factory.broadcast(new_lines)


if __name__ == '__main__':

    threading.Thread(target=monitoring_loop, daemon=True).start() # Monitoring loop for inotify
    threading.Thread(target=bottle.run, daemon=True).start()

    factory = BroadcastServerFactory("ws://localhost:%d" % WS_PORT)
    factory.protocol = MetricsProtocol

    loop = asyncio.get_event_loop()
    server = loop.run_until_complete(loop.create_server(factory, '0.0.0.0', WS_PORT))
    
    loop.run_forever() # Websocket server loop
