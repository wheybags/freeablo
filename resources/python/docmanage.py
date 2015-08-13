import rocket
import freeablo
import freeablo_input
from collections import namedtuple
from singleton import Singleton
class DocManager(object):
    __metaclass__ = Singleton

    def __init__(self):

        context = rocket.contexts['default']

        self.guiWasClicked = False
        self.docs = {}
        self.paused = False
        self.pauseHiddenDocs = []
        self.pauseHandle = "resources/gui/pausemenu.rml"
        self.loadDoc(self.pauseHandle)

    def showDoc(self, docpath):
        handle = self.docs[docpath]
        handle["doc"].Show()
        handle["visible"] = True

    def hideDoc(self, docpath):
        handle = self.docs[docpath]
        handle["doc"].Hide()
        handle["visible"] = False

    def toggleDoc(self, docpath):
        if self.docs[docpath]["visible"]:
            self.hideDoc(docpath)
        else:
            self.showDoc(docpath)

    def loadDoc(self, docpath):
        if not docpath in self.docs:
            context = rocket.contexts["default"]
            newHandle = {"doc": context.LoadDocument(docpath), "visible": False}
            self.docs[docpath] = newHandle
        
    def reloadDoc(self, docpath):
        self.closeDoc(docpath)
        self.loadDoc(docpath)
        
    def getDoc(self, docpath):
        handle = self.docs[docpath]
        return handle["doc"]
    
    def closeDoc(self, docpath):
        handle = self.docs[docpath]
        handle["doc"].Close()
        del self.docs[docpath]

    def pause(self):
        for docpath in self.docs:
            handle = self.docs[docpath]
            if handle["visible"]:
                self.toggleDoc(docpath)
                self.pauseHiddenDocs.append(docpath)

        self.paused = True
        self.toggleDoc(self.pauseHandle)
        freeablo.pause()

    def unpause(self):
        self.toggleDoc(self.pauseHandle)

        for docpath in self.pauseHiddenDocs:
            self.toggleDoc(docpath)

        self.pauseHiddenDocs = []

        self.paused = False
        freeablo.unpause()

    def togglePause(self):
        if self.paused:
            self.unpause()
        else:
            self.pause()

    def onKeyDown(self, event):
        if event.parameters['key_identifier'] == rocket.key_identifier.ESCAPE:
            self.togglePause()

    def bodyClicked(self):
        # librocket generates click events in reverse hierarchical order, so
        # if there is an element inside body and that is clicked, we get an event for
        # that first, then for body. We can use this to find when body is clicked
        # outside the inner element by tracking the inner elements clicked status,
        # as has been done here. When the background (ie, body) is clicked, we send
        # a signal to the engine that this click was not on the gui, so it is for
        # the engine to process. resources/gui/base.rml takes care of the case where
        # the click occurs outside the boundaries of any currently visible rml documents
        if not self.guiWasClicked:
            freeablo_input.baseClicked()

        self.guiWasClicked = False

    def guiClicked(self):
	
        self.guiWasClicked = True


def init():
    global manager
    manager = DocManager()

