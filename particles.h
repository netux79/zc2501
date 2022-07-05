#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include "sprite.h"
#include "zdefs.h"

class particle : public sprite
{
public:
    int layer, cset, color;
    fix step;
    particle(fix X,fix Y,int layer,int cset,int color);
    virtual ~particle();
    virtual bool animate(int index);
    virtual void draw(BITMAP *dest);
};

class pFaroresWindDust : public particle
{
public:
    fix os;
    int ot;
    int timer;
    bool initialized;
    pFaroresWindDust(fix X,fix Y,int layer,int cset,int color,int timer);
    virtual bool animate(int index);
};

class pTwilight : public particle
{
public:
    int delay;
    pTwilight(fix X,fix Y,int layer,int cset,int color, int delay);
    virtual bool animate(int index);
};

#endif
