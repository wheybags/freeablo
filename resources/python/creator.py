import freeablo
import rocket

class Creator:
    
    currentMenu = 0
    selectedClass = 0

    def __init__(self, doc, menu = None):
        self.doc = doc
        self.menu = menu

    def showEnterMenu(self):
        freeablo.showEnterNameMenu(self.selectedClass)

    def showInvalidNameMenu(self):
        freeablo.showInvalidNameMenu(self.selectedClass)

    def onOkClick(self):

        freeablo.playClickButtonSound()

        if self.currentMenu == 0:
            freeablo.showChooseClassMenu()
        elif self.currentMenu == 1:
            self.showEnterMenu()
        elif self.currentMenu == 2:
            if self.doc.GetElementById("nick").value == "":
                self.showInvalidNameMenu()
            else:
                freeablo.startGame()

        elif self.currentMenu == 3:
            self.showEnterMenu()
        else:
            freeablo.startGame()

    def onDeleteClick(self):
        self.menu.deleteEntry(self.menu.current)


    def onCancelClick(self):

        freeablo.playClickButtonSound()

        if self.currentMenu == 1:
            freeablo.showSelectHeroMenuNoFade()
        elif self.currentMenu == 2:
            freeablo.showChooseClassMenu()
        else:
            freeablo.showMainMenu()
        
    def setClass(self, classNumber):

        self.selectedClass = classNumber

        new_hero  = self.doc.GetElementById("heros_new")
        warrior = self.doc.GetElementById("heros_warrior")
        rogue = self.doc.GetElementById("heros_rogue")
        sorcerer = self.doc.GetElementById("heros_sorcerer")

        level = self.doc.GetElementById("level")
        strength = self.doc.GetElementById("strength")
        magic = self.doc.GetElementById("magic")
        dexterity = self.doc.GetElementById("dexterity")
        vitality = self.doc.GetElementById("vitality")

       # Hide all

        classes = [new_hero, warrior, rogue, sorcerer]
        for c in classes:
            c.style.display = "none"

        # Show one

        stats = [0, 0, 0, 0]
        level_value = 1

        myClass = None

        if classNumber == -1:
            myClass = new_hero
            stats = ['--', '--', '--', '--']
            level_value = '--'

        if classNumber == 0:
            myClass = warrior
            stats = [30, 10, 20, 25]

        if classNumber == 1:
            myClass = rogue
            stats = [20, 15, 30, 20]

        if classNumber == 2:
            myClass = sorcerer
            stats = [15, 35, 15, 20]

        myClass.style.display = "block"

        level.inner_rml = str(level_value)
        strength.inner_rml = str(stats[0])
        magic.inner_rml = str(stats[1])
        dexterity.inner_rml = str(stats[2])
        vitality.inner_rml = str(stats[3])

