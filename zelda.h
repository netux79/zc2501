#ifndef _ZELDA_H_
#define _ZELDA_H_

/*********************************/
/********** Definitions **********/
/*********************************/

#include <vector>
#include "zdefs.h"
#include "zcarray.h"
#include "zcsys.h"
#include "zeldadat.h"
#include "sfx.h"
#include "zcmusic.h"
#include "scriptdraw.h"

#define  MAXMIDIS     ZC_MIDI_COUNT+MAXCUSTOMTUNES

// saved games
#define MAXSAVES      6 // It's constant enough... :p

// game.maps[] flags
#define mSECRET             8192                                 // only overworld and caves use this
#define mVISITED            16384                                // only overworld uses this

#define mDOOR_UP            1                                 // only dungeons use this
#define mDOOR_DOWN          2                                 //        ''
#define mDOOR_LEFT          4                                 //        ''
#define mDOOR_RIGHT         8                                 //        ''

#define mITEM              16                                 // item (main screen)
#define mBELOW             32                                 // special item (underground)
#define mNEVERRET          64                                 // enemy never returns
#define mTMPNORET         128                                 // enemies don't return until you leave the dungeon

#define mLOCKBLOCK        256                                 // if the lockblock on the screen has been triggered
#define mBOSSLOCKBLOCK    512                                 // if the bosslockblock on the screen has been triggered

#define mCHEST           1024                                 // if the unlocked check on this screen has been opened
#define mLOCKEDCHEST     2048                                 // if the locked chest on this screen has been opened
#define mBOSSCHEST       4096                                 // if the boss chest on this screen has been opened
#define mOTHER3         32768                                 // overwrite this value, use for expansion

/*********************************/
/******** Enums & Structs ********/
/*********************************/

enum { qQUIT=1, qRESET, qEXIT, qGAMEOVER, qCONT, qWON, qERROR };

// "special" walk flags
enum
{
    spw_none, spw_door, spw_clipright, spw_floater, spw_trap, spw_halfstep,
    spw_water, spw_wizzrobe, spw_clipbottomright
};

// death & fade defs
enum
{
    fade_none, fade_flicker, fade_invisible, fade_flash_die,
    fade_blue_poof
};

/*********************************/
/*********** Procedures **********/
/*********************************/

void Z_eventlog(const char *format, ...);
void Z_scripterrlog(const char * const format, ...);

void ALLOFF(bool messagesToo = true, bool decorationsToo = true);
void centerLink();
fix  LinkX();
fix  LinkY();
fix  LinkZ();
int  LinkHClk();
int  LinkNayrusLoveShieldClk();
int  LinkHoverClk();
int  LinkSwordClk();
int  LinkItemClk();
int  LinkAction();
int  LinkCharged();
bool LinkGetDontDraw();
void setSwordClk(int newclk);
void setItemClk(int newclk);
int  LinkLStep();
void LinkCheckItems();
fix  LinkModifiedX();
fix  LinkModifiedY();
fix  GuyX(int j);
fix  GuyY(int j);
int  GuyID(int j);
int  GuyMisc(int j);
void StunGuy(int j,int stun);
bool  GuySuperman(int j);
int  GuyCount();
int  LinkDir();
void add_grenade(int wx, int wy, int wz, int size, int parentid);
fix distance(int x1, int y1, int x2, int y2);
bool getClock();
void setClock(bool state);
void CatchBrang();
int LinkAction();

void do_dcounters();
void donewmsg(int str);
void dismissmsg();
void dointro();
void init_dmap();
int  init_game();
int  cont_game();
void restart_level();
void quit_game();

int get_currdmap();
int get_dlevel();
int get_currscr();
int get_homescr();
int get_bmaps(int si);
bool no_subscreen();
void setZScriptVersion(int s_version);

INLINE void sfx(int index)
{
    sfx(index,128,false);
}

INLINE void sfx(int index,int pan)
{
    sfx(index,vbound(pan, 0, 255) ,false);
}

/**********************************/
/******** Global Variables ********/
/**********************************/


extern ZCMUSIC *zcmusic;

extern zinitdata  zinit;
extern int lens_hint_item[MAXITEMS][2];                     //aclk, aframe
extern int lens_hint_weapon[MAXWPNS][5];                    //aclk, aframe, dir, x, y
extern int strike_hint_counter;
extern int strike_hint_timer;
extern int strike_hint;

extern RGB_MAP rgb_table;
extern COLOR_MAP trans_table, trans_table2;
extern BITMAP     *framebuf, *scrollbuf, *tmp_scr, *msgbmpbuf, *msgdisplaybuf, *pricesdisplaybuf, *real_screen, *prim_bmp, *lens_scr;
extern DATAFILE *data, *sfxdata, *fontsdata, *mididata;
extern FONT  *nfont, *zfont, *z3font, *z3smallfont, *deffont, *lfont, *lfont_l, *pfont, *mfont, *ztfont, *sfont, *sfont2, *sfont3, *spfont, *ssfont1, *ssfont2, *ssfont3, *ssfont4, *gblafont,
       *goronfont, *zoranfont, *hylian1font, *hylian2font, *hylian3font, *hylian4font, *gboraclefont, *gboraclepfont, *dsphantomfont, *dsphantompfont;
extern PALETTE  RAMpal;
extern byte     *colordata;
extern itemdata *itemsbuf;
extern wpndata  *wpnsbuf;
extern comboclass *combo_class_buf;
extern guydata  *guysbuf;
extern item_drop_object    item_drop_sets[MAXITEMDROPSETS];

extern bool ewind_restart;
extern word     msgclk, msgstr, msgpos, msgptr, msgcolour, msgspeed,msg_w,
       msg_h,
       msgorig,
       msg_xpos,
       msg_ypos,
       cursor_x,
       cursor_y;
extern bool msg_onscreen, msg_active,msgspace;
extern FONT     *msgfont;
extern word     door_combo_set_count;
extern word     introclk, intropos, dmapmsgclk, linkedmsgclk;
extern short    lensclk;
extern int     lensid;
extern int    Bpos;
extern byte screengrid[22];
extern byte ffcgrid[4];
extern volatile int logic_counter;
extern bool screenscrolling;
extern bool close_button_quit;
extern int fullscreen;

extern PALETTE tempbombpal;
extern bool usebombpal;

extern int homescr,currscr,frame,currmap,dlevel,warpscr,worldscr;
extern int newscr_clk,opendoors,currdmap,fadeclk,currgame,listpos;
extern int lastentrance,lastentrance_dmap, prices[3],loadside, Bwpn, Awpn;
extern int digi_volume,midi_volume,sfx_volume,emusic_volume,currmidi,hasitem,whistleclk,pan_style;
extern int Akey,Bkey,Skey,Lkey,Rkey,Pkey,Exkey1,Exkey2,Exkey3,Exkey4,Quit;
extern int DUkey, DDkey, DLkey, DRkey;
extern int hs_startx, hs_starty, hs_xdist, hs_ydist, clockclk, clock_zoras[eMAXGUYS];
extern int swordhearts[4], currcset, gfc, gfc2, pitx, pity, refill_what, refill_why;
extern int heart_beep_timer, new_enemy_tile_start, nets, magicitem, nayruitem;
extern int magiccastclk, castx, casty, quakeclk, wavy, df_x, df_y, nl1_x, nl1_y, nl2_x, nl2_y, magicdrainclk, conveyclk;

extern bool blockmoving;
extern bool Throttlefps, ShowFPS, Playing, TransLayers;
extern bool refreshpal,blockpath,loaded_guys,freeze_guys;
extern bool loaded_enemies,drawguys,watch;
extern bool Udown,Ddown,Ldown,Rdown,Adown,Bdown,Sdown,Mdown,LBdown,RBdown,Pdown,Ex1down,Ex2down,Ex3down,Ex4down,AUdown,ADdown,ALdown,ARdown;
extern bool NESquit,boughtsomething;
extern bool fixed_door, darkroom,naturaldark,BSZ;            //,NEWSUBSCR;
extern bool hookshot_used, hookshot_frozen, pull_link, add_chainlink;
extern bool del_chainlink, hs_fix, cheat_superman, checklink;
extern bool ewind_restart, didpit, heart_beep, castnext;
extern bool add_df1asparkle, add_df1bsparkle, add_nl1asparkle, add_nl1bsparkle, add_nl2asparkle, add_nl2bsparkle;
extern bool is_on_conveyor, activated_timed_warp;

extern byte COOLSCROLL;

extern int add_asparkle, add_bsparkle;

extern int    cheat_goto_dmap, cheat_goto_screen;
extern short  visited[6];
extern byte   guygrid[176];
extern mapscr tmpscr[2];
extern mapscr tmpscr2[6];
extern mapscr tmpscr3[6];
extern ffscript *ffscripts[512];
extern ffscript *itemscripts[256];
extern ffscript *globalscripts[NUMSCRIPTGLOBAL];

extern ffscript *guyscripts[256];
extern ffscript *wpnscripts[256];
extern ffscript *linkscripts[3];
extern ffscript *screenscripts[256];
extern SAMPLE customsfxdata[WAV_COUNT];
extern int sfxdat;

#define MAX_ZCARRAY_SIZE	4096
typedef ZCArray<long32> ZScriptArray;
extern ZScriptArray localRAM[MAX_ZCARRAY_SIZE];
extern byte arrayOwner[MAX_ZCARRAY_SIZE];
extern ZScriptDrawingRenderTarget* zscriptDrawingRenderTarget;

dword getNumGlobalArrays();

extern int  resx,resy,scrx,scry;
extern bool sbig;                                           // big screen
extern int screen_scale; //user adjustable screen size.
extern char quest_path[1024];
extern gamedata *saves;
extern gamedata *game;

extern volatile int lastfps;
extern volatile int framecnt;
extern void throttleFPS();

// quest file data
extern zquestheader QHeader;
extern byte                quest_rules[QUESTRULES_SIZE];
extern byte                midi_flags[MIDIFLAGS_SIZE];
extern word                map_count;
extern MsgStr              *MsgStrings;
extern int                 msg_strings_size;
extern DoorComboSet        *DoorComboSets;
extern dmap                *DMaps;
extern miscQdata           QMisc;
extern std::vector<mapscr> TheMaps;
extern zcmap               *ZCMaps;

/**********************************/
/*********** Misc Data ************/
/**********************************/

extern const char startguy[8];
extern const char gambledat[12*6];
extern const byte stx[4][9];
extern const byte sty[4][9];
extern const byte ten_rupies_x[10];
extern const byte ten_rupies_y[10];
extern zctune tunes[MAXMIDIS];

#endif
