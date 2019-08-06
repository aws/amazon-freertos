import Queue
import sys
import threading
import securityAgent
import time
from testClass import runTest
from bleAdapter import bleAdapter
import dbus.mainloop.glib
try:
  from gi.repository import GObject
except ImportError:
  import gobject as GObject

mainloop = GObject.MainLoop()

def main():
    time.sleep(2)
    runTest.printTestsSummary()

def errorConnectCb():
    print("Connection error")
    sys.stdout.flush()
    connectEvent.put(0)
