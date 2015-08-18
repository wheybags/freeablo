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
        self.document.GetElementById("rightRing").inner_rml=""
        self.document.GetElementById("leftRing").inner_rml=""
        self.document.GetElementById("body").inner_rml=""
        self.document.GetElementById("head").inner_rml=""
        self.document.GetElementById("amulet").inner_rml=""


        for child in inventory_element.child_nodes:
            child.inner_rml = ""
        inventory = freeablo.updateInventory()

        leftHandItem = inventory["leftHand"]
        leftHandContainer = self.document.GetElementById("leftHand")
        leftHand = self.document.CreateElement("div")



        if leftHandItem["empty"] is False:
            leftHand.SetAttribute("empty", "false")
            leftHand.style.width  = str(28 * leftHandItem["sizeX"]) + "px"
            leftHand.style.height = str(28 * leftHandItem["sizeY"]) + "px"
            if leftHandItem["sizeX"] == 1:
                leftHand.style.left = str(int(leftHand.style.left)+10) + "px"
            if leftHandItem["sizeY"] == 2:
                leftHand.style.top = str(int(leftHand.style.top)+14) + "px"
            leftHand.SetAttribute("class", "itemGraphic" + str(leftHandItem["graphic"]))

        else:
            leftHand.style.width = "0"
            leftHand.style.height = "0"
            leftHand.SetAttribute("empty", "true")
        leftHandContainer.AppendChild(leftHand)

        rightHandItem = inventory["rightHand"]
        rightHandContainer = self.document.GetElementById("rightHand")
        rightHand = self.document.CreateElement("div")

        if rightHandItem["empty"] is False:
            rightHand.SetAttribute("empty", "false")
            rightHand.style.width = str(28 * rightHandItem["sizeX"]) + "px"
            rightHand.style.height = str(28 * rightHandItem["sizeY"]) + "px"
            if rightHandItem["sizeX"] == 1:
                rightHand.style.left = str(int(rightHand.style.left)+10) + "px"
            if rightHandItem["sizeY"] == 2:
                rightHand.style.top = str(int(rightHand.style.top)+14) + "px"
            rightHand.SetAttribute("class", "itemGraphic" + str(rightHandItem["graphic"]))

        else:
            rightHand.style.width = "0"
            rightHand.style.height = "0"
            rightHand.SetAttribute("empty", "true")
        rightHandContainer.AppendChild(rightHand)

        bodyItem = inventory["body"]
        bodyContainer = self.document.GetElementById("body")
        body = self.document.CreateElement("div")


        if bodyItem["empty"] is False:
            body.SetAttribute("empty", "false")
            body.style.width = "57px"
            body.style.height = "87px"
            body.SetAttribute("class", "itemGraphic" + str(bodyItem["graphic"]))


        else:
            body.style.width = "0"
            body.style.height = "0"
            body.SetAttribute("empty", "true")
        bodyContainer.AppendChild(body)

        headItem = inventory["head"]
        headContainer = self.document.GetElementById("head")
        head = self.document.CreateElement("div")

        if headItem["empty"] is False:
            head.SetAttribute("empty", "false")
            head.style.width = "56px"
            head.style.height = "56px"
            head.SetAttribute("class", "itemGraphic" + str(headItem["graphic"]))

        else:
            head.style.width = "0"
            head.style.height = "0"
            head.SetAttribute("empty", "true")
        headContainer.AppendChild(head)

        amuletItem = inventory["amulet"]
        amuletContainer = self.document.GetElementById("amulet")
        amulet = self.document.CreateElement("div")

        if amuletItem["empty"] is False:
            amulet.SetAttribute("empty", "false")
            amulet.style.width = "28px"
            amulet.style.height = "28px"
            amulet.SetAttribute("class", "itemGraphic" + str(amuletItem["graphic"]))

        else:
            amulet.style.width = "0"
            amulet.style.height = "0"
            amulet.SetAttribute("empty", "true")
        amuletContainer.AppendChild(amulet)

        leftRingItem = inventory["leftRing"]
        leftRingContainer = self.document.GetElementById("leftRing")
        leftRing = self.document.CreateElement("div")

        if leftRingItem["empty"] is False:
            leftRing.SetAttribute("empty", "false")
            leftRing.style.width = "28px"
            leftRing.style.height = "28px"
            leftRing.SetAttribute("class", "itemGraphic" + str(leftRingItem["graphic"]))

        else:
            leftRing.style.width = "0"
            leftRing.style.height = "0"
            leftRing.SetAttribute("empty", "true")
        leftRingContainer.AppendChild(leftRing)

        rightRingItem = inventory["rightRing"]
        rightRingContainer = self.document.GetElementById("rightRing")
        rightRing = self.document.CreateElement("div")

        if rightRingItem["empty"] is False:
            rightRing.SetAttribute("empty", "false")
            rightRing.style.width = "28px"
            rightRing.style.height = "28px"
            rightRing.SetAttribute("class", "itemGraphic" + str(rightRingItem["graphic"]))

        else:
            rightRing.style.width = "0"
            rightRing.style.height = "0"
            rightRing.SetAttribute("empty", "true")
        rightRingContainer.AppendChild(rightRing)

        global cursor
        cursorItem = inventory["cursor"]
        cursor = cursorItem["empty"]
        inventoryBox = inventory["inventoryBox"]
        for i, item in enumerate(inventoryBox):
            parent = self.document.GetElementById("inv-item-socket" + str(i))
            element = self.document.CreateElement("div")
            element.SetAttribute("InvX", str(item["invX"]))
            element.SetAttribute("InvY", str(item["invY"]))
            if item["empty"] is False:
                element.SetAttribute("class", "itemGraphic"+str(item["graphic"]))
                element.SetAttribute("SizeX", str(item["sizeX"]))
                element.SetAttribute("SizeY", str(item["sizeY"]))
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
        self.document.GetElementById("container").SetAttribute("class", freeablo.getInvClass())
        self.updateInventory(event)

    def onLoadBelt(self, event):
        self.updateBelt(event)

    def updateBelt(self, event):
        beltParent = self.document.GetElementById("beltContainer")

        for child in beltParent.child_nodes:
            child.inner_rml=""

        inventory = freeablo.updateInventory()
        global cursor
        cursor = inventory["cursor"]["empty"]
        for i, item in enumerate(inventory["belt"]):
            parent = self.document.GetElementById("belt" + str(i))

            element = self.document.CreateElement("div")
            element.SetAttribute("InvX", str(item["invX"]))
            element.SetAttribute("InvY", str(item["invY"]))
            element.SetAttribute("BeltX", str(i))
            element.SetAttribute("empty", "false")
            element.SetAttribute("class", "beltItem")
            if item["empty"] is False:             
                element.SetAttribute("class", "itemGraphic" + str(item["graphic"]));
                element.style.width = "28px"
                element.style.height = "28px"
            else:
                element.SetAttribute("empty", "true")
                element.style.width="0px"
                element.style.height="0px"
            parent.AppendChild(element)

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

    def onHeadClick(self, event):
        global cursor
        slot = event.current_element.child_nodes[0]
        if cursor is True and slot.GetAttribute("empty") == "false":
            freeablo.placeItem(10, 4, 0, 0, 0, 0, 0)
            self.updateInventory(event)
        elif cursor is False and slot.GetAttribute("empty") == "true":
            freeablo.placeItem(4, 10, 0, 0, 0, 0, 0)
            self.updateInventory(event)

    def onAmuletClick(self, event):
        global cursor
        slot = event.current_element.child_nodes[0]
        if cursor is True and slot.GetAttribute("empty") == "false":
            freeablo.placeItem(10, 6, 0, 0, 0, 0, 0)
            self.updateInventory(event)
        elif cursor is False and slot.GetAttribute("empty") == "true":
            freeablo.placeItem(6, 10, 0, 0, 0, 0, 0)
            self.updateInventory(event)

    def onBodyClick(self, event):
        global cursor
        slot = event.current_element.child_nodes[0]
        if cursor is True and slot.GetAttribute("empty") == "false":
            freeablo.placeItem(10, 3, 0, 0, 0, 0, 0)
            self.updateInventory(event)
        elif cursor is False and slot.GetAttribute("empty") == "true":
            freeablo.placeItem(3, 10, 0, 0, 0, 0, 0)
            self.updateInventory(event)



    def onRingClick(self, event):
        global cursor
        slot = event.current_element.child_nodes[0]
        if event.current_element.GetAttribute("id") =="leftRing":
            to=14
        else:
            to=13
        if cursor is True and slot.GetAttribute("empty") == "false":
            freeablo.placeItem(10, to, 0, 0, 0, 0, 0)
            self.updateInventory(event)
        elif cursor is False and slot.GetAttribute("empty") == "true":
                freeablo.placeItem(to, 10, 0, 0, 0, 0, 0)
                self.updateInventory(event)

    def onWeaponClick(self, event):
        global cursor
        slot = event.current_element.child_nodes[0]
        if event.current_element.GetAttribute("id") == "leftHand":
            to=12
        else:
            to=11

        #cursor -> weapon slot
        if cursor is True and slot.GetAttribute("empty") == "false":
            freeablo.placeItem(10, to, 0, 0, 0, 0, 0)
            self.updateInventory(event)
        #weapon slot -> cursor
        elif cursor is False and slot.GetAttribute("empty") == "true":
                freeablo.placeItem(to, 10, 0, 0, 0, 0, 0)
                self.updateInventory(event)

    def onBeltClick(self, event):
        global cursor
        element = event.current_element.child_nodes[0]

        beltx = int(element.GetAttribute("BeltX"))
        if cursor is True and element.GetAttribute("empty") == "false":
            freeablo.placeItem(10, 8, 0, 0, 0, 0, beltx)
        elif cursor is False and element.GetAttribute("empty") =="true":            
            freeablo.placeItem(8, 10, 0, 0, 0, 0, beltx)
            cursor = True
        self.updateBelt(event)


    def socketMouseDown(self, event):
        global cursor
        if cursor is True and "itemGraphic" in event.current_element.child_nodes[0].GetAttribute("class"):
            y, x = (int(event.current_element.child_nodes[0].GetAttribute("InvX")), int(event.current_element.child_nodes[0].GetAttribute("InvY")))
            freeablo.placeItem(10, 0, x, y, x, y, 0)


        elif cursor is False and "itemGraphic" not in event.current_element.child_nodes[0].GetAttribute("class"):

            x, y = (int(event.current_element.child_nodes[0].GetAttribute("InvX")), int(event.current_element.child_nodes[0].GetAttribute("InvY")))

            freeablo.placeItem(0, 10, 0, 0, y, x, 0)
            cursor = True
        self.updateInventory(event)
        event.StopPropagation()

