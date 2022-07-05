#include "particles.h"

particle::~particle()
{
}

bool particle::animate(int index)
{
    //these are here to bypass compiler warnings about unused arguments
    index=index;
    return true;
}

void particle::draw(BITMAP *dest)
{
    int tcs=cset;
    tcs &= 15;
    tcs <<= CSET_SHFT;
    putpixel(dest, x, y+yofs, tcs+color);
}

particle::particle(fix X,fix Y,int L,int CS,int C) : sprite()
{
    x=X;
    y=Y;
    layer=L;
    cset=CS;
    color=C;
    yofs = 54;
}


pFaroresWindDust::pFaroresWindDust(fix X,fix Y,int L,int CS,int C, int T) : particle(X,Y,L,CS,C)
{
    initialized=false;
    timer=T;
}

bool pFaroresWindDust::animate(int index)
{
    index=index;
    
    if(!initialized)
    {
        os=step;
        ot=timer;
        initialized=true;
    }
    
    step=os*(double)timer/(double)ot;
    
    if(timer>0)
    {
        --timer;
    }
    
    move(step);
    return (!timer);
}

pTwilight::pTwilight(fix X,fix Y,int L,int CS,int C, int T) : particle(X,Y,L,CS,C)
{
    dir = up;
    delay = T;
}

bool pTwilight::animate(int index)
{
    index=index;
    
    if(delay>0)
    {
        delay--;
    }
    else
    {
        move(step);
    }
    
    return (y<0)!=0;
}
