# -*- coding: utf-8 -*-
import docmanage
import freeablo
screenX = 0
screenY = 0
class DraggableWidget(object):

    def __init__(self, document, xoffset, yoffset):
        self.document = document
        self.xoffset = xoffset
        self.yoffset = yoffset

    def updateInventory(self):
        inventory = freeablo.updateInventory()
        global cursor
        cursorItem = inventory["cursor"]
        if cursorItem["empty"] is False:
            cursor.SetAttribute("empty", "false")
            cursor.style.width = str(28*cursorItem["sizeX"])
            cursor.style.height = str(28*cursorItem["sizeY"])
            cursor.SetAttribute("InvX", str(cursorItem["invX"]))
            cursor.SetAttribute("InvY", str(cursorItem["invY"]))
        else:
            cursor.SetAttribute("empty", "true")
        inventoryBox = inventory["inventoryBox"]
        for i, item in enumerate(inventoryBox):
            parent = self.document.GetElementById("inv-item-socket" + str(i))
            element = self.document.CreateElement("div")
            element.SetAttribute("InvX", str(item["invX"]))
            element.SetAttribute("InvY", str(item["invY"]))
            if item["empty"] is False:
                element.SetAttribute("class", "item")
                #element.style.backgroundImage = "/data/inv/objcurs.cel&frame=" +str(11 + item["graphic"]);
                #element.style.display = "inline-block"
                element.SetAttribute("onClick", "instance.onItemClick(event)")
                element.SetAttribute("SizeX", str(item["sizeX"]))
                element.SetAttribute("SizeY", str(item["sizeY"]))
                element.style.backgroundDecorator = "image;"
                element.style.width = str(28*item["sizeX"])
                element.style.height = str(28*item["sizeY"])
                element.SetAttribute("CornerX", str(item["cornerX"]))
                element.SetAttribute("CornerY", str(item["cornerY"]))
                if item["real"] is True:
                    element.SetAttribute("real", "true")
                else:
                    element.style.display = "none"
                    element.SetAttribute("real", "false")
            parent.AppendChild(element)

    def onLoad(self, event):
        global cursor

        cursorDoc = docmanage.manager.getDoc("resources/gui/cursor.rml")
        cursorDoc.PullToFront()
        print cursorDoc.inner_rml
        cursor = cursorDoc.CreateElement("div")
        cursor.SetAttribute("class", "cursor")
        cursorDoc.AppendChild(cursor)
        self.updateInventory()

    def onMouseMove(self, event):
        global cursor
        if cursor.GetAttribute("empty") == "false":
            mx = event.parameters["mouse_x"]
            my = event.parameters["mouse_y"]

            cursor.style.top=str(my - self.yoffset)
            cursor.style.left=str(mx - self.xoffset)
            #print "(%s, %s)" %  (cursor.style.top, cursor.style.left)
            #print cursor.parent_node.inner_rml



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
        global cursor
        print event.current_element.inner_rml

        if cursor.GetAttribute("empty") == "true" and event.current_element.child_nodes[0].GetAttribute("class") == "item":
            invCoords = (int(event.current_element.child_nodes[0].GetAttribute("InvX")), int(event.current_element.child_nodes[0].GetAttribute("InvY")))
            freeablo.placeItem(invCoords[1], invCoords[0], 10, invCoords[1], invCoords[0])
            #print event.current_element.parent_node.inner_rml

            for child in event.current_element.parent_node.child_nodes:
                if child == event.current_element:
                    continue
                else:
                    child.inner_rml = ""
            event.current_element.inner_rml = ""
            self.updateInventory()

        elif cursor.GetAttribute("empty") == "false" and event.current_element.child_nodes[0].GetAttribute("class") != "item":

            invCoords = (int(event.current_element.child_nodes[0].GetAttribute("InvX")), int(event.current_element.child_nodes[0].GetAttribute("InvY")))
            cursorCoords = (int(cursor.GetAttribute("InvX")), int(cursor.GetAttribute("InvY")))

            freeablo.placeItem(cursorCoords[1], cursorCoords[0], 0, invCoords[1], invCoords[0])
            cursor.SetAttribute("empty", "true")
            for child in event.current_element.parent_node.child_nodes:
                if child == event.current_element:
                    continue
                else:
            #        print "nuking ", child.inner_rml
                    child.inner_rml = ""
            event.current_element.inner_rml = ""
            self.updateInventory()
            #for child in event.current_element.parent_node.child_nodes:
            #    print child.inner_rml






        event.StopPropagation()

