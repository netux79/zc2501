#include "zdefs.h"
#include "sprite.h"
#include "zelda.h"
#include "maps.h"
#include "tiles.h"

/**********************************/
/******* Sprite Base Class ********/
/**********************************/

sprite::sprite()
{
    uid = getNextUID();
    x=y=z=tile=shadowtile=cs=flip=c_clk=clk=xofs=yofs=zofs=hxofs=hyofs=fall=0;
    txsz=1;
    tysz=1;
    id=-1;
    hxsz=hysz=16;
    hzsz=1;
    yofs=PLAYFIELD_OFFSET;
    dir=down;
    angular=canfreeze=false;
    drawstyle=0;
    extend=0;
    lasthitclk=0;
    lasthit=0;
    angle=0;
    misc=0;
    
    for(int i=0; i<10; i++)
    {
        dummy_int[i]=0;
        dummy_fix[i]=0;
        dummy_float[i]=0;
        dummy_bool[i]=0;
    }
    
    for(int i=0; i<16; i++) miscellaneous[i] = 0;
    
    scriptcoldet = 1;
}

sprite::sprite(sprite const & other):
    x(other.x),
    y(other.y),
    z(other.z),
    fall(other.fall),
    tile(other.tile),
    shadowtile(other.shadowtile),
    cs(other.cs),
    flip(other.flip),
    c_clk(other.c_clk),
    clk(other.clk),
    misc(other.misc),
    xofs(other.xofs),
    yofs(other.yofs),
    zofs(other.zofs),
    hxofs(other.hxofs),
    hyofs(other.hyofs),
    hxsz(other.hxsz),
    hysz(other.hysz),
    hzsz(other.hzsz),
    txsz(other.txsz),
    tysz(other.tysz),
    id(other.id),
    dir(other.dir),
    angular(other.angular),
    canfreeze(other.canfreeze),
    angle(other.angle),
    lasthit(other.lasthit),
    lasthitclk(other.lasthitclk),
    drawstyle(other.drawstyle),
    extend(other.extend)
{
    uid = getNextUID();
    
    for(int i=0; i<10; ++i)
    {
        dummy_int[i]=other.dummy_int[i];
        dummy_fix[i]=other.dummy_fix[i];
        dummy_float[i]=other.dummy_float[i];
        dummy_bool[i]=other.dummy_bool[i];
    }
    
    for(int i=0; i<16; i++) miscellaneous[i] = other.miscellaneous[i];
    
    scriptcoldet = other.scriptcoldet;
}

sprite::sprite(fix X,fix Y,int T,int CS,int F,int Clk,int Yofs):
    x(X),y(Y),tile(T),cs(CS),flip(F),clk(Clk),yofs(Yofs)
{
    uid = getNextUID();
    hxsz=hysz=16;
    hxofs=hyofs=xofs=0;
    txsz=1;
    tysz=1;
    id=-1;
    dir=down;
    angular=canfreeze=false;
    extend=0;
    
    for(int i=0; i<16; i++) miscellaneous[i] = 0;
    
    scriptcoldet = 1;
    
    drawstyle=0;
    lasthitclk=0;
    lasthit=0;
    angle=0;
    hzsz=1;
    misc=0;
    c_clk=0;
    shadowtile=0;
}

sprite::~sprite()
{
}

long sprite::getNextUID()
{
    static long nextid = 0;
    return nextid++;
}

void sprite::draw2(BITMAP *)                            // top layer for special needs
{
}

void sprite::drawcloaked2(BITMAP *)                     // top layer for special needs
{
}

bool sprite::animate(int)
{
    ++c_clk;
    return false;
}
int sprite::real_x(fix fx)
{
    int rx=fx.v>>16;
    
    switch(dir)
    {
    case 9:
    case 13:
        if(fx.v&0xFFFF)
            ++rx;
            
        break;
    }
    
    return rx;
}

int sprite::real_y(fix fy)
{
    return fy.v>>16;
}

int sprite::real_z(fix fz)
{
    return fz.v>>16;
}

bool sprite::hit(sprite *s)
{
    if(!(scriptcoldet&1)) return false;
    
    if(id<0 || s->id<0 || clk<0) return false;
    
    return hit(s->x+s->hxofs,s->y+s->hyofs,s->z+s->zofs,s->hxsz,s->hysz,s->hzsz);
}

bool sprite::hit(int tx,int ty,int tz,int txsz2,int tysz2,int tzsz2)
{
    if(!(scriptcoldet&1)) return false;
    
    if(id<0 || clk<0) return false;
    
    return tx+txsz2>x+hxofs &&
           ty+tysz2>y+hyofs &&
           tz+tzsz2>z+zofs &&
           
           tx<x+hxofs+hxsz &&
           ty<y+hyofs+hysz &&
           tz<z+zofs+hzsz;
}

int sprite::hitdir(int tx,int ty,int txsz2,int tysz2,int dir2)
{
    if(!(scriptcoldet&1)) return 0xFF;
    
    int cx1=x+hxofs+(hxsz>>1);
    int cy1=y+hyofs+(hysz>>1);
    int cx2=tx+(txsz2>>1);
    int cy2=ty+(tysz2>>1);
    
    if(dir2>=left && abs(cy1-cy2)<=8)
        return (cx2-cx1<0)?left:right;
        
    return (cy2-cy1<0)?up:down;
}

void sprite::move(fix dx,fix dy)
{
    x+=dx;
    y+=dy;
}

void sprite::move(fix s)
{
    if(angular)
    {
        x += cos(angle)*s;
        y += sin(angle)*s;
        return;
    }
    
    switch(dir)
    {
    case 8:
    case up:
        y-=s;
        break;
        
    case 12:
    case down:
        y+=s;
        break;
        
    case 14:
    case left:
        x-=s;
        break;
        
    case 10:
    case right:
        x+=s;
        break;
        
    case 15:
    case l_up:
        x-=s;
        y-=s;
        break;
        
    case 9:
    case r_up:
        x+=s;
        y-=s;
        break;
        
    case 13:
    case l_down:
        x-=s;
        y+=s;
        break;
        
    case 11:
    case r_down:
        x+=s;
        y+=s;
        break;
        
    case -1:
        break;
    }
}

void sprite::draw(BITMAP* dest)
{
    int sx = real_x(x+xofs);
    int sy = real_y(y+yofs)-real_z(z+zofs);
    
    if(id<0)
        return;
        
    int e = extend>=3 ? 3 : extend;
    
    if(clk>=0)
    {
        switch(e)
        {
            BITMAP *temp;
            
        case 1:
            temp = create_bitmap_ex(8,16,32);
            blit(dest, temp, sx, sy-16, 0, 0, 16, 32);
            
            if(drawstyle==0 || drawstyle==3)
            {
                overtile16(temp,tile-TILES_PER_ROW,0,0,cs,flip);
                overtile16(temp,tile,0,16,cs,flip);
            }
            
            if(drawstyle==1)
            {
                overtiletranslucent16(temp,tile-TILES_PER_ROW,0,0,cs,flip,128);
                overtiletranslucent16(temp,tile,0,16,cs,flip,128);
            }
            
            if(drawstyle==2)
            {
                overtilecloaked16(temp,tile-TILES_PER_ROW,0,0,flip);
                overtilecloaked16(temp,tile,0,16,flip);
            }
            
            masked_blit(temp, dest, 0, 0, sx, sy-16, 16, 32);
            destroy_bitmap(temp);
            break;
            
        case 2:
            temp = create_bitmap_ex(8,48,32);
            blit(dest, temp, sx-16, sy-16, 0, 0, 48, 32);
            
            if(drawstyle==0 || drawstyle==3)
            {
                overtile16(temp,tile-TILES_PER_ROW,16,0,cs,flip);
                overtile16(temp,tile-TILES_PER_ROW-(flip?-1:1),0,0,cs,flip);
                overtile16(temp,tile-TILES_PER_ROW+(flip?-1:1),32,0,cs,flip);
                overtile16(temp,tile,16,16,cs,flip);
                overtile16(temp,tile-(flip?-1:1),0,16,cs,flip);
                overtile16(temp,tile+(flip?-1:1),32,16,cs,flip);
            }
            
            if(drawstyle==1)
            {
                overtiletranslucent16(temp,tile-TILES_PER_ROW,16,0,cs,flip,128);
                overtiletranslucent16(temp,tile-TILES_PER_ROW-(flip?-1:1),0,0,cs,flip,128);
                overtiletranslucent16(temp,tile-TILES_PER_ROW+(flip?-1:1),32,0,cs,flip,128);
                overtiletranslucent16(temp,tile,16,16,cs,flip,128);
                overtiletranslucent16(temp,tile-(flip?-1:1),0,16,cs,flip,128);
                overtiletranslucent16(temp,tile+(flip?-1:1),32,16,cs,flip,128);
            }
            
            if(drawstyle==2)
            {
                overtilecloaked16(temp,tile-TILES_PER_ROW,16,0,flip);
                overtilecloaked16(temp,tile-TILES_PER_ROW-(flip?-1:1),0,0,flip);
                overtilecloaked16(temp,tile-TILES_PER_ROW+(flip?-1:1),32,0,flip);
                overtilecloaked16(temp,tile,16,16,flip);
                overtilecloaked16(temp,tile-(flip?-1:1),0,16,flip);
                overtilecloaked16(temp,tile+(flip?-1:1),32,16,flip);
            }
            
            masked_blit(temp, dest, 8, 0, sx-8, sy-16, 32, 32);
            destroy_bitmap(temp);
            break;
            
        case 3:
        {
            int tileToDraw;
            
            switch(flip)
            {
            case 1:
                for(int i=0; i<tysz; i++)
                {
                    for(int j=txsz-1; j>=0; j--)
                    {
                        tileToDraw=tile+(i*TILES_PER_ROW)+j;
                        
                        if(tileToDraw%TILES_PER_ROW<j) // Wrapped around
                            tileToDraw+=TILES_PER_ROW*(tysz-1);
                            
                        if(drawstyle==0 || drawstyle==3) overtile16(dest,tileToDraw,sx+(txsz-j-1)*16,sy+i*16,cs,flip);
                        else if(drawstyle==1) overtiletranslucent16(dest,tileToDraw,sx+(txsz-j-1)*16,sy+i*16,cs,flip,128);
                        else if(drawstyle==2) overtilecloaked16(dest,tileToDraw,sx+(txsz-j-1)*16,sy+i*16,flip);
                    }
                }
                
                break;
                
            case 2:
                for(int i=tysz-1; i>=0; i--)
                {
                    for(int j=0; j<txsz; j++)
                    {
                        tileToDraw=tile+(i*TILES_PER_ROW)+j;
                        
                        if(tileToDraw%TILES_PER_ROW<j)
                            tileToDraw+=TILES_PER_ROW*(tysz-1);
                            
                        if(drawstyle==0 || drawstyle==3) overtile16(dest,tileToDraw,sx+j*16,sy+(tysz-i-1)*16,cs,flip);
                        else if(drawstyle==1) overtiletranslucent16(dest,tileToDraw,sx+j*16,sy+(tysz-i-1)*16,cs,flip,128);
                        else if(drawstyle==2) overtilecloaked16(dest,tileToDraw,sx+j*16,sy+(tysz-i-1)*16,flip);
                    }
                }
                
                break;
                
            case 3:
                for(int i=tysz-1; i>=0; i--)
                {
                    for(int j=txsz-1; j>=0; j--)
                    {
                        tileToDraw=tile+(i*TILES_PER_ROW)+j;
                        
                        if(tileToDraw%TILES_PER_ROW<j)
                            tileToDraw+=TILES_PER_ROW*(tysz-1);
                            
                        if(drawstyle==0 || drawstyle==3) overtile16(dest,tileToDraw,sx+(txsz-j-1)*16,sy+(tysz-i-1)*16,cs,flip);
                        else if(drawstyle==1) overtiletranslucent16(dest,tileToDraw,sx+(txsz-j-1)*16,sy+(tysz-i-1)*16,cs,flip,128);
                        else if(drawstyle==2) overtilecloaked16(dest,tileToDraw,sx+(txsz-j-1)*16,sy+(tysz-i-1)*16,flip);
                    }
                }
                
                break;
                
            case 0:
                for(int i=0; i<tysz; i++)
                {
                    for(int j=0; j<txsz; j++)
                    {
                        tileToDraw=tile+(i*TILES_PER_ROW)+j;
                        
                        if(tileToDraw%TILES_PER_ROW<j)
                            tileToDraw+=TILES_PER_ROW*(tysz-1);
                            
                        if(drawstyle==0 || drawstyle==3) overtile16(dest,tileToDraw,sx+j*16,sy+i*16,cs,flip);
                        else if(drawstyle==1) overtiletranslucent16(dest,tileToDraw,sx+j*16,sy+i*16,cs,flip,128);
                        else if(drawstyle==2) overtilecloaked16(dest,tileToDraw,sx+j*16,sy+i*16,flip);
                    }
                }
                
                break;
            }
            
            case 0:
            default:
                if(drawstyle==0 || drawstyle==3)
                    overtile16(dest,tile,sx,sy,cs,flip);
                else if(drawstyle==1)
                    overtiletranslucent16(dest,tile,sx,sy,cs,flip,128);
                else if(drawstyle==2)
                    overtilecloaked16(dest,tile,sx,sy,flip);
                    
                break;
            }
            break;
        }
    }
    else
    {
        if(e!=3)
        {
            int t  = wpnsbuf[iwSpawn].tile;
            int cs2 = wpnsbuf[iwSpawn].csets&15;
            
            if(BSZ)
            {
                if(clk>=-10) ++t;
                
                if(clk>=-5) ++t;
            }
            else
            {
                if(clk>=-12) ++t;
                
                if(clk>=-6) ++t;
            }
            
            overtile16(dest,t,sx,sy,cs2,0);
        }
        else
        {
            sprite w((fix)sx,(fix)sy,wpnsbuf[extend].tile,wpnsbuf[extend].csets&15,0,0,0);
            w.xofs = xofs;
            w.yofs = yofs;
            w.zofs = zofs;
            w.txsz = txsz;
            w.tysz = tysz;
            w.extend = 3;
            
            if(BSZ)
            {
                if(clk>=-10)
                {
                    if(tile/TILES_PER_ROW==(tile+txsz)/TILES_PER_ROW)
                        w.tile+=txsz;
                    else
                        w.tile+=txsz+(tysz-1)*TILES_PER_ROW;
                }
                
                if(clk>=-5)
                {
                    if(tile/TILES_PER_ROW==(tile+txsz)/TILES_PER_ROW)
                        w.tile+=txsz;
                    else
                        w.tile+=txsz+(tysz-1)*TILES_PER_ROW;
                }
            }
            else
            {
                if(clk>=-12)
                {
                    if(tile/TILES_PER_ROW==(tile+txsz)/TILES_PER_ROW)
                        w.tile+=txsz;
                    else
                        w.tile+=txsz+(tysz-1)*TILES_PER_ROW;
                }
                
                if(clk>=-6)
                {
                    if(tile/TILES_PER_ROW==(tile+txsz)/TILES_PER_ROW)
                        w.tile+=txsz;
                    else
                        w.tile+=txsz+(tysz-1)*TILES_PER_ROW;
                }
            }
            
            w.draw(dest);
        }
    }
}

void sprite::draw8(BITMAP* dest)
{
    int sx = real_x(x+xofs);
    int sy = real_y(y+yofs)-real_z(z+zofs);
    
    if(id<0)
        return;
        
    if(clk>=0)
    {
        switch(drawstyle)
        {
        case 0:                                               //normal
            overtile8(dest,tile,sx,sy,cs,flip);
            break;
            
        case 1:                                               //phantom
            overtiletranslucent8(dest,tile,sx,sy,cs,flip,128);
            break;
        }
    }
}

void sprite::drawcloaked(BITMAP* dest)
{
    int sx = real_x(x+xofs);
    int sy = real_y(y+yofs)-real_z(z+zofs);
    
    if(id<0)
        return;
        
    if(clk>=0)
    {
        overtilecloaked16(dest,tile,sx,sy,flip);
    }
    else
    {
        int t  = wpnsbuf[iwSpawn].tile;
        int cs2 = wpnsbuf[iwSpawn].csets&15;
        
        if(BSZ)
        {
            if(clk>=-10) ++t;
            
            if(clk>=-5) ++t;
        }
        else
        {
            if(clk>=-12) ++t;
            
            if(clk>=-6) ++t;
        }
        
        overtile16(dest,t,x,sy,cs2,0);
    }
}

void sprite::drawshadow(BITMAP* dest,bool translucent)
{
    if(extend == 4 || shadowtile==0 || id<0)
    {
        return;
    }
    
    int shadowcs = wpnsbuf[iwShadow].csets & 0xFFFF;
    int shadowflip = wpnsbuf[iwShadow].misc & 0xFF;
    
    int sx = real_x(x+xofs)+(txsz-1)*8;
    int sy = real_y(y+yofs+(tysz-1)*16);
    
    if(clk>=0)
    {
        if(translucent)
        {
            overtiletranslucent16(dest,shadowtile,sx,sy,shadowcs,shadowflip,128);
        }
        else
        {
            overtile16(dest,shadowtile,sx,sy,shadowcs,shadowflip);
        }
    }
}

/***************************************************************************/

/**********************************/
/********** Sprite List ***********/
/**********************************/

#define SLMAX 255

//class enemy;

sprite_list::sprite_list() : count(0) {}
void sprite_list::clear()
{
    while(count>0) del(0);
}

sprite *sprite_list::spr(int index)
{
    if(index<0 || index>=count)
        return NULL;
        
    return sprites[index];
}

bool sprite_list::swap(int a,int b)
{
    if(a<0 || a>=count || b<0 || b>=count)
        return false;
        
    sprite *c = sprites[a];
    sprites[a] = sprites[b];
    sprites[b] = c;
    containedUIDs[sprites[a]->getUID()] = a;
    containedUIDs[sprites[b]->getUID()] = b;
    return true;
}

bool sprite_list::add(sprite *s)
{
    if(count>=SLMAX)
    {
        delete s;
        return false;
    }
    
    containedUIDs[s->getUID()] = count;
    sprites[count++]=s;
    return true;
}

bool sprite_list::remove(sprite *s)
// removes pointer from list but doesn't delete it
{
    map<long, int>::iterator it = containedUIDs.find(s->getUID());
    
    if(it != containedUIDs.end())
        containedUIDs.erase(it);
        
    int j=0;
    
    for(; j<count; j++)
        if(sprites[j] == s)
            goto gotit;
            
    return false;
    
gotit:

    for(int i=j; i<count-1; i++)
    {
        sprites[i]=sprites[i+1];
        containedUIDs[sprites[i]->getUID()] = i;
    }
    
    --count;
    return true;
}

fix sprite_list::getX(int j)
{
    if((j>=count)||(j<0))
    {
        return (fix)1000000;
    }
    
    return sprites[j]->x;
}

fix sprite_list::getY(int j)
{
    if((j>=count)||(j<0))
    {
        return (fix)1000000;
    }
    
    return sprites[j]->y;
}

int sprite_list::getID(int j)
{
    if((j>=count)||(j<0))
    {
        return -1;
    }
    
    return sprites[j]->id;
}

int sprite_list::getMisc(int j)
{
    if((j>=count)||(j<0))
    {
        return -1;
    }
    
    return sprites[j]->misc;
}

bool sprite_list::del(int j)
{
    if(j<0||j>=count)
        return false;
        
    map<long, int>::iterator it = containedUIDs.find(sprites[j]->getUID());
    
    if(it != containedUIDs.end())
        containedUIDs.erase(it);
        
    delete sprites[j];
    
    for(int i=j; i<count-1; i++)
    {
        sprites[i]=sprites[i+1];
        containedUIDs[sprites[i]->getUID()] = i;
    }
    
    --count;
    return true;
}

void sprite_list::draw(BITMAP* dest,bool lowfirst)
{
    switch(lowfirst)
    {
    case true:
        for(int i=0; i<count; i++)
        {
            sprites[i]->draw(dest);
        }
        
        break;
        
    case false:
        for(int i=count-1; i>=0; i--)
        {
            sprites[i]->draw(dest);
        }
        
        break;
    }
}

void sprite_list::drawshadow(BITMAP* dest,bool translucent, bool lowfirst)
{
    switch(lowfirst)
    {
    case true:
        for(int i=0; i<count; i++)
            sprites[i]->drawshadow(dest,translucent);
            
        break;
        
    case false:
        for(int i=count-1; i>=0; i--)
            sprites[i]->drawshadow(dest,translucent);
            
        break;
    }
}

void sprite_list::draw2(BITMAP* dest,bool lowfirst)
{
    switch(lowfirst)
    {
    case true:
        for(int i=0; i<count; i++)
            sprites[i]->draw2(dest);
            
        break;
        
    case false:
        for(int i=count-1; i>=0; i--)
            sprites[i]->draw2(dest);
            
        break;
    }
}

void sprite_list::drawcloaked2(BITMAP* dest,bool lowfirst)
{
    switch(lowfirst)
    {
    case true:
        for(int i=0; i<count; i++)
            sprites[i]->drawcloaked2(dest);
            
        break;
        
    case false:
    
        for(int i=count-1; i>=0; i--)
            sprites[i]->drawcloaked2(dest);
            
        break;
    }
}

void sprite_list::animate()
{
    int i=0;
    
    while(i<count)
    {
        if(!(freeze_guys && sprites[i]->canfreeze))
        {
            if(sprites[i]->animate(i))
            {
                del(i);
                --i;
            }
        }
        
        ++i;
    }
}

void sprite_list::check_conveyor()
{
    int i=0;
    
    while(i<count)
    {
        sprites[i]->check_conveyor();
        ++i;
    }
}

int sprite_list::Count()
{
    return count;
}

int sprite_list::hit(sprite *s)
{
    for(int i=0; i<count; i++)
        if(sprites[i]->hit(s))
            return i;
            
    return -1;
}

int sprite_list::hit(int x,int y,int z, int xsize, int ysize, int zsize)
{
    for(int i=0; i<count; i++)
        if(sprites[i]->hit(x,y,z,xsize,ysize,zsize))
            return i;
            
    return -1;
}

// returns the number of sprites with matching id
int sprite_list::idCount(int id, int mask)
{
    int c=0;
    
    for(int i=0; i<count; i++)
    {
        if(((sprites[i]->id)&mask) == (id&mask))
        {
            ++c;
        }
    }
    
    return c;
}

// returns index of first sprite with matching id, -1 if none found
int sprite_list::idFirst(int id, int mask)
{
    for(int i=0; i<count; i++)
    {
        if(((sprites[i]->id)&mask) == (id&mask))
        {
            return i;
        }
    }
    
    return -1;
}

// returns index of last sprite with matching id, -1 if none found
int sprite_list::idLast(int id, int mask)
{
    for(int i=count-1; i>=0; i--)
    {
        if(((sprites[i]->id)&mask) == (id&mask))
        {
            return i;
        }
    }
    
    return -1;
}

// returns the number of sprites with matching id
int sprite_list::idCount(int id)
{
    return idCount(id,0xFFFF);
}

// returns index of first sprite with matching id, -1 if none found
int sprite_list::idFirst(int id)
{
    return idFirst(id,0xFFFF);
}

// returns index of last sprite with matching id, -1 if none found
int sprite_list::idLast(int id)
{
    return idLast(id,0xFFFF);
}

sprite * sprite_list::getByUID(long uid)
{
    map<long, int>::iterator it = containedUIDs.find(uid);
    
    if(it != containedUIDs.end())
        return spr(it->second);
        
    return NULL;
}

/**********************************/
/********** Moving Block **********/
/**********************************/

movingblock::movingblock() : sprite()
{
    id=1;
}

void movingblock::draw(BITMAP *dest)
{
    if(clk)
    {
        //    sprite::draw(dest);
        overcombo(dest,real_x(x+xofs),real_y(y+yofs),bcombo,cs);
    }
}

void sprite::check_conveyor()
{
    int deltax=0;
    int deltay=0;
    
    if(conveyclk<=0 && (z==0 || (tmpscr->flags2&fAIRCOMBOS)))
    {
        int ctype=(combobuf[MAPCOMBO(x+8,y+8)].type);
        deltax=combo_class_buf[ctype].conveyor_x_speed;
        deltay=combo_class_buf[ctype].conveyor_y_speed;
        
        if(deltax!=0||deltay!=0)
        {
            if(deltay<0&&!_walkflag(x,y+8-2,2))
            {
                y=y-abs(deltay);
            }
            else if(deltay>0&&!_walkflag(x,y+15+2,2))
            {
                y=y+abs(deltay);
            }
            
            if(deltax<0&&!_walkflag(x-2,y+8,1))
            {
                x=x-abs(deltax);
            }
            else if(deltax>0&&!_walkflag(x+15+2,y+8,1))
            {
                x=x+abs(deltax);
            }
        }
    }
}

void movingblock::push(fix bx,fix by,int d2,int f)
{
    trigger=false;
    endx=x=bx;
    endy=y=by;
    dir=d2;
    oldflag=f;
    word *di = &(tmpscr->data[(int(y)&0xF0)+(int(x)>>4)]);
    byte *ci = &(tmpscr->cset[(int(y)&0xF0)+(int(x)>>4)]);
    bcombo =  tmpscr->data[(int(y)&0xF0)+(int(x)>>4)];
    oldcset = tmpscr->cset[(int(y)&0xF0)+(int(x)>>4)];
    cs     = (isdungeon() && !get_bit(quest_rules, qr_PUSHBLOCKCSETFIX)) ? 9 : oldcset;
    tile = combobuf[bcombo].tile;
    flip = combobuf[bcombo].flip;
    *di = tmpscr->undercombo;
    *ci = tmpscr->undercset;
    putcombo(scrollbuf,x,y,*di,*ci);
    clk=32;
    blockmoving=true;
}

bool movingblock::animate(int index)
{
    //these are here to bypass compiler warnings about unused arguments
    index=index;
    
    if(clk<=0)
        return false;
        
    move((fix)0.5);
    
    if(--clk==0)
    {
        bool bhole=false;
        blockmoving=false;
        int f1 = tmpscr->sflag[(int(y)&0xF0)+(int(x)>>4)];
        int f2 = MAPCOMBOFLAG(x,y);
        
        tmpscr->data[(int(y)&0xF0)+(int(x)>>4)]=bcombo;
        tmpscr->cset[(int(y)&0xF0)+(int(x)>>4)]=oldcset;
        
        if((f1==mfBLOCKTRIGGER)||f2==mfBLOCKTRIGGER)
        {
            trigger=true;
            tmpscr->sflag[(int(y)&0xF0)+(int(x)>>4)]=mfPUSHED;
            //the above line used to be in the following if statement.
            //However, it caused inherent-flag pushblocks to not lock into
            //block trigger combos unless the block trigger is also an
            //inherent flag
            /*
            if(f2==mfBLOCKTRIGGER)
            {
              tmpscr->sflag[(int(y)&0xF0)+(int(x)>>4)]=mfPUSHED;
            }
            */
        }
        
        if((f1==mfBLOCKHOLE)||f2==mfBLOCKHOLE)
        {
            tmpscr->data[(int(y)&0xF0)+(int(x)>>4)]+=1;
            bhole=true;
        }
        
        if(bhole)
        {
            tmpscr->sflag[(int(y)&0xF0)+(int(x)>>4)]=mfNONE;
        }
        else
        {
            f2 = MAPCOMBOFLAG(x,y);
            
            if(!((f2==mfPUSHUDINS && dir<=down) ||
                    (f2==mfPUSHLRINS && dir>=left) ||
                    (f2==mfPUSHUINS && dir==up) ||
                    (f2==mfPUSHDINS && dir==down) ||
                    (f2==mfPUSHLINS && dir==left) ||
                    (f2==mfPUSHRINS && dir==right) ||
                    (f2==mfPUSH4INS)))
            {
                tmpscr->sflag[(int(y)&0xF0)+(int(x)>>4)]=mfPUSHED;
            }
        }
        
        if(oldflag>=mfPUSHUDINS&&oldflag&&!trigger&&!bhole)
        {
            tmpscr->sflag[(int(y)&0xF0)+(int(x)>>4)]=oldflag;
        }
        
        for(int i=0; i<176; i++)
        {
            if(tmpscr->sflag[i]==mfBLOCKTRIGGER||combobuf[tmpscr->data[i]].flag==mfBLOCKTRIGGER)
            {
                trigger=false;
            }
        }
        
        //triggers a secret
        f2 = MAPCOMBOFLAG(x,y);
        
        if((oldflag==mfPUSH4 ||
            (oldflag==mfPUSHUD && dir<=down) ||
            (oldflag==mfPUSHLR && dir>=left) ||
            (oldflag==mfPUSHU && dir==up) ||
            (oldflag==mfPUSHD && dir==down) ||
            (oldflag==mfPUSHL && dir==left) ||
            (oldflag==mfPUSHR && dir==right) ||
            f2==mfPUSH4 ||
            (f2==mfPUSHUD && dir<=down) ||
            (f2==mfPUSHLR && dir>=left) ||
            (f2==mfPUSHU && dir==up) ||
            (f2==mfPUSHD && dir==down) ||
            (f2==mfPUSHL && dir==left) ||
            (f2==mfPUSHR && dir==right)) ||
           trigger)
        {
            if(hiddenstair(0,true))
            {
                sfx(tmpscr->secretsfx);
            }
            else
            {
                hidden_entrance(0,true,true);
                
                if((combobuf[bcombo].type == cPUSH_WAIT) ||
                        (combobuf[bcombo].type == cPUSH_HW) ||
                        (combobuf[bcombo].type == cPUSH_HW2) || trigger)
                {
                    sfx(tmpscr->secretsfx);
                }
            }
            
            if(isdungeon() && tmpscr->flags&fSHUTTERS)
            {
                opendoors=8;
            }
            
            if(!isdungeon())
            {
                if(combobuf[bcombo].type==cPUSH_HEAVY || combobuf[bcombo].type==cPUSH_HW
                        || combobuf[bcombo].type==cPUSH_HEAVY2 || combobuf[bcombo].type==cPUSH_HW2)
                {
                    if(!(tmpscr->flags5&fTEMPSECRETS)) setmapflag(mSECRET);
                }
            }
        }
        
        putcombo(scrollbuf,x,y,bcombo,cs);
    }
    
    return false;
}
