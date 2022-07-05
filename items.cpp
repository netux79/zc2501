#include "items.h"
#include "guys.h"
#include "maps.h"
#include "zelda.h"
#include "zdefs.h"

char *item_string[ITEMCNT];

extern zinitdata zinit;
extern sprite_list  guys, items;

int fairy_cnt=0;

/**********************************/
/**********  Item Class  **********/
/**********************************/

item::~item()
{
    if(itemsbuf[id].family==itype_fairy && itemsbuf[id].misc3>0 && misc>0)
        killfairy(misc);
}

bool item::animate(int)
{
    if(is_side_view())
    {
        if(can_drop(x,y) && !(pickup & ipDUMMY) && !(pickup & ipCHECK))
        {
            y+=fall/100;
            
            if((fall/100)==0 && fall>0)
                fall*=(fall>0 ? 2 : 0.5); // That oughta do something about the floatiness.
                
            if(fall <= (int)zinit.terminalv)
            {
                fall += zinit.gravity;
            }
        }
        else if(!can_drop(x,y) && !(pickup & ipDUMMY) && !(pickup & ipCHECK))
        {
            fall = -fall/2; // LA key bounce.
        }
    }
    else
    {
        z-=fall/100;
        
        if(z<0)
        {
            z = 0;
            fall = -fall/2;
        }
        else if(z <= 1 && abs(fall) < (int)zinit.gravity)
        {
            z=0;
            fall=0;
        }
        else if(fall <= (int)zinit.terminalv)
        {
            fall += zinit.gravity;
        }
    }
    
    // Maybe it fell off the bottom in sideview, or was moved by a script.
    if(y>352 || y<-176 || x<-256 || x > 512)
    {
        return true;
    }
    
    if((++clk)>=0x8000)
    {
        clk=0x7000;
    }
    
    if(flash)
    {
        cs = o_cset;
        
        if(frame&8)
        {
            cs >>= 4;
        }
        else
        {
            cs &= 15;
        }
    }
    
    if(anim)
    {
        int spd = o_speed;
        
        if(aframe==0)
        {
            spd *= o_delay+1;
        }
        
        if(++aclk >= spd)
        {
            aclk=0;
            
            if(++aframe >= frames)
            {
                aframe=0;
            }
        }
        
        //tile = o_tile + aframe;
        if(extend > 2)
        {
            if(o_tile/TILES_PER_ROW==(o_tile+txsz*aframe)/TILES_PER_ROW)
                tile=o_tile+txsz*aframe;
            else
                tile=o_tile+(txsz*aframe)+((tysz-1)*TILES_PER_ROW)*((o_tile+txsz*aframe)/TILES_PER_ROW)-(o_tile/TILES_PER_ROW);
        }
        else
            tile = o_tile + aframe;
    }
    
    if(itemsbuf[id].family ==itype_fairy && itemsbuf[id].misc3)
    {
        movefairy(x,y,misc);
    }
    
    if(fadeclk==0 && !subscreenItem)
    {
        return true;
    }
    
    if(pickup&ipTIMER)
    {
        if(++clk2 == 512)
        {
            return true;
        }
    }
    
    return false;
}

void item::draw(BITMAP *dest)
{
    if(pickup&ipNODRAW || tile==0)
        return;
        
    if(!(pickup&ipFADE) || fadeclk<0 || fadeclk&1)
    {
        if(clk2>32 || (clk2&2)==0 || itemsbuf[id].family == itype_fairy)
        {
            sprite::draw(dest);
        }
    }
}

item::item(fix X,fix Y,fix Z,int i,int p,int c, bool isDummy) : sprite()
{
    x=X;
    y=Y;
    z=Z;
    id=i;
    pickup=p;
    clk=c;
    misc=clk2=0;
    aframe=aclk=0;
    anim=flash=twohand=subscreenItem=false;
    dummy_int[0]=PriceIndex=-1;
    
    if(id<0 || id>iMax) //>, not >= for dummy items such as the HC Piece display in the subscreen
        return;
        
    o_tile = itemsbuf[id].tile;
    tile = itemsbuf[id].tile;
    cs = itemsbuf[id].csets&15;
    o_cset = itemsbuf[id].csets;
    o_speed = itemsbuf[id].speed;
    o_delay = itemsbuf[id].delay;
    frames = itemsbuf[id].frames;
    flip = itemsbuf[id].misc>>2;
    
    if(itemsbuf[id].misc&1)
        flash=true;
        
    if(itemsbuf[id].misc&2)
        twohand=true;
        
    anim = itemsbuf[id].frames>0;
    
    if(pickup&ipBIGRANGE)
    {
        hxofs=-8;
        hxsz=17;
        hyofs=-4;
        hysz=20;
    }
    else if(pickup&ipBIGTRI)
    {
        hxofs=-8;
        hxsz=28;
        hyofs=-4;
        hysz=20;
    }
    else
    {
        hxsz=1;
        hyofs=4;
        hysz=12;
    }
    
    if(!isDummy && itemsbuf[id].family == itype_fairy && itemsbuf[id].misc3)
    {
        misc = ++fairy_cnt;
        
        if(addfairy(x, y, itemsbuf[id].misc3, misc))
            sfx(itemsbuf[id].usesound);
    }
}

// easy way to draw an item

void putitem(BITMAP *dest,int x,int y,int item_id)
{
    item temp((fix)x,(fix)y,(fix)0,item_id,0,0);
    temp.yofs=0;
    temp.animate(0);
    temp.draw(dest);
}

void putitem2(BITMAP *dest,int x,int y,int item_id, int &aclk, int &aframe, int flash)
{
    item temp((fix)x,(fix)y,(fix)0,item_id,0,0,true);
    temp.yofs=0;
    temp.aclk=aclk;
    temp.aframe=aframe;
    
    if(flash)
    {
        temp.flash=(flash != 0);
    }
    
    temp.animate(0);
    temp.draw(dest);
    aclk=temp.aclk;
    aframe=temp.aframe;
}

//some methods for dealing with items
int getItemFamily(itemdata* items, int item)
{
    return items[item].family;
}

void removeItemsOfFamily(gamedata *g, itemdata *items, int family)
{
    for(int i=0; i<MAXITEMS; i++)
    {
        if(items[i].family == family)
            g->set_item(i,false);
    }
}

void removeLowerLevelItemsOfFamily(gamedata *g, itemdata *items, int family, int level)
{
    for(int i=0; i<MAXITEMS; i++)
    {
        if(items[i].family == family && items[i].fam_type < level)
            g->set_item(i, false);
    }
}

void removeItemsOfFamily(zinitdata *z, itemdata *items, int family)
{
    for(int i=0; i<MAXITEMS; i++)
    {
        if(items[i].family == family)
            z->items[i]=false;
    }
}

int getHighestLevelOfFamily(zinitdata *source, itemdata *items, int family)
{
    int result = -1;
    int highestlevel = -1;
    
    for(int i=0; i<MAXITEMS; i++)
    {
        if(items[i].family == family && source->items[i])
        {
            if(items[i].fam_type >= highestlevel)
            {
                highestlevel = items[i].fam_type;
                result=i;
            }
        }
    }
    
    return result;
}

int getHighestLevelOfFamily(gamedata *source, itemdata *items, int family, bool checkenabled)
{
    int result = -1;
    int highestlevel = -1;
    
    for(int i=0; i<MAXITEMS; i++)
    {
        if(items[i].family == family && source->get_item(i) && (checkenabled?(!(source->items_off[i])):1))
        {
            if(items[i].fam_type >= highestlevel)
            {
                highestlevel = items[i].fam_type;
                result=i;
            }
        }
    }
    
    return result;
}

int getItemID(itemdata *items, int family, int level)
{
    if(level<0) return getCanonicalItemID(items, family);
    
    for(int i=0; i<MAXITEMS; i++)
    {
        if(items[i].family == family && items[i].fam_type == level)
            return i;
    }
    
    return -1;
}

int getItemIDPower(itemdata *items, int family, int power)
{
    for(int i=0; i<MAXITEMS; i++)
    {
        if(items[i].family == family && items[i].power == power)
            return i;
    }
    
    return -1;
}

/* Retrieves the canonical item of a given item family, the item with least non-0 level */
int getCanonicalItemID(itemdata *items, int family)
{
    int lowestid = -1;
    int lowestlevel = -1;
    
    for(int i=0; i<MAXITEMS; i++)
    {
        if(items[i].family == family && (items[i].fam_type < lowestlevel || lowestlevel == -1))
        {
            lowestlevel = items[i].fam_type;
            lowestid = i;
        }
    }
    
    return lowestid;
}

void addOldStyleFamily(zinitdata *dest, itemdata *items, int family, char levels)
{
    for(int i=0; i<8; i++)
    {
        if(levels & (1<<i))
        {
            int id = getItemID(items, family, i+1);
            
            if(id != -1)
                dest->items[id]=true;
        }
    }
}

int computeOldStyleBitfield(zinitdata *source, itemdata *items, int family)
{
    int rval=0;
    
    for(int i=0; i<MAXITEMS; i++)
    {
        if(items[i].family == family && source->items[i])
        {
            if(items[i].fam_type > 0)
                rval |= 1<<(items[i].fam_type-1);
        }
    }
    
    return rval;
}

bool is_side_view()
{
    return (tmpscr->flags7&fSIDEVIEW)!=0;
}

int select_dropitem(int item_set, int x, int y)
{
    int total_chance=0;
    
    for(int k=0; k<11; ++k)
    {
        int current_chance=item_drop_sets[item_set].chance[k];
        
        if(k>0)
        {
            int current_item=item_drop_sets[item_set].item[k-1];
            
            if((!get_bit(quest_rules,qr_ENABLEMAGIC)||(game->get_maxmagic()<=0))&&(itemsbuf[current_item].family == itype_magic))
            {
                current_chance=0;
            }
            
            if((!get_bit(quest_rules,qr_TRUEARROWS))&&(itemsbuf[current_item].family == itype_arrowammo))
            {
                current_chance=0;
            }
        }
        
        total_chance+=current_chance;
    }
    
    if(total_chance==0)
        return -1;
        
    int item_chance=(rand()%total_chance)+1;
    
    int drop_item=-1;
    
    for(int k=10; k>=0; --k)
    {
    
        int current_chance=item_drop_sets[item_set].chance[k];
        int current_item=(k==0 ? -1 : item_drop_sets[item_set].item[k-1]);
        
        if((!get_bit(quest_rules,qr_ENABLEMAGIC)||(game->get_maxmagic()<=0))&&(current_item>=0&&itemsbuf[current_item].family == itype_magic))
        {
            current_chance=0;
        }
        
        if((!get_bit(quest_rules,qr_TRUEARROWS))&&(current_item>=0&&itemsbuf[current_item].family == itype_arrowammo))
        {
            current_chance=0;
        }
        
        if(current_chance>0&&item_chance<=current_chance)
        {
            drop_item=current_item;
            break;
        }
        else
        {
            item_chance-=current_chance;
        }
    }
    
    if(drop_item>=0 && itemsbuf[drop_item].family==itype_fairy)
    {
        for(int j=0; j<items.Count(); ++j)
        {
            if((itemsbuf[items.spr(j)->id].family==itype_fairy)&&((abs(items.spr(j)->x-x)<32)||(abs(items.spr(j)->y-y)<32)))
            {
                drop_item=-1;
                break;
            }
        }
    }
    
    return drop_item;
}

bool can_drop(fix x, fix y)
{
    return !_walkflag(x,y+16,0);
}

bool addfairy(fix x, fix y, int misc3, int id)
{
    addenemy(x,y,eITEMFAIRY,id);
    ((enemy*)guys.spr(guys.Count()-1))->dstep=misc3;
    ((enemy*)guys.spr(guys.Count()-1))->step=(misc3/100.0);
    movefairy(x,y,id);
    return true;
}

const char *old_item_string[iLast] =
{
    "Rupee (1)", "Rupee (5)", "Heart", "Bomb (Normal)", "Clock",
    "Sword 1 (Wooden)", "Sword 2 (White)", "Sword 3 (Magic)", "Shield 2 (Magic)", "Key (Normal)",
    "Candle 1 (Blue)", "Candle 2 (Red)", "Letter", "Arrow 1 (Wooden)", "Arrow 2 (Silver)",
    "Bow 1 (Short)", "Bait", "Ring 1 (Blue)", "Ring 2 (Red)", "Bracelet 2",
    "Triforce (Fragment)", "Map", "Compass", "Boomerang 1 (Wooden)", "Boomerang 2 (Magic)",
    "Wand", "Raft", "Ladder 1", "Heart Container", "Potion 1 (Blue)",
    "Potion 2 (Red)", "Whistle", "Magic Book", "Key (Magic)", "Fairy (Moving)",
    "Boomerang 3 (Fire)", "Sword 4 (Master)", "Shield 3 (Mirror)", "Rupee (20)", "Rupee (50)",
    "Rupee (200)", "Wallet 1 (500)", "Wallet 2 (999)", "Dust Pile",
    "Triforce (Whole)", "Selection (1)", "Misc 1", "Misc 2", "Bomb (Super)","Heart Container Piece",
    "Amulet 1", "Flippers", "Hookshot 1 (Short)", "Lens of Truth", "Hammer", "Boots",
    "Bracelet 3", "Arrow 3 (Golden)", "Magic Container", "Magic Jar 1 (Small)",
    "Magic Jar 2 (Large)", "Ring 3 (Golden)", "Kill All Enemies", "Amulet 2",
    "Din's Fire", "Farore's Wind", "Nayru's Love", "Key (Boss)", "Bow 2 (Long)", "Fairy (Stationary)",
    "Arrow Ammunition (1)", "Arrow Ammunition (5)", "Arrow Ammunition (10)", "Arrow Ammunition (30)",
    "Quiver 1 (Small)", "Quiver 2 (Medium)", "Quiver 3 (Large)", "Bomb Ammunition (1)", "Bomb Ammunition (4)", "Bomb Ammunition (8)", "Bomb Ammunition (30)",
    "Bomb Bag 1 (Small)", "Bomb Bag 2 (Medium)", "Bomb Bag 3 (Large)", "Key (Level Specific)", "Selection (2)", "Rupee (10)", "Rupee (100)", "Cane of Byrna",
    "Hookshot 2 (Long)", "Letter (used)", "Roc's Feather", "Hover Boots","Shield 1 (Small)","Scroll: Spin Attack", "Scroll: Cross Beams", "Scroll: Quake Hammer",
    "Scroll: Super Quake", "Scroll: Hurricane Spin", "Whisp Ring 1", "Whisp Ring 2", "Charge Ring 1", "Charge Ring 2", "Scroll: Peril Beam", "Wallet 3 (Magic)",
    "Quiver 4 (Magic)", "Bomb Bag 4 (Magic)", "Bracelet 1 (Worn-out Glove)", "Ladder 2 (Four-Way)", "Wealth Medal 1 (75%)", "Wealth Medal 2 (50%)",
    "Wealth Medal 3 (25%)", "Heart Ring 1 (Slow)", "Heart Ring 2", "Heart Ring 3 (Fast)", "Magic Ring 1 (Slow)", "Magic Ring 2", "Magic Ring 3 (Fast)",
    "Magic Ring 4 (Light Force)", "Stone of Agony", "Stomp Boots", "Peril Ring", "Whimsical Ring",
    "Custom Item 01", "Custom Item 02", "Custom Item 03", "Custom Item 04", "Custom Item 05",
    "Custom Item 06", "Custom Item 07", "Custom Item 08", "Custom Item 09", "Custom Item 10",
    "Custom Item 11", "Custom Item 12", "Custom Item 13", "Custom Item 14", "Custom Item 15",
    "Custom Item 16", "Custom Item 17", "Custom Item 18", "Custom Item 19", "Custom Item 20"
};
