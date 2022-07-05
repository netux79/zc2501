#include "zdefs.h"
#include "zcsys.h"

void textout_shadow_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg)
{
    if(bg!=-1)
    {
        rectfill(bmp, x, y, x+text_length(f, s), y+text_height(f), bg);
    }
    
    textout_ex(bmp, f, s, x+1, y, shadow, -1);
    textout_ex(bmp, f, s, x+1, y+1, shadow, -1);
    textout_ex(bmp, f, s, x, y+1, shadow, -1);
}

void textout_shadow_centre_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg)
{
    textout_shadow_ex(bmp, f, s, x-(text_length(f, s)/2), y, shadow, bg);
}

void textout_shadow_right_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg)
{
    textout_shadow_ex(bmp, f, s, x-text_length(f, s), y, shadow, bg);
}

void textout_shadow_u_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg)
{
    if(bg!=-1)
    {
        rectfill(bmp, x-1, y, x+text_length(f, s), y+text_height(f), bg);
    }
    
    textout_ex(bmp, f, s, x+1, y, shadow, -1);
    textout_ex(bmp, f, s, x+1, y+1, shadow, -1);
    textout_ex(bmp, f, s, x, y+1, shadow, -1);
    textout_ex(bmp, f, s, x-1, y+1, shadow, -1);
    textout_ex(bmp, f, s, x-1, y, shadow, -1);
}

void textout_shadow_centre_u_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg)
{
    textout_shadow_u_ex(bmp, f, s, x-(text_length(f, s)/2), y, shadow, bg);
}

void textout_shadow_right_u_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg)
{
    textout_shadow_u_ex(bmp, f, s, x-text_length(f, s), y, shadow, bg);
}

void textout_shadow_o_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg)
{
    if(bg!=-1)
    {
        rectfill(bmp, x-1, y-1, x+text_length(f, s), y+text_height(f), bg);
    }
    
    textout_ex(bmp, f, s, x+1, y-1, shadow, -1);
    textout_ex(bmp, f, s, x+1, y, shadow, -1);
    textout_ex(bmp, f, s, x+1, y+1, shadow, -1);
    textout_ex(bmp, f, s, x, y+1, shadow, -1);
    textout_ex(bmp, f, s, x-1, y+1, shadow, -1);
    textout_ex(bmp, f, s, x-1, y, shadow, -1);
    textout_ex(bmp, f, s, x-1, y-1, shadow, -1);
    textout_ex(bmp, f, s, x, y-1, shadow, -1);
}

void textout_shadow_centre_o_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg)
{
    textout_shadow_o_ex(bmp, f, s, x-(text_length(f, s)/2), y, shadow, bg);
}

void textout_shadow_right_o_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg)
{
    textout_shadow_o_ex(bmp, f, s, x-text_length(f, s), y, shadow, bg);
}

void textout_shadow_plus_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg)
{
    if(bg!=-1)
    {
        rectfill(bmp, x-1, y-1, x+text_length(f, s), y+text_height(f), bg);
    }
    
    textout_ex(bmp, f, s, x+1, y, shadow, -1);
    textout_ex(bmp, f, s, x, y+1, shadow, -1);
    textout_ex(bmp, f, s, x-1, y, shadow, -1);
    textout_ex(bmp, f, s, x, y-1, shadow, -1);
}

void textout_shadow_centre_plus_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg)
{
    textout_shadow_plus_ex(bmp, f, s, x-(text_length(f, s)/2), y, shadow, bg);
}

void textout_shadow_right_plus_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg)
{
    textout_shadow_plus_ex(bmp, f, s, x-text_length(f, s), y, shadow, bg);
}

void textout_shadow_x_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg)
{
    if(bg!=-1)
    {
        rectfill(bmp, x-1, y-1, x+text_length(f, s), y+text_height(f), bg);
    }
    
    textout_ex(bmp, f, s, x+1, y-1, shadow, -1);
    textout_ex(bmp, f, s, x+1, y+1, shadow, -1);
    textout_ex(bmp, f, s, x-1, y+1, shadow, -1);
    textout_ex(bmp, f, s, x-1, y-1, shadow, -1);
}

void textout_shadow_centre_x_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg)
{
    textout_shadow_x_ex(bmp, f, s, x-(text_length(f, s)/2), y, shadow, bg);
}

void textout_shadow_right_x_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg)
{
    textout_shadow_x_ex(bmp, f, s, x-text_length(f, s), y, shadow, bg);
}

void textout_shadowed_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg)
{
    textout_shadow_ex(bmp, f, s, x, y, shadow, bg);
    textout_ex(bmp, f, s, x, y, color, -1);
}

void textout_shadowed_centre_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg)
{
    textout_shadowed_ex(bmp, f, s, x-(text_length(f, s)/2), y, color, shadow, bg);
}

void textout_shadowed_right_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg)
{
    textout_shadowed_ex(bmp, f, s, x-text_length(f, s), y, color, shadow, bg);
}

void textout_shadowed_u_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg)
{
    textout_shadow_u_ex(bmp, f, s, x, y, shadow, bg);
    textout_ex(bmp, f, s, x, y, color, -1);
}

void textout_shadowed_centre_u_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg)
{
    textout_shadowed_u_ex(bmp, f, s, x-(text_length(f, s)/2), y, color, shadow, bg);
}

void textout_shadowed_right_u_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg)
{
    textout_shadowed_u_ex(bmp, f, s, x-text_length(f, s), y, color, shadow, bg);
}

void textout_shadowed_o_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg)
{
    textout_shadow_o_ex(bmp, f, s, x, y, shadow, bg);
    textout_ex(bmp, f, s, x, y, color, -1);
}

void textout_shadowed_centre_o_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg)
{
    textout_shadowed_o_ex(bmp, f, s, x-(text_length(f, s)/2), y, color, shadow, bg);
}

void textout_shadowed_right_o_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg)
{
    textout_shadowed_o_ex(bmp, f, s, x-text_length(f, s), y, color, shadow, bg);
}

void textout_shadowed_plus_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg)
{
    textout_shadow_plus_ex(bmp, f, s, x, y, shadow, bg);
    textout_ex(bmp, f, s, x, y, color, -1);
}

void textout_shadowed_centre_plus_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg)
{
    textout_shadowed_plus_ex(bmp, f, s, x-(text_length(f, s)/2), y, color, shadow, bg);
}

void textout_shadowed_right_plus_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg)
{
    textout_shadowed_plus_ex(bmp, f, s, x-text_length(f, s), y, color, shadow, bg);
}

void textout_shadowed_x_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg)
{
    textout_shadow_x_ex(bmp, f, s, x, y, shadow, bg);
    textout_ex(bmp, f, s, x, y, color, -1);
}

void textout_shadowed_centre_x_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg)
{
    textout_shadowed_x_ex(bmp, f, s, x-(text_length(f, s)/2), y, color, shadow, bg);
}

void textout_shadowed_right_x_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg)
{
    textout_shadowed_x_ex(bmp, f, s, x-text_length(f, s), y, color, shadow, bg);
}
