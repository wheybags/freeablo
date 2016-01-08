# This script is responsible for managing dialogues
# depending on game progress

import freeablo
from random import randint

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
        rml = DIALOGUE_PATH + "NPCdrunk.rml"
    elif npcId == 'NPChealer':
        openingSound = "sfx/Towners/Healer37.wav"
        rml = DIALOGUE_PATH + "NPChealer.rml"
    elif npcId == 'NPCboy':
        openingSound = "sfx/Towners/Pegboy32.wav"
        rml = DIALOGUE_PATH + "NPCboy.rml"
    elif npcId == 'NPCmaid':
        openingSound = "sfx/Towners/Bmaid31.wav"
        rml = DIALOGUE_PATH + "NPCmaid.rml"
    elif npcId == 'NPCwitch':
        openingSound = "sfx/Towners/Witch38.wav"
        rml = DIALOGUE_PATH + "NPCwitch.rml"
    elif npcId == 'NPCtavern':
        openingSound = "sfx/Towners/Tavown00.wav"
    elif npcId.startswith('NPCcow'):
        number = randint(1,2)
        openingSound = "sfx/Towners/Cow" + str(number) + ".wav"
      


    if openingSound != "":
        freeablo.playSound(openingSound)

    if rml != "":
        freeablo.openDialogue(rml)
    else:
        print 'There is no RML for %s' % npcId

