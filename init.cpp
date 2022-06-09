//--------------------------------------------------------
//  Zelda Classic
//  by Jeremy Craner, 1999-2000
//
//  zquest.cc
//
//  Main code for the quest editor.
//
//--------------------------------------------------------

#include "precompiled.h" //always first

#include <map>
#include <vector>
#include <algorithm>
#include <string>

#include <stdio.h>

#include "zc_alleg.h"
#include "zdefs.h"
#include "zsys.h"
#include "init.h"
#include "zelda.h"

#define vc(x)  ((x)+224)                                    // offset to 'VGA color' x (row 14)

//using namespace std;

extern FONT *sfont2;
extern FONT *lfont;
extern FONT *pfont;
extern dmap *DMaps;
extern itemdata *itemsbuf;
extern byte quest_rules[20];
extern char *item_string[];

void getitem(int id, bool nosound);

// NOTE: This method has been severely hacked to fix an annoying problem at game start:
// items (ie the Small Wallet) which modify max counter values need to be processed after
// the values for those counters specified in init data, as the author expects these items
// to modify the max counter. BUT the counter value should NOT be updated, ie, starting with
// the bomb item does not give 8 free bombs at quest start.
// I don't like this solution one bit, but can't come up with anything better -DD

void resetItems(gamedata *game2, zinitdata *zinit2, bool lvlitems)
{
    game2->set_maxlife(zinit2->hc*HP_PER_HEART);
    game2->set_maxbombs(zinit2->max_bombs);
    game2->set_maxcounter(zinit2->max_bombs/zc_max(1,zinit2->bomb_ratio), 6);
    game2->set_maxmagic(zinit2->max_magic);
    game2->set_maxarrows(zinit2->max_arrows);
    game2->set_maxcounter(zinit2->max_rupees, 1);
    game2->set_maxcounter(zinit2->max_keys, 5);
    
    //set up the items
    for(int i=0; i<MAXITEMS; i++)
    {
        if(zinit2->items[i] && (itemsbuf[i].flags & ITEM_GAMEDATA))
        {
            if(!game2->get_item(i))
                getitem(i,true);
        }
        else
            game2->set_item_no_flush(i,false);
            
        game2->items_off[i] = 0;
        
        // Fix them DMap items
        // Since resetItems() gets called before AND after init_dmap()...
        if(get_currdmap() > -1)
            game2->items_off[i] |= DMaps[get_currdmap()].disableditems[i];
    }
    
    flushItemCache();
    
    //Then set up the counters
    game2->set_bombs(zinit2->bombs);
    
    if(zinit2->bombs > 0 && zinit2->max_bombs > 0) game2->set_item(iBombs, true);
    
    game2->set_keys(zinit2->keys);
    game2->set_sbombs(zinit2->super_bombs);
    
    if(zinit2->super_bombs > 0 && (zinit2->max_bombs/zc_max(1,zinit2->bomb_ratio)) > 0) game2->set_item(iSBomb, true);
    
    game2->set_HCpieces(zinit2->hcp);
    game2->set_rupies(zinit2->rupies);
    game2->set_hcp_per_hc(zinit2->hcp_per_hc);
    game2->set_cont_hearts(zinit2->cont_heart);
    game2->set_cont_percent(get_bit(zinit2->misc, idM_CONTPERCENT) != 0);
    
    for(int i=0; i<MAXLEVELS; i++)
    {
        // Kludge to prevent two bits (liTRIFORCE and liBOSS) which aren't
        // completely stored in Init Data, from being erased when this is run in-game.
        if(lvlitems)
            game2->lvlitems[i]=0;
        else
            game2->lvlitems[i]&=~(liMAP|liCOMPASS|liBOSSKEY| (i>0 && i<=8 ? liTRIFORCE : 0));
            
        game2->lvlitems[i]|=get_bit(zinit2->map,i)?liMAP:0;
        game2->lvlitems[i]|=get_bit(zinit2->compass,i)?liCOMPASS:0;
        game2->lvlitems[i]|=get_bit(zinit2->boss_key,i)?liBOSSKEY:0;
        game2->lvlkeys[i]=zinit2->level_keys[i];
    }
    
    for(int i=0; i<8; i++)
    {
        game2->lvlitems[i+1]|=get_bit(&zinit2->triforce,i)?liTRIFORCE:0;
    }
    
    game2->set_magic(zc_min(zinit2->magic,zinit2->max_magic));
    game2->set_magicdrainrate(get_bit(zinit2->misc,idM_DOUBLEMAGIC)?1:2);
    game2->set_canslash(get_bit(zinit2->misc,idM_CANSLASH)?1:0);
    
    game2->set_arrows(zinit2->arrows);
    
    //flush the cache again (in case bombs became illegal to use by setting bombs to 0)
    flushItemCache();
}

