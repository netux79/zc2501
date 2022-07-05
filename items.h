#ifndef _ITEMS_H_
#define _ITEMS_H_

#include "sprite.h"
#include "zdefs.h"


extern char *item_string[ITEMCNT];

extern int fairy_cnt;
void movefairy(fix &x,fix &y,int misc);
void killfairy(int misc);
int addenemy(int x,int y,int id,int clk);
bool addfairy(fix x, fix y, int misc3, int id);
bool can_drop(fix x, fix y);
int select_dropitem(int item_set, int x, int y);
bool is_side_view();

class item : public sprite
{
public:
    int pickup,clk2;
    int aclk,aframe;
    char PriceIndex;
    bool flash,twohand,anim, subscreenItem;
    int o_tile,o_cset, o_speed, o_delay, frames;
    item(fix X,fix Y,fix Z,int i,int p,int c, bool isDummy = false);
    virtual ~item();
    virtual bool animate(int index);
    virtual void draw(BITMAP *dest);
};

// easy way to draw an item
void putitem(BITMAP *dest,int x,int y,int item_id);
void putitem2(BITMAP *dest,int x,int y,int item_id, int &aclk, int &aframe, int flash);
#endif
