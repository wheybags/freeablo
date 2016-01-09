# This script is responsible for managing dialogues
# depending on game progress

import freeablo
from random import randint

# Globals

entrySelected = '<span class="pentagon_left"/>%s<span class="pentagon_right"/>'
entryNotSelected = '<span style="visibility: hidden;" class="pentagon_left"/>%s<span style="visibility: hidden;" class="pentagon_right"/>'
DIALOGUE_PATH = "resources/gui/dialogues/"

def openDialogue(rml, openingSound):

    global DIALOGUE_PATH

    freeablo.stopSound()

    if openingSound != "":
        freeablo.playSound(openingSound)

    if rml != "":
        rml = DIALOGUE_PATH + rml
        freeablo.openDialogue(rml)

def openScrollbox(rml, openingSound):

    global DIALOGUE_PATH

    freeablo.stopSound()

    if openingSound != "":
        freeablo.playSound(openingSound)

    if rml != "":
        rml = DIALOGUE_PATH + rml
        freeablo.openDialogueScrollbox(rml)

def talkTo(npcId):

    print npcId

    if npcId == 'NPCsmith':
        openDialogue("NPCsmith.rml","sfx/Towners/Bsmith44.wav")
    elif npcId == 'NPCstorytell':
        openDialogue("NPCstorytell.rml","sfx/Towners/storyt25.wav")
    elif npcId == 'NPCdrunk':
        openDialogue("NPCdrunk.rml","sfx/Towners/Drunk27.wav")
    elif npcId == 'NPChealer':
        openDialogue("NPChealer.rml","sfx/Towners/Healer37.wav")
    elif npcId == 'NPCboy':
        openDialogue("NPCboy.rml","sfx/Towners/Pegboy32.wav")
    elif npcId == 'NPCmaid':
        openDialogue("NPCmaid.rml","sfx/Towners/Bmaid31.wav")
    elif npcId == 'NPCwitch':
        openDialogue("NPCwitch.rml","sfx/Towners/Witch38.wav")
    elif npcId == 'NPCtavern':
        openScrollbox("NPCtavern.rml", "sfx/Towners/Tavown00.wav")
    elif npcId.startswith('NPCcow'):
        number = randint(1,2)
        openDialogue("", "sfx/Towners/Cow" + str(number) + ".wav")
