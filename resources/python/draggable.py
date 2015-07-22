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

    def updateInventory(self, event):

        inventory_element = self.document.GetElementById("inventory")
        self.document.GetElementById("rightHand").inner_rml=""
        self.document.GetElementById("leftHand").inner_rml=""


        for child in inventory_element.child_nodes:
            child.inner_rml = ""
        inventory = freeablo.updateInventory()

        leftHandItem = inventory["leftHand"]
        leftHandContainer = self.document.GetElementById("leftHand")
        leftHand = self.document.CreateElement("div")
        leftHand.SetAttribute("class", "item")


        if leftHandItem["empty"] is False:
            leftHand.SetAttribute("empty", "false")
            leftHand.style.width = "53px"
            leftHand.style.height = "83px"

        else:
            leftHand.style.width = "0"
            leftHand.style.height = "0"
            leftHand.SetAttribute("empty", "true")
        leftHandContainer.AppendChild(leftHand)

        rightHandItem = inventory["rightHand"]
        rightHandContainer = self.document.GetElementById("rightHand")
        rightHand = self.document.CreateElement("div")
        rightHand.SetAttribute("class", "item")

        if rightHandItem["empty"] is False:
            rightHand.SetAttribute("empty", "false")
            rightHand.style.width = "53px"
            rightHand.style.height = "83px"

        else:
            rightHand.style.width = "0"
            rightHand.style.height = "0"
            rightHand.SetAttribute("empty", "true")
        rightHandContainer.AppendChild(rightHand)

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
        cursor = cursorDoc.CreateElement("div")
        cursor.SetAttribute("class", "cursor")
        cursorDoc.AppendChild(cursor)
        self.updateInventory(event)

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

    def onWeaponClick(self, event):
        global cursor
        slot = event.current_element.child_nodes[0]
        if cursor.GetAttribute("empty") == "true" and slot.GetAttribute("empty") == "false":
            freeablo.placeItem(10, 12, 0, 0, 0, 0)
            self.updateInventory(event)
        elif cursor.GetAttribute("empty") == "false" and slot.GetAttribute("empty") == "true":
                freeablo.placeItem(12, 10, 0, 0, 0, 0)
                self.updateInventory(event)


    def socketMouseDown(self, event):
        global cursor
        if cursor.GetAttribute("empty") == "true" and event.current_element.child_nodes[0].GetAttribute("class") == "item":
            invCoords = (int(event.current_element.child_nodes[0].GetAttribute("InvX")), int(event.current_element.child_nodes[0].GetAttribute("InvY")))
            freeablo.placeItem(10, 0, invCoords[1], invCoords[0], invCoords[1], invCoords[0])


        elif cursor.GetAttribute("empty") == "false" and event.current_element.child_nodes[0].GetAttribute("class") != "item":

            invCoords = (int(event.current_element.child_nodes[0].GetAttribute("InvX")), int(event.current_element.child_nodes[0].GetAttribute("InvY")))
            cursorCoords = (int(cursor.GetAttribute("InvX")), int(cursor.GetAttribute("InvY")))
            freeablo.placeItem(0, 10, cursorCoords[1], cursorCoords[0], invCoords[1], invCoords[0])
            cursor.SetAttribute("empty", "true")
        self.updateInventory(event)
        event.StopPropagation()

