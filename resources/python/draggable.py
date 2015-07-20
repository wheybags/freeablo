# -*- coding: utf-8 -*-
import inspect
import freeablo
class DraggableWidget(object):
    def __init__(self, document, xoffset, yoffset):
        self.document = document
        self.xoffset = xoffset
        self.yoffset = yoffset
        self.cursor = None
    def onLoad(self, event):

        for i,item in enumerate(freeablo.getItemBox()):

            parent = self.document.GetElementById("inv-item-socket" + str(i))
            element = self.document.CreateElement("div")
            if item[1] is False:
                element.SetAttribute("class", "item")
                #element.style.backgroundImage = "/data/inv/objcurs.cel;" #+str(11 + item[0]);
                #element.style.display = "inline-block"
                #element.style.backgroundDecorator = "image;"
                element.SetAttribute("onClick", "instance.onItemClick(event)")
                if item[4] is True:
                    element.style.width = str(28*item[3])
                    element.style.height = str(28*item[2])
                else:
                    element.style.display = "none"
            else:
                element.style.display = "inline-block;"

            parent.AppendChild(element)

    def onItemClick(self, element):
        self.cursor = element

    def onDrag(self, event):

        mx = event.parameters['mouse_x']
        my = event.parameters['mouse_y']
        event.current_element.style.left= str(mx+self.xoffset)
        event.current_element.style.top = str(my+self.yoffset)
    def guiMouseDown(self, event):
        pass

    def bodyMouseDown(self, event):
        pass

    def bodyKeyDown(self, event):
        pass

    def socketMouseDown(self, event):
        if event.current_element.child_nodes[0].GetAttribute("class") == "item":
            self.onItemClick(event.current_element.child_nodes[0])

        event.StopPropagation()

