# This script is responsible for managing dialogues
# depending on game progress

import freeablo

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

