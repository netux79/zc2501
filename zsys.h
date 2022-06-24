//--------------------------------------------------------
//  Zelda Classic
//  by Jeremy Craner, 1999-2000
//
//  zsys.h
//
//  System functions, etc.
//
//--------------------------------------------------------

#ifndef _ZSYS_H_
#define _ZSYS_H_

#include "zdefs.h"

#define FILENAME8_3   0
#define FILENAME8__   1
#define FILENAMEALL   2

char *time_str_short(dword time);
char *time_str_short2(dword time);
char *time_str_med(dword time);
char *time_str_long(dword time);

void extract_name(char *path,char *name,int type);
void temp_name(char temporaryname[]);
void chop_path(char *path);
int  vbound(int x,int low,int high);
float vbound(float x,float low,float high);
int  used_switch(int argc,char *argv[],const char *s);
bool isinRect(int x,int y,int rx1,int ry1,int rx2,int ry2);

extern char datapwd[8];

void resolve_password(char *pwd);

bool decode_007(byte *buf, dword size, dword key, word check1, word check2, int method);
void encode_007(byte *buf, dword size, dword key, word *check1, word *check2, int method);
int encode_file_007(const char *srcfile, const char *destfile, int key, const char *header, int method);
int decode_file_007(const char *srcfile, const char *destfile, const char *header, int method, bool packed, const char *password);
void copy_file(const char *src, const char *dest);

int  get_bit(byte *bitstr,int bit);
void set_bit(byte *bitstr,int bit,byte val);

void Z_error(const char *format,...);
void Z_message(const char *format,...);

int anim_3_4(int clk, int speed);

int bound(int &x,int low,int high);

void textout_shadow_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg);
void textout_shadow_centre_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg);
void textout_shadow_right_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg);
void textout_shadow_u_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg);
void textout_shadow_centre_u_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg);
void textout_shadow_right_u_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg);
void textout_shadow_o_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg);
void textout_shadow_centre_o_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg);
void textout_shadow_right_o_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg);
void textout_shadow_plus_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg);
void textout_shadow_centre_plus_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg);
void textout_shadow_right_plus_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg);
void textout_shadow_x_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg);
void textout_shadow_centre_x_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg);
void textout_shadow_right_x_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int shadow, int bg);
void textout_shadowed_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg);
void textout_shadowed_centre_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg);
void textout_shadowed_right_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg);
void textout_shadowed_u_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg);
void textout_shadowed_centre_u_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg);
void textout_shadowed_right_u_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg);
void textout_shadowed_o_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg);
void textout_shadowed_centre_o_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg);
void textout_shadowed_right_o_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg);

void textout_shadowed_plus_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg);
void textout_shadowed_centre_plus_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg);
void textout_shadowed_right_plus_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg);
void textout_shadowed_x_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg);
void textout_shadowed_centre_x_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg);
void textout_shadowed_right_x_ex(BITMAP *bmp, const FONT *f, const char *s, int x, int y, int color, int shadow, int bg);

void textprintf_shadow_ex(BITMAP *bmp, const FONT *f, int x, int y, int shadow, int bg, const char *format, ...);
void textprintf_shadow_centre_ex(BITMAP *bmp, const FONT *f, int x, int y, int shadow, int bg, const char *format, ...);
void textprintf_shadow_right_ex(BITMAP *bmp, const FONT *f, int x, int y, int shadow, int bg, const char *format, ...);
void textprintf_shadow_u_ex(BITMAP *bmp, const FONT *f, int x, int y, int shadow, int bg, const char *format, ...);
void textprintf_shadow_centre_u_ex(BITMAP *bmp, const FONT *f, int x, int y, int shadow, int bg, const char *format, ...);
void textprintf_shadow_right_u_ex(BITMAP *bmp, const FONT *f, int x, int y, int shadow, int bg, const char *format, ...);
void textprintf_shadow_o_ex(BITMAP *bmp, const FONT *f, int x, int y, int shadow, int bg, const char *format, ...);
void textprintf_shadow_centre_o_ex(BITMAP *bmp, const FONT *f, int x, int y, int shadow, int bg, const char *format, ...);
void textprintf_shadow_right_o_ex(BITMAP *bmp, const FONT *f, int x, int y, int shadow, int bg, const char *format, ...);

void textprintf_shadow_plus_ex(BITMAP *bmp, const FONT *f, int x, int y, int shadow, int bg, const char *format, ...);
void textprintf_shadow_centre_plus_ex(BITMAP *bmp, const FONT *f, int x, int y, int shadow, int bg, const char *format, ...);
void textprintf_shadow_right_plus_ex(BITMAP *bmp, const FONT *f, int x, int y, int shadow, int bg, const char *format, ...);
void textprintf_shadow_x_ex(BITMAP *bmp, const FONT *f, int x, int y, int shadow, int bg, const char *format, ...);
void textprintf_shadow_centre_x_ex(BITMAP *bmp, const FONT *f, int x, int y, int shadow, int bg, const char *format, ...);
void textprintf_shadow_right_x_ex(BITMAP *bmp, const FONT *f, int x, int y, int shadow, int bg, const char *format, ...);

void textprintf_shadowed_ex(BITMAP *bmp, const FONT *f, int x, int y, int color, int shadow, int bg, const char *format, ...);
void textprintf_shadowed_centre_ex(BITMAP *bmp, const FONT *f, int x, int y, int color, int shadow, int bg, const char *format, ...);
void textprintf_shadowed_right_ex(BITMAP *bmp, const FONT *f, int x, int y, int color, int shadow, int bg, const char *format, ...);
void textprintf_shadowed_u_ex(BITMAP *bmp, const FONT *f, int x, int y, int color, int shadow, int bg, const char *format, ...);
void textprintf_shadowed_centre_u_ex(BITMAP *bmp, const FONT *f, int x, int y, int color, int shadow, int bg, const char *format, ...);
void textprintf_shadowed_right_u_ex(BITMAP *bmp, const FONT *f, int x, int y, int color, int shadow, int bg, const char *format, ...);
void textprintf_shadowed_o_ex(BITMAP *bmp, const FONT *f, int x, int y, int color, int shadow, int bg, const char *format, ...);
void textprintf_shadowed_centre_o_ex(BITMAP *bmp, const FONT *f, int x, int y, int color, int shadow, int bg, const char *format, ...);
void textprintf_shadowed_right_o_ex(BITMAP *bmp, const FONT *f, int x, int y, int color, int shadow, int bg, const char *format, ...);

void textprintf_shadowed_plus_ex(BITMAP *bmp, const FONT *f, int x, int y, int color, int shadow, int bg, const char *format, ...);
void textprintf_shadowed_centre_plus_ex(BITMAP *bmp, const FONT *f, int x, int y, int color, int shadow, int bg, const char *format, ...);
void textprintf_shadowed_right_plus_ex(BITMAP *bmp, const FONT *f, int x, int y, int color, int shadow, int bg, const char *format, ...);
void textprintf_shadowed_x_ex(BITMAP *bmp, const FONT *f, int x, int y, int color, int shadow, int bg, const char *format, ...);
void textprintf_shadowed_centre_x_ex(BITMAP *bmp, const FONT *f, int x, int y, int color, int shadow, int bg, const char *format, ...);
void textprintf_shadowed_right_x_ex(BITMAP *bmp, const FONT *f, int x, int y, int color, int shadow, int bg, const char *format, ...);

void quit_game();

int zc_trace_handler(const char *);
void zc_trace_clear();

#endif                                                      // _ZSYS_H_

