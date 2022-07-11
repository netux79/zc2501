#include "zdefs.h"
#include "maps.h"
#include "zelda.h"
#include "link.h"
#include "pal.h"
#include "subscr.h"

extern LinkClass Link;

int CSET_SIZE = 16;                                         // this is only changed to 4 in the NES title screen
int CSET_SHFT = 4;                                          // log2 of CSET_SIZE

bool usingdrypal = false; //using dried up lake colors
RGB olddrypal; //palette to restore when lake rehydrates

byte nes_pal[] =
{
    31,31,31,                                                 //  0
    0, 0,63,                                                  //  1
    0, 0,47,                                                  //  2
    17,10,47,                                                 //  3
    37, 0,33,                                                 //  4
    42, 0, 8,                                                 //  5
    42, 4, 0,                                                 //  6
    34, 5, 0,                                                 //  7
    20,12, 0,                                                 //  8
    0,30, 0,                                                  //  9
    0,26, 0,                                                  // 10
    0,22, 0,                                                  // 11
    0,16,22,                                                  // 12
    0, 0, 0,                                                  // 13 0D
    0, 0, 0,                                                  // 14 0E
    0, 0, 0,                                                  // 15 0F
    47,47,47,                                                 // 16
    0,30,62,                                                  // 17
    0,22,62,                                                  // 18
    26,17,63,                                                 // 19
    54, 0,51,                                                 // 20
    57, 0,22,                                                 // 21
    62,14, 0,                                                 // 22
    57,23, 4,                                                 // 23
    43,31, 0,                                                 // 24
    0,46, 0,                                                  // 25
    0,42, 0,                                                  // 26
    0,42,17,                                                  // 27
    0,34,34,                                                  // 28
    0, 0, 0,                                                  // 29
    0, 0, 0,                                                  // 30
    0, 0, 0,                                                  // 31
    62,62,62,                                                 // 32
    15,47,63,                                                 // 33
    26,34,63,                                                 // 34
    38,30,62,                                                 // 35
    62,30,62,                                                 // 36
    62,22,38,                                                 // 37
    62,30,22,                                                 // 38
    63,40,17,                                                 // 39
    62,46, 0,                                                 // 40
    46,62, 6,                                                 // 41
    22,54,21,                                                 // 42
    22,62,38,                                                 // 43
    0,58,54,                                                  // 44
    30,30,30,                                                 // 45
    0, 0, 0,                                                  // 46
    0, 0, 0,                                                  // 47
    63,63,63,                                                 // 48 !
    41,57,63,                                                 // 49
    52,52,62,                                                 // 50
    54,46,62,                                                 // 51
    62,46,62,                                                 // 52
    62,41,48,                                                 // 53
    60,52,44,                                                 // 54
    63,56,42,                                                 // 55
    62,54,30,                                                 // 56
    54,62,30,                                                 // 57
    46,62,46,                                                 // 58
    46,62,54,                                                 // 59
    0,63,63,                                                  // 60
    62,54,62,                                                 // 61
    0,54,50,                                                  // 62
    31,63,63                                                  // 63
};

RGB _RGB(byte *si)
{
    RGB x;
    x.r = si[0];
    x.g = si[1];
    x.b = si[2];
    x.filler=0; // Suppress warning "used undefined"
    return x;
}

RGB _RGB(int r,int g,int b)
{
    RGB x;
    x.r = r;
    x.g = g;
    x.b = b;
    x.filler=0; // Suppress warning "used undefined"
    return x;
}

void loadfullpal()
{
    for(int i=0; i<240; i++)
        RAMpal[i]=_RGB(colordata+i*3);

    /* Some quests use colors in this range (and 255),
       so we need to keep them here accordingly */
    for(int i=240; i<253; i++)
        RAMpal[i]=((RGB*)data[PAL_GUI].dat)[i];
    RAMpal[255]=((RGB*)data[PAL_GUI].dat)[255];

    RAMpal[BLACK] = _RGB(0,0,0);
    RAMpal[WHITE] = _RGB(63,63,63);
    
    refreshpal=true;
}

void loadlvlpal(int level)
{
    byte *si = colordata + CSET(level*pdLEVEL+poLEVEL)*3;
    
    for(int i=0; i<16*3; i++)
    {
        RAMpal[CSET(2)+i] = _RGB(si);
        si+=3;
    }
    
    for(int i=0; i<16; i++)
    {
        RAMpal[CSET(9)+i] = _RGB(si);
        si+=3;
    }
    
    if(!get_bit(quest_rules,qr_NOLEVEL3FIX) && level==3)
        RAMpal[CSET(6)+2] = NESpal(0x37);
        
    create_rgb_table(&rgb_table, RAMpal, NULL);
    create_zc_trans_table(&trans_table, RAMpal, 128, 128, 128);
    memcpy(&trans_table2, &trans_table, sizeof(COLOR_MAP));
    
    for(int q=0; q<PAL_SIZE; q++)
    {
        trans_table2.data[0][q] = q;
        trans_table2.data[q][q] = q;
    }
    
    refreshpal=true;
}

void loadpalset(int cset,int dataset)
{
    int j = CSET(dataset)*3;
    
    for(int i=0; i<16; i++,j+=3)
    {
        RAMpal[CSET(cset)+i] = _RGB(&colordata[j]);
    }
    
    if(cset==6 && !get_bit(quest_rules,qr_NOLEVEL3FIX) && DMaps[currdmap].color==3)
        RAMpal[CSET(6)+2] = NESpal(0x37);
        
    refreshpal=true;
}

void ringcolor(bool forceDefault)
{
    int itemid = current_item_id(itype_ring);
    
    if(!forceDefault && itemid>-1)
    {
        loadpalset(6,itemsbuf[itemid].misc1 ? pSprite(zc_min(29,itemsbuf[itemid].misc1)):6);
    }
    else
    {
        loadpalset(6,6);
    }
    
    refreshpal=true;
}

void loadfadepal(int dataset)
{
    byte *si = colordata + CSET(dataset)*3;
    
    for(int i=0; i<pdFADE*16; i++)
    {
        RAMpal[CSET(2)+i] = _RGB(si);
        si+=3;
    }
    
    refreshpal=true;
}

void interpolatedfade()
{
    int dpos = 64;
    int lpos = 32;
    int last = CSET(5)-1;
    
    if(get_bit(quest_rules,qr_FADECS5))
    {
        last += 16;
        loadpalset(5,5);
    }
    
    loadlvlpal(DMaps[currdmap].color);
    byte *si = colordata + CSET(DMaps[currdmap].color*pdLEVEL+poFADE1)*3;
    
    for(int i=0; i<16; i++)
    {
        int light = si[0]+si[1]+si[2];
        si+=3;
        fade_interpolate(RAMpal,black_palette,RAMpal,light?lpos:dpos,CSET(2)+i,CSET(2)+i);
    }
    
    fade_interpolate(RAMpal,black_palette,RAMpal,dpos,CSET(3),last);
    refreshpal=true;
}

void fade(int level,bool blackall,bool fromblack)
{
    int cx = fromblack ? 30 : 0;
    
    for(int i=0; i<=30; i+=(get_bit(quest_rules,qr_FADE))?2:1)
    {
        if(get_bit(quest_rules,qr_FADE))
        {
            int dpos = (cx<<6)/30;
            int lpos = zc_min(dpos,blackall?64:32);
            int last = CSET(5)-1;
            
            if(get_bit(quest_rules,qr_FADECS5))
            {
                last += 16;
                loadpalset(5,5);
            }
            
            loadlvlpal(level);
            byte *si = colordata + CSET(level*pdLEVEL+poFADE1)*3;
            
            for(int j=0; j<16; ++j)
            {
                int light = si[0]+si[1]+si[2];
                si+=3;
                fade_interpolate(RAMpal,black_palette,RAMpal,light?lpos:dpos,CSET(2)+j,CSET(2)+j);
            }
            
            fade_interpolate(RAMpal,black_palette,RAMpal,dpos,CSET(3),last);
            refreshpal=true;
        }
        else
        {
            switch(cx)
            {
            case 0:
                loadlvlpal(level);
                break;
                
            case 10:
                loadfadepal(level*pdLEVEL+poFADE1);
                break;
                
            case 20:
                loadfadepal(level*pdLEVEL+poFADE2);
                break;
                
            case 30:
                if(blackall)
                {
                    for(int j=0; j<pdFADE*16; j++)
                        RAMpal[CSET(2)+j]=black_palette[0];
                        
                    refreshpal=true;
                }
                else
                    loadfadepal(level*pdLEVEL+poFADE3);
                    
                break;
            }
        }
        
        if(!get_bit(quest_rules,qr_NOLEVEL3FIX) && level==3)
            RAMpal[CSET(6)+2] = NESpal(0x37);
            
        //put_passive_subscr(framebuf,0,passive_subscreen_offset,false,false);
        advanceframe(true);
        
        if(Quit)
            break;
            
        fromblack ? --cx : ++cx;
        
        if(get_bit(quest_rules,qr_FADE))
        {
            fromblack ? --cx : ++cx;
        }
    }
    
    // Make sure the palette is set exactly right before returning...
    if(fromblack)
        loadlvlpal(level);
}


// false: change screen lighting to naturaldark
// true: lighten room
void lighting(bool existslight, bool setnaturaldark)
{
    bool newstate = !existslight && (setnaturaldark ? ((TheMaps[currmap*MAPSCRS+currscr].flags&fDARK) != 0) : naturaldark);
    
    if(darkroom != newstate)
    {
fade((Link.getSpecialCave()>0) ? (Link.getSpecialCave()>=GUYCAVE) ? 10 : 11 : DMaps[currdmap].color, false, darkroom);
        darkroom = newstate;
    }
    
    if(setnaturaldark)
        naturaldark = newstate;
}

// Only used during Insta-Warps
void lightingInstant()
{
    bool newstate = ((TheMaps[currmap*MAPSCRS+currscr].flags&fDARK) != 0);
    
    if(darkroom != newstate)
    {
int level = (Link.getSpecialCave()>0) ? (Link.getSpecialCave()>=GUYCAVE) ? 10 : 11 : DMaps[currdmap].color;

        if(darkroom) // Old room dark, new room lit
        {
            loadlvlpal(level);
            
            if(get_bit(quest_rules,qr_FADECS5))
                loadpalset(5,5);
        }
        else // Old room lit, new room dark
        {
            if(get_bit(quest_rules,qr_FADE))
            {
                int last = CSET(5)-1;
                int light;
                
                if(get_bit(quest_rules,qr_FADECS5))
                {
                    last += 16;
                    loadpalset(5,5);
                }
                
                byte *si = colordata + CSET(level*pdLEVEL+poFADE1)*3;
                
                for(int j=0; j<16; ++j)
                {
                    light = si[0]+si[1]+si[2];
                    si+=3;
                    fade_interpolate(RAMpal,black_palette,RAMpal,light?32:64,CSET(2)+j,CSET(2)+j);
                }
                
                fade_interpolate(RAMpal,black_palette,RAMpal,64,CSET(3),last);
            }
            else // No interpolated fading
                loadfadepal(level*pdLEVEL+poFADE3);
        }
        
        if(!get_bit(quest_rules,qr_NOLEVEL3FIX) && level==3)
            RAMpal[CSET(6)+2] = NESpal(0x37);
            
        create_rgb_table(&rgb_table, RAMpal, NULL);
        create_zc_trans_table(&trans_table, RAMpal, 128, 128, 128);
        memcpy(&trans_table2, &trans_table, sizeof(COLOR_MAP));
        
        for(int q=0; q<PAL_SIZE; q++)
        {
            trans_table2.data[0][q] = q;
            trans_table2.data[q][q] = q;
        }
        
        darkroom = newstate;
    }
    
    naturaldark = newstate;
}

byte drycolors[11] = {0x12,0x11,0x22,0x21,0x31,0x32,0x33,0x35,0x34,0x36,0x37};

void dryuplake()
{
    if(whistleclk<0 || whistleclk>=88)
        return;
        
    if((++whistleclk)&7)
        return;
        
    if(whistleclk<88)
    {
        if(tmpscr->flags7 & fWHISTLEPAL)
        {
            if(!usingdrypal)
            {
                usingdrypal = true;
                olddrypal = RAMpal[CSET(3)+3];
            }
            
            RAMpal[CSET(3)+3] = NESpal(drycolors[whistleclk>>3]);
            refreshpal=true;
            
        }
    }
    else
    {
        if(tmpscr->flags & fWHISTLE)
        {
            if(hiddenstair(0,true))
            {
                sfx(tmpscr->secretsfx);
            }
        }
    }
}

void rehydratelake(bool instant)
{
    if(whistleclk==-1)
        return;
        
    if(instant && usingdrypal)
    {
        usingdrypal = false;
        RAMpal[CSET(3)+3] = olddrypal;
        refreshpal=true;
        return;
    }
    
    whistleclk&=0xF8;
    
    do
    {
        whistleclk-=8;
        
        if(usingdrypal)
        {
            RAMpal[CSET(3)+3] = NESpal(drycolors[whistleclk>>3]);
            refreshpal=true;
        }
        
        advanceframe(true);
        
        if(((whistleclk>>3)&3) == 1)
            for(int i=0; i<4 && !Quit; i++)
                advanceframe(true);
    }
    while(whistleclk!=0 && !Quit);
    
    whistleclk=-1;
    
    if(usingdrypal)
    {
        usingdrypal = false;
        RAMpal[CSET(3)+3] = olddrypal;
        refreshpal=true;
    }
}

static int palclk[3];
static int palpos[3];

void reset_pal_cycling()
{
    for(int i=0; i<3; i++)
        palclk[i]=palpos[i]=0;
}

void cycle_palette()
{
    if(!get_bit(quest_rules,qr_FADE) || darkroom)
        return;
        
    int level = (Link.getSpecialCave()==0) ? DMaps[currdmap].color : (Link.getSpecialCave()<GUYCAVE ? 11 : 10);
    
    for(int i=0; i<3; i++)
    {
        palcycle c = QMisc.cycles[level][i];
        
        if(c.count&0xF0)
        {
            if(++palclk[i] >= c.speed)
            {
                palclk[i]=0;
                
                if(++palpos[i] >= (c.count>>4))
                    palpos[i]=0;
                    
                byte *si = colordata + CSET(level*pdLEVEL+poFADE1+1+palpos[i])*3;
                si += (c.first&15)*3;
                
                for(int col=c.first&15; col<=(c.count&15); col++)
                {
                    RAMpal[CSET(c.first>>4)+col] = _RGB(si);
                    si+=3;
                }
                
                refreshpal=true;
            }
        }
    }
    
    // No need to do handle refreshpal here; it's done in updatescr().
}

int reverse_NESpal(RGB c)
{
    int dist = 12000;
    int index = 0;
    
    for(int i = 0; (i < 64) && (dist != 0); i++)
    {
        int r = (c.r - NESpal(i).r);
        int g = (c.g - NESpal(i).g);
        int b = (c.b - NESpal(i).b);
        int d = r*r + g*g + b*b;
        
        if(d < dist)
        {
            dist = d;
            index = i;
        }
    }
    
    return index;
}

/* create_zc_trans_table:
  *  Constructs a translucency color table for the specified palette. The
  *  r, g, and b parameters specifiy the solidity of each color component,
  *  ranging from 0 (totally transparent) to 255 (totally solid). If the
  *  callback function is not NULL, it will be called 256 times during the
  *  calculation, allowing you to display a progress indicator.
  */
void create_zc_trans_table(COLOR_MAP *table, AL_CONST PALETTE pal, int r, int g, int b)
{
    int tmp[768], *q;
    int x, y, i, j, k;
    unsigned char *p;
    RGB c;
    
    for(x=0; x<256; x++)
    {
        tmp[x*3]   = pal[x].r * (255-r) / 255;
        tmp[x*3+1] = pal[x].g * (255-g) / 255;
        tmp[x*3+2] = pal[x].b * (255-b) / 255;
    }
    
    for(x=0; x<PAL_SIZE; x++)
    {
        i = pal[x].r * r / 255;
        j = pal[x].g * g / 255;
        k = pal[x].b * b / 255;
        
        p = table->data[x];
        q = tmp;
        
        if(rgb_map)
        {
            for(y=0; y<PAL_SIZE; y++)
            {
                c.r = i + *(q++);
                c.g = j + *(q++);
                c.b = k + *(q++);
                p[y] = rgb_map->data[c.r>>1][c.g>>1][c.b>>1];
            }
        }
        else
        {
            for(y=0; y<PAL_SIZE; y++)
            {
                c.r = i + *(q++);
                c.g = j + *(q++);
                c.b = k + *(q++);
                p[y] = bestfit_color(pal, c.r, c.g, c.b);
            }
        }
    }
}

/* 1.5k lookup table for color matching */
unsigned int col_diff[3*128];

/* bestfit_init:
  *  Color matching is done with weighted squares, which are much faster
  *  if we pregenerate a little lookup table...
  */
void bestfit_init(void)
{
    int i;
    
    col_diff[0] = col_diff[128] = col_diff[256] = 0;
    
    for(i=1; i<64; i++)
    {
        int k = i * i;
        col_diff[0  +i] = col_diff[0  +128-i] = k * (59 * 59);
        col_diff[128+i] = col_diff[128+128-i] = k * (30 * 30);
        col_diff[256+i] = col_diff[256+128-i] = k * (11 * 11);
    }
}

/* create_rgb_table:
  *  Fills an RGB_MAP lookup table with conversion data for the specified
  *  palette. This is the faster version by Jan Hubicka.
  *
  *  Uses alg. similar to floodfill - it adds one seed per every color in
  *  palette to its best position. Then areas around seed are filled by
  *  same color because it is best approximation for them, and then areas
  *  about them etc...
  *
  *  It does just about 80000 tests for distances and this is about 100
  *  times better than normal 256*32000 tests so the calculation time
  *  is now less than one second at all computers I tested.
  */
void create_rgb_table_range(RGB_MAP *table, AL_CONST PALETTE pal, unsigned char start, unsigned char end)
{
#define UNUSED 65535
#define LAST 65532

    /* macro add adds to single linked list */
#define add(i)    (next[(i)] == UNUSED ? (next[(i)] = LAST, \
                                          (first != LAST ? (next[last] = (i)) : (first = (i))), \
                                          (last = (i))) : 0)
    
    /* same but w/o checking for first element */
#define add1(i)   (next[(i)] == UNUSED ? (next[(i)] = LAST, \
                                          next[last] = (i), \
                                          (last = (i))) : 0)
    
    /* calculates distance between two colors */
#define dist(a1, a2, a3, b1, b2, b3) \
          (col_diff[ ((a2) - (b2)) & 0x7F] + \
           (col_diff + 128)[((a1) - (b1)) & 0x7F] + \
           (col_diff + 256)[((a3) - (b3)) & 0x7F])
    
    /* converts r,g,b to position in array and back */
#define pos(r, g, b) \
          (((r) / 2) * 32 * 32 + ((g) / 2) * 32 + ((b) / 2))
    
#define depos(pal, r, g, b) \
          ((b) = ((pal) & 31) * 2, \
           (g) = (((pal) >> 5) & 31) * 2, \
           (r) = (((pal) >> 10) & 31) * 2)
    
    /* is current color better than pal1? */
#define better(r1, g1, b1, pal1) \
          (((int)dist((r1), (g1), (b1), \
                      (pal1).r, (pal1).g, (pal1).b)) > (int)dist2)
    
    /* checking of position */
#define dopos(rp, gp, bp, ts) \
          if ((rp > -1 || r > 0) && (rp < 1 || r < 61) && \
              (gp > -1 || g > 0) && (gp < 1 || g < 61) && \
              (bp > -1 || b > 0) && (bp < 1 || b < 61)) { \
          i = first + rp * 32 * 32 + gp * 32 + bp; \
          if (!data[i]) {     \
            data[i] = val;    \
            add1(i);          \
          }                   \
          else if ((ts) && (data[i] != val)) { \
              dist2 = (rp ? (col_diff+128)[(r+2*rp-pal[val].r) & 0x7F] : r2) + \
                (gp ? (col_diff    )[(g+2*gp-pal[val].g) & 0x7F] : g2) + \
                (bp ? (col_diff+256)[(b+2*bp-pal[val].b) & 0x7F] : b2); \
              if (better((r+2*rp), (g+2*gp), (b+2*bp), pal[data[i]])) { \
                data[i] = val; \
                add1(i);      \
              }               \
            }                 \
        }
    
    int i, curr, r, g, b, val, dist2;
    unsigned int r2, g2, b2;
    unsigned short next[32*32*32];
    unsigned char *data;
    int first = LAST;
    int last = LAST;
    
#define AVERAGE_COUNT   18000
    
    if(col_diff[1] == 0)
        bestfit_init();
        
    memset(next, 255, sizeof(next));
    memset(table->data, 0, sizeof(char)*32*32*32);
    
    data = (unsigned char *)table->data;
    
    /* add starting seeds for floodfill */
    for(i=start; i<PAL_SIZE&&i<=end; i++)
    {
        curr = pos(pal[i].r, pal[i].g, pal[i].b);
        
        if(next[curr] == UNUSED)
        {
            data[curr] = i;
            add(curr);
        }
    }
    
    /* main floodfill: two versions of loop for faster growing in blue axis */
    while(first < LAST)
    {
        depos(first, r, g, b);
        
        /* calculate distance of current color */
        val = data[first];
        r2 = (col_diff+128)[((pal[val].r)-(r)) & 0x7F];
        g2 = (col_diff)[((pal[val].g)-(g)) & 0x7F];
        b2 = (col_diff+256)[((pal[val].b)-(b)) & 0x7F];
        
        /* try to grow to all directions */
        dopos(0, 0, 1, 1);
        dopos(0, 0,-1, 1);
        dopos(1, 0, 0, 1);
        dopos(-1, 0, 0, 1);
        dopos(0, 1, 0, 1);
        dopos(0,-1, 0, 1);
        
        /* faster growing of blue direction */
        if((b > 0) && (data[first-1] == val))
        {
            b -= 2;
            first--;
            b2 = (col_diff+256)[((pal[val].b)-(b)) & 0x7F];
            
            dopos(-1, 0, 0, 0);
            dopos(1, 0, 0, 0);
            dopos(0,-1, 0, 0);
            dopos(0, 1, 0, 0);
            
            first++;
        }
        
        /* get next from list */
        i = first;
        first = next[first];
        next[i] = UNUSED;
        
        /* second version of loop */
        if(first != LAST)
        {
            depos(first, r, g, b);
            
            val = data[first];
            r2 = (col_diff+128)[((pal[val].r)-(r)) & 0x7F];
            g2 = (col_diff)[((pal[val].g)-(g)) & 0x7F];
            b2 = (col_diff+256)[((pal[val].b)-(b)) & 0x7F];
            
            dopos(0, 0, 1, 1);
            dopos(0, 0,-1, 1);
            dopos(1, 0, 0, 1);
            dopos(-1, 0, 0, 1);
            dopos(0, 1, 0, 1);
            dopos(0,-1, 0, 1);
            
            if((b < 61) && (data[first + 1] == val))
            {
                b += 2;
                first++;
                b2 = (col_diff+256)[((pal[val].b)-(b)) & 0x7f];
                
                dopos(-1, 0, 0, 0);
                dopos(1, 0, 0, 0);
                dopos(0,-1, 0, 0);
                dopos(0, 1, 0, 0);
                
                first--;
            }
            
            i = first;
            first = next[first];
            next[i] = UNUSED;
        }
    }
}
