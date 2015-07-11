# -*- coding: utf-8 -*-
from docmanage import DocManager
import inspect
import pprint
class DraggableWidget:
    def __init__(self, document, xoffset, yoffset):
        self.document = document
        self.xoffset = xoffset
        self.yoffset = yoffset

    def onDrag(self, event):

        mx = event.parameters['mouse_x']
        my = event.parameters['mouse_y']
        event.current_element.style.left= str(mx+self.xoffset)
        event.current_element.style.top = str(my+self.yoffset)
        #print " (%s, %s)" % (event.current_element.style.left,
        #    event.current_element.style.top)

    def guiMouseDown(self, event):
        pass
        #print "guiMouseDown called: %s" % (event.current_element)
        #for attr in inspect.getmembers(event.current_element):
        #    print attr[0]

    def bodyMouseDown(self, event):

        print "bodyMouseDown called: %s" % (event.current_element)
        #for attr in inspect.getmembers(event):
        #    print "%s : %s" % (attr[0], attr[1])
    def bodyKeyDown(self, event):
        print "bodyKeyDown called: %s" % (event.current_element)
