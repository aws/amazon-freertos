#!/usr/bin/python

from __future__ import absolute_import, print_function, unicode_literals

from optparse import OptionParser, make_option
import dbus
import time
import dbus.mainloop.glib
import bleAdapter
try:
  from gi.repository import GObject
except ImportError:
  import gobject as GObject
import testutils
import startTests
import threading
import securityAgent

devices = {}

def backGroundEvents():
    try:
        mainloop = GObject.MainLoop()
        mainloop.run()

    except KeyboardInterrupt:
        mainloop.quit()
        print("Thread: KeyboardInterrupt")
    return

if __name__ == '__main__':

  dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

  testutils.removeBondedDevices()

  #startBackGroundEvents = threading.Thread(target=backGroundEvents)
  #startBackGroundEvents.start()
  startTests.main()
