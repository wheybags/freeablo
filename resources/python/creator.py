import freeablo
import rocket

class Creator:
    
    currentMenu = 0
    selectedClass = 0

    def __init__(self, doc):
        self.doc = doc

    def showEnterMenu(self):
        freeablo.showEnterNameMenu(self.selectedClass)

    def onOkClick(self):

        freeablo.playClickButtonSound()

        if self.currentMenu == 0:
            self.showEnterMenu()
        elif self.currentMenu == 1:
            freeablo.showInvalidNameMenu()
        else:
            freeablo.startGame()

    def onCancelClick(self):

        freeablo.playClickButtonSound()

        if self.currentMenu == 1:
            freeablo.showChooseClassMenu()
        else:
            freeablo.showMainMenu()
        
    def setClass(self, classNumber):

        warrior = self.doc.GetElementById("heros_warrior")
        rogue = self.doc.GetElementById("heros_rogue")
        sorcerer = self.doc.GetElementById("heros_sorcerer")

        strength = self.doc.GetElementById("strength")
        magic = self.doc.GetElementById("magic")
        dexterity = self.doc.GetElementById("dexterity")
        vitality = self.doc.GetElementById("vitality")

       # Hide all

        classes = [warrior, rogue, sorcerer]
        for c in classes:
            c.style.height = "0px"
            c.style.visibility = "hidden"

        # Show one

        stats = [0, 0, 0, 0]

        selectedClass = None

        if classNumber == 0:
            selectedClass = warrior
            stats = [30, 10, 20, 25]

        if classNumber == 1:
            selectedClass = rogue
            stats = [20, 15, 30, 20]

        if classNumber == 2:
            selectedClass = sorcerer
            stats = [15, 35, 15, 20]

        selectedClass.style.height = "74px"
        selectedClass.style.visibility = "visible"

        strength.inner_rml = str(stats[0])
        magic.inner_rml = str(stats[1])
        dexterity.inner_rml = str(stats[2])
        vitality.inner_rml = str(stats[3])

