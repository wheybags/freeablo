# This script is responsible for managing dialogues
# depending on game progress

import freeablo

# Globals

entrySelected = '<span class="pentagon_left"/>%s<span class="pentagon_right"/>'
entryNotSelected = '<span style="visibility: hidden;" class="pentagon_left"/>%s<span style="visibility: hidden;" class="pentagon_right"/>'

def talkTo(npcId):

    openingSound = ""
    rml = ""

    if npcId == 'NPCsmith':
        openingSound = "sfx/Towners/Bsmith44.wav"
        rml = "resources/gui/dialogues/NPCsmith.rml"
    elif npcId == 'NPCstorytell':
        openingSound = "sfx/Towners/storyt25.wav"
        rml = "resources/gui/dialogues/NPCstorytell.rml"
    elif npcId == 'NPCdrunk':
        openingSound = "sfx/Towners/Drunk27.wav"
    elif npcId == 'NPChealer':
        openingSound = "sfx/Towners/Healer37.wav"
    elif npcId == 'NPCboy':
        openingSound = "sfx/Towners/Pegboy32.wav"


    if openingSound != "":
        freeablo.playSound(openingSound)

    if rml != "":
        freeablo.openDialogue(rml)
    else:
        print 'There is no RML for %s' % npcId

