# -*- coding: utf-8 -*-
import inspect
import freeablo
class DraggableWidget(object):
    def __init__(self, document, xoffset, yoffset):
        self.document = document
        self.xoffset = xoffset
        self.yoffset = yoffset
    def onLoad(self, event):

        for i,item in enumerate(freeablo.getItemBox()):

            parent = self.document.GetElementById("inv-item-socket" + str(i))
            element = self.document.CreateElement("div")
            if item[1] is False:
                element.SetAttribute("class", "item")
                #element.style.backgroundImage = "/data/inv/objcurs.cel;" #+str(11 + item[0]);
                #element.style.display = "inline-block"
                #element.style.backgroundDecorator = "image;"
            else:
                element.style.display = "inline-block;"

            parent.AppendChild(element)

    def onDrag(self, event):

        mx = event.parameters['mouse_x']
        my = event.parameters['mouse_y']
        event.current_element.style.left= str(mx+self.xoffset)
        event.current_element.style.top = str(my+self.yoffset)
        event.StopPropagation()
        #print " (%s, %s)" % (event.current_element.style.left,
        #    event.current_element.style.top)

    def guiMouseDown(self, event):
        pass
        #print "guiMouseDown called: %s" % (event.current_element)
        #for attr in inspect.getmembers(event.current_element):
        #    print attr[0]

    def bodyMouseDown(self, event):
        print "bodyMouseDown called: %s" % (event.current_element)

        #for attr in inspect.getmembers(element.style):
        #    print "%s : %s" % (attr[0], attr[1])
        #print freeablo.getItemBox()

    def bodyKeyDown(self, event):
        print "bodyKeyDown called: %s" % (event.current_element)

    def socketMouseDown(self, event):
        print "socketMouseDown called: %s" % (event.current_element.tag_name)
