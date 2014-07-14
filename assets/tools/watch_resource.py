import os
import traceback
from socket import *
from watchdog.events import FileSystemEventHandler
from watchdog.observers import Observer

CMD_PORT = 6010
FILE_PORT = 6020
BUFFER = 1024
HOST = '0.0.0.0' # change if other device
BASE_DIR = os.path.abspath(os.path.dirname(__file__)) + '/..'
RESOURCE_DIR = BASE_DIR + '/res'

cmdSocket = None

class ResourceHandler(FileSystemEventHandler):
    def on_created(self, event):
        print('created: ' + event.src_path)
        on_resource(event, event.src_path)
    def on_modified(self, event):
        print('modified: ' + event.src_path)
        on_resource(event, event.src_path)
    def on_moved(self, event):
        print('moved: ' + event.src_path)
        on_resource(event, event.dest_path)

def on_resource(event, path):
    if event.is_directory or ('tmp' in path):
        return
    if upload(path):
        cmdSocket.send('sendrequest {"cmd":"reload","modulefiles":[]}\n')

def upload(src_path):
    print("upload: " + src_path)
    success = False
    if not os.path.exists(src_path):
        print("not found: " + src_path)
        return
    filename = os.path.relpath(src_path, BASE_DIR)
    mtime = os.path.getmtime(src_path)
    json = '{"filename":"/%s", "lastmodifytime":"%d"}' % (filename, mtime)
    soc = socket(AF_INET, SOCK_STREAM)
    try:
        soc.connect((HOST, FILE_PORT))
        soc.send('%03d %s' % (len(json), json))
        fp = open(src_path, 'rb')
        data = fp.read(BUFFER)
        while (data):
            soc.send(data)
            data = fp.read(BUFFER)
        fp.close()
        success = True
    except Exception, e:
        traceback.print_exc()
    finally:
        soc.close()
    return success

if __name__ == "__main__":
    HOST = raw_input("type IP shown your device (like 127.0.0.1) >>> ")
    obs = Observer()
    obs.schedule(ResourceHandler(), RESOURCE_DIR, recursive=True)
    obs.start()
    cmdSocket = socket(AF_INET, SOCK_STREAM)
    cmdSocket.connect((HOST, CMD_PORT))
    cmdSocket.send('debugmsg on\n')
    for root, dirs, files in os.walk(RESOURCE_DIR):
        for file in files:
            upload(root + '/' + file)
    try:
        while True:
            print cmdSocket.recv(BUFFER)
    except KeyboardInterrupt:
        obs.stop()
    obs.join()
    cmdSocket.send('exit\n')
    cmdSocket.close()
