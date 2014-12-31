import rocket

class Menu(object):
    """
    Class for representing rml menus, which are lists of entries that can be selected with the keyboard,
    or clicked with the mouse. For an example of usage, see resources/gui/pausemenu.rml.
    """

    def __init__(self, doc, selfName, containerId, entries, fmtSelected, fmtNotSelected):
        """
        Arguments:
        doc -- The rocket.Document instance to operate on.
        selfName -- The name by which this instance can be referred to from rml.
            Needed to access members from rml callbacks, eg: <span id="..." onclick="selfName.setSelected(...
        containerId -- The id of the element within which to place the menu rml
        entries -- A list of dicts, each one containing the following fields:
            text -- Mandatory. The text for this entry
            func -- Optional. The function to be called when a user presses return with this entry hilighted
            textFunc -- Optional. The function to be called (without () at the end) when a user clicks on 
                the entry. Must be accessible from within the scope of rml callbacks. 
                Required for similar reasons to the selfName field above
        fmtSelected -- The format for rml for entries when they are selected. Must contain %s somewhere, which
            will be replaced with entry.text for each entry.
        fmtNotSelected -- Same as above, but for unselected entries.
        """

        self.doc = doc
        self.entries = entries
        self.fmtSelected = fmtSelected
        self.fmtNotSelected = fmtNotSelected

        menuHtmlStr = ""
        for i, val in enumerate(self.entries):
            args = val["args"] if "args" in val else ""
            onclick = (val["strFunc"]+"({0})").format(args) if "strFunc" in val else ""
            entryStr = '<span id="menuEntry%05d" onmouseover="%s.setSelected(%05d)" onclick="%s">' % (i, selfName, i, onclick)
            entryStr += self.fmtNotSelected % val["text"]
            entryStr += '</span><br/>'
            menuHtmlStr += entryStr

        container = self.doc.GetElementById(containerId)
        container.inner_rml = menuHtmlStr

        self.current = 0
        self.setSelected(0)

    def getEntryElement(self, num):
        return self.doc.GetElementById('menuEntry%05d' % num)

    def setSelected(self, num):
        self.setNotSelected(self.current)

        elem = self.getEntryElement(num)
        elem.inner_rml = self.fmtSelected % self.entries[num]["text"]
        self.current = num
    
    def setNotSelected(self, num):
        elem = self.getEntryElement(num)
        elem.inner_rml = self.fmtNotSelected % self.entries[num]["text"]

    def onKeyDown(self, event):
        if event.parameters['key_identifier'] == rocket.key_identifier.DOWN:
            self.setSelected((self.current + 1) % len(self.entries))
            return True
        elif event.parameters['key_identifier'] == rocket.key_identifier.UP:
            self.setSelected((self.current - 1) % len(self.entries))
            return True
        elif event.parameters['key_identifier'] == rocket.key_identifier.RETURN:
            currentEntry = self.entries[self.current]
            if("func" in currentEntry):
                currentEntry["func"]()
            return True

        return False
