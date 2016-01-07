# This script is responsible for managing dialogues
# depending on game progress

import freeablo

# Globals

entrySelected = '<span class="pentagon_left"/>%s<span class="pentagon_right"/>'
entryNotSelected = '<span style="visibility: hidden;" class="pentagon_left"/>%s<span style="visibility: hidden;" class="pentagon_right"/>'

def talkTo(npcId):

    openingSound = ""
    rml = ""

    DIALOGUE_PATH = "resources/gui/dialogues/"

    if npcId == 'NPCsmith':
        openingSound = "sfx/Towners/Bsmith44.wav"
        rml = DIALOGUE_PATH + "NPCsmith.rml"
    elif npcId == 'NPCstorytell':
        openingSound = "sfx/Towners/storyt25.wav"
        rml = DIALOGUE_PATH + "NPCstorytell.rml"
    elif npcId == 'NPCdrunk':
        openingSound = "sfx/Towners/Drunk27.wav"
    elif npcId == 'NPChealer':
        openingSound = "sfx/Towners/Healer37.wav"
        rml = DIALOGUE_PATH + "NPChealer.rml"
    elif npcId == 'NPCboy':
        openingSound = "sfx/Towners/Pegboy32.wav"
    elif npcId == 'NPCmaid':
        openingSound = "sfx/Towners/Bmaid31.wav"
        rml = DIALOGUE_PATH + "NPCmaid.rml"


    if openingSound != "":
        freeablo.playSound(openingSound)

    if rml != "":
        freeablo.openDialogue(rml)
    else:
        print 'There is no RML for %s' % npcId

