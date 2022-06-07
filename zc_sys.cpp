//--------------------------------------------------------
//  Zelda Classic
//  by Jeremy Craner, 1999-2000
//
//  zc_sys.cc
//
//  System functions, input handlers, GUI stuff, etc.
//  for Zelda Classic.
//
//--------------------------------------------------------

// to prevent <map> from generating errors
#define __GTHREAD_HIDE_WIN32API 1

#include "precompiled.h" //always first

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <map>
#include <ctype.h>
#include "zc_alleg.h"
#include "gamedata.h"
#include "zc_init.h"
#include "zquest.h"
#include "init.h"
#include "zdefs.h"
#include "zelda.h"
#include "tiles.h"
#include "colors.h"
#include "pal.h"
#include "zsys.h"
#include "qst.h"
#include "zc_sys.h"
#include "subscr.h"
#include "maps.h"
#include "sprite.h"
#include "guys.h"
#include "link.h"
#include "title.h"
#include "particles.h"

static int sfx_voice[WAV_COUNT];

extern FONT *lfont;
extern LinkClass Link;
extern sprite_list  guys, items, Ewpns, Lwpns, Sitems, chainlinks, decorations, particles;
byte disable_direct_updating;
byte use_dwm_flush;
byte use_save_indicator;
byte midi_patch_fix;
bool midi_paused=false;
//extern movingblock mblock2; //mblock[4]?
//extern int db;

static const char *ZC_str = "Zelda Classic";
static  const char *qst_dir_name = "linux_qst_dir";
#ifdef _MSC_VER
#define getcwd _getcwd
#endif

bool rF12();
bool rF5();
bool rF11();
bool rI();
bool rQ();
bool zc_key_pressed();

/**********************************/
/******** System functions ********/
/**********************************/

static char cfg_sect[] = "zeldadx";

void load_game_configs()
{
    joystick_index = get_config_int(cfg_sect,"joystick_index",0);
    
    if((unsigned int)joystick_index > 4)
        joystick_index = 0; // 4 is the max number supported by allegro
        
    Akey = get_config_int(cfg_sect,"key_a",KEY_ALT);
    Bkey = get_config_int(cfg_sect,"key_b",KEY_ZC_LCONTROL);
    Skey = get_config_int(cfg_sect,"key_s",KEY_ENTER);
    Lkey = get_config_int(cfg_sect,"key_l",KEY_Z);
    Rkey = get_config_int(cfg_sect,"key_r",KEY_X);
    Pkey = get_config_int(cfg_sect,"key_p",KEY_SPACE);
    Exkey1 = get_config_int(cfg_sect,"key_ex1",KEY_Q);
    Exkey2 = get_config_int(cfg_sect,"key_ex2",KEY_W);
    Exkey3 = get_config_int(cfg_sect,"key_ex3",KEY_A);
    Exkey4 = get_config_int(cfg_sect,"key_ex4",KEY_S);
    
    DUkey = get_config_int(cfg_sect,"key_up",   KEY_UP);
    DDkey = get_config_int(cfg_sect,"key_down", KEY_DOWN);
    DLkey = get_config_int(cfg_sect,"key_left", KEY_LEFT);
    DRkey = get_config_int(cfg_sect,"key_right",KEY_RIGHT);
    
    Abtn = get_config_int(cfg_sect,"btn_a",2);
    Bbtn = get_config_int(cfg_sect,"btn_b",1);
    Sbtn = get_config_int(cfg_sect,"btn_s",10);
    Mbtn = get_config_int(cfg_sect,"btn_m",9);
    Lbtn = get_config_int(cfg_sect,"btn_l",5);
    Rbtn = get_config_int(cfg_sect,"btn_r",6);
    Pbtn = get_config_int(cfg_sect,"btn_p",12);
    Exbtn1 = get_config_int(cfg_sect,"btn_ex1",7);
    Exbtn2 = get_config_int(cfg_sect,"btn_ex2",8);
    Exbtn3 = get_config_int(cfg_sect,"btn_ex3",4);
    Exbtn4 = get_config_int(cfg_sect,"btn_ex4",3);
    
    digi_volume = get_config_int(cfg_sect,"digi",248);
    midi_volume = get_config_int(cfg_sect,"midi",255);
    sfx_volume = get_config_int(cfg_sect,"sfx",248);
    emusic_volume = get_config_int(cfg_sect,"emusic",248);
    pan_style = get_config_int(cfg_sect,"pan",1);
    // 1 <= zcmusic_bufsz <= 128
    zcmusic_bufsz = vbound(get_config_int(cfg_sect,"zcmusic_bufsz",64),1,128);
    zc_vsync = get_config_int(cfg_sect,"vsync",0);
    Throttlefps = get_config_int(cfg_sect,"throttlefps",1)!=0;
    TransLayers = get_config_int(cfg_sect,"translayers",1)!=0;
    SnapshotFormat = get_config_int(cfg_sect,"snapshot_format",3);
    ShowFPS = get_config_int(cfg_sect,"showfps",0)!=0;
    NESquit = get_config_int(cfg_sect,"fastquit",0)!=0;
    title_version = get_config_int(cfg_sect,"title",2);
    
    //default - scale x2, 640 x 480
    resx = get_config_int(cfg_sect,"resx",640);
    resy = get_config_int(cfg_sect,"resy",480);
    //screen_scale = get_config_int(cfg_sect,"screen_scale",2);
    
    scanlines = get_config_int(cfg_sect,"scanlines",0)!=0;
    fullscreen = get_config_int(cfg_sect,"fullscreen",1);
    disable_triplebuffer = (byte) get_config_int(cfg_sect,"doublebuffer",0);
    can_triplebuffer_in_windowed_mode = (byte) get_config_int(cfg_sect,"triplebuffer",0);
    
    zc_color_depth = (byte) get_config_int(cfg_sect,"color_depth",8);
    
    frame_rest_suggest = (byte) get_config_int(cfg_sect,"frame_rest_suggest",1);
    frame_rest_suggest = zc_min(2, frame_rest_suggest);
    
    forceExit = (byte) get_config_int(cfg_sect,"force_exit",0);
	ss_enable = get_config_int(cfg_sect,"ss_enable",1) ? 1 : 0;
    ss_after = vbound(get_config_int(cfg_sect,"ss_after",14), 0, 14);
    ss_speed = vbound(get_config_int(cfg_sect,"ss_speed",2), 0, 6);
    ss_density = vbound(get_config_int(cfg_sect,"ss_density",3), 0, 6);
    heart_beep = get_config_int(cfg_sect,"heart_beep",1)!=0;
    gui_colorset = get_config_int(cfg_sect,"gui_colorset",0);
    sfxdat = get_config_int(cfg_sect,"use_sfx_dat",1);
    fullscreen = get_config_int(cfg_sect,"fullscreen",1);
    use_save_indicator = get_config_int(cfg_sect,"save_indicator",0);
}

void save_game_configs()
{
    set_config_int(cfg_sect,"joystick_index",joystick_index);
    set_config_int(cfg_sect,"key_a",Akey);
    set_config_int(cfg_sect,"key_b",Bkey);
    set_config_int(cfg_sect,"key_s",Skey);
    set_config_int(cfg_sect,"key_l",Lkey);
    set_config_int(cfg_sect,"key_r",Rkey);
    set_config_int(cfg_sect,"key_p",Pkey);
    set_config_int(cfg_sect,"key_ex1",Exkey1);
    set_config_int(cfg_sect,"key_ex2",Exkey2);
    set_config_int(cfg_sect,"key_ex3",Exkey3);
    set_config_int(cfg_sect,"key_ex4",Exkey4);
    
    set_config_int(cfg_sect,"key_up",   DUkey);
    set_config_int(cfg_sect,"key_down", DDkey);
    set_config_int(cfg_sect,"key_left", DLkey);
    set_config_int(cfg_sect,"key_right",DRkey);
    
    set_config_int(cfg_sect,"btn_a",Abtn);
    set_config_int(cfg_sect,"btn_b",Bbtn);
    set_config_int(cfg_sect,"btn_s",Sbtn);
    set_config_int(cfg_sect,"btn_m",Mbtn);
    set_config_int(cfg_sect,"btn_l",Lbtn);
    set_config_int(cfg_sect,"btn_r",Rbtn);
    set_config_int(cfg_sect,"btn_p",Pbtn);
    set_config_int(cfg_sect,"btn_ex1",Exbtn1);
    set_config_int(cfg_sect,"btn_ex2",Exbtn2);
    set_config_int(cfg_sect,"btn_ex3",Exbtn3);
    set_config_int(cfg_sect,"btn_ex4",Exbtn4);
    
    set_config_int(cfg_sect,"digi",digi_volume);
    set_config_int(cfg_sect,"midi",midi_volume);
    set_config_int(cfg_sect,"sfx",sfx_volume);
    set_config_int(cfg_sect,"emusic",emusic_volume);
    set_config_int(cfg_sect,"pan",pan_style);
    set_config_int(cfg_sect,"zcmusic_bufsz",zcmusic_bufsz);
    set_config_int(cfg_sect,"vsync",zc_vsync);
    set_config_int(cfg_sect,"throttlefps", (int)Throttlefps);
    set_config_int(cfg_sect,"translayers",(int)TransLayers);
    set_config_int(cfg_sect,"snapshot_format",SnapshotFormat);
    set_config_int(cfg_sect,"showfps",(int)ShowFPS);
    set_config_int(cfg_sect,"fastquit",(int)NESquit);
    set_config_int(cfg_sect,"title",title_version);
    
    set_config_int(cfg_sect,"resx",resx);
    set_config_int(cfg_sect,"resy",resy);
    
    //sbig depricated as of 2.5 RC3. handled exclusively by resx, resy now.
    //set_config_int(cfg_sect,"screen_scale",screen_scale);
    //set_config_int(cfg_sect,"sbig",sbig);
    //set_config_int(cfg_sect,"sbig2",sbig2);
    
    set_config_int(cfg_sect,"scanlines",scanlines);
    set_config_int(cfg_sect,"ss_enable",ss_enable);
    set_config_int(cfg_sect,"ss_after",ss_after);
    set_config_int(cfg_sect,"ss_speed",ss_speed);
    set_config_int(cfg_sect,"ss_density",ss_density);
    set_config_int(cfg_sect,"heart_beep",heart_beep);
    set_config_int(cfg_sect,"gui_colorset",gui_colorset);
    set_config_int(cfg_sect,"use_sfx_dat",sfxdat);
    set_config_int(cfg_sect,"fullscreen",fullscreen);
    set_config_int(cfg_sect,"doublebuffer",disable_triplebuffer);
    set_config_int(cfg_sect,"triplebuffer",can_triplebuffer_in_windowed_mode);
    set_config_int(cfg_sect,"color_depth",zc_color_depth);
    set_config_int(cfg_sect,"frame_rest_suggest",frame_rest_suggest);
    set_config_int(cfg_sect,"force_exit",forceExit);
    
    set_config_int(cfg_sect,"save_indicator",use_save_indicator);
    
    flush_config_file();
}

//----------------------------------------------------------------

// Timers

void fps_callback()
{
    lastfps=framecnt;
    dword tempsecs = fps_secs;
    ++tempsecs;
    //avgfps=((long double)avgfps*fps_secs+lastfps)/(++fps_secs); // DJGPP doesn't like this
    avgfps=((long double)avgfps*fps_secs+lastfps)/(tempsecs);
    ++fps_secs;
    framecnt=0;
}

END_OF_FUNCTION(fps_callback)

int Z_init_timers()
{
    static bool didit = false;
    const static char *err_str = "Couldn't allocate timer";
    err_str = err_str; //Unused variable warning
    
    if(didit)
        return 1;
        
    didit = true;
    
    LOCK_VARIABLE(lastfps);
    LOCK_VARIABLE(framecnt);
    LOCK_FUNCTION(fps_callback);
    
    if(install_int_ex(fps_callback,SECS_TO_TIMER(1)))
        return 0;
        
    return 1;
}

void Z_remove_timers()
{
    remove_int(fps_callback);
}

//----------------------------------------------------------------

void go()
{
    scare_mouse();
    blit(screen,tmp_scr,scrx,scry,0,0,screen->w,screen->h);
    unscare_mouse();
}

void comeback()
{
    scare_mouse();
    blit(tmp_scr,screen,0,0,scrx,scry,screen->w,screen->h);
    unscare_mouse();
}

void dump_pal(BITMAP *dest)
{
    for(int i=0; i<256; i++)
        rectfill(dest,(i&63)<<2,(i&0xFC0)>>4,((i&63)<<2)+3,((i&0xFC0)>>4)+3,i);
}

void show_paused(BITMAP *target)
{
    //  return;
    char buf[7] = "PAUSED";
    
    for(int i=0; buf[i]!=0; i++)
        buf[i]+=0x60;
        
    //  text_mode(-1);
    if(sbig)
    {
        int x = scrx+40-((screen_scale-1)*120);
        int y = scry+224+((screen_scale-1)*104);
        textout_ex(target,zfont,buf,x,y,-1,-1);
    }
    else
        textout_ex(target,zfont,buf,scrx+40,scry+224,-1,-1);
}

void show_fps(BITMAP *target)
{
    char buf[50];
    
    //  text_mode(-1);
    sprintf(buf,"%2d/60",lastfps);
    
    //  sprintf(buf,"%d/%u/%f/%u",lastfps,int(avgfps),avgfps,fps_secs);
    for(int i=0; buf[i]!=0; i++)
        if(buf[i]!=' ')
            buf[i]+=0x60;
            
    if(sbig)
    {
        int x = scrx+40-((screen_scale-1)*120);
        int y = scry+216+((screen_scale-1)*104);
        textout_ex(target,zfont,buf,x,y,-1,-1);
        // textout_ex(target,zfont,buf,scrx+40-120,scry+216+104,-1,-1);
    }
    else
    {
        textout_ex(target,zfont,buf,scrx+40,scry+216,-1,-1);
    }
}

void show_saving(BITMAP *target)
{
    if(!use_save_indicator)
        return;
    
    char buf[10] = "SAVING...";
    
    for(int i=0; buf[i]!=0; i++)
        buf[i]+=0x60;
        
    if(sbig)
    {
        int x = scrx+200+((screen_scale-1)*120);
        int y = scry+224+((screen_scale-1)*104);
        textout_ex(target,zfont,buf,x,y,-1,-1);
    }
    else
        textout_ex(target,zfont,buf,scrx+200,scry+224,-1,-1);
}

//----------------------------------------------------------------

// sets the video mode and initializes the palette and mouse sprite
bool game_vid_mode(int mode,int wait)
{
    if(set_gfx_mode(mode,resx,resy,0,0)!=0)
    {
        return false;
    }
    
    scrx = (resx-320)>>1;
    scry = (resy-240)>>1;
    
    set_mouse_sprite((BITMAP*)data[BMP_MOUSE].dat);
    
    for(int i=240; i<256; i++)
        RAMpal[i]=((RGB*)data[PAL_GUI].dat)[i];
        
    set_palette(RAMpal);
    clear_to_color(screen,BLACK);
    
    rest(wait);
    return true;
}

void init_NES_mode()
{
    // qst.dat may not load correctly without this...
    QHeader.templatepath[0]='\0';
    
    if(!init_colordata(true, &QHeader, &QMisc))
    {
        return;
    }
    
    loadfullpal();
    init_tiles(false, &QHeader);
}

//----------------------------------------------------------------

qword trianglelines[16]=
{
    0x0000000000000000ULL,
    0xFD00000000000000ULL,
    0xFDFD000000000000ULL,
    0xFDFDFD0000000000ULL,
    0xFDFDFDFD00000000ULL,
    0xFDFDFDFDFD000000ULL,
    0xFDFDFDFDFDFD0000ULL,
    0xFDFDFDFDFDFDFD00ULL,
    0xFDFDFDFDFDFDFDFDULL,
    0x00FDFDFDFDFDFDFDULL,
    0x0000FDFDFDFDFDFDULL,
    0x000000FDFDFDFDFDULL,
    0x00000000FDFDFDFDULL,
    0x0000000000FDFDFDULL,
    0x000000000000FDFDULL,
    0x00000000000000FDULL,
};

word screen_triangles[28][32];
/*
  qword triangles[4][16]= //[direction][value]
  {
  {
  0x00000000, 0x10000000, 0x21000000, 0x32100000, 0x43210000, 0x54321000, 0x65432100, 0x76543210, 0x87654321, 0x88765432, 0x88876543, 0x88887654, 0x88888765, 0x88888876, 0x88888887, 0x88888888
  },
  {
  0x00000000, 0xF0000000, 0xEF000000, 0xFDF00000, 0xCFDF0000, 0xBCFDF000, 0xABCFDF00, 0x9ABCFDF0, 0x89ABCFDF, 0x889ABCFD, 0x8889ABCD, 0x88889ABC, 0x888889AB, 0x8888889A, 0x88888889, 0x88888888
  },
  {
  0x00000000, 0x00000001, 0x00000012, 0x00000123, 0x00001234, 0x00012345, 0x00123456, 0x01234567, 0x12345678, 0x23456788, 0x34567888, 0x45678888, 0x56788888, 0x67888888, 0x78888888, 0x88888888
  },
  {
  0x00000000, 0x0000000F, 0x000000FE, 0x00000FED, 0x0000FEDC, 0x000FEDCB, 0x00FEDCBA, 0x0FEDCBA9, 0xFEDCBA98, 0xEDCBA988, 0xDCBA9888, 0xCBA98888, 0xBA988888, 0xA9888888, 0x98888888, 0x88888888
  }
  };
  */


/*
  byte triangles[4][16][8]= //[direction][value][line]
  {
  {
  {
  0,  0,  0,  0,  0,  0,  0,  0
  },
  {
  1,  0,  0,  0,  0,  0,  0,  0
  },
  {
  2,  1,  0,  0,  0,  0,  0,  0
  },
  {
  3,  2,  1,  0,  0,  0,  0,  0
  },
  {
  4,  3,  2,  1,  0,  0,  0,  0
  },
  {
  5,  4,  3,  2,  1,  0,  0,  0
  },
  {
  6,  5,  4,  3,  2,  1,  0,  0
  },
  {
  7,  6,  5,  4,  3,  2,  1,  0
  },
  {
  8,  7,  6,  5,  4,  3,  2,  1
  },
  {
  8,  8,  7,  6,  5,  4,  3,  2
  },
  {
  8,  8,  8,  7,  6,  5,  4,  3
  },
  {
  8,  8,  8,  8,  7,  6,  5,  4
  },
  {
  8,  8,  8,  8,  8,  7,  6,  5
  },
  {
  8,  8,  8,  8,  8,  8,  7,  6
  },
  {
  8,  8,  8,  8,  8,  8,  8,  7
  },
  {
  8,  8,  8,  8,  8,  8,  8,  8
  }
  },
  {
  {
  0,  0,  0,  0,  0,  0,  0,  0
  },
  {
  15,  0,  0,  0,  0,  0,  0,  0
  },
  {
  14, 15,  0,  0,  0,  0,  0,  0
  },
  {
  13, 14, 15,  0,  0,  0,  0,  0
  },
  {
  12, 13, 14, 15,  0,  0,  0,  0
  },
  {
  11, 12, 13, 14, 15,  0,  0,  0
  },
  {
  10, 11, 12, 13, 14, 15,  0,  0
  },
  {
  9, 10, 11, 12, 13, 14, 15,  0
  },
  {
  8,  9, 10, 11, 12, 13, 14, 15
  },
  {
  8,  8,  9, 10, 11, 12, 13, 14
  },
  {
  8,  8,  8,  9, 10, 11, 12, 13
  },
  {
  8,  8,  8,  8,  9, 10, 11, 12
  },
  {
  8,  8,  8,  8,  8,  9, 10, 11
  },
  {
  8,  8,  8,  8,  8,  8,  9, 10
  },
  {
  8,  8,  8,  8,  8,  8,  8,  9
  },
  {
  8,  8,  8,  8,  8,  8,  8,  8
  }
  },
  {
  {
  0,  0,  0,  0,  0,  0,  0,  0
  },
  {
  0,  0,  0,  0,  0,  0,  0,  1
  },
  {
  0,  0,  0,  0,  0,  0,  1,  2
  },
  {
  0,  0,  0,  0,  0,  1,  2,  3
  },
  {
  0,  0,  0,  0,  1,  2,  3,  4
  },
  {
  0,  0,  0,  1,  2,  3,  4,  5
  },
  {
  0,  0,  1,  2,  3,  4,  5,  6
  },
  {
  0,  1,  2,  3,  4,  5,  6,  7
  },
  {
  1,  2,  3,  4,  5,  6,  7,  8
  },
  {
  2,  3,  4,  5,  6,  7,  8,  8
  },
  {
  3,  4,  5,  6,  7,  8,  8,  8
  },
  {
  4,  5,  6,  7,  8,  8,  8,  8
  },
  {
  5,  6,  7,  8,  8,  8,  8,  8
  },
  {
  6,  7,  8,  8,  8,  8,  8,  8
  },
  {
  7,  8,  8,  8,  8,  8,  8,  8
  },
  {
  8,  8,  8,  8,  8,  8,  8,  8
  }
  },
  {
  {
  0,  0,  0,  0,  0,  0,  0,  0
  },
  {
  0,  0,  0,  0,  0,  0,  0, 15
  },
  {
  0,  0,  0,  0,  0,  0, 15, 14
  },
  {
  0,  0,  0,  0,  0, 15, 14, 13
  },
  {
  0,  0,  0,  0, 15, 14, 13, 12
  },
  {
  0,  0,  0, 15, 14, 13, 12, 11
  },
  {
  0,  0, 15, 14, 13, 12, 11, 10
  },
  {
  0, 15, 14, 13, 12, 11, 10,  9
  },
  {
  15, 14, 13, 12, 11, 10,  9,  8
  },
  {
  14, 13, 12, 11, 10,  9,  8,  8
  },
  {
  13, 12, 11, 10,  9,  8,  8,  8
  },
  {
  12, 11, 10,  9,  8,  8,  8,  8
  },
  {
  11, 10,  9,  8,  8,  8,  8,  8
  },
  {
  10,  9,  8,  8,  8,  8,  8,  8
  },
  {
  9,  8,  8,  8,  8,  8,  8,  8
  },
  {
  8,  8,  8,  8,  8,  8,  8,  8
  }
  }
  };
  */



/*
  for (int blockrow=0; blockrow<30; ++i)
  {
  for (int linerow=0; linerow<8; ++i)
  {
  qword *triangleline=(qword*)(tmp_scr->line[(blockrow*8+linerow)]);
  for (int blockcolumn=0; blockcolumn<40; ++i)
  {
  triangleline=triangles[0][screen_triangles[blockrow][blockcolumn]][linerow];
  ++triangleline;
  }
  }
  }
  */

// the ULL suffixes are to prevent this warning:
// warning: integer constant is too large for "long" type

qword triangles[4][16][8]= //[direction][value][line]
{
    {
        {
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL
        },
        {
            0xFD00000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL
        },
        {
            0xFDFD000000000000ULL,
            0xFD00000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL
        },
        {
            0xFDFDFD0000000000ULL,
            0xFDFD000000000000ULL,
            0xFD00000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL
        },
        {
            0xFDFDFDFD00000000ULL,
            0xFDFDFD0000000000ULL,
            0xFDFD000000000000ULL,
            0xFD00000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL
        },
        {
            0xFDFDFDFDFD000000ULL,
            0xFDFDFDFD00000000ULL,
            0xFDFDFD0000000000ULL,
            0xFDFD000000000000ULL,
            0xFD00000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL
        },
        {
            0xFDFDFDFDFDFD0000ULL,
            0xFDFDFDFDFD000000ULL,
            0xFDFDFDFD00000000ULL,
            0xFDFDFD0000000000ULL,
            0xFDFD000000000000ULL,
            0xFD00000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL
        },
        {
            0xFDFDFDFDFDFDFD00ULL,
            0xFDFDFDFDFDFD0000ULL,
            0xFDFDFDFDFD000000ULL,
            0xFDFDFDFD00000000ULL,
            0xFDFDFD0000000000ULL,
            0xFDFD000000000000ULL,
            0xFD00000000000000ULL,
            0x0000000000000000ULL
        },
        {
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFD00ULL,
            0xFDFDFDFDFDFD0000ULL,
            0xFDFDFDFDFD000000ULL,
            0xFDFDFDFD00000000ULL,
            0xFDFDFD0000000000ULL,
            0xFDFD000000000000ULL,
            0xFD00000000000000ULL
        },
        {
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFD00ULL,
            0xFDFDFDFDFDFD0000ULL,
            0xFDFDFDFDFD000000ULL,
            0xFDFDFDFD00000000ULL,
            0xFDFDFD0000000000ULL,
            0xFDFD000000000000ULL
        },
        {
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFD00ULL,
            0xFDFDFDFDFDFD0000ULL,
            0xFDFDFDFDFD000000ULL,
            0xFDFDFDFD00000000ULL,
            0xFDFDFD0000000000ULL
        },
        {
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFD00ULL,
            0xFDFDFDFDFDFD0000ULL,
            0xFDFDFDFDFD000000ULL,
            0xFDFDFDFD00000000ULL
        },
        {
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFD00ULL,
            0xFDFDFDFDFDFD0000ULL,
            0xFDFDFDFDFD000000ULL
        },
        {
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFD00ULL,
            0xFDFDFDFDFDFD0000ULL
        },
        {
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFD00ULL
        },
        {
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL
        }
    },
    {
        {
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL
        },
        {
            0x00000000000000FDULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL
        },
        {
            0x000000000000FDFDULL,
            0x00000000000000FDULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL
        },
        {
            0x0000000000FDFDFDULL,
            0x000000000000FDFDULL,
            0x00000000000000FDULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL
        },
        {
            0x00000000FDFDFDFDULL,
            0x0000000000FDFDFDULL,
            0x000000000000FDFDULL,
            0x00000000000000FDULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL
        },
        {
            0x000000FDFDFDFDFDULL,
            0x00000000FDFDFDFDULL,
            0x0000000000FDFDFDULL,
            0x000000000000FDFDULL,
            0x00000000000000FDULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL
        },
        {
            0x0000FDFDFDFDFDFDULL,
            0x000000FDFDFDFDFDULL,
            0x00000000FDFDFDFDULL,
            0x0000000000FDFDFDULL,
            0x000000000000FDFDULL,
            0x00000000000000FDULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL
        },
        {
            0x00FDFDFDFDFDFDFDULL,
            0x0000FDFDFDFDFDFDULL,
            0x000000FDFDFDFDFDULL,
            0x00000000FDFDFDFDULL,
            0x0000000000FDFDFDULL,
            0x000000000000FDFDULL,
            0x00000000000000FDULL,
            0x0000000000000000ULL
        },
        {
            0xFDFDFDFDFDFDFDFDULL,
            0x00FDFDFDFDFDFDFDULL,
            0x0000FDFDFDFDFDFDULL,
            0x000000FDFDFDFDFDULL,
            0x00000000FDFDFDFDULL,
            0x0000000000FDFDFDULL,
            0x000000000000FDFDULL,
            0x00000000000000FDULL
        },
        {
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0x00FDFDFDFDFDFDFDULL,
            0x0000FDFDFDFDFDFDULL,
            0x000000FDFDFDFDFDULL,
            0x00000000FDFDFDFDULL,
            0x0000000000FDFDFDULL,
            0x000000000000FDFDULL
        },
        {
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0x00FDFDFDFDFDFDFDULL,
            0x0000FDFDFDFDFDFDULL,
            0x000000FDFDFDFDFDULL,
            0x00000000FDFDFDFDULL,
            0x0000000000FDFDFDULL
        },
        {
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0x00FDFDFDFDFDFDFDULL,
            0x0000FDFDFDFDFDFDULL,
            0x000000FDFDFDFDFDULL,
            0x00000000FDFDFDFDULL
        },
        {
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0x00FDFDFDFDFDFDFDULL,
            0x0000FDFDFDFDFDFDULL,
            0x000000FDFDFDFDFDULL
        },
        {
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0x00FDFDFDFDFDFDFDULL,
            0x0000FDFDFDFDFDFDULL
        },
        {
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0x00FDFDFDFDFDFDFDULL
        },
        {
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL
        }
    },
    {
        {
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL
        },
        {
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0xFD00000000000000ULL
        },
        {
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0xFD00000000000000ULL,
            0xFDFD000000000000ULL
        },
        {
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0xFD00000000000000ULL,
            0xFDFD000000000000ULL,
            0xFDFDFD0000000000ULL
        },
        {
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0xFD00000000000000ULL,
            0xFDFD000000000000ULL,
            0xFDFDFD0000000000ULL,
            0xFDFDFDFD00000000ULL
        },
        {
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0xFD00000000000000ULL,
            0xFDFD000000000000ULL,
            0xFDFDFD0000000000ULL,
            0xFDFDFDFD00000000ULL,
            0xFDFDFDFDFD000000ULL
        },
        {
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0xFD00000000000000ULL,
            0xFDFD000000000000ULL,
            0xFDFDFD0000000000ULL,
            0xFDFDFDFD00000000ULL,
            0xFDFDFDFDFD000000ULL,
            0xFDFDFDFDFDFD0000ULL
        },
        {
            0x0000000000000000ULL,
            0xFD00000000000000ULL,
            0xFDFD000000000000ULL,
            0xFDFDFD0000000000ULL,
            0xFDFDFDFD00000000ULL,
            0xFDFDFDFDFD000000ULL,
            0xFDFDFDFDFDFD0000ULL,
            0xFDFDFDFDFDFDFD00ULL
        },
        {
            0xFD00000000000000ULL,
            0xFDFD000000000000ULL,
            0xFDFDFD0000000000ULL,
            0xFDFDFDFD00000000ULL,
            0xFDFDFDFDFD000000ULL,
            0xFDFDFDFDFDFD0000ULL,
            0xFDFDFDFDFDFDFD00ULL,
            0xFDFDFDFDFDFDFDFDULL
        },
        {
            0xFDFD000000000000ULL,
            0xFDFDFD0000000000ULL,
            0xFDFDFDFD00000000ULL,
            0xFDFDFDFDFD000000ULL,
            0xFDFDFDFDFDFD0000ULL,
            0xFDFDFDFDFDFDFD00ULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL
        },
        {
            0xFDFDFD0000000000ULL,
            0xFDFDFDFD00000000ULL,
            0xFDFDFDFDFD000000ULL,
            0xFDFDFDFDFDFD0000ULL,
            0xFDFDFDFDFDFDFD00ULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL
        },
        {
            0xFDFDFDFD00000000ULL,
            0xFDFDFDFDFD000000ULL,
            0xFDFDFDFDFDFD0000ULL,
            0xFDFDFDFDFDFDFD00ULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL
        },
        {
            0xFDFDFDFDFD000000ULL,
            0xFDFDFDFDFDFD0000ULL,
            0xFDFDFDFDFDFDFD00ULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL
        },
        {
            0xFDFDFDFDFDFD0000ULL,
            0xFDFDFDFDFDFDFD00ULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL
        },
        {
            0xFDFDFDFDFDFDFD00ULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL
        },
        {
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL
        }
    },
    {
        {
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL
        },
        {
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x00000000000000FDULL
        },
        {
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x00000000000000FDULL,
            0x000000000000FDFDULL
        },
        {
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x00000000000000FDULL,
            0x000000000000FDFDULL,
            0x0000000000FDFDFDULL
        },
        {
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x00000000000000FDULL,
            0x000000000000FDFDULL,
            0x0000000000FDFDFDULL,
            0x00000000FDFDFDFDULL
        },
        {
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x00000000000000FDULL,
            0x000000000000FDFDULL,
            0x0000000000FDFDFDULL,
            0x00000000FDFDFDFDULL,
            0x000000FDFDFDFDFDULL
        },
        {
            0x0000000000000000ULL,
            0x0000000000000000ULL,
            0x00000000000000FDULL,
            0x000000000000FDFDULL,
            0x0000000000FDFDFDULL,
            0x00000000FDFDFDFDULL,
            0x000000FDFDFDFDFDULL,
            0x0000FDFDFDFDFDFDULL
        },
        {
            0x0000000000000000ULL,
            0x00000000000000FDULL,
            0x000000000000FDFDULL,
            0x0000000000FDFDFDULL,
            0x00000000FDFDFDFDULL,
            0x000000FDFDFDFDFDULL,
            0x0000FDFDFDFDFDFDULL,
            0x00FDFDFDFDFDFDFDULL
        },
        {
            0x00000000000000FDULL,
            0x000000000000FDFDULL,
            0x0000000000FDFDFDULL,
            0x00000000FDFDFDFDULL,
            0x000000FDFDFDFDFDULL,
            0x0000FDFDFDFDFDFDULL,
            0x00FDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL
        },
        {
            0x000000000000FDFDULL,
            0x0000000000FDFDFDULL,
            0x00000000FDFDFDFDULL,
            0x000000FDFDFDFDFDULL,
            0x0000FDFDFDFDFDFDULL,
            0x00FDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL
        },
        {
            0x0000000000FDFDFDULL,
            0x00000000FDFDFDFDULL,
            0x000000FDFDFDFDFDULL,
            0x0000FDFDFDFDFDFDULL,
            0x00FDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL
        },
        {
            0x00000000FDFDFDFDULL,
            0x000000FDFDFDFDFDULL,
            0x0000FDFDFDFDFDFDULL,
            0x00FDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL
        },
        {
            0x000000FDFDFDFDFDULL,
            0x0000FDFDFDFDFDFDULL,
            0x00FDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL
        },
        {
            0x0000FDFDFDFDFDFDULL,
            0x00FDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL
        },
        {
            0x00FDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL
        },
        {
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL,
            0xFDFDFDFDFDFDFDFDULL
        }
    }
};

int black_opening_count=0;
int black_opening_x,black_opening_y;
int black_opening_shape;

int choose_opening_shape()
{
    // First, count how many bits are set
    int numBits=0;
    int bitCounter;
    
    for(int i=0; i<4; i++)
    {
        if(COOLSCROLL&(1<<i))
            numBits++;
    }
    
    // Shouldn't happen...
    if(numBits==0)
        return bosCIRCLE;
        
    // Pick a bit
    bitCounter=rand()%numBits+1;
    
    for(int i=0; i<4; i++)
    {
        // If this bit is set, decrement the bit counter
        if(COOLSCROLL&(1<<i))
            bitCounter--;
            
        // When the counter hits 0, return a value based on
        // which bit it stopped on.
        // Reminder: enum {bosCIRCLE=0, bosOVAL, bosTRIANGLE, bosSMAS, bosMAX};
        if(bitCounter==0)
            return i;
    }
    
    // Shouldn't be necessary, but the compiler might complain, at least
    return bosCIRCLE;
}

void close_black_opening(int x, int y, bool wait)
{
    black_opening_shape=choose_opening_shape();
    
    int w=256, h=224;
    int blockrows=28, blockcolumns=32;
    int xoffset=(x-(w/2))/8, yoffset=(y-(h/2))/8;
    
    for(int blockrow=0; blockrow<blockrows; ++blockrow)  //30
    {
        for(int blockcolumn=0; blockcolumn<blockcolumns; ++blockcolumn)  //40
        {
            screen_triangles[blockrow][blockcolumn]=zc_max(abs(int(double(blockcolumns-1)/2-blockcolumn+xoffset)),abs(int(double(blockrows-1)/2-blockrow+yoffset)))|0x0100|((blockrow-yoffset<blockrows/2)?0:0x8000)|((blockcolumn-xoffset<blockcolumns/2)?0x4000:0);
        }
    }
    
    black_opening_count = 66;
    black_opening_x = x;
    black_opening_y = y;
    lensclk = 0;
    //black_opening_shape=(black_opening_shape+1)%bosMAX;
    
    if(wait)
    {
        for(int i=0; i<66; i++)
        {
            draw_screen(tmpscr);
            put_passive_subscr(framebuf,&QMisc,0,passive_subscreen_offset,false,sspUP);
            syskeys();
            advanceframe(true);
            
            if(Quit)
            {
                break;
            }
        }
    }
}

void open_black_opening(int x, int y, bool wait)
{
    black_opening_shape=choose_opening_shape();
    
    int w=256, h=224;
    int blockrows=28, blockcolumns=32;
    int xoffset=(x-(w/2))/8, yoffset=(y-(h/2))/8;
    
    for(int blockrow=0; blockrow<blockrows; ++blockrow)  //30
    {
        for(int blockcolumn=0; blockcolumn<blockcolumns; ++blockcolumn)  //40
        {
            screen_triangles[blockrow][blockcolumn]=zc_max(abs(int(double(blockcolumns-1)/2-blockcolumn+xoffset)),abs(int(double(blockrows-1)/2-blockrow+yoffset)))|0x0100|((blockrow-yoffset<blockrows/2)?0:0x8000)|((blockcolumn-xoffset<blockcolumns/2)?0x4000:0);
        }
    }
    
    black_opening_count = -66;
    black_opening_x = x;
    black_opening_y = y;
    lensclk = 0;
    
    if(wait)
    {
        for(int i=0; i<66; i++)
        {
            draw_screen(tmpscr);
            put_passive_subscr(framebuf,&QMisc,0,passive_subscreen_offset,false,sspUP);
            syskeys();
            advanceframe(true);
            
            if(Quit)
            {
                break;
            }
        }
    }
}

void black_opening(BITMAP *dest,int x,int y,int a,int max_a)
{
    clear_to_color(tmp_scr,BLACK);
    int w=256, h=224;
    
    switch(black_opening_shape)
    {
    case bosOVAL:
    {
        double new_w=(w/2)+abs(w/2-x);
        double new_h=(h/2)+abs(h/2-y);
        double b=sqrt(((new_w*new_w)/4)+(new_h*new_h));
        ellipsefill(tmp_scr,x,y,int(2*a*b/max_a)/8*8,int(a*b/max_a)/8*8,0);
        break;
    }
    
    case bosTRIANGLE:
    {
        double new_w=(w/2)+abs(w/2-x);
        double new_h=(h/2)+abs(h/2-y);
        double r=a*(new_w*sqrt((double)3)+new_h)/max_a;
        double P2= (PI/2);
        double P23=(2*PI/3);
        double P43=(4*PI/3);
        double Pa= (-4*PI*a/(3*max_a));
        double angle=P2+Pa;
        double a0=angle;
        double a2=angle+P23;
        double a4=angle+P43;
        triangle(tmp_scr, x+int(cos(a0)*r), y-int(sin(a0)*r),
                 x+int(cos(a2)*r), y-int(sin(a2)*r),
                 x+int(cos(a4)*r), y-int(sin(a4)*r),
                 0);
        break;
    }
    
    case bosSMAS:
    {
        int distance=zc_max(abs(w/2-x),abs(h/2-y))/8;
        
        for(int blockrow=0; blockrow<28; ++blockrow)  //30
        {
            for(int linerow=0; linerow<8; ++linerow)
            {
                qword *triangleline=(qword*)(tmp_scr->line[(blockrow*8+linerow)]);
                
                for(int blockcolumn=0; blockcolumn<32; ++blockcolumn)  //40
                {
                    *triangleline=triangles[(screen_triangles[blockrow][blockcolumn]&0xC000)>>14]
                                  [zc_min(zc_max((((31+distance)*(max_a-a)/max_a)+((screen_triangles[blockrow][blockcolumn]&0x0FFF)-0x0100)-(15+distance)),0),15)]
                                  [linerow];
                    ++triangleline;
                    
                    if(linerow==0)
                    {
                    }
                }
            }
        }
        
        break;
    }
    
    case bosCIRCLE:
    default:
    {
        double new_w=(w/2)+abs(w/2-x);
        double new_h=(h/2)+abs(h/2-y);
        int r=int(sqrt((new_w*new_w)+(new_h*new_h))*a/max_a);
        //circlefill(tmp_scr,x,y,a<<3,0);
        circlefill(tmp_scr,x,y,r,0);
        break;
    }
    }
    
    masked_blit(tmp_scr,dest,0,0,0,0,320,240);
}

//----------------------------------------------------------------

bool item_disabled(int item)                 //is this item disabled?
{
    return (item>=0 && game->items_off[item] != 0);
}

bool can_use_item(int item_type, int item)                  //can Link use this item?
{
    if(current_item(item_type, true) >=item)
    {
        return true;
    }
    
    return false;
}

bool has_item(int item_type, int it)                        //does Link possess this item?
{
    switch(item_type)
    {
    case itype_bomb:
    case itype_sbomb:
    {
        int itemid = getItemID(itemsbuf, item_type, it);
        
        if(itemid == -1)
            return false;
            
        return (game->get_item(itemid));
    }
    
    case itype_clock:
        return Link.getClock()?1:0;
        
    case itype_key:
        return (game->get_keys()>0);
        
    case itype_magiccontainer:
        return (game->get_maxmagic()>=MAGICPERBLOCK);
        
    case itype_triforcepiece:                               //it: -2=any, -1=current level, other=that level
    {
        switch(it)
        {
        case -2:
        {
            for(int i=0; i<MAXLEVELS; i++)
            {
                if(game->lvlitems[i]&liTRIFORCE)
                {
                    return true;
                }
            }
            
            return false;
            break;
        }
        
        case -1:
            return (game->lvlitems[dlevel]&liTRIFORCE);
            break;
            
        default:
            if(it>=0&&it<MAXLEVELS)
            {
                return (game->lvlitems[it]&liTRIFORCE);
            }
            
            break;
        }
        
        return 0;
    }
    
    case itype_map:                                         //it: -2=any, -1=current level, other=that level
    {
        switch(it)
        {
        case -2:
        {
            for(int i=0; i<MAXLEVELS; i++)
            {
                if(game->lvlitems[i]&liMAP)
                {
                    return true;
                }
            }
            
            return false;
        }
        break;
        
        case -1:
            return (game->lvlitems[dlevel]&liMAP)!=0;
            break;
            
        default:
            if(it>=0&&it<MAXLEVELS)
            {
                return (game->lvlitems[it]&liMAP)!=0;
            }
            
            break;
        }
        
        return 0;
    }
    
    case itype_compass:                                     //it: -2=any, -1=current level, other=that level
    {
        switch(it)
        {
        case -2:
        {
            for(int i=0; i<MAXLEVELS; i++)
            {
                if(game->lvlitems[i]&liCOMPASS)
                {
                    return true;
                }
            }
            
            return false;
            break;
        }
        
        case -1:
            return (game->lvlitems[dlevel]&liCOMPASS)!=0;
            break;
            
        default:
            if(it>=0&&it<MAXLEVELS)
            {
                return (game->lvlitems[it]&liCOMPASS)!=0;
            }
            
            break;
        }
        
        return 0;
    }
    
    case itype_bosskey:                                     //it: -2=any, -1=current level, other=that level
    {
        switch(it)
        {
        case -2:
        {
            for(int i=0; i<MAXLEVELS; i++)
            {
                if(game->lvlitems[i]&liBOSSKEY)
                {
                    return true;
                }
            }
            
            return false;
            break;
        }
        
        case -1:
            return (game->lvlitems[dlevel]&liBOSSKEY)?1:0;
            break;
            
        default:
            if(it>=0&&it<MAXLEVELS)
            {
                return (game->lvlitems[it]&liBOSSKEY)?1:0;
            }
            
            break;
        }
        
        return 0;
    }
    
    default:
        //it=(1<<(it-1));
        /*if (item_type>=itype_max)
        {
          Z_error("Error","has_item exception",NULL,NULL,"O&K",NULL,'k',0,lfont);
        
          return false;
        }*/
        int itemid = getItemID(itemsbuf, item_type, it);
        
        if(itemid == -1)
            return false;
            
        return game->get_item(itemid);
        break;
    }
}


int current_item(int item_type, bool checkenabled)           //item currently being used
{
    switch(item_type)
    {
    case itype_clock:
        return has_item(itype_clock,1) ? 1 : 0;
        break;
        
    case itype_key:
        return game->get_keys();
        
    case itype_lkey:
        return game->lvlkeys[get_dlevel()];
        
    case itype_magiccontainer:
        return game->get_maxmagic()/MAGICPERBLOCK;
        
    case itype_triforcepiece:
    {
        int count=0;
        
        for(int i=0; i<MAXLEVELS; i++)
        {
            count+=(game->lvlitems[i]&liTRIFORCE)?1:0;
        }
        
        return count;
        break;
    }
    
    case itype_map:
    {
        int count=0;
        
        for(int i=0; i<MAXLEVELS; i++)
        {
            count+=(game->lvlitems[i]&liMAP)?1:0;
        }
        
        return count;
        break;
    }
    
    case itype_compass:
    {
        int count=0;
        
        for(int i=0; i<MAXLEVELS; i++)
        {
            count+=(game->lvlitems[i]&liCOMPASS)?1:0;
        }
        
        return count;
        break;
    }
    
    case itype_bosskey:
    {
        int count=0;
        
        for(int i=0; i<MAXLEVELS; i++)
        {
            count+=(game->lvlitems[i]&liBOSSKEY)?1:0;
        }
        
        return count;
        break;
    }
    
    default:
        int maxid = getHighestLevelOfFamily(game, itemsbuf, item_type, checkenabled);
        
        if(maxid == -1)
            return 0;
            
        return itemsbuf[maxid].fam_type;
        break;
    }
}

int current_item(int item_type)           //item currently being used
{
    return current_item(item_type, true);
}

std::map<int, int> itemcache;

// Not actually used by anything at the moment...
void removeFromItemCache(int itemid)
{
    itemcache.erase(itemid);
}

void flushItemCache()
{
    itemcache.clear();
    
    //also fix the active subscreen if items were deleted -DD
    if(game != NULL)
    {
        verifyBothWeapons();
        load_Sitems(&QMisc);
    }
}

// This is used often, so it should be as direct as possible.
int current_item_id(int itemtype, bool checkmagic)
{
    if(itemtype!=itype_ring)  // Rings must always be checked.
    {
        std::map<int,int>::iterator res = itemcache.find(itemtype);
        
        if(res != itemcache.end())
            return res->second;
    }
    
    int result = -1;
    int highestlevel = -1;
    
    for(int i=0; i<MAXITEMS; i++)
    {
        if(game->get_item(i) && itemsbuf[i].family==itemtype && !item_disabled(i))
        {
            if((checkmagic || itemtype == itype_ring) && itemtype != itype_magicring)
            {
                //printf("Checkmagic for %d: %d (%d %d)\n",i,checkmagiccost(i),itemsbuf[i].magic*game->get_magicdrainrate(),game->get_magic());
                if(!checkmagiccost(i))
                {
                    continue;
                }
            }
            
            if(itemsbuf[i].fam_type >= highestlevel)
            {
                highestlevel = itemsbuf[i].fam_type;
                result=i;
            }
        }
    }
    
    itemcache[itemtype] = result;
    return result;
}

int current_item_power(int itemtype)
{
    int result = current_item_id(itemtype,true);
    return (result<0) ? 0 : itemsbuf[result].power;
}

int item_tile_mod(bool)
{
    int tile=0;
    int ret=0;
    
    ret=game->get_bombs();
    
    switch(ret)
    {
    case 0:
        ret=0;
        break;
        
    default:
        if(current_item_id(itype_bomb,false)>=0)
            ret=itemsbuf[current_item_id(itype_bomb,false)].ltm;
        else
            ret=0;
            
        break;
        
    }
    
    tile+=ret;
    
    ret=game->get_sbombs();
    
    switch(ret)
    {
    case 0:
        ret=0;
        break;
        
    default:
        if(current_item_id(itype_sbomb,false)>=0)
            ret=itemsbuf[current_item_id(itype_sbomb,false)].ltm;
        else
            ret=0;
            
        break;
    }
    
    tile+=ret;
    
    ret=current_item(itype_clock);
    
    switch(ret)
    {
    case 1:
        ret=itemsbuf[iClock].ltm;
        break;
        
    default:
        ret=0;
        break;
    }
    
    tile+=ret;
    
    ret=current_item(itype_key);
    
    switch(ret)
    {
    case 0:
        ret=0;
        break;
        
    default:
        ret=itemsbuf[iKey].ltm;
        break;
    }
    
    tile+=ret;
    
    ret=current_item(itype_lkey);
    
    switch(ret)
    {
    case 0:
        ret=0;
        break;
        
    default:
        ret=itemsbuf[iLevelKey].ltm;
        break;
    }
    
    tile+=ret;
    
    ret=current_item(itype_map);
    
    switch(ret)
    {
    case 0:
        ret=0;
        break;
        
    default:
        ret=itemsbuf[iMap].ltm;
        break;
    }
    
    tile+=ret;
    
    ret=current_item(itype_compass);
    
    switch(ret)
    {
    case 0:
        ret=0;
        break;
        
    default:
        ret=itemsbuf[iCompass].ltm;
        break;
    }
    
    tile+=ret;
    
    ret=current_item(itype_bosskey);
    
    switch(ret)
    {
    case 0:
        ret=0;
        break;
        
    default:
        ret=itemsbuf[iBossKey].ltm;
        break;
    }
    
    tile+=ret;
    
    ret=current_item(itype_magiccontainer);
    
    switch(ret)
    {
    case 0:
        ret=0;
        break;
        
    default:
        ret=itemsbuf[iMagicC].ltm;
        break;
    }
    
    tile+=ret;
    
    ret=current_item(itype_triforcepiece);
    
    switch(ret)
    {
    case 0:
        ret=0;
        break;
        
    default:
        ret=itemsbuf[iTriforce].ltm;
        break;
    }
    
    tile+=ret;
    
    for(int i=0; i<itype_max; i++)
    {
        ret=current_item_id(i,false);
        
        if(ret >= 0)
            tile+=itemsbuf[ret].ltm;
    }
    
    return tile;
}

int dmap_tile_mod()
{
    return 0;
}

// Hints are drawn on a separate layer to combo reveals.
void draw_lens_under(BITMAP *dest, bool layer)
{
    bool hints = (layer && get_bit(quest_rules,qr_LENSHINTS));
    
    int strike_hint_table[11]=
    {
        mfARROW, mfBOMB, mfBRANG, mfWANDMAGIC,
        mfSWORD, mfREFMAGIC, mfHOOKSHOT,
        mfREFFIREBALL, mfHAMMER, mfSWORDBEAM, mfWAND
    };
    
    //  int page = tmpscr->cpage;
    {
        int blink_rate=1;
        //    int temptimer=0;
        int tempitem, tempweapon=0;
        strike_hint=strike_hint_table[strike_hint_counter];
        
        if(strike_hint_timer>32)
        {
            strike_hint_timer=0;
            strike_hint_counter=((strike_hint_counter+1)%11);
        }
        
        ++strike_hint_timer;
        
        for(int i=0; i<176; i++)
        {
            int x = (i & 15) << 4;
            int y = (i & 0xF0) + playing_field_offset;
            int tempitemx=-16, tempitemy=-16;
            int tempweaponx=-16, tempweapony=-16;
            
            for(int iter=0; iter<2; ++iter)
            {
                int checkflag=0;
                
                if(iter==0)
                {
                    checkflag=combobuf[tmpscr->data[i]].flag;
                }
                else
                {
                    checkflag=tmpscr->sflag[i];
                }
                
                if(checkflag==mfSTRIKE)
                {
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sSTRIKE],tmpscr->secretcset[sSTRIKE]);
                    else
                    {
                        checkflag = strike_hint;
                    }
                }
                
                switch(checkflag)
                {
                case 0:
                case mfZELDA:
                case mfPUSHED:
                case mfENEMY0:
                case mfENEMY1:
                case mfENEMY2:
                case mfENEMY3:
                case mfENEMY4:
                case mfENEMY5:
                case mfENEMY6:
                case mfENEMY7:
                case mfENEMY8:
                case mfENEMY9:
                case mfSINGLE:
                case mfSINGLE16:
                case mfNOENEMY:
                case mfTRAP_H:
                case mfTRAP_V:
                case mfTRAP_4:
                case mfTRAP_LR:
                case mfTRAP_UD:
                case mfNOGROUNDENEMY:
                case mfNOBLOCKS:
                case mfSCRIPT1:
                case mfSCRIPT2:
                case mfSCRIPT3:
                case mfSCRIPT4:
                case mfSCRIPT5:
                    break;
                    
                case mfPUSHUD:
                case mfPUSHLR:
                case mfPUSH4:
                case mfPUSHU:
                case mfPUSHD:
                case mfPUSHL:
                case mfPUSHR:
                case mfPUSHUDNS:
                case mfPUSHLRNS:
                case mfPUSH4NS:
                case mfPUSHUNS:
                case mfPUSHDNS:
                case mfPUSHLNS:
                case mfPUSHRNS:
                case mfPUSHUDINS:
                case mfPUSHLRINS:
                case mfPUSH4INS:
                case mfPUSHUINS:
                case mfPUSHDINS:
                case mfPUSHLINS:
                case mfPUSHRINS:
                    if(!hints && ((!key[KEY_N] && (lensclk&16))
                                  || (key[KEY_N] && (frame&16))))
                    {
                        putcombo(dest,x,y,tmpscr->undercombo,tmpscr->undercset);
                    }
                    
                    if((!key[KEY_N] && (lensclk&blink_rate))
                            || (key[KEY_N] && (frame&blink_rate)))
                    {
                        if(hints)
                        {
                            switch(combobuf[tmpscr->data[i]].type)
                            {
                            case cPUSH_HEAVY:
                            case cPUSH_HW:
                                tempitem=getItemIDPower(itemsbuf,itype_bracelet,1);
                                tempitemx=x, tempitemy=y;
                                
                                if(tempitem>-1)
                                    putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                                    
                                break;
                                
                            case cPUSH_HEAVY2:
                            case cPUSH_HW2:
                                tempitem=getItemIDPower(itemsbuf,itype_bracelet,2);
                                tempitemx=x, tempitemy=y;
                                
                                if(tempitem>-1)
                                    putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                                    
                                break;
                            }
                        }
                    }
                    
                    break;
                    
                case mfWHISTLE:
                    if(hints)
                    {
                        tempitem=getItemID(itemsbuf,itype_whistle,1);
                        
                        if(tempitem<0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                    }
                    
                    break;
                    
                    //Why is this here?
                case mfFAIRY:
                case mfMAGICFAIRY:
                case mfALLFAIRY:
                    if(hints)
                    {
                        tempitem=getItemID(itemsbuf, itype_fairy,1);//iFairyMoving;
                        
                        if(tempitem < 0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                    }
                    
                    break;
                    
                case mfBCANDLE:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sBCANDLE],tmpscr->secretcset[sBCANDLE]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_candle,1);
                        
                        if(tempitem<0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                    }
                    
                    break;
                    
                case mfRCANDLE:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sRCANDLE],tmpscr->secretcset[sRCANDLE]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_candle,2);
                        
                        if(tempitem<0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                    }
                    
                    break;
                    
                case mfWANDFIRE:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sWANDFIRE],tmpscr->secretcset[sWANDFIRE]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_wand,1);
                        
                        if(tempitem<0) break;
                        
                        tempweapon=wFire;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        else
                        {
                            tempweaponx=x;
                            tempweapony=y;
                        }
                        
                        putweapon(dest,tempweaponx,tempweapony,tempweapon, 0, up, lens_hint_weapon[tempweapon][0], lens_hint_weapon[tempweapon][1],-1);
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                    }
                    
                    break;
                    
                case mfDINSFIRE:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sDINSFIRE],tmpscr->secretcset[sDINSFIRE]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_dinsfire,1);
                        
                        if(tempitem<0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                    }
                    
                    break;
                    
                case mfARROW:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sARROW],tmpscr->secretcset[sARROW]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_arrow,1);
                        
                        if(tempitem<0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                    }
                    
                    break;
                    
                case mfSARROW:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sSARROW],tmpscr->secretcset[sSARROW]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_arrow,2);
                        
                        if(tempitem<0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                    }
                    
                    break;
                    
                case mfGARROW:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sGARROW],tmpscr->secretcset[sGARROW]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_arrow,3);
                        
                        if(tempitem<0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                    }
                    
                    break;
                    
                case mfBOMB:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sBOMB],tmpscr->secretcset[sBOMB]);
                    else
                    {
                        //tempitem=getItemID(itemsbuf,itype_bomb,1);
                        tempweapon = wLitBomb;
                        
                        //if (tempitem<0) break;
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempweaponx=x;
                            tempweapony=y;
                        }
                        
                        putweapon(dest,tempweaponx,tempweapony+lens_hint_weapon[tempweapon][4],tempweapon, 0, up, lens_hint_weapon[tempweapon][0], lens_hint_weapon[tempweapon][1],-1);
                    }
                    
                    break;
                    
                case mfSBOMB:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sSBOMB],tmpscr->secretcset[sSBOMB]);
                    else
                    {
                        //tempitem=getItemID(itemsbuf,itype_sbomb,1);
                        //if (tempitem<0) break;
                        tempweapon = wLitSBomb;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempweaponx=x;
                            tempweapony=y;
                        }
                        
                        putweapon(dest,tempweaponx,tempweapony+lens_hint_weapon[tempweapon][4],tempweapon, 0, up, lens_hint_weapon[tempweapon][0], lens_hint_weapon[tempweapon][1],-1);
                    }
                    
                    break;
                    
                case mfARMOS_SECRET:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sSTAIRS],tmpscr->secretcset[sSTAIRS]);
                        
                    break;
                    
                case mfBRANG:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sBRANG],tmpscr->secretcset[sBRANG]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_brang,1);
                        
                        if(tempitem<0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                    }
                    
                    break;
                    
                case mfMBRANG:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sMBRANG],tmpscr->secretcset[sMBRANG]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_brang,2);
                        
                        if(tempitem<0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                    }
                    
                    break;
                    
                case mfFBRANG:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sFBRANG],tmpscr->secretcset[sFBRANG]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_brang,3);
                        
                        if(tempitem<0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                    }
                    
                    break;
                    
                case mfWANDMAGIC:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sWANDMAGIC],tmpscr->secretcset[sWANDMAGIC]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_wand,1);
                        
                        if(tempitem<0) break;
                        
                        tempweapon=itemsbuf[tempitem].wpn3;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        else
                        {
                            tempweaponx=x;
                            tempweapony=y;
                            --lens_hint_weapon[wMagic][4];
                            
                            if(lens_hint_weapon[wMagic][4]<-8)
                            {
                                lens_hint_weapon[wMagic][4]=8;
                            }
                        }
                        
                        putweapon(dest,tempweaponx,tempweapony+lens_hint_weapon[tempweapon][4],tempweapon, 0, up, lens_hint_weapon[tempweapon][0], lens_hint_weapon[tempweapon][1],-1);
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                    }
                    
                    break;
                    
                case mfREFMAGIC:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sREFMAGIC],tmpscr->secretcset[sREFMAGIC]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_shield,3);
                        
                        if(tempitem<0) break;
                        
                        tempweapon=ewMagic;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        else
                        {
                            tempweaponx=x;
                            tempweapony=y;
                            
                            if(lens_hint_weapon[ewMagic][2]==up)
                            {
                                --lens_hint_weapon[ewMagic][4];
                            }
                            else
                            {
                                ++lens_hint_weapon[ewMagic][4];
                            }
                            
                            if(lens_hint_weapon[ewMagic][4]>8)
                            {
                                lens_hint_weapon[ewMagic][2]=up;
                            }
                            
                            if(lens_hint_weapon[ewMagic][4]<=0)
                            {
                                lens_hint_weapon[ewMagic][2]=down;
                            }
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                        putweapon(dest,tempweaponx,tempweapony+lens_hint_weapon[tempweapon][4],tempweapon, 0, lens_hint_weapon[ewMagic][2], lens_hint_weapon[tempweapon][0], lens_hint_weapon[tempweapon][1],-1);
                    }
                    
                    break;
                    
                case mfREFFIREBALL:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sREFFIREBALL],tmpscr->secretcset[sREFFIREBALL]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_shield,3);
                        
                        if(tempitem<0) break;
                        
                        tempweapon=ewFireball;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                            tempweaponx=x;
                            tempweapony=y;
                            ++lens_hint_weapon[ewFireball][3];
                            
                            if(lens_hint_weapon[ewFireball][3]>8)
                            {
                                lens_hint_weapon[ewFireball][3]=-8;
                                lens_hint_weapon[ewFireball][4]=8;
                            }
                            
                            if(lens_hint_weapon[ewFireball][3]>0)
                            {
                                ++lens_hint_weapon[ewFireball][4];
                            }
                            else
                            {
                                --lens_hint_weapon[ewFireball][4];
                            }
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                        putweapon(dest,tempweaponx+lens_hint_weapon[tempweapon][3],tempweapony+lens_hint_weapon[ewFireball][4],tempweapon, 0, up, lens_hint_weapon[tempweapon][0], lens_hint_weapon[tempweapon][1],-1);
                    }
                    
                    break;
                    
                case mfSWORD:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sSWORD],tmpscr->secretcset[sSWORD]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_sword,1);
                        
                        if(tempitem<0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                    }
                    
                    break;
                    
                case mfWSWORD:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sWSWORD],tmpscr->secretcset[sWSWORD]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_sword,2);
                        
                        if(tempitem<0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                    }
                    
                    break;
                    
                case mfMSWORD:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sMSWORD],tmpscr->secretcset[sMSWORD]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_sword,3);
                        
                        if(tempitem<0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                    }
                    
                    break;
                    
                case mfXSWORD:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sXSWORD],tmpscr->secretcset[sXSWORD]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_sword,4);
                        
                        if(tempitem<0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                    }
                    
                    break;
                    
                case mfSWORDBEAM:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sSWORDBEAM],tmpscr->secretcset[sSWORDBEAM]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_sword,1);
                        
                        if(tempitem<0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 1);
                    }
                    
                    break;
                    
                case mfWSWORDBEAM:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sWSWORDBEAM],tmpscr->secretcset[sWSWORDBEAM]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_sword,2);
                        
                        if(tempitem<0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || key[KEY_N] && (frame&blink_rate))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 2);
                    }
                    
                    break;
                    
                case mfMSWORDBEAM:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sMSWORDBEAM],tmpscr->secretcset[sMSWORDBEAM]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_sword,3);
                        
                        if(tempitem<0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 3);
                    }
                    
                    break;
                    
                case mfXSWORDBEAM:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sXSWORDBEAM],tmpscr->secretcset[sXSWORDBEAM]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_sword,4);
                        
                        if(tempitem<0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || key[KEY_N] && (frame&blink_rate))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 4);
                    }
                    
                    break;
                    
                case mfHOOKSHOT:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sHOOKSHOT],tmpscr->secretcset[sHOOKSHOT]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_hookshot,1);
                        
                        if(tempitem<0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                    }
                    
                    break;
                    
                case mfWAND:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sWAND],tmpscr->secretcset[sWAND]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_wand,1);
                        
                        if(tempitem<0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                    }
                    
                    break;
                    
                case mfHAMMER:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[sHAMMER],tmpscr->secretcset[sHAMMER]);
                    else
                    {
                        tempitem=getItemID(itemsbuf,itype_hammer,1);
                        
                        if(tempitem<0) break;
                        
                        if((!key[KEY_N] && (lensclk&blink_rate))
                                || (key[KEY_N] && (frame&blink_rate)))
                        {
                            tempitemx=x;
                            tempitemy=y;
                        }
                        
                        putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                    }
                    
                    break;
                    
                case mfARMOS_ITEM:
                case mfDIVE_ITEM:
                    if(!getmapflag())
                        //          putitem2(dest,x,y,tmpscr->catchall);
                        putitem2(dest,x,y,tmpscr->catchall, lens_hint_item[tmpscr->catchall][0], lens_hint_item[tmpscr->catchall][1], 0);
                        
                    break;
                    
                case 16:
                case 17:
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                case 26:
                case 27:
                case 28:
                case 29:
                case 30:
                case 31:
                    if(!hints)
                        putcombo(dest,x,y,tmpscr->secretcombo[checkflag-16+4],
                                 tmpscr->secretcset[checkflag-16+4]);
                                 
                    break;
                    
                default:
                    if(layer && ((checkflag!=mfRAFT && checkflag!=mfRAFT_BRANCH&& checkflag!=mfRAFT_BOUNCE) || get_bit(quest_rules,qr_RAFTLENS)))
                    {
                        if((!key[KEY_N] && (lensclk&1)) || (key[KEY_N] && (frame&1)))
                        {
                            rectfill(dest,x,y,x+15,y+15,WHITE);
                        }
                    }
                    
                    break;
                }
            }
        }
        
        if(layer)
        {
            if(tmpscr->door[0]==dWALK)
                rectfill(dest, 120, 16+playing_field_offset, 135, 31+playing_field_offset, WHITE);
                
            if(tmpscr->door[1]==dWALK)
                rectfill(dest, 120, 144+playing_field_offset, 135, 159+playing_field_offset, WHITE);
                
            if(tmpscr->door[2]==dWALK)
                rectfill(dest, 16, 80+playing_field_offset, 31, 95+playing_field_offset, WHITE);
                
            if(tmpscr->door[3]==dWALK)
                rectfill(dest, 224, 80+playing_field_offset, 239, 95+playing_field_offset, WHITE);
                
            if(tmpscr->door[0]==dBOMB)
            {
                showbombeddoor(dest, 0);
            }
            
            if(tmpscr->door[1]==dBOMB)
            {
                showbombeddoor(dest, 1);
            }
            
            if(tmpscr->door[2]==dBOMB)
            {
                showbombeddoor(dest, 2);
            }
            
            if(tmpscr->door[3]==dBOMB)
            {
                showbombeddoor(dest, 3);
            }
        }
        
        if(tmpscr->stairx + tmpscr->stairy)
        {
            if(!hints)
                putcombo(dest,tmpscr->stairx,tmpscr->stairy+playing_field_offset,tmpscr->secretcombo[sSTAIRS],tmpscr->secretcset[sSTAIRS]);
            else
            {
                if(tmpscr->flags&fWHISTLE)
                {
                    tempitem=getItemID(itemsbuf,itype_whistle,1);
                    int tempitemx=-16;
                    int tempitemy=-16;
                    
                    if((!key[KEY_N] && (lensclk&(blink_rate/4)))
                            || (key[KEY_N] && (frame&(blink_rate/4))))
                    {
                        tempitemx=tmpscr->stairx;
                        tempitemy=tmpscr->stairy+playing_field_offset;
                    }
                    
                    putitem2(dest,tempitemx,tempitemy,tempitem, lens_hint_item[tempitem][0], lens_hint_item[tempitem][1], 0);
                }
            }
        }
    }
}

BITMAP *lens_scr_d; // The "d" is for "destructible"!

void draw_lens_over()
{
    // Oh, what the heck.
    static BITMAP *lens_scr = NULL;
    static int last_width = -1;
    int width = itemsbuf[current_item_id(itype_lens,true)].misc1;
    
    // Only redraw the circle if the size has changed
    if(width != last_width)
    {
        if(lens_scr == NULL)
        {
            lens_scr = create_bitmap_ex(8,2*288,2*(240-playing_field_offset));
        }
        
        clear_to_color(lens_scr, BLACK);
        circlefill(lens_scr, 288, 240-playing_field_offset, width, 0);
        circle(lens_scr, 288, 240-playing_field_offset, width+2, 0);
        circle(lens_scr, 288, 240-playing_field_offset, width+5, 0);
        last_width=width;
    }
    
    masked_blit(lens_scr, framebuf, 288-(LinkX()+8), 240-playing_field_offset-(LinkY()+8), 0, playing_field_offset, 256, 168);
}

//----------------------------------------------------------------

void draw_wavy(BITMAP *source, BITMAP *target, int amplitude, bool interpol)
{
    //recreating a big bitmap every frame is highly sluggish.
    static BITMAP *wavebuf = create_bitmap_ex(8,288,240-original_playing_field_offset);
    
    clear_to_color(wavebuf,0);
    blit(source,wavebuf,0,original_playing_field_offset,16,0,256,224-original_playing_field_offset);
    
    int ofs;
    //  int amplitude=8;
    //  int wavelength=4;
    amplitude = zc_min(2048,amplitude); // some arbitrary limit to prevent crashing
    int amp2=168;
    int i=frame%amp2;
    
    for(int j=0; j<168; j++)
    {
        if(j&1 && interpol)
        {
            ofs=int(sin((double(i+j)*2*PI/amp2))*amplitude);
        }
        else
        {
            ofs=-int(sin((double(i+j)*2*PI/amp2))*amplitude);
        }
        
        if(ofs)
        {
            for(int k=0; k<256; k++)
            {
                target->line[j+original_playing_field_offset][k]=wavebuf->line[j][k+ofs+16];
            }
        }
    }
}

void draw_fuzzy(int fuzz)
// draws from right half of scrollbuf to framebuf
{
    int firstx, firsty, xstep, ystep, i, y, dx, dy;
    byte *start, *si, *di;
    
    if(fuzz<1)
        fuzz = 1;
        
    xstep = 128%fuzz;
    
    if(xstep > 0)
        xstep = fuzz-xstep;
        
    ystep = 112%fuzz;
    
    if(ystep > 0)
        ystep = fuzz-ystep;
        
    firsty = 1;
    
    for(y=0; y<224;)
    {
        start = &(scrollbuf->line[y][256]);
        
        for(dy=0; dy<ystep && dy+y<224; dy++)
        {
            si = start;
            di = &(framebuf->line[y+dy][0]);
            i = xstep;
            firstx = 1;
            
            for(dx=0; dx<256; dx++)
            {
                *(di++) = *si;
                
                if(++i >= fuzz)
                {
                    if(!firstx)
                        si += fuzz;
                    else
                    {
                        si += fuzz-xstep;
                        firstx = 0;
                    }
                    
                    i = 0;
                }
            }
        }
        
        if(!firsty)
            y += fuzz;
        else
        {
            y += ystep;
            ystep = fuzz;
            firsty = 0;
        }
    }
}

void updatescr(bool allowwavy)
{
    static BITMAP *wavybuf = create_bitmap_ex(8,256,224);
    static BITMAP *panorama = create_bitmap_ex(8,256,224);
        
    if(toogam)
    {
        textout_ex(framebuf,font,"no walls",8,216,1,-1);
    }
    
    if(Showpal)
        dump_pal(framebuf);
        
    if(!Playing)
        black_opening_count=0;
        
    if(black_opening_count<0) //shape is opening up
    {
        black_opening(framebuf,black_opening_x,black_opening_y,(66+black_opening_count),66);
        
        if(Advance||(!Paused))
        {
            ++black_opening_count;
        }
    }
    else if(black_opening_count>0) //shape is closing
    {
        black_opening(framebuf,black_opening_x,black_opening_y,black_opening_count,66);
        
        if(Advance||(!Paused))
        {
            --black_opening_count;
        }
    }
    
    if(refreshpal)
    {
        refreshpal=false;
        RAMpal[253] = _RGB(0,0,0);
        RAMpal[254] = _RGB(63,63,63);
        set_palette_range(RAMpal,0,255,false);
        
        create_rgb_table(&rgb_table, RAMpal, NULL);
        create_zc_trans_table(&trans_table, RAMpal, 128, 128, 128);
        memcpy(&trans_table2, &trans_table, sizeof(COLOR_MAP));
        
        for(int q=0; q<PAL_SIZE; q++)
        {
            trans_table2.data[0][q] = q;
            trans_table2.data[q][q] = q;
        }
    }
    
    if(details)
        show_details();
    if(show_ff_scripts)
        show_ffscript_names();
    
    bool clearwavy = (wavy <= 0);
    
    if(wavy <= 0)
    {
        // So far one thing can alter wavy apart from scripts: Wavy DMaps.
        wavy = (DMaps[currdmap].flags&dmfWAVY ? 4 : 0);
    }
    
    blit(framebuf, wavybuf, 0, 0, 0, 0, 256, 224);
    
    if(wavy && Playing && allowwavy)
    {
        draw_wavy(framebuf, wavybuf, wavy,false);
    }
    
    if(clearwavy)
        wavy = 0; // Wavy was set by a DMap flag. Clear it.
    else if(Playing && !Paused)
        wavy--; // Wavy was set by a script. Decrement it.
        
    if(!(msgdisplaybuf->clip) && Playing && msgpos && !screenscrolling)
    {
        masked_blit(msgdisplaybuf,framebuf,0,0,0,playing_field_offset,256,168);
    }
    
    bool nosubscr = (tmpscr->flags3&fNOSUBSCR && !(tmpscr->flags3&fNOSUBSCROFFSET));
    
    if(nosubscr)
    {
        rectfill(panorama,0,0,255,passive_subscreen_height/2,0);
        rectfill(panorama,0,168+passive_subscreen_height/2,255,168+passive_subscreen_height-1,0);
        blit(wavybuf,panorama,0,playing_field_offset,0,passive_subscreen_height/2,256,224-passive_subscreen_height);
    }
    
    //TODO: Optimize blit 'overcalls' -Gleeok
    BITMAP *source = nosubscr ? panorama : wavybuf;
    BITMAP *target = NULL;
    
    bool dontusetb = triplebuffer_not_available ||
                     !(Throttlefps ^ (true && key[KEY_TILDE]));
                     
    if(dontusetb)
        target=screen;
    else
        target=tb_page[curr_tb_page];
        
//  static BITMAP *tempscreen=NULL;
    static BITMAP *scanlinesbmp=NULL;
    
    if(resx != SCREEN_W || resy != SCREEN_H)
    {
        Z_message("Conflicting variables warning: screen_scale %i, resx %i, resy %i, w %i, h %i\n", screen_scale, resx, resy, SCREEN_W, SCREEN_H);
        resx = SCREEN_W;
        resy = SCREEN_H;
        screen_scale = zc_max(zc_min(resx / 320, resy / 240), 1);
    }
    
    if(!sbig && screen_scale > 1)
        sbig = true;
        
    const int sx = 256 * screen_scale;
    const int sy = 224 * screen_scale;
    const int scale_mul = screen_scale - 1;
    const int mx = scale_mul * 128;
    const int my = scale_mul * 112;
    
    if(sbig)
    {
        if(scanlines)
        {
            if(!scanlinesbmp)
                scanlinesbmp = create_bitmap_ex(8, sx, sy);
                
            stretch_blit(source, scanlinesbmp, 0, 0, 256, 224, 0, 0, sx, sy);
            
            for(int i=0; i<224; ++i)
                _allegro_hline(scanlinesbmp, 0, (i*screen_scale)+1, sx, BLACK);
                
            blit(scanlinesbmp, target, 0, 0, scrx+32-mx, scry+8-my, sx, sy);
        }
        else
        {
            stretch_blit(source, target, 0, 0, 256, 224, scrx+32-mx, scry+8-my, sx, sy);
        }
        
        if(quakeclk>0)
            rectfill(target, // I don't know if these are right...
                     scrx+32 - mx, //x1
                     scry+8 - my + sy, //y1
                     scrx+32 - mx + sx, //x2
                     scry+8 - my + sy + (16 * scale_mul), //y2
                     0);
                     
        //stretch_blit(nosubscr?panorama:wavybuf,target,0,0,256,224,scrx+32-128,scry+8-112,512,448);
        //if(quakeclk>0) rectfill(target,scrx+32-128,scry+8-112+448,scrx+32-128+512,scry+8-112+456,0);
    }
    else
    {
        blit(source,target,0,0,scrx+32,scry+8,256,224);
        
        if(quakeclk>0) rectfill(target,scrx+32,scry+8+224,scrx+32+256,scry+8+232,0);
    }
    
    if(ShowFPS)
        show_fps(target);
        
    if(Paused)
        show_paused(target);
        
    if(details)
    {
        textprintf_ex(target,font,0,SCREEN_H-8,254,BLACK,"%-6d (%s)", idle_count, time_str_long(idle_count));
    }
    
    if(!dontusetb)
    {
        if(!poll_scroll())
        {
            request_video_bitmap(tb_page[curr_tb_page]);
            curr_tb_page=(curr_tb_page+1)%3;
            clear_to_color(tb_page[curr_tb_page],BLACK);
        }
    }
    
    //if(panorama!=NULL) destroy_bitmap(panorama);
    
    ++framecnt;
}

//----------------------------------------------------------------

PALETTE sys_pal;

void f_Quit(int type)
{
    if(type==qQUIT && !Playing)
        return;
        
    music_pause();
    pause_all_sfx();
    system_pal();
    clear_keybuf();
    
    switch(type)
    {
    case qQUIT:
        onQuit();
        break;
        
    case qRESET:
        onReset();
        break;
        
    case qEXIT:
        onExit();
        break;
    }
    
    if(Quit)
    {
        kill_sfx();
        music_stop();
        clear_to_color(screen,BLACK);
    }
    else
    {
        game_pal();
        music_resume();
        resume_all_sfx();
    }
    
    eat_buttons();
    
    if(key[KEY_ESC])
        key[KEY_ESC]=0;
        
    if(key[KEY_ENTER])
        key[KEY_ENTER]=0;
}

//----------------------------------------------------------------

int onNoWalls()
{
    toogam = !toogam;
    
    if(toogam)
    {
        cheat_superman=true;
        setClock(true);
    }
    
    return D_O_K;
}

int onGoFast()
{
    gofast=gofast?false:true;
    return D_O_K;
}

int onKillCheat()
{
    for(int i=0; i<guys.Count(); i++)
    {
        if(!(((enemy*)guys.spr(i))->flags & guy_doesntcount))((enemy*)guys.spr(i))->kickbucket();
    }
    
    return D_O_K;
}

int onShowLayer0()
{
    show_layer_0 = !show_layer_0;
    return D_O_K;
}
int onShowLayer1()
{
    show_layer_1 = !show_layer_1;
    return D_O_K;
}
int onShowLayer2()
{
    show_layer_2 = !show_layer_2;
    return D_O_K;
}
int onShowLayer3()
{
    show_layer_3 = !show_layer_3;
    return D_O_K;
}
int onShowLayer4()
{
    show_layer_4 = !show_layer_4;
    return D_O_K;
}
int onShowLayer5()
{
    show_layer_5 = !show_layer_5;
    return D_O_K;
}
int onShowLayer6()
{
    show_layer_6 = !show_layer_6;
    return D_O_K;
}
int onShowLayerO()
{
    show_layer_over=!show_layer_over;
    return D_O_K;
}
int onShowLayerP()
{
    show_layer_push=!show_layer_push;
    return D_O_K;
}
int onShowLayerS()
{
    show_sprites=!show_sprites;
    return D_O_K;
}
int onShowLayerF()
{
    show_ffcs=!show_ffcs;
    return D_O_K;
}
int onShowLayerW()
{
    show_walkflags=!show_walkflags;
    return D_O_K;
}
int onShowFFScripts()
{
    show_ff_scripts=!show_ff_scripts;
    return D_O_K;
}
int onShowHitboxes()
{
    show_hitboxes=!show_hitboxes;
    return D_O_K;
}

int onLightSwitch()
{
    do_cheat_light=true;
    return D_O_K;
}

void syskeys()
{
    int oldtitle_version;
    
    if(close_button_quit)
    {
        close_button_quit=false;
        f_Quit(qEXIT);
    }
    
    poll_joystick();
    
    mouse_down=gui_mouse_b();
    
    if(ReadKey(KEY_F1))
    {
        if(key[KEY_ZC_LCONTROL] || key[KEY_ZC_RCONTROL])
        {
            halt=!halt;
            //zinit.subscreen=(zinit.subscreen+1)%ssdtMAX;
        }
        else
        {
            Throttlefps=!Throttlefps;
            logic_counter=0;
        }
    }
    
    //  if(ReadKey(KEY_F1))    Vsync=!Vsync;
    /*
      if(ReadKey(KEY_F1))    set_bit(QHeader.rules4,qr4_NEWENEMYTILES,
      1-((get_bit(QHeader.rules4,qr4_NEWENEMYTILES))));
      */
    
    if(ReadKey(KEY_OPENBRACE))    if(frame_rest_suggest > 0) frame_rest_suggest--;
    
    if(ReadKey(KEY_CLOSEBRACE))    if(frame_rest_suggest <= 2) frame_rest_suggest++;
    
    if(ReadKey(KEY_F2))    ShowFPS=!ShowFPS;
    
    if(ReadKey(KEY_F3) && Playing)    Paused=!Paused;
    
    if(ReadKey(KEY_F4) && Playing)
    {
        Paused=true;
        Advance=true;
    }
    
    if(ReadKey(KEY_F6))    if(!get_bit(quest_rules, qr_NOCONTINUE)) f_Quit(qQUIT);
    
    if(ReadKey(KEY_F9))    f_Quit(qRESET);
    
    if(ReadKey(KEY_F10))   f_Quit(qEXIT);
    
    if(cheat>=1)
    {
        if(ReadKey(KEY_ASTERISK) || ReadKey(KEY_H))   game->set_life(game->get_maxlife());
        
        if(ReadKey(KEY_SLASH_PAD) || ReadKey(KEY_M))  game->set_magic(game->get_maxmagic());
        
        if(ReadKey(KEY_R))          game->set_drupy(999);
    }
    
    if(cheat>=2)
    {
        if(rI())
        {
            setClock(!getClock());
            cheat_superman=getClock();
        }
    }
    
    if(cheat>=4)
    {
        if(rF11())
        {
            onNoWalls();
        }
        
        if(rQ())
        {
            onGoFast();
        }
        
        if(ReadKey(KEY_F))
        {
            if(Link.getAction()==freeze)
            {
                Link.unfreeze();
            }
            else
            {
                Link.Freeze();
            }
        }
        
        if(ReadKey(KEY_0))   onShowLayer0();
        
        if(ReadKey(KEY_1))   onShowLayer1();
        
        if(ReadKey(KEY_2))   onShowLayer2();
        
        if(ReadKey(KEY_3))   onShowLayer3();
        
        if(ReadKey(KEY_4))   onShowLayer4();
        
        if(ReadKey(KEY_5))   onShowLayer5();
        
        if(ReadKey(KEY_6))   onShowLayer6();
        
        //if(ReadKey(KEY_7))   onShowLayerO();
        if(ReadKey(KEY_7))   onShowLayerF();
        
        if(ReadKey(KEY_8))   onShowLayerS();
        
        if(ReadKey(KEY_W))   onShowLayerW();
        
        if(ReadKey(KEY_L))   onLightSwitch();
    }
    
    if(!SystemKeys)
        goto bottom;
        
    if(ReadKey(KEY_D))
    {
        details = !details;
        rectfill(screen,0,0,319,7,BLACK);
        rectfill(screen,0,8,31,239,BLACK);
        rectfill(screen,288,8,319,239,BLACK);
        rectfill(screen,32,232,287,239,BLACK);
    }
    
    if(ReadKey(KEY_V))
    {
        Throttlefps=!Throttlefps;
        /*
          remove_int(fps_callback);
        
          install_int_ex(fps_callback,SECS_TO_TIMER(1));
          */
        avgfps=fps_secs=framecnt=0;
        logic_counter = 0;
    }
    
    if(ReadKey(KEY_P))   Paused=!Paused;
    
    //if(ReadKey(KEY_P))   centerLink();
    if(ReadKey(KEY_A))
    {
        Paused=true;
        Advance=true;
    }
    
    if(ReadKey(KEY_G))   db=(db==999)?0:999;
    if(ReadKey(KEY_F8))  Showpal=!Showpal;

    if(ReadKey(KEY_PLUS_PAD) || ReadKey(KEY_EQUALS))
    {
        //change containers
        if(key[KEY_ZC_LCONTROL] || key[KEY_ZC_RCONTROL])
        {
            //magic containers
            if(key[KEY_LSHIFT] || key[KEY_RSHIFT])
            {
                game->set_maxmagic(zc_min(game->get_maxmagic()+MAGICPERBLOCK,MAGICPERBLOCK*8));
            }
            else
            {
                game->set_maxlife(zc_min(game->get_maxlife()+HP_PER_HEART,HP_PER_HEART*24));
            }
        }
        else
        {
            if(key[KEY_LSHIFT] || key[KEY_RSHIFT])
            {
                game->set_magic(zc_min(game->get_magic()+1,game->get_maxmagic()));
            }
            else
            {
                game->set_life(zc_min(game->get_life()+1,game->get_maxlife()));
            }
        }
    }
    
    if(ReadKey(KEY_MINUS_PAD) || ReadKey(KEY_MINUS))
    {
        //change containers
        if(key[KEY_ZC_LCONTROL] || key[KEY_ZC_RCONTROL])
        {
            //magic containers
            if(key[KEY_LSHIFT] || key[KEY_RSHIFT])
            {
                game->set_maxmagic(zc_max(game->get_maxmagic()-MAGICPERBLOCK,0));
                game->set_magic(zc_min(game->get_maxmagic(), game->get_magic()));
                //heart containers
            }
            else
            {
                game->set_maxlife(zc_max(game->get_maxlife()-HP_PER_HEART,HP_PER_HEART));
                game->set_life(zc_min(game->get_maxlife(), game->get_life()));
            }
        }
        else
        {
            if(key[KEY_LSHIFT] || key[KEY_RSHIFT])
            {
                game->set_magic(zc_max(game->get_magic()-1,0));
            }
            else
            {
                game->set_life(zc_max(game->get_life()-1,0));
            }
        }
    }
    
    if(ReadKey(KEY_COMMA))  jukebox(currmidi-1);
    
    if(ReadKey(KEY_STOP))   jukebox(currmidi+1);
    
    /*
      if(ReadKey(KEY_TILDE)) {
      wavyout();
      zinit.subscreen=(zinit.subscreen+1)%3;
      wavyin();
      }
      */
    
    verifyBothWeapons();
    
bottom:

    //while(Playing && keypressed())
    //readkey();
    // What's the Playing check for?
    clear_keybuf();
}

// 99*360 + 59*60
#define MAXTIME  21405240

void advanceframe(bool allowwavy, bool sfxcleanup)
{
    if(zcmusic!=NULL)
    {
        zcmusic_poll();
    }
    
    while(Paused && !Advance && !Quit)
    {
        // have to call this, otherwise we'll get an infinite loop
        syskeys();
        // to keep fps constant
        updatescr(allowwavy);
        throttleFPS();
        
        // to keep music playing
        if(zcmusic!=NULL)
        {
            zcmusic_poll();
        }
    }
    
    if(Quit)
        return;
        
    if(Playing && game->get_time()<MAXTIME)
        game->change_time(1);
        
    Advance=false;
    ++frame;
    
    syskeys();
    // Someday... maybe install a Turbo button here?
    updatescr(allowwavy);
    throttleFPS();
    
    //textprintf_ex(screen,font,0,72,254,BLACK,"%d %d", lastentrance, lastentrance_dmap);
    if(sfxcleanup)
        sfx_cleanup();
}

void zapout()
{
    set_clip_rect(scrollbuf, 0, 0, scrollbuf->w, scrollbuf->h);
    blit(framebuf,scrollbuf,0,0,256,0,256,224);
    
    script_drawing_commands.Clear();
    
    // zap out
    for(int i=1; i<=24; i++)
    {
        draw_fuzzy(i);
        syskeys();
        advanceframe(true);
        
        if(Quit)
        {
            break;
        }
    }
}

void zapin()
{
    draw_screen(tmpscr);
    set_clip_rect(scrollbuf, 0, 0, scrollbuf->w, scrollbuf->h);
    put_passive_subscr(framebuf,&QMisc,0,passive_subscreen_offset,false,sspUP);
    blit(framebuf,scrollbuf,0,0,256,0,256,224);
    
    // zap out
    for(int i=24; i>=1; i--)
    {
        draw_fuzzy(i);
        syskeys();
        advanceframe(true);
        
        if(Quit)
        {
            break;
        }
    }
}


void wavyout(bool showlink)
{
    draw_screen(tmpscr, showlink);
    put_passive_subscr(framebuf,&QMisc,0,passive_subscreen_offset,false,sspUP);
    
    BITMAP *wavebuf = create_bitmap_ex(8,288,224);
    clear_to_color(wavebuf,0);
    blit(framebuf,wavebuf,0,0,16,0,256,224);
    
    PALETTE wavepal;
    
    int ofs;
    int amplitude=8;
    
    int wavelength=4;
    double palpos=0, palstep=4, palstop=126;
    
    for(int i=0; i<168; i+=wavelength)
    {
        for(int l=0; l<256; l++)
        {
            wavepal[l].r=vbound(int(RAMpal[l].r+((palpos/palstop)*(63-RAMpal[l].r))),0,63);
            wavepal[l].g=vbound(int(RAMpal[l].g+((palpos/palstop)*(63-RAMpal[l].g))),0,63);
            wavepal[l].b=vbound(int(RAMpal[l].b+((palpos/palstop)*(63-RAMpal[l].b))),0,63);
        }
        
        palpos+=palstep;
        
        if(palpos>=0)
        {
            set_palette(wavepal);
        }
        else
        {
            set_palette(RAMpal);
        }
        
        for(int j=0; j+playing_field_offset<224; j++)
        {
            for(int k=0; k<256; k++)
            {
                ofs=0;
                
                if((j<i)&&(j&1))
                {
                    ofs=int(sin((double(i+j)*2*PI/168.0))*amplitude);
                }
                
                framebuf->line[j+playing_field_offset][k]=wavebuf->line[j+playing_field_offset][k+ofs+16];
            }
        }
        
        syskeys();
        advanceframe(true);
        
        //    animate_combos();
        if(Quit)
            break;
    }
    
    destroy_bitmap(wavebuf);
}

void wavyin()
{
    draw_screen(tmpscr);
    put_passive_subscr(framebuf,&QMisc,0,passive_subscreen_offset,false,sspUP);
    
    BITMAP *wavebuf = create_bitmap_ex(8,288,224);
    clear_to_color(wavebuf,0);
    blit(framebuf,wavebuf,0,0,16,0,256,224);
    
    PALETTE wavepal;
    
    //Breaks dark rooms.
    //In any case I don't think we need this, since palette is already loaded in doWarp() (famous last words...) -DD
    /*
      loadfullpal();
      loadlvlpal(DMaps[currdmap].color);
      ringcolor(false);
    */
    refreshpal=false;
    int ofs;
    int amplitude=8;
    int wavelength=4;
    double palpos=168, palstep=4, palstop=126;
    
    for(int i=0; i<168; i+=wavelength)
    {
        for(int l=0; l<256; l++)
        {
            wavepal[l].r=vbound(int(RAMpal[l].r+((palpos/palstop)*(63-RAMpal[l].r))),0,63);
            wavepal[l].g=vbound(int(RAMpal[l].g+((palpos/palstop)*(63-RAMpal[l].g))),0,63);
            wavepal[l].b=vbound(int(RAMpal[l].b+((palpos/palstop)*(63-RAMpal[l].b))),0,63);
        }
        
        palpos-=palstep;
        
        if(palpos>=0)
        {
            set_palette(wavepal);
        }
        else
        {
            set_palette(RAMpal);
        }
        
        for(int j=0; j+playing_field_offset<224; j++)
        {
            for(int k=0; k<256; k++)
            {
                ofs=0;
                
                if((j<(167-i))&&(j&1))
                {
                    ofs=int(sin((double(i+j)*2*PI/168.0))*amplitude);
                }
                
                framebuf->line[j+playing_field_offset][k]=wavebuf->line[j+playing_field_offset][k+ofs+16];
            }
        }
        
        syskeys();
        advanceframe(true);
        //    animate_combos();
        
        if(Quit)
            break;
    }
    
    destroy_bitmap(wavebuf);
}

void blackscr(int fcnt,bool showsubscr)
{
    reset_pal_cycling();
    script_drawing_commands.Clear();
    
    while(fcnt>0)
    {
        clear_bitmap(framebuf);
        
        if(showsubscr)
        {
            put_passive_subscr(framebuf,&QMisc,0,passive_subscreen_offset,false,sspUP);
        }
        
        syskeys();
        advanceframe(true);
        
        if(Quit)
            break;
            
        --fcnt;
    }
}

void openscreen()
{
    reset_pal_cycling();
    black_opening_count=0;
    
    if(COOLSCROLL)
    {
        open_black_opening(LinkX()+8, (LinkY()-LinkZ())+8+playing_field_offset, true);
        return;
    }
    else
    {
        Link.setDontDraw(true);
        show_subscreen_dmap_dots=false;
        show_subscreen_numbers=false;
        //    show_subscreen_items=false;
        show_subscreen_life=false;
    }
    
    int x=128;
    
    for(int i=0; i<80; i++)
    {
        draw_screen(tmpscr);
        //? draw_screen already draws the subscreen -DD
        //put_passive_subscr(framebuf,&QMisc,0,passive_subscreen_offset,false,sspUP);
        x=128-(((i*128/80)/8)*8);
        
        if(x>0)
        {
            rectfill(framebuf,0,playing_field_offset,x,167+playing_field_offset,0);
            rectfill(framebuf,256-x,playing_field_offset,255,167+playing_field_offset,0);
        }
        
        //    x=((80-i)/2)*4;
        /*
          --x;
          switch(++c)
          {
          case 5: c=0;
          case 0:
          case 2:
          case 3: --x; break;
          }
          */
        syskeys();
        advanceframe(true);
        
        if(Quit)
        {
            break;
        }
    }
    
    Link.setDontDraw(false);
    show_subscreen_items=true;
    show_subscreen_dmap_dots=true;
}

int TriforceCount()
{
    int c=0;
    
    for(int i=1; i<=8; i++)
        if(game->lvlitems[i]&liTRIFORCE)
            ++c;
            
    return c;
}

int onContinue()
{
    return D_CLOSE;
}

int onVsync()
{
    Throttlefps = !Throttlefps;
    return D_O_K;
}

int onFrameSkip()
{
    FrameSkip = !FrameSkip;
    return D_O_K;
}

int onTransLayers()
{
    TransLayers = !TransLayers;
    return D_O_K;
}

int onNESquit()
{
    NESquit = !NESquit;
    return D_O_K;
}

int onShowFPS()
{
    ShowFPS = !ShowFPS;
    scare_mouse();
    
    if(ShowFPS)
        show_fps(screen);
        
    if(sbig)
        stretch_blit(fps_undo,screen,0,0,64,16,scrx+40-120,scry+216+96,128,32);
    else
        blit(fps_undo,screen,0,0,scrx+40,scry+216,64,16);
        
    if(Paused)
        show_paused(screen);
        
    unscare_mouse();
    return D_O_K;
}

bool is_Fkey(int k)
{
    switch(k)
    {
    case KEY_F1:
    case KEY_F2:
    case KEY_F3:
    case KEY_F4:
    case KEY_F5:
    case KEY_F6:
    case KEY_F7:
    case KEY_F8:
    case KEY_F9:
    case KEY_F10:
    case KEY_F11:
    case KEY_F12:
        return true;
    }
    
    return false;
}

//shnarf
const char *key_str[] =
{
    "(none)",         "a",              "b",              "c",
    "d",              "e",              "f",              "g",
    "h",              "i",              "j",              "k",
    "l",              "m",              "n",              "o",
    "p",              "q",              "r",              "s",
    "t",              "u",              "v",              "w",
    "x",              "y",              "z",              "0",
    "1",              "2",              "3",              "4",
    "5",              "6",              "7",              "8",
    "9",              "num 0",          "num 1",          "num 2",
    "num 3",          "num 4",          "num 5",          "num 6",
    "num 7",          "num 8",          "num 9",          "f1",
    "f2",             "f3",             "f4",             "f5",
    "f6",             "f7",             "f8",             "f9",
    "f10",            "f11",            "f12",            "esc",
    "~",              "-",              "=",              "backspace",
    "tab",            "{",              "}",              "enter",
    ":",              "quote",          "\\",             "\\ (2)",
    ",",              ".",              "/",              "space",
    "insert",         "delete",         "home",           "end",
    "page up",        "page down",      "left",           "right",
    "up",             "down",           "num /",          "num *",
    "num -",          "num +",          "num delete",     "num enter",
    "print screen",   "pause",          "abnt c1",        "yen",
    "kana",           "convert",        "no convert",     "at",
    "circumflex",     ": (2)",          "kanji",          "num =",
    "back quote",     ";",              "command",        "unknown (0)",
    "unknown (1)",    "unknown (2)",    "unknown (3)",    "unknown (4)",
    "unknown (5)",    "unknown (6)",    "unknown (7)",    "left shift",
    "right shift",    "left control",   "right control",  "alt",
    "alt gr",         "left win",       "right win",      "menu",
    "scroll lock",    "number lock",    "caps lock",      "MAX"
};

const char *pan_str[4] = { "MONO", " 1/2", " 3/4", "FULL" };
//extern int zcmusic_bufsz;

static char str_a[80],str_b[80],str_s[80],str_m[16],str_l[16],str_r[16],str_p[16],str_ex1[16],str_ex2[16],str_ex3[16],str_ex4[16];

/*
int midi_dp[3] = {0,147,104};
int digi_dp[3] = {1,147,120};
int pan_dp[3]  = {0,147,136};
int buf_dp[3]  = {0,147,152};
*/
int midi_dp[3] = {0,0,0};
int digi_dp[3] = {1,0,0};
int emus_dp[3] = {2,0,0};
int buf_dp[3]  = {0,0,0};
int sfx_dp[3]  = {3,0,0};
int pan_dp[3]  = {0,0,0};

int onQuit()
{
    if(Playing)
    {
        int ret=0;
        
        if(get_bit(quest_rules, qr_NOCONTINUE))
        {
        }
        
        Quit=qQUIT;
        
        // Trying to evade a door repair charge?
        if(repaircharge)
        {
            game->change_drupy(-repaircharge);
            repaircharge=0;
        }

    }
    
    return D_O_K;
}

int onReset()
{
    Quit=qRESET;
    return D_O_K;
}

int onExit()
{
    Quit=qEXIT;
    return D_O_K;
}

void color_layer(RGB *src,RGB *dest,char r,char g,char b,char pos,int from,int to)
{
    PALETTE tmp;
    
    for(int i=0; i<256; i++)
    {
        tmp[i].r=r;
        tmp[i].g=g;
        tmp[i].b=b;
    }
    
    fade_interpolate(src,tmp,dest,pos,from,to);
}


void system_pal()
{
    PALETTE pal;
    copy_pal((RGB*)data[PAL_GUI].dat, pal);
    
    // set up the grayscale palette
    for(int i=128; i<192; i++)
    {
        pal[i].r = i-128;
        pal[i].g = i-128;
        pal[i].b = i-128;
    }
    
    
    switch(gui_colorset)
    {
        /*
          enum
          {
          jcBOX, jcLIGHT, jcMEDLT, jcMEDDARK, jcDARK, jcBOXFG,
          jcTITLEL, jcTITLER, jcTITLEFG, jcTEXTBG, jcTEXTFG, jcSELBG, jcSELFG,
          jcMAX
          };
          */
    case 1:  //Windows 98
    {
        pal[dvc(1)] = _RGB(0*63/255,   0*63/255,   0*63/255);
        pal[dvc(2)] = _RGB(128*63/255, 128*63/255, 128*63/255);
        pal[dvc(3)] = _RGB(192*63/255, 192*63/255, 192*63/255);
        pal[dvc(4)] = _RGB(223*63/255, 223*63/255, 223*63/255);
        pal[dvc(5)] = _RGB(255*63/255, 255*63/255, 255*63/255);
        pal[dvc(6)] = _RGB(255*63/255, 255*63/255, 225*63/255);
        pal[dvc(7)] = _RGB(255*63/255, 225*63/255, 160*63/255);
        pal[dvc(8)] = _RGB(0*63/255,   0*63/255,  80*63/255);
        
        byte palrstart=  0*63/255, palrend=166*63/255,
             palgstart=  0*63/255, palgend=202*63/255,
             palbstart=128*63/255, palbend=240*63/255,
             paldivs=7;
             
        for(int i=0; i<paldivs; i++)
        {
            pal[dvc(15-paldivs+1)+i].r = palrstart+((palrend-palrstart)*i/(paldivs-1));
            pal[dvc(15-paldivs+1)+i].g = palgstart+((palgend-palgstart)*i/(paldivs-1));
            pal[dvc(15-paldivs+1)+i].b = palbstart+((palbend-palbstart)*i/(paldivs-1));
        }
    }
    break;
    
    case 2:  //Windows 99
    {
        pal[dvc(1)] = _RGB(0*63/255,   0*63/255,   0*63/255);
        pal[dvc(2)] = _RGB(64*63/255,  64*63/255,  64*63/255);
        pal[dvc(3)] = _RGB(128*63/255, 128*63/255, 128*63/255);
        pal[dvc(4)] = _RGB(192*63/255, 192*63/255, 192*63/255);
        pal[dvc(5)] = _RGB(223*63/255, 223*63/255, 223*63/255);
        pal[dvc(6)] = _RGB(255*63/255, 255*63/255, 255*63/255);
        pal[dvc(7)] = _RGB(255*63/255, 255*63/255, 225*63/255);
        pal[dvc(8)] = _RGB(255*63/255, 225*63/255, 160*63/255);
        pal[dvc(9)] = _RGB(0*63/255,   0*63/255,  80*63/255);
        
        byte palrstart=  0*63/255, palrend=166*63/255,
             palgstart=  0*63/255, palgend=202*63/255,
             
             palbstart=128*63/255, palbend=240*63/255,
             paldivs=6;
             
        for(int i=0; i<paldivs; i++)
        {
            pal[dvc(15-paldivs+1)+i].r = palrstart+((palrend-palrstart)*i/(paldivs-1));
            pal[dvc(15-paldivs+1)+i].g = palgstart+((palgend-palgstart)*i/(paldivs-1));
            pal[dvc(15-paldivs+1)+i].b = palbstart+((palbend-palbstart)*i/(paldivs-1));
        }
    }
    break;
    
    case 3:  //Windows 2000 Blue
    {
        pal[dvc(1)] = _RGB(0*63/255,   0*63/255,   0*63/255);
        pal[dvc(2)] = _RGB(16*63/255,  15*63/255, 116*63/255);
        pal[dvc(3)] = _RGB(82*63/255,  80*63/255, 182*63/255);
        pal[dvc(4)] = _RGB(162*63/255, 158*63/255, 250*63/255);
        pal[dvc(5)] = _RGB(255*63/255, 255*63/255, 255*63/255);
        pal[dvc(6)] = _RGB(255*63/255, 255*63/255, 127*63/255);
        pal[dvc(7)] = _RGB(255*63/255, 225*63/255,  63*63/255);
        pal[dvc(8)] = _RGB(0*63/255,   0*63/255,  80*63/255);
        
        byte palrstart=  0*63/255, palrend=162*63/255,
             palgstart=  0*63/255, palgend=158*63/255,
             palbstart= 80*63/255, palbend=250*63/255,
             paldivs=7;
             
        for(int i=0; i<paldivs; i++)
        {
            pal[dvc(15-paldivs+1)+i].r = palrstart+((palrend-palrstart)*i/(paldivs-1));
            pal[dvc(15-paldivs+1)+i].g = palgstart+((palgend-palgstart)*i/(paldivs-1));
            pal[dvc(15-paldivs+1)+i].b = palbstart+((palbend-palbstart)*i/(paldivs-1));
        }
    }
    break;
    
    case 687:  //Windows 2000 Gold (6-87 was the North American release date of LoZ)
    {
        pal[dvc(1)] = _RGB(0*63/255,   0*63/255,   0*63/255);
        pal[dvc(2)] = _RGB(64*63/255,  64*63/255,  43*63/255);
        pal[dvc(3)] = _RGB(128*63/255, 128*63/255,  85*63/255);
        pal[dvc(4)] = _RGB(223*63/255, 200*63/255, 128*63/255); // Old Gold
        pal[dvc(5)] = _RGB(223*63/255, 223*63/255, 149*63/255);
        pal[dvc(6)] = _RGB(255*63/255, 255*63/255, 255*63/255);
        pal[dvc(7)] = _RGB(255*63/255, 255*63/255, 225*63/255);
        pal[dvc(8)] = _RGB(255*63/255, 225*63/255, 160*63/255);
        pal[dvc(9)] = _RGB(80*63/255,  80*63/255,   0*63/255);
        
        byte palrstart=128*63/255, palrend=240*63/255,
             palgstart=128*63/255, palgend=202*63/255,
             palbstart=  0*63/255, palbend=166*63/255,
             paldivs=6;
             
        for(int i=0; i<paldivs; i++)
        {
            pal[dvc(15-paldivs+1)+i].r = palrstart+((palrend-palrstart)*i/(paldivs-1));
            pal[dvc(15-paldivs+1)+i].g = palgstart+((palgend-palgstart)*i/(paldivs-1));
            pal[dvc(15-paldivs+1)+i].b = palbstart+((palbend-palbstart)*i/(paldivs-1));
        }
    }
    break;
    
    case 4104:  //Windows 2000 Easter (4-1-04 is April Fools Day, the date of this release)
    {
        pal[dvc(1)] = _RGB(0*63/255,   0*63/255,   0*63/255);
        pal[dvc(2)] = _RGB(64*63/255,  64*63/255,  64*63/255);
        pal[dvc(3)] = _RGB(128*63/255, 128*63/255, 128*63/255);
        pal[dvc(4)] = _RGB(252*63/255, 186*63/255, 188*63/255);
        pal[dvc(5)] = _RGB(254*63/255, 238*63/255, 238*63/255);
        pal[dvc(6)] = _RGB(244*63/255, 243*63/255, 161*63/255);
        pal[dvc(7)] = _RGB(120*63/255, 173*63/255, 189*63/255);
        pal[dvc(8)] = _RGB(220*63/255, 183*63/255, 227*63/255);
        
        byte palrstart=244*63/255, palrend=220*63/255,
             palgstart=243*63/255, palgend=183*63/255,
             palbstart=161*63/255, palbend=227*63/255,
             paldivs=7;
             
        for(int i=0; i < paldivs; i++)
        {
            pal[dvc(15-paldivs+1)+i].r = palrstart+((palrend-palrstart)*i/(paldivs-1));
            pal[dvc(15-paldivs+1)+i].g = palgstart+((palgend-palgstart)*i/(paldivs-1));
            pal[dvc(15-paldivs+1)+i].b = palbstart+((palbend-palbstart)*i/(paldivs-1));
        }
    }
    break;
    
    default:  //Windows 2000
    {
        pal[dvc(1)] = _RGB(0*63/255,   0*63/255,   0*63/255);
        pal[dvc(2)] = _RGB(66*63/255,  65*63/255,  66*63/255);
        pal[dvc(3)] = _RGB(132*63/255, 130*63/255, 132*63/255);
        pal[dvc(4)] = _RGB(212*63/255, 208*63/255, 200*63/255);
        pal[dvc(5)] = _RGB(255*63/255, 255*63/255, 255*63/255);
        pal[dvc(6)] = _RGB(255*63/255, 255*63/255, 225*63/255);
        pal[dvc(7)] = _RGB(255*63/255, 225*63/255, 160*63/255);
        pal[dvc(8)] = _RGB(0*63/255,   0*63/255,  80*63/255);
        
        byte palrstart= 10*63/255, palrend=166*63/255,
             palgstart= 36*63/255, palgend=202*63/255,
             palbstart=106*63/255, palbend=240*63/255,
             paldivs=7;
             
        for(int i=0; i<paldivs; i++)
        {
            pal[dvc(15-paldivs+1)+i].r = palrstart+((palrend-palrstart)*i/(paldivs-1));
            pal[dvc(15-paldivs+1)+i].g = palgstart+((palgend-palgstart)*i/(paldivs-1));
            pal[dvc(15-paldivs+1)+i].b = palbstart+((palbend-palbstart)*i/(paldivs-1));
        }
    }
    break;
    }
    
    color_layer(pal, pal, 24,16,16, 28, 128,191);
    
    for(int i=0; i<256; i+=2)
    {
        int v = (i>>3)+2;
        int c = (i>>3)+192;
        pal[c] = _RGB(v,v,v+(v>>1));
        /*
          if(i<240)
          {
          _allegro_hline(tmp_scr,0,i,319,c);
          _allegro_hline(tmp_scr,0,i+1,319,c);
          }
          */
    }
    
    // draw the vertical screen gradient
    for(int i=0; i<240; ++i)
    {
        _allegro_hline(tmp_scr,0,i,319,192+(i*31/239));
    }
    
    /*
      palrstart= 10*63/255; palrend=166*63/255;
      palgstart= 36*63/255; palgend=202*63/255;
      palbstart=106*63/255; palbend=240*63/255;
      paldivs=32;
      for(int i=0; i<paldivs; i++)
      {
      pal[223-paldivs+1+i].r = palrstart+((palrend-palrstart)*i/(paldivs-1));
      pal[223-paldivs+1+i].g = palgstart+((palgend-palgstart)*i/(paldivs-1));
      pal[223-paldivs+1+i].b = palbstart+((palbend-palbstart)*i/(paldivs-1));
      }
      */
    BITMAP *panorama = create_bitmap_ex(8,256,224);
    int ts_height, ts_start;
    
    if(tmpscr->flags3&fNOSUBSCR && !(tmpscr->flags3&fNOSUBSCROFFSET))
    {
        clear_to_color(panorama,0);
        blit(framebuf,panorama,0,playing_field_offset,0,28,256,224-passive_subscreen_height);
        ts_height=224-passive_subscreen_height;
        ts_start=28;
    }
    else
    {
        blit(framebuf,panorama,0,0,0,0,256,224);
        ts_height=224;
        ts_start=0;
    }
    
    // gray scale the current frame
    for(int y=0; y<ts_height; y++)
    {
        for(int x=0; x<256; x++)
        {
            int c = panorama->line[y+ts_start][x];
            int gray = zc_min((RAMpal[c].r*42 + RAMpal[c].g*75 + RAMpal[c].b*14) >> 7, 63);
            tmp_scr->line[y+8+ts_start][x+32] = gray+128;
        }
    }
    
    destroy_bitmap(panorama);
    
    // save the fps_undo section
    blit(tmp_scr,fps_undo,40,216,0,0,64,16);
    
    // display everything
    vsync();
    set_palette_range(pal,0,255,false);
    
    if(sbig)
        stretch_blit(tmp_scr,screen,0,0,320,240,scrx-(160*(screen_scale-1)),scry-(120*(screen_scale-1)),screen_scale*320,screen_scale*240);
    else
        blit(tmp_scr,screen,0,0,scrx,scry,320,240);
        
    if(ShowFPS)
        show_fps(screen);
        
    if(Paused)
        show_paused(screen);
        
    //  sys_pal = pal;
    memcpy(sys_pal,pal,sizeof(pal));
}

void switch_out_callback()
{
}

void switch_in_callback()
{
}

void game_pal()
{
    clear_to_color(screen,BLACK);
    set_palette_range(RAMpal,0,255,false);
}

static char bar_str[] = "";

void music_pause()
{
    //al_pause_duh(tmplayer);
    zcmusic_pause(zcmusic, ZCM_PAUSE);
    midi_pause();
    midi_paused=true;
}

void music_resume()
{
    //al_resume_duh(tmplayer);
    zcmusic_pause(zcmusic, ZCM_RESUME);
    midi_resume();
    midi_paused=false;
}

void music_stop()
{
    //al_stop_duh(tmplayer);
    //unload_duh(tmusic);
    //tmusic=NULL;
    //tmplayer=NULL;
    zcmusic_stop(zcmusic);
    zcmusic_unload_file(zcmusic);
    stop_midi();
    midi_paused=false;
    currmidi=0;
}

/*****************************/
/**** Custom Sound System ****/
/*****************************/

INLINE int mixvol(int v1,int v2)
{
    return (zc_min(v1,255)*zc_min(v2,255)) >> 8;
}

// Run an NSF, or a MIDI if the NSF is missing somehow.
bool try_zcmusic(char *filename, int track, int midi)
{
    ZCMUSIC *newzcmusic = NULL;
    
    // Try the ZC directory first
    {
        char exepath[2048];
        char musicpath[2048];
        get_executable_name(exepath, 2048);
        replace_filename(musicpath, exepath, filename, 2048);
        newzcmusic=(ZCMUSIC*)zcmusic_load_file(musicpath);
    }
    
    // Not in ZC directory, try the quest directory
    if(newzcmusic==NULL)
    {
        char musicpath[2048];
        replace_filename(musicpath, quest_path, filename, 2048);
        newzcmusic=(ZCMUSIC*)zcmusic_load_file(musicpath);
    }
    
    // Found it
    if(newzcmusic!=NULL)
    {
        zcmusic_stop(zcmusic);
        zcmusic_unload_file(zcmusic);
        stop_midi();
        
        zcmusic=newzcmusic;
        zcmusic_play(zcmusic, emusic_volume);
        
        if(track>0)
            zcmusic_change_track(zcmusic,track);
            
        return true;
    }
    
    // Not found, play MIDI - unless this was called by a script (yay, magic numbers)
    else if(midi>-1000)
        jukebox(midi);
        
    return false;
}

void jukebox(int index,int loop)
{
    music_stop();
    
    if(index<0)         index=MAXMIDIS-1;
    
    if(index>=MAXMIDIS) index=0;
    
    music_stop();
    
    // Allegro's DIGMID driver (the one normally used on on Linux) gets
    // stuck notes when a song stops. This fixes it.
    if(strcmp(midi_driver->name, "DIGMID")==0)
        set_volume(0, 0);
        
    set_volume(-1, mixvol(tunes[index].volume,midi_volume>>1));
    play_midi((MIDI*)tunes[index].data,loop);
    
    if(tunes[index].start>0)
        midi_seek(tunes[index].start);
        
    midi_loop_end = tunes[index].loop_end;
    midi_loop_start = tunes[index].loop_start;
    
    currmidi=index;
    master_volume(digi_volume,midi_volume);
    midi_paused=false;
}

void jukebox(int index)
{
    if(index<0)         index=MAXMIDIS-1;
    
    if(index>=MAXMIDIS) index=0;
    
    // do nothing if it's already playing
    if(index==currmidi && midi_pos>=0)
    {
        midi_paused=false;
        return;
    }
    
    jukebox(index,tunes[index].loop);
}

void play_DmapMusic()
{
    static char tfile[2048];
    static int ttrack=0;
    bool domidi=false;
    
    // Seems like this ought to call try_zcmusic()...
    
    if(DMaps[currdmap].tmusic[0]!=0)
    {
        if(zcmusic==NULL ||
           strcmp(zcmusic->filename,DMaps[currdmap].tmusic)!=0 ||
           (zcmusic->type==ZCMF_GME && zcmusic->track != DMaps[currdmap].tmusictrack))
        {
            if(zcmusic != NULL)
            {
                zcmusic_stop(zcmusic);
                zcmusic_unload_file(zcmusic);
                zcmusic = NULL;
            }
            
            // Try the ZC directory first
            {
                char exepath[2048];
                char musicpath[2048];
                get_executable_name(exepath, 2048);
                replace_filename(musicpath, exepath, DMaps[currdmap].tmusic, 2048);
                zcmusic=(ZCMUSIC*)zcmusic_load_file(musicpath);
            }
            
            // Not in ZC directory, try the quest directory
            if(zcmusic==NULL)
            {
                char musicpath[2048];
                replace_filename(musicpath, quest_path, DMaps[currdmap].tmusic, 2048);
                zcmusic=(ZCMUSIC*)zcmusic_load_file(musicpath);
            }
            
            if(zcmusic!=NULL)
            {
                stop_midi();
                strcpy(tfile,DMaps[currdmap].tmusic);
                zcmusic_play(zcmusic, emusic_volume);
                int temptracks=0;
                temptracks=zcmusic_get_tracks(zcmusic);
                temptracks=(temptracks<2)?1:temptracks;
                ttrack = vbound(DMaps[currdmap].tmusictrack,0,temptracks-1);
                zcmusic_change_track(zcmusic,ttrack);
            }
            else
            {
                tfile[0] = 0;
                domidi=true;
            }
        }
    }
    else
    {
        domidi=true;
    }
    
    if(domidi)
    {
        int m=DMaps[currdmap].midi;
        
        switch(m)
        {
        case 1:
            jukebox(ZC_MIDI_OVERWORLD);
            break;
            
        case 2:
            jukebox(ZC_MIDI_DUNGEON);
            break;
            
        case 3:
            jukebox(ZC_MIDI_LEVEL9);
            break;
            
        default:
            if(m>=4 && m<4+MAXCUSTOMMIDIS)
                jukebox(m-4+ZC_MIDI_COUNT);
            else
                music_stop();
        }
    }
}

void playLevelMusic()
{
    int m=tmpscr->screen_midi;
    
    switch(m)
    {
    case -2:
        music_stop();
        break;
        
    case -1:
        play_DmapMusic();
        break;
        
    case 1:
        jukebox(ZC_MIDI_OVERWORLD);
        break;
        
    case 2:
        jukebox(ZC_MIDI_DUNGEON);
        break;
        
    case 3:
        jukebox(ZC_MIDI_LEVEL9);
        break;
        
    default:
        if(m>=4 && m<4+MAXCUSTOMMIDIS)
            jukebox(m-4+ZC_MIDI_COUNT);
        else
            music_stop();
    }
}

void master_volume(int dv,int mv)
{
    if(dv>=0) digi_volume=zc_max(zc_min(dv,255),0);
    
    if(mv>=0) midi_volume=zc_max(zc_min(mv,255),0);
    
    int i = zc_min(zc_max(currmidi,0),MAXMIDIS-1);
    set_volume(digi_volume,mixvol(tunes[i].volume,midi_volume));
}

/*****************/
/*****  SFX  *****/
/*****************/

// array of voices, one for each sfx sample in the data file
// 0+ = voice #
// -1 = voice not allocated
void Z_init_sound()
{
    for(int i=0; i<WAV_COUNT; i++)
        sfx_voice[i]=-1;
        
    for(int i=0; i<ZC_MIDI_COUNT; i++)
        tunes[i].data = (MIDI*)mididata[i].dat;
        
    for(int j=0; j<MAXCUSTOMMIDIS; j++)
        tunes[ZC_MIDI_COUNT+j].data=NULL;
        
    master_volume(digi_volume,midi_volume);
}

// returns number of voices currently allocated
int sfx_count()
{
    int c=0;
    
    for(int i=0; i<WAV_COUNT; i++)
        if(sfx_voice[i]!=-1)
            ++c;
            
    return c;
}

// clean up finished samples
void sfx_cleanup()
{
    for(int i=0; i<WAV_COUNT; i++)
        if(sfx_voice[i]!=-1 && voice_get_position(sfx_voice[i])<0)
        {
            deallocate_voice(sfx_voice[i]);
            sfx_voice[i]=-1;
        }
}

// allocates a voice for the sample "wav_index" (index into zelda.dat)
// if a voice is already allocated (and/or playing), then it just returns true
// Returns true:  voice is allocated
//         false: unsuccessful
bool sfx_init(int index)
{
    // check index
    if(index<=0 || index>=WAV_COUNT)
        return false;
        
    if(sfx_voice[index]==-1)
    {
        if(sfxdat)
        {
            if(index<Z35)
            {
                sfx_voice[index]=allocate_voice((SAMPLE*)sfxdata[index].dat);
            }
            else
            {
                sfx_voice[index]=allocate_voice((SAMPLE*)sfxdata[Z35].dat);
            }
        }
        else
        {
            sfx_voice[index]=allocate_voice(&customsfxdata[index]);
        }
        
        voice_set_volume(sfx_voice[index], sfx_volume);
    }
    
    return sfx_voice[index] != -1;
}

// plays an sfx sample
void sfx(int index,int pan,bool loop, bool restart)
{
    if(!sfx_init(index))
        return;
        
    voice_set_playmode(sfx_voice[index],loop?PLAYMODE_LOOP:PLAYMODE_PLAY);
    voice_set_pan(sfx_voice[index],pan);
    
    int pos = voice_get_position(sfx_voice[index]);
    
    if(restart) voice_set_position(sfx_voice[index],0);
    
    if(pos<=0)
        voice_start(sfx_voice[index]);
}

// true if sfx is allocated
bool sfx_allocated(int index)
{
    return (index>0 && index<WAV_COUNT && sfx_voice[index]!=-1);
}

// start it (in loop mode) if it's not already playing,
// otherwise adjust it to play in loop mode -DD
void cont_sfx(int index)
{
    if(!sfx_init(index))
    {
        return;
    }
    
    if(voice_get_position(sfx_voice[index])<=0)
    {
        voice_set_position(sfx_voice[index],0);
        voice_set_playmode(sfx_voice[index],PLAYMODE_LOOP);
        voice_start(sfx_voice[index]);
    }
    else
    {
        adjust_sfx(index, 128, true);
    }
}

// adjust parameters while playing
void adjust_sfx(int index,int pan,bool loop)
{
    if(index<=0 || index>=WAV_COUNT || sfx_voice[index]==-1)
        return;
        
    voice_set_playmode(sfx_voice[index],loop?PLAYMODE_LOOP:PLAYMODE_PLAY);
    voice_set_pan(sfx_voice[index],pan);
}

// pauses a voice
void pause_sfx(int index)
{
    if(index>0 && index<WAV_COUNT && sfx_voice[index]!=-1)
        voice_stop(sfx_voice[index]);
}

// resumes a voice
void resume_sfx(int index)
{
    if(index>0 && index<WAV_COUNT && sfx_voice[index]!=-1)
        voice_start(sfx_voice[index]);
}

// pauses all active voices
void pause_all_sfx()
{
    for(int i=0; i<WAV_COUNT; i++)
        if(sfx_voice[i]!=-1)
            voice_stop(sfx_voice[i]);
}

// resumes all paused voices
void resume_all_sfx()
{
    for(int i=0; i<WAV_COUNT; i++)
        if(sfx_voice[i]!=-1)
            voice_start(sfx_voice[i]);
}

// stops an sfx and deallocates the voice
void stop_sfx(int index)
{
    if(index<=0 || index>=WAV_COUNT)
        return;
        
    if(sfx_voice[index]!=-1)
    {
        deallocate_voice(sfx_voice[index]);
        sfx_voice[index]=-1;
    }
}

// Stops SFX played by Link's item of the given family
void stop_item_sfx(int family)
{
    int id=current_item_id(family);
    
    if(id<0)
        return;
        
    stop_sfx(itemsbuf[id].usesound);
}

void kill_sfx()
{
    for(int i=0; i<WAV_COUNT; i++)
        if(sfx_voice[i]!=-1)
        {
            deallocate_voice(sfx_voice[i]);
            sfx_voice[i]=-1;
        }
}

int pan(int x)
{
    switch(pan_style)
    {
    case 0:
        return 128;
        
    case 1:
        return vbound((x>>1)+68,0,255);
        
    case 2:
        return vbound(((x*3)>>2)+36,0,255);
    }
    
    return vbound(x,0,255);
}

/*******************************/
/******* Input Handlers ********/
/*******************************/

#define MAX_BUTTONS_CHK		13 // button range 0 - 12

bool joybtn(int b)
{
    if(b == 0)
        return false;
        
    return joy[joystick_index].button[b-1].b !=0;
}

int next_press_key()
{
    char k[128];
    
    for(int i=0; i<128; i++)
        k[i]=key[i];
        
    for(;;)
    {
        for(int i=0; i<128; i++)
            if(key[i]!=k[i])
                return i;
    }
    
    //	return (readkey()>>8);
}

int next_press_btn()
{
    clear_keybuf();
    bool b[MAX_BUTTONS_CHK];
    
    for(int i=1; i<MAX_BUTTONS_CHK; i++)
        b[i]=joybtn(i);
        
    //first, we need to wait until they're pressing no buttons
    for(;;)
    {
        if(keypressed())
        {
            switch(readkey()>>8)
            {
            case KEY_ESC:
                return -1;
                
            case KEY_SPACE:
                return 0;
            }
        }
        
        poll_joystick();
        bool done = true;
        
        for(int i=1; i<MAX_BUTTONS_CHK; i++)
        {
            if(joybtn(i)) done = false;
        }
        
        if(done) break;
    }
    
    //now, we need to wait for them to press any button
    for(;;)
    {
        if(keypressed())
        {
            switch(readkey()>>8)
            {
            case KEY_ESC:
                return -1;
                
            case KEY_SPACE:
                return 0;
            }
        }
        
        poll_joystick();
        
        for(int i=1; i<MAX_BUTTONS_CHK; i++)
        {
            if(joybtn(i)) return i;
        }
    }
}

static bool rButton(bool(proc)(),bool &flag)
{
    if(!proc())
    {
        flag=false;
    }
    else if(!flag)
    {
        flag=true;
        return true;
    }
    
    return false;
}

bool control_state[18]=
{
    false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false
};

bool button_press[18] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
bool button_hold[18] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};

void load_control_state()
{
#define STICK_PRECISION   56 //define your own sensitivity

    control_state[0]=key[DUkey]||joy[joystick_index].stick[0].axis[1].d1||joy[joystick_index].stick[0].axis[1].pos< -STICK_PRECISION;
    control_state[1]=key[DDkey]||joy[joystick_index].stick[0].axis[1].d2||joy[joystick_index].stick[0].axis[1].pos > STICK_PRECISION;
    control_state[2]=key[DLkey]||joy[joystick_index].stick[0].axis[0].d1||joy[joystick_index].stick[0].axis[0].pos< -STICK_PRECISION;
    control_state[3]=key[DRkey]||joy[joystick_index].stick[0].axis[0].d2||joy[joystick_index].stick[0].axis[0].pos > STICK_PRECISION;
    control_state[4]=key[Akey]||joybtn(Abtn);
    control_state[5]=key[Bkey]||joybtn(Bbtn);
    control_state[6]=key[Skey]||joybtn(Sbtn);
    control_state[7]=key[Lkey]||joybtn(Lbtn);
    control_state[8]=key[Rkey]||joybtn(Rbtn);
    control_state[9]=key[Pkey]||joybtn(Pbtn);
    control_state[10]=key[Exkey1]||joybtn(Exbtn1);
    control_state[11]=key[Exkey2]||joybtn(Exbtn2);
    control_state[12]=key[Exkey3]||joybtn(Exbtn3);
    control_state[13]=key[Exkey4]||joybtn(Exbtn4);
    
    if(num_joysticks != 0)
    {
        //this is a workaround to a really stupid allegro bug.
        control_state[14]= joy[joystick_index].stick[1].axis[0].pos - 128 < -STICK_PRECISION;
        control_state[15]= joy[joystick_index].stick[1].axis[0].pos - 128 > STICK_PRECISION;
        control_state[16]= joy[joystick_index].stick[0].axis[2].pos < -STICK_PRECISION;
        control_state[17]= joy[joystick_index].stick[0].axis[2].pos > STICK_PRECISION;
    }
    
    button_press[0]=rButton(Up,button_hold[0]);
    button_press[1]=rButton(Down,button_hold[1]);
    button_press[2]=rButton(Left,button_hold[2]);
    button_press[3]=rButton(Right,button_hold[3]);
    button_press[4]=rButton(cAbtn,button_hold[4]);
    button_press[5]=rButton(cBbtn,button_hold[5]);
    button_press[6]=rButton(cSbtn,button_hold[6]);
    button_press[7]=rButton(cLbtn,button_hold[7]);
    button_press[8]=rButton(cRbtn,button_hold[8]);
    button_press[9]=rButton(cPbtn,button_hold[9]);
    button_press[10]=rButton(cEx1btn,button_hold[10]);
    button_press[11]=rButton(cEx2btn,button_hold[11]);
    button_press[12]=rButton(cEx3btn,button_hold[12]);
    button_press[13]=rButton(cEx4btn,button_hold[13]);
    button_press[14]=rButton(AxisUp,button_hold[14]);
    button_press[15]=rButton(AxisDown,button_hold[15]);
    button_press[16]=rButton(AxisLeft,button_hold[16]);
    button_press[17]=rButton(AxisRight,button_hold[17]);
    
}

// Returns true if any game key is pressed. This is needed because keypressed()
// doesn't detect modifier keys and control_state[] can be modified by scripts.
bool zc_key_pressed()
{
    if((key[DUkey]||joy[joystick_index].stick[0].axis[1].d1||joy[joystick_index].stick[0].axis[1].pos< -STICK_PRECISION) ||
       (key[DDkey]||joy[joystick_index].stick[0].axis[1].d2||joy[joystick_index].stick[0].axis[1].pos > STICK_PRECISION) ||
       (key[DLkey]||joy[joystick_index].stick[0].axis[0].d1||joy[joystick_index].stick[0].axis[0].pos< -STICK_PRECISION) ||
       (key[DRkey]||joy[joystick_index].stick[0].axis[0].d2||joy[joystick_index].stick[0].axis[0].pos > STICK_PRECISION) ||
       (key[Akey]||joybtn(Abtn)) ||
       (key[Bkey]||joybtn(Bbtn)) ||
       (key[Skey]||joybtn(Sbtn)) ||
       (key[Lkey]||joybtn(Lbtn)) ||
       (key[Rkey]||joybtn(Rbtn)) ||
       (key[Pkey]||joybtn(Pbtn)) ||
       (key[Exkey1]||joybtn(Exbtn1)) ||
       (key[Exkey2]||joybtn(Exbtn2)) ||
       (key[Exkey3]||joybtn(Exbtn3)) ||
       (key[Exkey4]||joybtn(Exbtn4))) // Skipping joystick axes
        return true;
    
    return false;
}

bool Up()
{
    return control_state[0];
}
bool Down()
{
    return control_state[1];
}
bool Left()
{
    return control_state[2];
}
bool Right()
{
    return control_state[3];
}
bool cAbtn()
{
    return control_state[4];
}
bool cBbtn()
{
    return control_state[5];
}
bool cSbtn()
{
    return control_state[6];
}
bool cLbtn()
{
    return control_state[7];
}
bool cRbtn()
{
    return control_state[8];
}
bool cPbtn()
{
    return control_state[9];
}
bool cEx1btn()
{
    return control_state[10];
}
bool cEx2btn()
{
    return control_state[11];
}
bool cEx3btn()
{
    return control_state[12];
}
bool cEx4btn()
{
    return control_state[13];
}
bool AxisUp()
{
    return control_state[14];
}
bool AxisDown()
{
    return control_state[15];
}
bool AxisLeft()
{
    return control_state[16];
}
bool AxisRight()
{
    return control_state[17];
}

bool cMbtn()
{
    return key[KEY_ESC]||joybtn(Mbtn);
}
bool cF12()
{
    return key[KEY_F12] != 0;
}
bool cF11()
{
    return key[KEY_F11] != 0;
}
bool cF5()
{
    return key[KEY_F5]  != 0;
}
bool cQ()
{
    return key[KEY_Q]   != 0;
}
bool cI()
{
    return key[KEY_I]   != 0;
}

bool rUp()
{
    return rButton(Up,Udown);
}
bool rDown()
{
    return rButton(Down,Ddown);
}
bool rLeft()
{
    return rButton(Left,Ldown);
}
bool rRight()
{
    return rButton(Right,Rdown);
}
bool rAbtn()
{
    return rButton(cAbtn,Adown);
}
bool rBbtn()
{
    return rButton(cBbtn,Bdown);
}
bool rSbtn()
{
    return rButton(cSbtn,Sdown);
}
bool rMbtn()
{
    return rButton(cMbtn,Mdown);
}
bool rLbtn()
{
    return rButton(cLbtn,LBdown);
}
bool rRbtn()
{
    return rButton(cRbtn,RBdown);
}
bool rPbtn()
{
    return rButton(cPbtn,Pdown);
}
bool rEx1btn()
{
    return rButton(cEx1btn,Ex1down);
}
bool rEx2btn()
{
    return rButton(cEx2btn,Ex2down);
}
bool rEx3btn()
{
    return rButton(cEx3btn,Ex3down);
}
bool rEx4btn()
{
    return rButton(cEx4btn,Ex4down);
}
bool rAxisUp()
{
    return rButton(AxisUp,AUdown);
}
bool rAxisDown()
{
    return rButton(AxisDown,ADdown);
}
bool rAxisLeft()
{
    return rButton(AxisLeft,ALdown);
}
bool rAxisRight()
{
    return rButton(AxisRight,ARdown);
}

bool rF12()
{
    return rButton(cF12, F12);
}
bool rF11()
{
    return rButton(cF11, F11);
}
bool rF5()
{
    return rButton(cF5, F5);
}
bool rQ()
{
    return rButton(cQ,  keyQ);
}
bool rI()
{
    return rButton(cI,  keyI);
}

bool drunk()
{
    return ((!(frame%((rand()%100)+1)))&&(rand()%MAXDRUNKCLOCK<Link.DrunkClock()));
}

bool DrunkUp()
{
    return drunk()?(rand()%2)?0:!Up():Up();
}
bool DrunkDown()
{
    return drunk()?(rand()%2)?0:!Down():Down();
}
bool DrunkLeft()
{
    return drunk()?(rand()%2)?0:!Left():Left();
}
bool DrunkRight()
{
    return drunk()?(rand()%2)?0:!Right():Right();
}
bool DrunkcAbtn()
{
    return drunk()?(rand()%2)?0:!cAbtn():cAbtn();
}
bool DrunkcBbtn()
{
    return drunk()?(rand()%2)?0:!cBbtn():cBbtn();
}
bool DrunkcSbtn()
{
    return drunk()?(rand()%2)?0:!cSbtn():cSbtn();
}
bool DrunkcMbtn()
{
    return drunk()?(rand()%2)?0:!cMbtn():cMbtn();
}
bool DrunkcLbtn()
{
    return drunk()?(rand()%2)?0:!cLbtn():cLbtn();
}
bool DrunkcRbtn()
{
    return drunk()?(rand()%2)?0:!cRbtn():cRbtn();
}
bool DrunkcPbtn()
{
    return drunk()?(rand()%2)?0:!cPbtn():cPbtn();
}

bool DrunkrUp()
{
    return drunk()?(rand()%2)?0:!rUp():rUp();
}
bool DrunkrDown()
{
    return drunk()?(rand()%2)?0:!rDown():rDown();
}
bool DrunkrLeft()
{
    return drunk()?(rand()%2)?0:!rLeft():rLeft();
}
bool DrunkrRight()
{
    return drunk()?(rand()%2)?0:!rRight():rRight();
}
bool DrunkrAbtn()
{
    return drunk()?(rand()%2)?0:!rAbtn():rAbtn();
}
bool DrunkrBbtn()
{
    return drunk()?(rand()%2)?0:!rBbtn():rBbtn();
}
bool DrunkrSbtn()
{
    return drunk()?(rand()%2)?0:!rSbtn():rSbtn();
}
bool DrunkrMbtn()
{
    return drunk()?(rand()%2)?0:!rMbtn():rMbtn();
}
bool DrunkrLbtn()
{
    return drunk()?(rand()%2)?0:!rLbtn():rLbtn();
}
bool DrunkrRbtn()
{
    return drunk()?(rand()%2)?0:!rRbtn():rRbtn();
}
bool DrunkrPbtn()
{
    return drunk()?(rand()%2)?0:!rPbtn():rPbtn();
}

void eat_buttons()
{
    rAbtn();
    rBbtn();
    rSbtn();
    rMbtn();
    rLbtn();
    rRbtn();
    rPbtn();
    rEx1btn();
    rEx2btn();
    rEx3btn();
    rEx4btn();
}

bool ReadKey(int k)
{
    if(key[k])
    {
        key[k]=0;
        return true;
    }
    
    return false;
}

void zc_putpixel(int layer, int x, int y, int cset, int color, int timer)
{
    timer=timer;
    particles.add(new particle(fix(x), fix(y), layer, cset, color));
}

/*** end of zc_sys.cc ***/

