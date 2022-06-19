//--------------------------------------------------------
//  Zelda Classic
//  by Jeremy Craner, 1999-2000
//
//  zelda.cc
//
//  Main code for Zelda Classic. Originally written in
//  SPHINX C--, now rewritten in DJGPP with Allegro.
//
//--------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string>
#include <map>
#include <vector>

#include "zscriptversion.h"
#include "zcmusic.h"
#include "zdefs.h"
#include "zelda.h"
#include "tiles.h"
#include "colors.h"
#include "pal.h"
#include "zsys.h"
#include "qst.h"
#include "fontsdat.h"
#include "particles.h"
#include "ffscript.h"
#include "zc_array.h"
#include "rendertarget.h"

ZCMUSIC *zcmusic = NULL;
zinitdata zinit;
int lens_hint_item[MAXITEMS][2];                            //aclk, aframe
int lens_hint_weapon[MAXWPNS][5];                           //aclk, aframe, dir, x, y
int strike_hint_counter=0;
int strike_hint_timer=0;
int strike_hint;

CScriptDrawingCommands script_drawing_commands;

using std::string;
using std::pair;
extern std::map<int, pair<string,string> > ffcmap;

int passive_subscreen_height=56;
int original_playing_field_offset=56;
int playing_field_offset=original_playing_field_offset;
int passive_subscreen_offset=0;
extern int directItem;
extern int directItemA;
extern int directItemB;

int favorite_combos[MAXFAVORITECOMBOS];
int favorite_comboaliases[MAXFAVORITECOMBOALIASES];

void playLevelMusic();

volatile int logic_counter=0;
void update_logic_counter()
{
    ++logic_counter;
}
END_OF_FUNCTION(update_logic_counter)

#ifdef _SCRIPT_COUNTER
volatile int script_counter=0;
void update_script_counter()
{
    ++script_counter;
}
END_OF_FUNCTION(update_script_counter)
#endif

void throttleFPS()
{
    if(Throttlefps)
    {
        int t = 0;
        while(logic_counter < 1)
        {
            int ms = t >= 16 ? 0 : 1;
            rest(ms);
            t++;
        }
    }
    
    logic_counter = 0;
}

int dmap_list_size=MAXDMAPS;
bool dmap_list_zero=true;

/**********************************/
/******** Global Variables ********/
/**********************************/

RGB_MAP rgb_table;
COLOR_MAP trans_table, trans_table2;

BITMAP     *framebuf, *scrollbuf, *tmp_bmp, *tmp_scr, *msgdisplaybuf, *pricesdisplaybuf, *real_screen, *temp_buf, *prim_bmp;
DATAFILE   *data, *sfxdata, *fontsdata, *mididata;
FONT       *nfont, *zfont, *z3font, *z3smallfont, *deffont, *lfont, *lfont_l, *pfont, *mfont, *ztfont, *sfont, *sfont2, *sfont3, *spfont, *ssfont1, *ssfont2, *ssfont3, *ssfont4, *gblafont,
           *goronfont, *zoranfont, *hylian1font, *hylian2font, *hylian3font, *hylian4font, *gboraclefont, *gboraclepfont, *dsphantomfont, *dsphantompfont;
PALETTE    RAMpal;
byte       *colordata;
//byte       *tilebuf;
itemdata   *itemsbuf;
wpndata    *wpnsbuf;
comboclass *combo_class_buf;
guydata    *guysbuf;
item_drop_object    item_drop_sets[MAXITEMDROPSETS];
ZCHEATS    zcheats;
/*char       palnames[MAXLEVELS][PALNAMESIZE];*/
/*
newcombo   *combobuf;
word animated_combo_table[MAXCOMBOS][2];                    //[0]=position in act2, [1]=original tile
word animated_combo_table4[MAXCOMBOS][2];                   //[0]=combo, [1]=clock
word animated_combos;
word animated_combo_table2[MAXCOMBOS][2];                    //[0]=position in act2, [1]=original tile
word animated_combo_table24[MAXCOMBOS][2];                   //[0]=combo, [1]=clock
word animated_combos2;
bool blank_tile_table[NEWMAXTILES];                         //keeps track of blank tiles
bool blank_tile_quarters_table[NEWMAXTILES*4];              //keeps track of blank tiles
*/
bool ewind_restart=false;

word     msgclk, msgstr,
         msgpos,	// screen position of the next character.
         msgptr,	// position within the string of the next character. <MSGSIZE.
         msgcolour,	// colour to use for the displayed text.
         msgspeed,	// delay between each character (5 = default).
         msg_w,
         msg_h,
         msg_count,
         cursor_x,
         cursor_y,
         msg_xpos=0,
         msg_ypos=0,
         msgorig=0;
bool msg_onscreen = false, msg_active = false, msgspace = false;
BITMAP   *msgbmpbuf = NULL;
FONT	 *msgfont;
word     door_combo_set_count;
word     introclk, intropos, dmapmsgclk, linkedmsgclk;
short    lensclk;
int     lensid; // Lens's item id. -1 if lens is off.
int    Bpos;
byte screengrid[22];
byte ffcgrid[4];
bool screenscrolling=false;
bool close_button_quit=false;
PALETTE tempbombpal;
bool usebombpal;

int readsize, writesize;
bool fake_pack_writing=false;
combo_alias combo_aliases[MAXCOMBOALIASES];  //Temporarily here so ZC can compile. All memory from this is freed after loading the quest file.

SAMPLE customsfxdata[WAV_COUNT];
unsigned char customsfxflag[WAV_COUNT>>3];
int sfxdat=1;
BITMAP *hw_screen;
int zqwin_scale;

int fullscreen;
int homescr,currscr,frame=0,currmap=0,dlevel,warpscr,worldscr;
int newscr_clk=0,opendoors=0,currdmap=0,fadeclk=-1,currgame=0,listpos=0;
int lastentrance=0,lastentrance_dmap=0,prices[3],loadside, Bwpn, Awpn;
int digi_volume,midi_volume,sfx_volume,emusic_volume,currmidi,hasitem,whistleclk,pan_style;
int joystick_index=0,Akey,Bkey,Skey,Lkey,Rkey,Pkey,Exkey1,Exkey2,Exkey3,Exkey4,Abtn,Bbtn,Sbtn,Mbtn,Lbtn,Rbtn,Pbtn,Exbtn1,Exbtn2,Exbtn3,Exbtn4,Quit=0;
int DUkey, DDkey, DLkey, DRkey;
int hs_startx, hs_starty, hs_xdist, hs_ydist, clockclk, clock_zoras[eMAXGUYS];
int cheat_goto_dmap=0, cheat_goto_screen=0, currcset;
int gfc, gfc2, pitx, pity, refill_what, refill_why, heart_beep_timer=0, new_enemy_tile_start=1580;
int nets=1580, magicitem=-1,nayruitem=-1, magiccastclk, quakeclk=0, wavy=0, castx, casty, df_x, df_y, nl1_x, nl1_y, nl2_x, nl2_y;
int magicdrainclk=0, conveyclk=3, memrequested=0;
int checkx, checky;
int skipcont=0;

bool Throttlefps, Paused=false, ShowFPS;
bool Playing, FrameSkip=false, TransLayers;
bool refreshpal,blockpath,loaded_guys,freeze_guys,
     loaded_enemies,drawguys,watch;
bool darkroom=false,naturaldark=false,BSZ;                         //,NEWSUBSCR;
bool Udown,Ddown,Ldown,Rdown,Adown,Bdown,Sdown,Mdown,LBdown,RBdown,Pdown,Ex1down,Ex2down,Ex3down,Ex4down,AUdown,ADdown,ALdown,ARdown,F12,F11, F5,keyI, keyQ,
     NESquit,boughtsomething=false,
     fixed_door=false, hookshot_used=false, hookshot_frozen=false,
     pull_link=false, add_chainlink=false, del_chainlink=false, hs_fix=false,
     cheat_superman=false, checklink=true, didpit=false, heart_beep=true,
     castnext=false, add_df1asparkle, add_df1bsparkle, add_nl1asparkle, add_nl1bsparkle, add_nl2asparkle, add_nl2bsparkle,
     is_on_conveyor, activated_timed_warp=false;

byte COOLSCROLL;

int  add_asparkle=0, add_bsparkle=0;

char   zeldadat_sig[52];
char   sfxdat_sig[52];
char   fontsdat_sig[52];
char   cheat_goto_dmap_str[4];
char   cheat_goto_screen_str[3];
short  visited[6];
byte   guygrid[176];
mapscr tmpscr[2];
mapscr tmpscr2[6];
mapscr tmpscr3[6];
gamedata *game=NULL;
ffscript *ffscripts[NUMSCRIPTFFC];
ffscript *itemscripts[NUMSCRIPTITEM];
ffscript *globalscripts[NUMSCRIPTGLOBAL];

//If only...
ffscript *guyscripts[NUMSCRIPTGUYS];
ffscript *wpnscripts[NUMSCRIPTWEAPONS];
ffscript *linkscripts[NUMSCRIPTLINK];
ffscript *screenscripts[NUMSCRIPTSCREEN];

extern refInfo globalScriptData;
extern word g_doscript;
extern bool global_wait;

//ZScript array storage
std::vector<ZScriptArray> globalRAM;
ZScriptArray localRAM[MAX_ZCARRAY_SIZE];
byte arrayOwner[MAX_ZCARRAY_SIZE];

//script bitmap drawing
ZScriptDrawingRenderTarget* zscriptDrawingRenderTarget;


void setZScriptVersion(int s_version)
{
    ZScriptVersion::setVersion(s_version);
}

void initZScriptArrayRAM(bool firstplay)
{
    for(word i = 0; i < MAX_ZCARRAY_SIZE; i++)
    {
        localRAM[i].Clear();
        arrayOwner[i]=255;
    }
    
    if(game->globalRAM.size() != 0)
        game->globalRAM.clear();
        
    if(firstplay)
    {
        //leave to global script ~Init to allocate global memory first time round
        game->globalRAM.resize(getNumGlobalArrays());
    }
    else
    {
        //allocate from save file
        game->globalRAM.resize(saves[currgame].globalRAM.size());
        
        for(dword i = 0; i < game->globalRAM.size(); i++)
        {
#ifdef _DEBUGARRAYALLOC
            al_trace("Global Array: %i\n",i);
#endif
            ZScriptArray &from = saves[currgame].globalRAM[i];
            ZScriptArray &to = game->globalRAM[i];
            to.Resize(from.Size());
            
            for(dword j = 0; j < from.Size(); j++)
            {
#ifdef _DEBUGARRAYALLOC
                al_trace("Element: %i\nInit: %i, From save file: %i\n", j, to[j], from[j]);
#endif
                to[j] = from[j];
            }
        }
    }
}

void initZScriptGlobalRAM()
{
    g_doscript = 1;
    globalScriptData.Clear();
    clear_global_stack();
}

dword getNumGlobalArrays()
{
    word scommand, pc = 0, ret = 0;
    
    do
    {
        scommand = globalscripts[GLOBAL_SCRIPT_INIT][pc].command;
        
        if(scommand == ALLOCATEGMEMV || scommand == ALLOCATEGMEMR)
            ret++;
            
        pc++;
    }
    while(scommand != 0xFFFF);
    
    return ret;
}

//movingblock mblock2; //mblock[4]?
//LinkClass   Link;

int resx,resy,scrx,scry;
bool sbig;                                                  // big screen
int screen_scale = 2; //default = 2 (640x480)
bool scanlines;                                             //do scanlines if sbig==1


// quest file data
zquestheader QHeader;
byte                quest_rules[QUESTRULES_SIZE];
byte                midi_flags[MIDIFLAGS_SIZE];
byte                music_flags[MUSICFLAGS_SIZE];
word                map_count;
MsgStr              *MsgStrings;
int					msg_strings_size;
DoorComboSet        *DoorComboSets;
dmap                *DMaps;
miscQdata           QMisc;
std::vector<mapscr> TheMaps;
zcmap               *ZCMaps;
byte                *quest_file;
dword               quest_map_pos[MAPSCRS*MAXMAPS2];

char     *quest_path=NULL;
gamedata *saves=NULL;

volatile int lastfps=0;
volatile int framecnt=0;

/**********************************/
/*********** Misc Data ************/
/**********************************/

const char startguy[8] = {-13,-13,-13,-14,-15,-18,-21,-40};
const char gambledat[12*6] =
{
    20,-10,-10, 20,-10,-10, 20,-40,-10, 20,-10,-40,
    50,-10,-10, 50,-10,-10, 50,-40,-10, 50,-10,-40,
    -10,20,-10, -10,20,-10, -40,20,-10, -10,20,-40,
    -10,50,-10, -10,50,-10, -40,50,-10, -10,50,-40,
    -10,-10,20, -10,-10,20, -10,-40,20, -40,-10,20,
    -10,-10,50, -10,-10,50, -10,-40,50, -40,-10,50
};
const byte stx[4][9] =
{
    { 48, 80, 80, 96,112,144,160,160,192},
    { 48, 80, 80, 96,128,144,160,160,192},
    { 80, 80,128,128,160,160,192,192,208},
    { 32, 48, 48, 80, 80,112,112,160,160}
};
const byte sty[4][9] =
{
    {112, 64,128, 96, 80, 96, 64,128,112},
    { 48, 32, 96, 64, 80, 64, 32, 96, 48},
    { 32,128, 64, 96, 64, 96, 48,112, 80},
    { 80, 48,112, 64, 96, 64, 96, 32,128}
};

const byte ten_rupies_x[10] = {120,112,128,96,112,128,144,112,128,120};
const byte ten_rupies_y[10] = {49,65,65,81,81,81,81,97,97,113};

zctune tunes[MAXMIDIS] =
{
    // (title)                              (s) (ls) (le) (l) (vol) (midi)(fmt)
    zctune((char *)"Zelda - Dungeon",     0,  -1,  -1,  1,  176,  NULL, 0),
    zctune((char *)"Zelda - Ending",      0, 129, 225,  1,  160,  NULL, 0),
    zctune((char *)"Zelda - Game Over",   0,  -1,  -1,  1,  224,  NULL, 0),
    zctune((char *)"Zelda - Level 9",     0,  -1,  -1,  1,  255,  NULL, 0),
    zctune((char *)"Zelda - Overworld",   0,  17,  -1,  1,  208,  NULL, 0),
    zctune((char *)"Zelda - Title",       0,  -1,  -1,  0,  168,  NULL, 0),
    zctune((char *)"Zelda - Triforce",    0,  -1,  -1,  0,  168,  NULL, 0)
};

// emusic enhancedMusic[MAXMUSIC];

FONT *setmsgfont()
{
    switch(MsgStrings[msgstr].font)
    {
    default:
        return zfont;
        
    case font_z3font:
        return z3font;
        
    case font_z3smallfont:
        return z3smallfont;
        
    case font_deffont:
        return deffont;
        
    case font_lfont:
        return lfont;
        
    case font_lfont_l:
        return lfont_l;
        
    case font_pfont:
        return pfont;
        
    case font_mfont:
        return mfont;
        
    case font_ztfont:
        return ztfont;
        
    case font_sfont:
        return sfont;
        
    case font_sfont2:
        return sfont2;
        
    case font_spfont:
        return spfont;
        
    case font_ssfont1:
        return ssfont1;
        
    case font_ssfont2:
        return ssfont2;
        
    case font_ssfont3:
        return ssfont3;
        
    case font_ssfont4:
        return ssfont4;
        
    case font_gblafont:
        return gblafont;
        
    case font_goronfont:
        return goronfont;
        
    case font_zoranfont:
        return zoranfont;
        
    case font_hylian1font:
        return hylian1font;
        
    case font_hylian2font:
        return hylian2font;
        
    case font_hylian3font:
        return hylian3font;
        
    case font_hylian4font:
        return hylian4font;
        
    case font_gboraclefont:
        return gboraclefont;
        
    case font_gboraclepfont:
        return gboraclepfont;
        
    case font_dsphantomfont:
        return dsphantomfont;
        
    case font_dsphantompfont:
        return dsphantompfont;
    }
}

void donewmsg(int str)
{
    //al_trace("donewmsg %d\n",str);
    if(msg_onscreen || msg_active)
        dismissmsg();
        
    linkedmsgclk=0;
    msg_active = true;
    // Don't set msg_onscreen - not onscreen just yet
    msgstr = str;
    msgorig = msgstr;
    msgfont = setmsgfont();
    msgcolour=QMisc.colors.msgtext;
    msgspeed=zinit.msg_speed;
    
    if(introclk==0 || (introclk>=72 && dmapmsgclk==0))
        clear_bitmap(msgdisplaybuf);
        
    clear_bitmap(msgdisplaybuf);
    set_clip_state(msgdisplaybuf, 1);
    clear_bitmap(msgbmpbuf);
    
    if(MsgStrings[msgstr].tile!=0)
    {
        frame2x2(msgbmpbuf,&QMisc,0,0,MsgStrings[msgstr].tile,MsgStrings[msgstr].cset,
                 (MsgStrings[msgstr].w>>3)+2,(MsgStrings[msgstr].h>>3)+2,0,true,0);
    }
    
    msgclk=msgpos=msgptr=0;
    msgspace=true;
    msg_w=MsgStrings[msgstr].w;
    msg_h=MsgStrings[msgstr].h;
    msg_xpos=MsgStrings[msgstr].x;
    msg_ypos=MsgStrings[msgstr].y;
    cursor_x=0;
    cursor_y=0;
}

// Called to make a message disappear
void dismissmsg()
{
    linkedmsgclk=0;
    msgstr = msgclk=msgpos=msgptr=0;
    cursor_x=0;
    cursor_y=0;
    msg_onscreen = msg_active = false;
    //Link.finishedmsg(); //Not possible?
    clear_bitmap(msgdisplaybuf);
    set_clip_state(msgdisplaybuf, 1);
}

void dointro()
{
    if(game->visited[currdmap]!=1 || (DMaps[currdmap].flags&dmfALWAYSMSG)!=0)
    {
        dmapmsgclk=0;
        game->visited[currdmap]=1;
        introclk=intropos=0;
    }
    else
    {
        introclk=intropos=72;
    }
}

bool bad_version(int version)
{
    // minimum zquest version allowed for any quest file
    if(version < 0x183)
        return true;
        
    return false;
}

extern char *weapon_string[];
extern char *item_string[];
extern char *sfx_string[];
extern char *guy_string[];


/**********************************/
/******* Other Source Files *******/
/**********************************/

void hit_close_button()
{
    close_button_quit=true;
    return;
}

void Z_eventlog(const char *format,...)
{
    if(get_bit(quest_rules,qr_LOG))
    {
        char buf[2048];
        
        va_list ap;
        va_start(ap, format);
        vsprintf(buf, format, ap);
        va_end(ap);
        al_trace("%s",buf);
    }
}

void Z_scripterrlog(const char * const format,...)
{
    if(get_bit(quest_rules,qr_SCRIPTERRLOG))
    {
        char buf[2048];
        
        va_list ap;
        va_start(ap, format);
        vsprintf(buf, format, ap);
        va_end(ap);
        al_trace("%s",buf);
    }
}


bool blockmoving;
#include "sprite.h"
movingblock mblock2;                                        //mblock[4]?

sprite_list  guys, items, Ewpns, Lwpns, Sitems, chainlinks, decorations, particles;

#include "zc_custom.h"
#include "link.h"
LinkClass   Link;

#include "maps.h"
#include "subscr.h"
#include "guys.h"

#include "title.h"
#include "ending.h"

#include "zc_sys.h"

// Wait... this is only used by ffscript.cpp!?
void addLwpn(int x,int y,int z,int id,int type,int power,int dir, int parentid)
{
    Lwpns.add(new weapon((fix)x,(fix)y,(fix)z,id,type,power,dir,-1,parentid));
}

void ALLOFF(bool messagesToo, bool decorationsToo)
{
    if(messagesToo)
    {
        clear_bitmap(msgdisplaybuf);
        set_clip_state(msgdisplaybuf, 1);
    }
    
    clear_bitmap(pricesdisplaybuf);
    set_clip_state(pricesdisplaybuf, 1);
    
    if(items.idCount(iPile))
    {
        loadlvlpal(DMaps[currdmap].color);
    }
    
    items.clear();
    guys.clear();
    Lwpns.clear();
    Ewpns.clear();
    chainlinks.clear();
    clearScriptHelperData();
    
    if(decorationsToo)
        decorations.clear();
        
    particles.clear();
    
    if(Link.getNayrusLoveShieldClk())
    {
        Link.setNayrusLoveShieldClk(Link.getNayrusLoveShieldClk());
    }
    
    Link.resetflags(false);
    Link.reset_hookshot();
    linkedmsgclk=0;
    add_asparkle=0;
    add_bsparkle=0;
    add_df1asparkle=false;
    add_df1bsparkle=false;
    add_nl1asparkle=false;
    add_nl1bsparkle=false;
    add_nl2asparkle=false;
    add_nl2bsparkle=false;
    //  for(int i=0; i<1; i++)
    mblock2.clk=0;
    dismissmsg();
    fadeclk=-1;
    introclk=intropos=72;
    
    lensclk = 0;
    lensid=-1;
    drawguys=Udown=Ddown=Ldown=Rdown=Adown=Bdown=Sdown=true;
    
    if(watch && !cheat_superman)
    {
        Link.setClock(false);
    }
    
    //  if(watch)
    //    Link.setClock(false);
    watch=freeze_guys=loaded_guys=loaded_enemies=blockpath=false;
    stop_sfx(WAV_BRANG);
    
    for(int i=0; i<176; i++)
        guygrid[i]=0;
        
    sle_clk=0;
    blockmoving=false;
    fairy_cnt=0;
    
    if(usebombpal)
    {
        memcpy(RAMpal, tempbombpal, PAL_SIZE*sizeof(RGB));
        refreshpal=true;
        usebombpal=false;
    }
}
void centerLink()
{
    Link.setX(120);
    Link.setY(80);
}
fix  LinkX()
{
    return Link.getX();
}
fix  LinkY()
{
    return Link.getY();
}
fix  LinkZ()
{
    return Link.getZ();
}
int  LinkHClk()
{
    return Link.getHClk();
}
int  LinkAction()
{
    return Link.getAction();
}
int  LinkCharged()
{
    return Link.isCharged();
}
int  LinkNayrusLoveShieldClk()
{
    return Link.getNayrusLoveShieldClk();
}
int  LinkHoverClk()
{
    return Link.getHoverClk();
}
int  LinkSwordClk()
{
    return Link.getSwordClk();
}
int  LinkItemClk()
{
    return Link.getItemClk();
}
void setSwordClk(int newclk)
{
    Link.setSwordClk(newclk);
}
void setItemClk(int newclk)
{
    Link.setItemClk(newclk);
}
int  LinkLStep()
{
    return Link.getLStep();
}
void LinkCheckItems()
{
    Link.checkitems();
}
bool LinkGetDontDraw()
{
    return Link.getDontDraw();
}
fix  GuyX(int j)
{
    return guys.getX(j);
}
fix  GuyY(int j)
{
    return guys.getY(j);
}
int  GuyID(int j)
{
    return guys.getID(j);
}
int  GuyMisc(int j)
{
    return guys.getMisc(j);
}
bool  GuySuperman(int j)
{
    if((j>=guys.Count())||(j<0))
    {
        return true;
    }
    
    return ((enemy*)guys.spr(j))->superman !=0;
}

int  GuyCount()
{
    return guys.Count();
}
void StunGuy(int j,int stun)
{
    if(stun<=0) return;
    
    if(((enemy*)guys.spr(j))->z==0 && canfall(((enemy*)guys.spr(j))->id))
    {
        ((enemy*)guys.spr(j))->stunclk=zc_min(360,stun*4);
        ((enemy*)guys.spr(j))->fall=-zc_min(FEATHERJUMP,(stun*8)+rand()%5);
    }
}

fix LinkModifiedX()
{
    return Link.getModifiedX();
}
fix LinkModifiedY()
{
    return Link.getModifiedY();
}
int LinkDir()
{
    return Link.getDir();
}
void add_grenade(int wx, int wy, int wz, int size, int parentid)
{
    if(size)
    {
        Lwpns.add(new weapon((fix)wx,(fix)wy,(fix)wz,wSBomb,0,16*DAMAGE_MULTIPLIER,LinkDir(),
                             -1, parentid));
        Lwpns.spr(Lwpns.Count()-1)->id=wSBomb;
    }
    else
    {
        Lwpns.add(new weapon((fix)wx,(fix)wy,(fix)wz,wBomb,0,4*DAMAGE_MULTIPLIER,LinkDir(),
                             -1, parentid));
        Lwpns.spr(Lwpns.Count()-1)->id=wBomb;
    }
    
    Lwpns.spr(Lwpns.Count()-1)->clk=48;
    Lwpns.spr(Lwpns.Count()-1)->misc=50;
}

fix distance(int x1, int y1, int x2, int y2)

{
    return (fix)sqrt(pow((double)abs(x1-x2),2)+pow((double)abs(y1-y2),2));
}

bool getClock()
{
    return Link.getClock();
}
void setClock(bool state)
{
    Link.setClock(state);
}
void CatchBrang()
{
    Link.Catch();
}

/**************************/
/***** Main Game Code *****/
/**************************/

int load_quest(gamedata *g)
{
    //setPackfilePassword(datapwd);
    int ret = loadquest(quest_path,&QHeader,&QMisc,tunes+ZC_MIDI_COUNT);
    //setPackfilePassword(NULL);
    
    if(!g->title[0] || g->get_hasplayed() == 0)
    {
        strcpy(g->version,QHeader.version);
        strcpy(g->title,QHeader.title);
    }
    else
    {
        if(!ret && strcmp(g->title,QHeader.title))
        {
            ret = qe_match;
        }
    }
    
    if(QHeader.minver[0])
    {
        if(strcmp(g->version,QHeader.minver) < 0)
            ret = qe_minver;
    }
    
    if(ret)
    {
        char buf1[80];
        Z_error(buf1,"Error loading %s:, %s",get_filename(quest_path), qst_error[ret]);
    }
    
    return ret;
}

void init_dmap()
{
    // readjust disabled items; could also do dmap-specific scripts here
    for(int i=0; i<MAXITEMS; i++)
    {
        game->items_off[i] &= (~1); // disable last bit - this one is set by dmap
        game->items_off[i] |= DMaps[currdmap].disableditems[i]; // and reset if required
    }
    
    flushItemCache();
    
    return;
}

// NOTE: This method has been severely hacked to fix an annoying problem at game start:
// items (ie the Small Wallet) which modify max counter values need to be processed after
// the values for those counters specified in init data, as the author expects these items
// to modify the max counter. BUT the counter value should NOT be updated, ie, starting with
// the bomb item does not give 8 free bombs at quest start.
// I don't like this solution one bit, but can't come up with anything better -DD

void resetItems(gamedata *game2, zinitdata *zinit2, bool lvlitems)
{
    game2->set_maxlife(zinit2->hc*HP_PER_HEART);
    game2->set_maxbombs(zinit2->max_bombs);
    game2->set_maxcounter(zinit2->max_bombs/zc_max(1,zinit2->bomb_ratio), 6);
    game2->set_maxmagic(zinit2->max_magic);
    game2->set_maxarrows(zinit2->max_arrows);
    game2->set_maxcounter(zinit2->max_rupees, 1);
    game2->set_maxcounter(zinit2->max_keys, 5);
    
    //set up the items
    for(int i=0; i<MAXITEMS; i++)
    {
        if(zinit2->items[i] && (itemsbuf[i].flags & ITEM_GAMEDATA))
        {
            if(!game2->get_item(i))
                getitem(i,true);
        }
        else
            game2->set_item_no_flush(i,false);
            
        game2->items_off[i] = 0;
        
        // Fix them DMap items
        // Since resetItems() gets called before AND after init_dmap()...
        if(get_currdmap() > -1)
            game2->items_off[i] |= DMaps[get_currdmap()].disableditems[i];
    }
    
    flushItemCache();
    
    //Then set up the counters
    game2->set_bombs(zinit2->bombs);
    
    if(zinit2->bombs > 0 && zinit2->max_bombs > 0) game2->set_item(iBombs, true);
    
    game2->set_keys(zinit2->keys);
    game2->set_sbombs(zinit2->super_bombs);
    
    if(zinit2->super_bombs > 0 && (zinit2->max_bombs/zc_max(1,zinit2->bomb_ratio)) > 0) game2->set_item(iSBomb, true);
    
    game2->set_HCpieces(zinit2->hcp);
    game2->set_rupies(zinit2->rupies);
    game2->set_hcp_per_hc(zinit2->hcp_per_hc);
    game2->set_cont_hearts(zinit2->cont_heart);
    game2->set_cont_percent(get_bit(zinit2->misc, idM_CONTPERCENT) != 0);
    
    for(int i=0; i<MAXLEVELS; i++)
    {
        // Kludge to prevent two bits (liTRIFORCE and liBOSS) which aren't
        // completely stored in Init Data, from being erased when this is run in-game.
        if(lvlitems)
            game2->lvlitems[i]=0;
        else
            game2->lvlitems[i]&=~(liMAP|liCOMPASS|liBOSSKEY| (i>0 && i<=8 ? liTRIFORCE : 0));
            
        game2->lvlitems[i]|=get_bit(zinit2->map,i)?liMAP:0;
        game2->lvlitems[i]|=get_bit(zinit2->compass,i)?liCOMPASS:0;
        game2->lvlitems[i]|=get_bit(zinit2->boss_key,i)?liBOSSKEY:0;
        game2->lvlkeys[i]=zinit2->level_keys[i];
    }
    
    for(int i=0; i<8; i++)
    {
        game2->lvlitems[i+1]|=get_bit(&zinit2->triforce,i)?liTRIFORCE:0;
    }
    
    game2->set_magic(zc_min(zinit2->magic,zinit2->max_magic));
    game2->set_magicdrainrate(get_bit(zinit2->misc,idM_DOUBLEMAGIC)?1:2);
    game2->set_canslash(get_bit(zinit2->misc,idM_CANSLASH)?1:0);
    
    game2->set_arrows(zinit2->arrows);
    
    //flush the cache again (in case bombs became illegal to use by setting bombs to 0)
    flushItemCache();
}

int init_game()
{
    srand(time(0));
    //introclk=intropos=msgclk=msgpos=dmapmsgclk=0;
    
//Some initialising globals
    didpit=false;
    Link.unfreeze();
    Link.reset_hookshot();
    Link.reset_ladder();
    linkedmsgclk=0;
    blockmoving=false;
    add_asparkle=0;
    add_bsparkle=0;
    add_df1asparkle=false;
    add_df1bsparkle=false;
    add_nl1asparkle=false;
    add_nl1bsparkle=false;
    add_nl2asparkle=false;
    add_nl2bsparkle=false;
    
    wavy=quakeclk=0;
    cheat_superman=false;
    
    for(int x = 0; x < MAXITEMS; x++)
    {
        lens_hint_item[x][0]=0;
        lens_hint_item[x][1]=0;
    }
    
    for(int x = 0; x < MAXWPNS; x++)
    {
        lens_hint_weapon[x][0]=0;
        lens_hint_weapon[x][1]=0;
    }
    
    
//Confuse the cheaters by moving the game data to a random location
    if(game != NULL)
        delete game;
        
    char *dummy = (char *) malloc((rand()%(RAND_MAX/2))+32);
    game = new gamedata;
    game->Clear();
    
    free(dummy);
    
//Copy saved data to RAM data (but not global arrays)
    game->Copy(saves[currgame]);
    flushItemCache();
    
//Load the quest
    //setPackfilePassword(datapwd);
    int ret = load_quest(game);
    
    if(ret != qe_OK)
    {
        Quit = qERROR;
        //setPackfilePassword(NULL);
        return 1;
    }
    
    //setPackfilePassword(NULL);
    
    bool firstplay = (game->get_hasplayed() == 0);
    
    BSZ = get_bit(quest_rules,qr_BSZELDA)!=0;
    //setuplinktiles(zinit.linkanimationstyle);
    
    COOLSCROLL = (get_bit(quest_rules,qr_COOLSCROLL)!=0 ? 1 : 0) |
                 (get_bit(quest_rules,qr_OVALWIPE)!=0 ? 2 : 0) |
                 (get_bit(quest_rules,qr_TRIANGLEWIPE)!=0 ? 4 : 0) |
                 (get_bit(quest_rules,qr_SMASWIPE)!=0 ? 8 : 0);
    identifyCFEnemies();
                 
    //  NEWSUBSCR = get_bit(quest_rules,qr_NEWSUBSCR);
    
    //  homescr = currscr = DMaps[0].cont;
    //  currdmap = warpscr = worldscr=0;
    if(firstplay)
    {
        game->set_continue_dmap(zinit.start_dmap);
        resetItems(game,&zinit,true);
    }
    
    currdmap = warpscr = worldscr=game->get_continue_dmap();
    init_dmap();
    
    if(game->get_continue_scrn() >= 0x80)
    {
        //if ((DMaps[currdmap].type&dmfTYPE)==dmOVERW || QHeader.zelda_version <= 0x190)
        if((DMaps[currdmap].type&dmfTYPE)==dmOVERW)
        {
            homescr = currscr = DMaps[currdmap].cont;
        }
        else
        {
            homescr = currscr = DMaps[currdmap].cont + DMaps[currdmap].xoff;
        }
    }
    else
    {
        homescr = currscr = game->get_continue_scrn();
    }
    
    lastentrance = currscr;
    game->set_continue_scrn(lastentrance);
    lastentrance_dmap = currdmap;
    currmap = DMaps[currdmap].map;
    dlevel = DMaps[currdmap].level;
    sle_x=sle_y=newscr_clk=opendoors=Bwpn=Bpos=0;
    fadeclk=-1;
    
    if(DMaps[currdmap].flags&dmfVIEWMAP)
    {
        game->maps[(currmap*MAPSCRSNORMAL)+currscr] |= mVISITED;              // mark as visited
    }
    
    for(int i=0; i<6; i++)
    {
        visited[i]=-1;
    }
    
    game->lvlitems[9] &= ~liBOSS;
    
    ALLOFF();
    whistleclk=-1;
    clockclk=0;
    currcset=DMaps[currdmap].color;
    darkroom=naturaldark=false;
    
    tmpscr[0].zero_memory();
    tmpscr[1].zero_memory();
    
    //Script-related nonsense
    script_drawing_commands.Clear();
    
    initZScriptArrayRAM(firstplay);
    initZScriptGlobalRAM();
    global_wait=false;
    
    //loadscr(0,currscr,up);
    loadscr(0,currdmap,currscr,-1,false);
    putscr(scrollbuf,0,0,&tmpscr[0]);
    putscrdoors(scrollbuf,0,0,&tmpscr[0]);
    
    //preloaded freeform combos
    //ffscript_engine(true); Can't do this here! Global arrays haven't been allocated yet... ~Joe
    
    Link.init();
    Link.resetflags(true);
    Link.setEntryPoints(LinkX(),LinkY());
    
    copy_pal((RGB*)data[PAL_GUI].dat,RAMpal);
    loadfullpal();
    ringcolor(false);
    loadlvlpal(DMaps[currdmap].color);
    lighting(false,true);
    wavy=quakeclk=0;
    
    if(firstplay)
    {
        game->set_life(zinit.start_heart*HP_PER_HEART);
    }
    else
    {
        if(game->get_cont_percent())
        {
            if(game->get_maxlife()%HP_PER_HEART==0)
                game->set_life(((game->get_maxlife()*game->get_cont_hearts()/100)/HP_PER_HEART)*HP_PER_HEART);
            else
                game->set_life(game->get_maxlife()*game->get_cont_hearts()/100);
        }
        else
        {
            game->set_life(game->get_cont_hearts()*HP_PER_HEART);
        }
    }
    
    if(firstplay)
        game->set_hasplayed(1);
        
    update_subscreens();
    
    load_Sitems(&QMisc);
    
//load the previous weapons -DD
    bool usesaved = (game->get_quest() == 0xFF); //What was wrong with firstplay?
    int apos = 0;
    int bpos = 0;
    
    if(!get_bit(quest_rules,qr_SELECTAWPN))
    {
        Awpn = selectSword();
        apos = -1;
        bpos = selectWpn_new(SEL_VERIFY_RIGHT, usesaved ? game->bwpn : 0xFF, -1);
    }
    else
    {
        apos = selectWpn_new(SEL_VERIFY_RIGHT, usesaved ? game->awpn : 0xFF);
        bpos = selectWpn_new(SEL_VERIFY_RIGHT, usesaved ? game->bwpn : 0xFF, apos);
        
        if(bpos==0xFF)
        {
            bpos=apos;
            apos=0;
        }
        
        Awpn = Bweapon(apos);
        directItemA = directItem;
    }
    
    game->awpn = apos;
    game->bwpn = bpos;
    Bwpn = Bweapon(bpos);
    directItemB = directItem;
    update_subscr_items();
    
    reset_subscr_items();
    
    Link.setDontDraw(false);
    show_subscreen_dmap_dots=true;
    show_subscreen_items=true;
    show_subscreen_numbers=true;
    show_subscreen_life=true;
    
    Playing=true;
    
    map_bkgsfx(true);
    openscreen();
    show_subscreen_numbers=true;
    show_subscreen_life=true;
    dointro();
    loadguys();
    
    activated_timed_warp=false;
    newscr_clk = frame;
    
    if(isdungeon() && currdmap>0) // currdmap>0 is weird, but at least one quest (Mario's Insane Rampage) depends on it
    {
        Link.stepforward(get_bit(quest_rules,qr_LTTPWALK) ? 11: 12, false);
    }
    
    if(!Quit)
        playLevelMusic();
        
    if(firstplay)
    {
        memset(game->screen_d, 0, MAXDMAPS * 64 * 8 * sizeof(long32));
        ZScriptVersion::RunScript(SCRIPT_GLOBAL, GLOBAL_SCRIPT_INIT);
    }
    else
    {
        ZScriptVersion::RunScript(SCRIPT_GLOBAL, GLOBAL_SCRIPT_CONTINUE); //Do this after global arrays have been loaded
    }
    
    initZScriptGlobalRAM(); //Call again so we're set up for GLOBAL_SCRIPT_GAME
    ffscript_engine(true);  //Here is a much safer place...
    
    return 0;
}

int cont_game()
{
    //  introclk=intropos=msgclk=msgpos=dmapmsgclk=0;
    didpit=false;
    Link.unfreeze();
    Link.reset_hookshot();
    Link.reset_ladder();
    linkedmsgclk=0;
    blockmoving=0;
    add_asparkle=0;
    add_bsparkle=0;
    add_df1asparkle=false;
    add_df1bsparkle=false;
    add_nl1asparkle=false;
    add_nl1bsparkle=false;
    add_nl2asparkle=false;
    add_nl2bsparkle=false;
    /*
      if(DMaps[currdmap].cont >= 0x80)
      {
      homescr = currscr = DMaps[0].cont;
      currdmap = warpscr = worldscr=0;
      currmap = DMaps[0].map;
      dlevel = DMaps[0].level;
      }
      */
    currdmap = lastentrance_dmap;
    homescr = currscr = lastentrance;
    currmap = DMaps[currdmap].map;
    dlevel = DMaps[currdmap].level;
    init_dmap();
    
    for(int i=0; i<6; i++)
    {
        visited[i]=-1;
    }
    
    if(dlevel==0)
    {
        game->lvlitems[9]&=~liBOSS;
    }
    
    ALLOFF();
    whistleclk=-1;
    currcset=DMaps[currdmap].color;
    darkroom=naturaldark=false;
    tmpscr[0].zero_memory();
    tmpscr[1].zero_memory();
    
//loadscr(0,currscr,up);
    loadscr(0,currdmap,currscr,-1,false);
    putscr(scrollbuf,0,0,&tmpscr[0]);
    putscrdoors(scrollbuf,0,0,&tmpscr[0]);
    script_drawing_commands.Clear();
    
    //preloaded freeform combos
    ffscript_engine(true);
    
    loadfullpal();
    ringcolor(false);
    loadlvlpal(DMaps[currdmap].color);
    lighting(false,true);
    Link.init();
    wavy=quakeclk=0;
    
    //if(get_bit(zinit.misc,idM_CONTPERCENT))
    if(game->get_cont_percent())
    {
        if(game->get_maxlife()%HP_PER_HEART==0)
            game->set_life(((game->get_maxlife()*game->get_cont_hearts()/100)/HP_PER_HEART)*HP_PER_HEART);
        else
            game->set_life(game->get_maxlife()*game->get_cont_hearts()/100);
    }
    else
    {
        game->set_life(game->get_cont_hearts()*HP_PER_HEART);
    }
        
    /*
      else
      game->life=3*HP_PER_HEART;
      */
    
    //  for(int i=0; i<128; i++)
    //    key[i]=0;
    
    update_subscreens();
    Playing=true;
    map_bkgsfx(true);
    openscreen();
    show_subscreen_numbers=true;
    show_subscreen_life=true;
    dointro();
    loadguys();
    
    if(!Quit)
    {
        //play_DmapMusic();
        playLevelMusic();
        
        if(isdungeon())
            Link.stepforward(get_bit(quest_rules,qr_LTTPWALK)?11:12, false);
            
        newscr_clk=frame;
        activated_timed_warp=false;
    }
    
    initZScriptGlobalRAM();
    return 0;
}

void restart_level()
{
    blackscr(16,true);
    map_bkgsfx(false);
    
    if(dlevel)
    {
        currdmap = lastentrance_dmap;
        homescr = currscr = lastentrance;
        init_dmap();
    }
    else
    {
        if((DMaps[currdmap].type&dmfTYPE)==dmOVERW)
        {
            homescr = currscr = DMaps[currdmap].cont;
        }
        else
        {
            homescr = currscr = DMaps[currdmap].cont + DMaps[currdmap].xoff;
        }
    }
    
    currmap = DMaps[currdmap].map;
    dlevel = DMaps[currdmap].level;
    
    for(int i=0; i<6; i++)
        visited[i]=-1;
        
    ALLOFF();
    whistleclk=-1;
    darkroom=naturaldark=false;
    tmpscr[0].zero_memory();
    tmpscr[1].zero_memory();
    
    loadscr(0,currdmap,currscr,-1,false);
    putscr(scrollbuf,0,0,&tmpscr[0]);
    putscrdoors(scrollbuf,0,0,&tmpscr[0]);
    //preloaded freeform combos
    ffscript_engine(true);
    
    loadfullpal();
    ringcolor(false);
    loadlvlpal(DMaps[currdmap].color);
    lighting(false,true);
    Link.init();
    currcset=DMaps[currdmap].color;
    openscreen();
    map_bkgsfx(true);
    Link.setEntryPoints(LinkX(),LinkY());
    show_subscreen_numbers=true;
    show_subscreen_life=true;
    loadguys();
    
    if(!Quit)
    {
        //play_DmapMusic();
        playLevelMusic();
        
        if(isdungeon())
            Link.stepforward(get_bit(quest_rules,qr_LTTPWALK)?11:12, false);
            
        newscr_clk=frame;
        activated_timed_warp=false;
    }
}


void putintro()
{
    if(!stricmp("                                                                        ", DMaps[currdmap].intro))
    {
        introclk=intropos=72;
        return;
    }
    
    if((cBbtn())&&(get_bit(quest_rules,qr_ALLOWMSGBYPASS)))
    {
        //finish writing out the string
        for(; intropos<72; ++intropos)
        {
            textprintf_ex(msgdisplaybuf,zfont,((intropos%24)<<3)+32,((intropos/24)<<3)+40,QMisc.colors.msgtext,-1,
                          "%c",DMaps[currdmap].intro[intropos]);
        }
    }
    
    if(intropos>=72)
    {
        if(dmapmsgclk >= 51)
            dmapmsgclk=50;
            
        return;
    }
    
    if(((introclk++)%6<5)&&((!cAbtn())||(!get_bit(quest_rules,qr_ALLOWFASTMSG))))
        return;
        
    dmapmsgclk=51;
    
    if(intropos == 0)
    {
        while(DMaps[currdmap].intro[intropos]==' ')
            ++intropos;
    }
    
    sfx(WAV_MSG);
    
    
    //using the clip value to indicate the bitmap is "dirty"
    //rather than add yet another global variable
    set_clip_state(msgdisplaybuf, 0);
    textprintf_ex(msgdisplaybuf,zfont,((intropos%24)<<3)+32,((intropos/24)<<3)+40,QMisc.colors.msgtext,-1,
                  "%c",DMaps[currdmap].intro[intropos]);
                  
    ++intropos;
    
    if(DMaps[currdmap].intro[intropos]==' ' && DMaps[currdmap].intro[intropos+1]==' ')
        while(DMaps[currdmap].intro[intropos]==' ')
            ++intropos;
}

//static char *dirstr[4] = {"Up","Down","Left","Right"};
//static char *dirstr[32] = {"U","D","L","R"," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "};

void show_ffscript_names()
{
    int ypos = 8;
    
    for(int i=0; i< MAXFFCS; i++)
    {
        if(tmpscr->ffscript[i])
        {
            textout_shadowed_ex(framebuf,font, ffcmap[tmpscr->ffscript[i]-1].second.c_str(),2,ypos,WHITE,BLACK,-1);
            ypos+=12;
        }
    }
}

void do_magic_casting()
{
    static int tempx, tempy;
    static byte linktilebuf[256];
    int ltile=0;
    int lflip=0;
    bool shieldModify=true;
    
    if(magicitem==-1)
    {
        return;
    }
    
    switch(itemsbuf[magicitem].family)
    {
    case itype_dinsfire:
    {
        if(magiccastclk==0)
        {
            Lwpns.add(new weapon(LinkX(),LinkY(),LinkZ(),wPhantom,pDINSFIREROCKET,0,up, magicitem, Link.getUID()));
            weapon *w1 = (weapon*)(Lwpns.spr(Lwpns.Count()-1));
            w1->step=4;
            //          Link.tile=(BSZ)?32:29;
            linktile(&Link.tile, &Link.flip, &Link.extend, ls_landhold2, Link.getDir(), zinit.linkanimationstyle);
            
            if(get_bit(quest_rules,qr_EXPANDEDLTM))
            {
                Link.tile+=item_tile_mod(shieldModify);
            }
            
            casty=Link.getY();
        }
        
        if(magiccastclk==64)
        {
            Lwpns.add(new weapon((fix)LinkX(),(fix)(-32),(fix)LinkZ(),wPhantom,pDINSFIREROCKETRETURN,0,down, magicitem, Link.getUID()));
            weapon *w1 = (weapon*)(Lwpns.spr(Lwpns.Count()-1));
            w1->step=4;
            //          Link.tile=29;
            linktile(&Link.tile, &Link.flip, &Link.extend, ls_landhold2, Link.getDir(), zinit.linkanimationstyle);
            
            if(get_bit(quest_rules,qr_EXPANDEDLTM))
            {
                Link.tile+=item_tile_mod(shieldModify);
            }
            
            castnext=false;
        }
        
        if(castnext)
        {
            linktile(&Link.tile, &Link.flip, &Link.extend, ls_cast, Link.getDir(), zinit.linkanimationstyle);
            
            if(get_bit(quest_rules,qr_EXPANDEDLTM))
            {
                Link.tile+=item_tile_mod(shieldModify);
            }
            
            if(get_bit(quest_rules,qr_MORESOUNDS))
                sfx(itemsbuf[magicitem].usesound,pan(int(Link.getX())));
                
            int flamemax=itemsbuf[magicitem].misc1;
            
            for(int flamecounter=((-1)*(flamemax/2))+1; flamecounter<=((flamemax/2)+1); flamecounter++)
            {
                Lwpns.add(new weapon((fix)LinkX(),(fix)LinkY(),(fix)LinkZ(),wFire,3,itemsbuf[magicitem].power*DAMAGE_MULTIPLIER,
                                     (tmpscr->flags7&fSIDEVIEW) ? (flamecounter<flamemax ? left : right) : 0, magicitem, Link.getUID()));
                weapon *w = (weapon*)(Lwpns.spr(Lwpns.Count()-1));
                w->step=(itemsbuf[magicitem].misc2/100.0);
                w->angular=true;
                w->angle=(flamecounter*PI/(flamemax/2.0));
            }
            
            castnext=false;
            magiccastclk=128;
        }
        
        if((magiccastclk++)>=226)
        {
            magicitem=-1;
            magiccastclk=0;
        }
    }
    break;
    
    case itype_faroreswind:
    {
        if(magiccastclk==0)
        {
            linktile(&ltile, &lflip, ls_stab, down, zinit.linkanimationstyle);
            
            if(get_bit(quest_rules,qr_EXPANDEDLTM))
            {
                ltile+=item_tile_mod(shieldModify);
            }
            
            unpack_tile(newtilebuf, ltile, lflip, true);
            memcpy(linktilebuf, unpackbuf, 256);
            tempx=Link.getX();
            tempy=Link.getY();
            linktile(&Link.tile, &Link.flip, &Link.extend, ls_pound, down, zinit.linkanimationstyle);
            
            if(get_bit(quest_rules,qr_EXPANDEDLTM))
            {
                Link.tile+=item_tile_mod(shieldModify);
            }
        }
        
        if(magiccastclk>=0&&magiccastclk<64)
        {
            Link.setX(tempx+((rand()%3)-1));
            Link.setY(tempy+((rand()%3)-1));
        }
        
        if(magiccastclk==64)
        {
            Link.setX(tempx);
            Link.setY(tempy);
            linktile(&Link.tile, &Link.flip, &Link.extend, ls_stab, down, zinit.linkanimationstyle);
            
            if(get_bit(quest_rules,qr_EXPANDEDLTM))
            {
                Link.tile+=item_tile_mod(shieldModify);
            }
        }
        
        if(magiccastclk==96)
        {
            if(get_bit(quest_rules,qr_MORESOUNDS))
                sfx(itemsbuf[magicitem].usesound,pan(int(Link.getX())));
                
            Link.setDontDraw(true);
            
            for(int i=0; i<16; ++i)
            {
                for(int j=0; j<16; ++j)
                {
                    if(linktilebuf[i*16+j])
                    {
                        if(itemsbuf[magicitem].misc1==1)  // Twilight
                        {
                            particles.add(new pTwilight(Link.getX()+j, Link.getY()-Link.getZ()+i, 5, 0, 0, (rand()%8)+i*4));
                            int k=particles.Count()-1;
                            particle *p = (particle*)(particles.spr(k));
                            p->step=3;
                        }
                        else if(itemsbuf[magicitem].misc1==2)  // Sands of Hours
                        {
                            particles.add(new pTwilight(Link.getX()+j, Link.getY()-Link.getZ()+i, 5, 1, 2, (rand()%16)+i*2));
                            int k=particles.Count()-1;
                            particle *p = (particle*)(particles.spr(k));
                            p->step=4;
                            
                            if(rand()%10 < 2)
                            {
                                p->color=1;
                                p->cset=0;
                            }
                        }
                        else
                        {
                            particles.add(new pFaroresWindDust(Link.getX()+j, Link.getY()-Link.getZ()+i, 5, 6, linktilebuf[i*16+j], rand()%96));
                            
                            int k=particles.Count()-1;
                            particle *p = (particle*)(particles.spr(k));
                            p->angular=true;
                            p->angle=rand();
                            p->step=(((double)j)/8);
                            p->yofs=Link.getYOfs();
                        }
                    }
                }
            }
        }
        
        if((magiccastclk++)>=226)
        {
            //attackclk=0;
            int nayrutemp=nayruitem;
            restart_level();
            nayruitem=nayrutemp;
            //xofs=0;
            //action=none;
            magicitem=-1;
            magiccastclk=0;
            Link.setDontDraw(false);
        }
    }
    break;
    
    case itype_nayruslove:
    {
        // See also Link.cpp, LinkClass::checkhit().
        if(magiccastclk==0)
        {
            Lwpns.add(new weapon(LinkX(),LinkY(),(fix)0,wPhantom,pNAYRUSLOVEROCKET1,0,left, magicitem, Link.getUID()));
            weapon *w1 = (weapon*)(Lwpns.spr(Lwpns.Count()-1));
            w1->step=4;
            Lwpns.add(new weapon(LinkX(),LinkY(),(fix)0,wPhantom,pNAYRUSLOVEROCKET2,0,right, magicitem, Link.getUID()));
            w1 = (weapon*)(Lwpns.spr(Lwpns.Count()-1));
            w1->step=4;
            //          Link.tile=(BSZ)?32:29;
            linktile(&Link.tile, &Link.flip, &Link.extend, ls_cast, Link.getDir(), zinit.linkanimationstyle);
            
            if(get_bit(quest_rules,qr_EXPANDEDLTM))
            {
                Link.tile+=item_tile_mod(shieldModify);
            }
            
            castx=Link.getX();
        }
        
        if(magiccastclk==64)
        {
            int d=zc_max(LinkX(),256-LinkX())+32;
            Lwpns.add(new weapon((fix)(LinkX()-d),(fix)LinkY(),(fix)LinkZ(),wPhantom,pNAYRUSLOVEROCKETRETURN1,0,right, magicitem,Link.getUID()));
            weapon *w1 = (weapon*)(Lwpns.spr(Lwpns.Count()-1));
            w1->step=4;
            Lwpns.add(new weapon((fix)(LinkX()+d),(fix)LinkY(),(fix)LinkZ(),wPhantom,pNAYRUSLOVEROCKETRETURN2,0,left, magicitem,Link.getUID()));
            w1 = (weapon*)(Lwpns.spr(Lwpns.Count()-1));
            w1->step=4;
            //          Link.tile=29;
            linktile(&Link.tile, &Link.flip, &Link.extend, ls_cast, Link.getDir(), zinit.linkanimationstyle);
            
            if(get_bit(quest_rules,qr_EXPANDEDLTM))
            {
                Link.tile+=item_tile_mod(shieldModify);
            }
            
            castnext=false;
        }
        
        if(castnext)
        {
            //          Link.tile=4;
            linktile(&Link.tile, &Link.flip, &Link.extend, ls_landhold2, Link.getDir(), zinit.linkanimationstyle);
            
            if(get_bit(quest_rules,qr_EXPANDEDLTM))
            {
                Link.tile+=item_tile_mod(shieldModify);
            }
            
            Link.setNayrusLoveShieldClk(itemsbuf[magicitem].misc1);
            
            if(get_bit(quest_rules,qr_MORESOUNDS))
            {
                if(nayruitem != -1)
                {
                    stop_sfx(itemsbuf[nayruitem].usesound+1);
                    stop_sfx(itemsbuf[nayruitem].usesound);
                }
                
                cont_sfx(itemsbuf[magicitem].usesound);
            }
            
            castnext=false;
            magiccastclk=128;
            nayruitem = magicitem;
        }
        
        // Finish the final spell pose
        if((magiccastclk++)>=160)
        {
            magicitem=-1;
            magiccastclk=0;
        }
    }
    break;
    
    default:
        magiccastclk=0;
        break;
    }
}

void update_hookshot()
{
    int hs_x, hs_y, hs_z,hs_dx, hs_dy;
    bool check_hs=false;
    int dist_bx, dist_by, hs_w;
    chainlinks.animate();
    //  char tempbuf[80];
    //  char tempbuf2[80];
    
    //find out where the head is and make it
    //easy to reference
    if(Lwpns.idFirst(wHookshot)>-1)
    {
        check_hs=true;
    }
    
    if(check_hs)
    {
        int parentitem = ((weapon*)Lwpns.spr(Lwpns.idFirst(wHookshot)))->parentitem;
        hs_x=Lwpns.spr(Lwpns.idFirst(wHookshot))->x;
        hs_y=Lwpns.spr(Lwpns.idFirst(wHookshot))->y;
        hs_z=Lwpns.spr(Lwpns.idFirst(wHookshot))->z;
        hs_dx=hs_x-hs_startx;
        hs_dy=hs_y-hs_starty;
        
        //extending
        if(((weapon*)Lwpns.spr(Lwpns.idFirst(wHookshot)))->misc==0)
        {
            int maxchainlinks=itemsbuf[parentitem].misc2;
            
            if(chainlinks.Count()<maxchainlinks)           //extending chain
            {
                if(abs(hs_dx)>=hs_xdist+8)
                {
                    hs_xdist=abs(hs_x-hs_startx);
                    chainlinks.add(new weapon((fix)hs_x, (fix)hs_y, (fix)hs_z,wHSChain, 0,0,Link.getDir(), parentitem,Link.getUID()));
                }
                else if(abs(hs_dy)>=hs_ydist+8)
                {
                    hs_ydist=abs(hs_y-hs_starty);
                    chainlinks.add(new weapon((fix)hs_x, (fix)hs_y, (fix)hs_z,wHSChain, 0,0,Link.getDir(), parentitem,Link.getUID()));
                }
            }                                                     //stretching chain
            else
            {
                dist_bx=(abs(hs_dx)-(8*chainlinks.Count()))/(chainlinks.Count()+1);
                dist_by=(abs(hs_dy)-(8*chainlinks.Count()))/(chainlinks.Count()+1);
                hs_w=8;
                
                if(hs_dx<0)
                {
                    dist_bx=0-dist_bx;
                    hs_w=-8;
                }
                
                if(hs_dy<0)
                {
                    dist_by=0-dist_by;
                    hs_w=-8;
                }
                
                for(int counter=0; counter<chainlinks.Count(); counter++)
                {
                    if(Link.getDir()>down)                            //chain is moving horizontally
                    {
                        chainlinks.spr(counter)->x=hs_startx+hs_w+dist_bx+(counter*(hs_w+dist_bx));
                    }
                    else
                    {
                        chainlinks.spr(counter)->y=hs_starty+hs_w+dist_by+(counter*(hs_w+dist_by));
                    }
                }
            }
        }                                                       //retracting
        else if(((weapon*)Lwpns.spr(Lwpns.idFirst(wHookshot)))->misc==1)
        {
            dist_bx=(abs(hs_dx)-(8*chainlinks.Count()))/(chainlinks.Count()+1);
            dist_by=(abs(hs_dy)-(8*chainlinks.Count()))/(chainlinks.Count()+1);
            hs_w=8;
            
            if(hs_dx<0)
            {
                dist_bx=0-dist_bx;
                hs_w=-8;
            }
            
            if(hs_dy<0)
            {
                dist_by=0-dist_by;
                hs_w=-8;
            }
            
            /* With ZScript modification, chains can conceivably move diagonally.*/
            //if (Link.getDir()>down)                               //chain is moving horizontally
            {
                if(abs(hs_dx)-(8*chainlinks.Count())>0)             //chain is stretched
                {
                    for(int counter=0; counter<chainlinks.Count(); counter++)
                    {
                        chainlinks.spr(counter)->x=hs_startx+hs_w+dist_bx+(counter*(hs_w+dist_bx));
                    }
                }
                else
                {
                    if(abs(hs_x-hs_startx)<=hs_xdist-8)
                    {
                        hs_xdist=abs(hs_x-hs_startx);
                        
                        if(pull_link==false)
                        {
                            chainlinks.del(chainlinks.idLast(wHSChain));
                        }
                        else
                        {
                            chainlinks.del(chainlinks.idFirst(wHSChain));
                        }
                    }
                }
            }                                                     //chain is moving vertically
            //else
            {
                if(abs(hs_dy)-(8*chainlinks.Count())>0)             //chain is stretched
                {
                    for(int counter=0; counter<chainlinks.Count(); counter++)
                    {
                        chainlinks.spr(counter)->y=hs_starty+hs_w+dist_by+(counter*(hs_w+dist_by));
                    }
                }
                else
                {
                    if(abs(hs_y-hs_starty)<=hs_ydist-8)
                    {
                        hs_ydist=abs(hs_y-hs_starty);
                        
                        if(pull_link==false)
                        {
                            chainlinks.del(chainlinks.idLast(wHSChain));
                        }
                        else
                        {
                            chainlinks.del(chainlinks.idFirst(wHSChain));
                        }
                    }
                }
            }
        }
    }
}

void do_dcounters()
{
    static bool sfxon = false;
    
    for(int i=0; i<32; i++)
    {
        if(game->get_dcounter(i)==0)
        {
            sfxon = false;
            continue;
        }
        
        if(frame&1)
        {
            sfxon = true;
            
            if(game->get_dcounter(i)>0)
            {
                int drain = (i==4 ? MAGICPERBLOCK/4 : 1);
                drain = zc_min(game->get_dcounter(i),drain);
                
                if(game->get_counter(i) < game->get_maxcounter(i))
                {
                    game->change_counter(drain, i);
                    game->change_dcounter(-drain, i);
                    
                    if(game->get_dcounter(i) < 0)  // Better safe
                        game->set_dcounter(0, i);
                }
                else
                {
                    game->set_dcounter(0, i);
                    game->set_counter(game->get_maxcounter(i), i);
                }
            }
            else
            {
                if(i!=1)   // Only rupee drain is sounded
                    sfxon = false;
                    
                int drain = (i==4 ? 2*game->get_magicdrainrate() : 1);
                
                if(game->get_counter(i)>0)
                {
                    game->change_counter(-drain, i);
                    game->change_dcounter(drain, i);
                    
                    if(game->get_dcounter(i) > 0)  // Better safe
                        game->set_dcounter(0, i);
                }
                else
                {
                    game->set_dcounter(0, i);
                    game->set_counter(0, i);
                }
            }
        }
        
        if((sfxon || i==1) && !lensclk && (i<2 || i==4)) // Life, Rupees and Magic
            sfx(WAV_MSG);
    }
}

void game_loop()
{

    //  walkflagx=0; walkflagy=0;
    if(fadeclk>=0)
    {
        if(fadeclk==0 && currscr<128)
            blockpath=false;
            
        --fadeclk;
    }
    
    // Three kinds of freezes: freeze, freezemsg, freezeff
    
    // freezemsg if message is being printed && qr_MSGFREEZE is on,
    // or if a message is being prepared && qr_MSGDISAPPEAR is on.
    bool freezemsg = ((msg_active || (intropos && intropos<72) || (linkedmsgclk && get_bit(quest_rules,qr_MSGDISAPPEAR)))
                      && get_bit(quest_rules,qr_MSGFREEZE));
                      
    // Messages also freeze FF combos.
    bool freezeff = freezemsg;
    
    bool freeze = false;
    
    for(int i=0; i<32; i++)
    {
        if(combobuf[tmpscr->ffdata[i]].type==cSCREENFREEZE) freeze=true;
        
        if(combobuf[tmpscr->ffdata[i]].type==cSCREENFREEZEFF) freezeff=true;
    }
    
    for(int i=0; i<176; i++)
    {
        if(combobuf[tmpscr->data[i]].type == cSCREENFREEZE) freeze=true;
        
        if(combobuf[tmpscr->data[i]].type == cSCREENFREEZEFF) freezeff=true;
        
        if(guygrid[i]>0)
        {
            --guygrid[i];
        }
    }
    
    animate_combos();
    load_control_state();
    
    if(!freezemsg)
    {
        script_drawing_commands.Clear();
    }
    
    if(!freezeff)
    {
        update_freeform_combos();
    }
    
    // Arbitrary Rule 637: neither 'freeze' nor 'freezeff' freeze the global script.
    if(!freezemsg && g_doscript)
    {
        ZScriptVersion::RunScript(SCRIPT_GLOBAL, GLOBAL_SCRIPT_GAME);
    }
    
    if(!freeze && !freezemsg)
    {
        mblock2.animate(0);
        items.animate();
        items.check_conveyor();
        guys.animate();
        roaming_item();
        dragging_item();
        Ewpns.animate();
        checklink = true;
        
        for(int i = 0; i < 1; i++)
        {
            if(Link.animate(0))
            {
                if(!Quit)
                {
                    Quit = qGAMEOVER;
                }
                
                return;
            }
            
            checklink=false;
        }
        
        do_magic_casting();
        Lwpns.animate();
        decorations.animate();
        particles.animate();
        update_hookshot();
        
        if(conveyclk<=0)
        {
            conveyclk=3;
        }
        
        --conveyclk;
        check_collisions();
        dryuplake();
        cycle_palette();
    }
    else if(freezemsg)
    {
        for(int i=0; i<guys.Count(); i++)
            if(((enemy*)guys.spr(i))->ignore_msg_freeze())
                guys.spr(i)->animate(i);
    }
    
    if(global_wait)
    {
        ZScriptVersion::RunScript(SCRIPT_GLOBAL, GLOBAL_SCRIPT_GAME);
        global_wait=false;
    }
    
    draw_screen(tmpscr);
    
    if(linkedmsgclk==1)
    {
        if(wpnsbuf[iwMore].tile!=0)
        {
            putweapon(framebuf,zinit.msg_more_x, message_more_y(), wPhantom, 4, up, lens_hint_weapon[wPhantom][0], lens_hint_weapon[wPhantom][1],-1);
        }
    }
    
    if(!freeze)
    {
        putintro();
    }
    
    if(dmapmsgclk>0)
    {
        Link.Freeze();
        
        if(dmapmsgclk<=50)
        {
            --dmapmsgclk;
        }
    }
    
    if(dmapmsgclk==1)
    {
        Link.finishedmsg();
        dmapmsgclk=0;
        introclk=72;
        clear_bitmap(msgdisplaybuf);
        set_clip_state(msgdisplaybuf, 1);
        //    clear_bitmap(pricesdisplaybuf);
    }
    
    if(!freeze)
    {
        if(introclk==0 || (introclk>=72 && dmapmsgclk==0))
        {
            putmsg();
            
            if(msgstr)
            {
                set_clip_state(msgdisplaybuf, 0);
                blit(msgbmpbuf, msgdisplaybuf, 0, 0, msg_xpos, msg_ypos, msg_w+16, msg_h+16);
            }
        }
        
        do_dcounters();
        
        if(!freezemsg && current_item(itype_heartring))
        {
            int itemid = current_item_id(itype_heartring);
            int fskip = itemsbuf[itemid].misc2;
            
            if(fskip == 0 || frame % fskip == 0)
                game->set_life(zc_min(game->get_life() + itemsbuf[itemid].misc1, game->get_maxlife()));
        }
        
        if(!freezemsg && current_item(itype_magicring))
        {
            int itemid = current_item_id(itype_magicring);
            int fskip = itemsbuf[itemid].misc2;
            
            if(fskip == 0 || frame % fskip == 0)
            {
                game->set_magic(zc_min(game->get_magic() + itemsbuf[itemid].misc1, game->get_maxmagic()));
            }
        }
        
        if(!freezemsg && current_item(itype_wallet))
        {
            int itemid = current_item_id(itype_wallet);
            int fskip = itemsbuf[itemid].misc2;
            
            if(fskip == 0 || frame % fskip == 0)
            {
                game->set_rupies(zc_min(game->get_rupies() + itemsbuf[itemid].misc1, game->get_maxcounter(1)));
            }
        }
        
        if(!freezemsg && current_item(itype_bombbag))
        {
            int itemid = current_item_id(itype_bombbag);
            
            if(itemsbuf[itemid].misc1)
            {
                int fskip = itemsbuf[itemid].misc2;
                
                if(fskip == 0 || frame % fskip == 0)
                {
                    game->set_bombs(zc_min(game->get_bombs() + itemsbuf[itemid].misc1, game->get_maxbombs()));
                }
                
                if(itemsbuf[itemid].flags & ITEM_FLAG1)
                {
                    int ratio = zinit.bomb_ratio;
                    
                    fskip = itemsbuf[itemid].misc2 * ratio;
                    
                    if(fskip == 0 || frame % fskip == 0)
                    {
                        game->set_sbombs(zc_min(game->get_sbombs() + zc_max(itemsbuf[itemid].misc1 / ratio, 1), game->get_maxbombs() / ratio));
                    }
                }
            }
        }
        
        if(!freezemsg && current_item(itype_quiver) && game->get_arrows() != game->get_maxarrows())
        {
            int itemid = current_item_id(itype_quiver);
            int fskip = itemsbuf[itemid].misc2;
            
            if(fskip == 0 || frame % fskip == 0)
            {
                game->set_arrows(zc_min(game->get_arrows() + itemsbuf[itemid].misc1, game->get_maxarrows()));
            }
        }
        
        if(lensclk)
        {
            draw_lens_over();
            --lensclk;
        }
        
        // Earthquake!
        if(quakeclk>0)
        {
            playing_field_offset=56+((int)(sin((double)(--quakeclk*2-frame))*4));
        }
        else
        {
            playing_field_offset=56;
        }
        
        // Other effects in zc_sys.cpp
    }
    
    //  putpixel(framebuf, walkflagx, walkflagy+playing_field_offset, vc(int(rand()%16)));
}

int get_currdmap()
{
    return currdmap;
}

int get_dlevel()
{
    return dlevel;
}

int get_currscr()
{
    return currscr;
}

int get_homescr()
{
    return homescr;
}

int get_bmaps(int si)
{
    return game->bmaps[si];
}

bool no_subscreen()
{
    return (tmpscr->flags3&fNOSUBSCR)!=0;
}

/**************************/
/********** Main **********/
/**************************/

// Returns the first no switch (-) argv param
char *get_cmd_arg(int argc, char *argv[])
{
   // assumes a switch won't be in argv[0] since it is the exe name.
   for (int i = 1; i < argc; i++)
      if (argv[i][0] != '-')
         return argv[i];
   return NULL;
}

int main(int argc, char* argv[])
{
    switch(IS_BETA)
    {
    
    case -1:
        Z_title("Zelda Classic %s Alpha (Build %d)",VerStr(ZELDA_VERSION), VERSION_BUILD);
        break;
        
    case 1:
        Z_title("Zelda Classic %s Beta (Build %d)",VerStr(ZELDA_VERSION), VERSION_BUILD);
        break;
        
    case 0:
        Z_title("Zelda Classic %s (Build %d)",VerStr(ZELDA_VERSION), VERSION_BUILD);
    }
    
    // Before anything else, let's register our custom trace handler:
    register_trace_handler(zc_trace_handler);

    Z_message("Initializing Allegro...");
    if(allegro_init() != 0)
    {
        Z_error("Failed Init!");
        exit(-1);
    }
    
    three_finger_flag=false;

    // Allocate quest path buffer
    memrequested += 2048;
    Z_message("Allocating quest path buffers (%s)...", byte_conversion2(2048,memrequested,-1,-1));
    quest_path = (char*)malloc(2048);
    memset(quest_path, 0, 2048);

    if(!quest_path)
    {
        Z_error("Allocation error");
        exit(-1);
    }
    
       // Get the quest file to run.
   char *temp = get_cmd_arg(argc, argv);

   if (temp != NULL)
      strcpy(quest_path, temp);

   if (strlen(quest_path) == 0)
   {
      printf("Provide a quest name as the first command argument:\n\t%s quest.qst\n", argv[0]);
      exit(-1);
   }

   // Validate that the quest file really exists.
   if (!exists(quest_path))
   {
      printf("Quest file doesn't exist or it's invalid: %s\n", quest_path);
      exit(-1);
   }
    
    Z_message("OK\n");

    // allocate quest data buffers
    if(!get_qst_buffers())
    {
        Z_error("Error");
        quit_game();
    }
    
    // set and load game configurations
    set_config_file("ag.cfg");
    if(exists("ag.cfg") != 0)
    {
        load_game_configs();
    }
    else
    {
        load_game_configs();
        save_game_configs();
    }
    
    if(install_timer() < 0)
    {
        Z_error(allegro_error);
        quit_game();
    }
    
    if(install_keyboard() < 0)
    {
        Z_error(allegro_error);
        quit_game();
    }
    
    if(install_joystick(JOY_TYPE_AUTODETECT) < 0)
    {
        Z_error(allegro_error);
        quit_game();
    }
    
    //set_keyboard_rate(1000,160);
    
    LOCK_VARIABLE(logic_counter);
    LOCK_FUNCTION(update_logic_counter);
    install_int_ex(update_logic_counter, BPS_TO_TIMER(60));
    
#ifdef _SCRIPT_COUNTER
    LOCK_VARIABLE(script_counter);
    LOCK_FUNCTION(update_script_counter);
    install_int_ex(update_script_counter, 1);
#endif
    
    if(!Z_init_timers())
    {
        Z_error("Couldn't Allocate Timers");
        quit_game();
    }
    
    Z_message("OK\n");
    
 
    set_color_depth(8);
    
    // allocate bitmap buffers
    Z_message("Allocating bitmap buffers... ");
    
    framebuf  = create_bitmap_ex(8,256,224);
    temp_buf  = create_bitmap_ex(8,256,224);
    scrollbuf = create_bitmap_ex(8,512,406);
    tmp_scr   = create_bitmap_ex(8,320,240);
    tmp_bmp   = create_bitmap_ex(8,32,32);
    prim_bmp  = create_bitmap_ex(8,512,512);
    msgdisplaybuf = create_bitmap_ex(8,256, 176);
    msgbmpbuf = create_bitmap_ex(8, 512+16, 512+16);
    pricesdisplaybuf = create_bitmap_ex(8,256, 176);
    
    if(!framebuf || !scrollbuf || !tmp_bmp || !tmp_scr || !msgdisplaybuf || !pricesdisplaybuf)
    {
        Z_error("Error");
        quit_game();
    }
    
    clear_bitmap(scrollbuf);
    clear_bitmap(framebuf);
    clear_bitmap(msgdisplaybuf);
    set_clip_state(msgdisplaybuf, 1);
    clear_bitmap(pricesdisplaybuf);
    set_clip_state(pricesdisplaybuf, 1);
    Z_message("OK\n");
    
    
    zcmusic_init();
    
    //  int mode = VidMode;                                       // from config file
    int tempmode=GFX_AUTODETECT;
    
    // load the data files
    resolve_password(datapwd);
//  setPackfilePassword(datapwd);
    packfile_password(datapwd);
    
    Z_message("Loading data files:\n");
    set_color_conversion(COLORCONV_NONE);
    
    sprintf(zeldadat_sig,"Zelda.Dat %s Build %d",VerStr(ZELDADAT_VERSION), ZELDADAT_BUILD);
    sprintf(sfxdat_sig,"SFX.Dat %s Build %d",VerStr(SFXDAT_VERSION), SFXDAT_BUILD);
    sprintf(fontsdat_sig,"Fonts.Dat %s Build %d",VerStr(FONTSDAT_VERSION), FONTSDAT_BUILD);
    
    packfile_password(NULL); // Temporary measure. -L
    Z_message("Zelda.Dat...");
    
    if((data=load_datafile("zelda.dat"))==NULL)
    {
        Z_error("failed");
        quit_game();
    }
    
    if(strncmp((char*)data[0].dat,zeldadat_sig,24))
    {
        Z_error("\nIncompatible version of zelda.dat.\nPlease upgrade to %s Build %d",VerStr(ZELDADAT_VERSION), ZELDADAT_BUILD);
        quit_game();
    }
    
    Z_message("OK\n");
    packfile_password(datapwd); // Temporary measure. -L
    
    Z_message("Fonts.Dat...");
    
    if((fontsdata=load_datafile("fonts.dat"))==NULL)
    {
        Z_error("failed");
        quit_game();
    }
    
    if(strncmp((char*)fontsdata[0].dat,fontsdat_sig,24))
    {
        Z_error("\nIncompatible version of fonts.dat.\nPlease upgrade to %s Build %d",VerStr(FONTSDAT_VERSION), FONTSDAT_BUILD);
        quit_game();
    }
    
    Z_message("OK\n");
    
//  setPackfilePassword(NULL);
    packfile_password(NULL);
    
    Z_message("SFX.Dat...");
    
    if((sfxdata=load_datafile("sfx.dat"))==NULL)
    {
        Z_error("failed");
        quit_game();
    }
    
    if(strncmp((char*)sfxdata[0].dat,sfxdat_sig,22) || sfxdata[Z35].type != DAT_ID('S', 'A', 'M', 'P'))
    {
        Z_error("\nIncompatible version of sfx.dat.\nPlease upgrade to %s Build %d",VerStr(SFXDAT_VERSION), SFXDAT_BUILD);
        quit_game();
    }
    
    Z_message("OK\n");
    
    mididata = (DATAFILE*)data[ZC_MIDI].dat;
    
    set_uformat(U_ASCII);
    deffont = font;
    nfont = (FONT*)fontsdata[FONT_GUI_PROP].dat;
    font = nfont;
    pfont = (FONT*)fontsdata[FONT_8xPROP_THIN].dat;
    lfont = (FONT*)fontsdata[FONT_LARGEPROP].dat;
    lfont_l = (FONT*)fontsdata[FONT_LARGEPROP_L].dat;
    zfont = (FONT*)fontsdata[FONT_NES].dat;
    z3font = (FONT*)fontsdata[FONT_Z3].dat;
    z3smallfont = (FONT*)fontsdata[FONT_Z3SMALL].dat;
    mfont = (FONT*)fontsdata[FONT_MATRIX].dat;
    ztfont = (FONT*)fontsdata[FONT_ZTIME].dat;
    sfont = (FONT*)fontsdata[FONT_6x6].dat;
    sfont2 = (FONT*)fontsdata[FONT_6x4].dat;
    sfont3 = (FONT*)fontsdata[FONT_12x8].dat;
    spfont = (FONT*)fontsdata[FONT_6xPROP].dat;
    ssfont1 = (FONT*)fontsdata[FONT_SUBSCREEN1].dat;
    ssfont2 = (FONT*)fontsdata[FONT_SUBSCREEN2].dat;
    ssfont3 = (FONT*)fontsdata[FONT_SUBSCREEN3].dat;
    ssfont4 = (FONT*)fontsdata[FONT_SUBSCREEN4].dat;
    gblafont = (FONT*)fontsdata[FONT_GB_LA].dat;
    goronfont = (FONT*)fontsdata[FONT_GORON].dat;
    zoranfont = (FONT*)fontsdata[FONT_ZORAN].dat;
    hylian1font = (FONT*)fontsdata[FONT_HYLIAN1].dat;
    hylian2font = (FONT*)fontsdata[FONT_HYLIAN2].dat;
    hylian3font = (FONT*)fontsdata[FONT_HYLIAN3].dat;
    hylian4font = (FONT*)fontsdata[FONT_HYLIAN4].dat;
    gboraclefont = (FONT*)fontsdata[FONT_GB_ORACLE].dat;
    gboraclepfont = (FONT*)fontsdata[FONT_GB_ORACLE_P].dat;
    dsphantomfont = (FONT*)fontsdata[FONT_DS_PHANTOM].dat;
    dsphantompfont = (FONT*)fontsdata[FONT_DS_PHANTOM_P].dat;
    
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<MAXSUBSCREENITEMS; j++)
        {
            memset(&custom_subscreen[i].objects[j],0,sizeof(subscreen_object));
        }
    }
    
    for(int i=0; i<WAV_COUNT; i++)
    {
        customsfxdata[i].data=NULL;
        sfx_string[i] = new char[36];
    }
    
    for(int i=0; i<WAV_COUNT>>3; i++)
    {
        customsfxflag[i] = 0;
    }
    
    for(int i=0; i<WPNCNT; i++)
    {
        weapon_string[i] = new char[64];
    }
    
    for(int i=0; i<ITEMCNT; i++)
    {
        item_string[i] = new char[64];
    }
    
    for(int i=0; i<eMAXGUYS; i++)
    {
        guy_string[i] = new char[64];
    }
    
    for(int i=0; i<512; i++)
    {
        ffscripts[i] = new ffscript[1];
        ffscripts[i][0].command = 0xFFFF;
    }
    
    for(int i=0; i<256; i++)
    {
        itemscripts[i] = new ffscript[1];
        itemscripts[i][0].command = 0xFFFF;
    }
    
    for(int i=0; i<256; i++)
    {
        guyscripts[i] = new ffscript[1];
        guyscripts[i][0].command = 0xFFFF;
    }
    
    for(int i=0; i<256; i++)
    {
        wpnscripts[i] = new ffscript[1];
        wpnscripts[i][0].command = 0xFFFF;
    }
    
    for(int i=0; i<256; i++)
    {
        screenscripts[i] = new ffscript[1];
        screenscripts[i][0].command = 0xFFFF;
    }
    
    for(int i=0; i<NUMSCRIPTGLOBAL; i++)
    {
        globalscripts[i] = new ffscript[1];
        globalscripts[i][0].command = 0xFFFF;
    }
    
    for(int i=0; i<3; i++)
    {
        linkscripts[i] = new ffscript[1];
        linkscripts[i][0].command = 0xFFFF;
    }
    
    //script drawing bitmap allocation
    zscriptDrawingRenderTarget = new ZScriptDrawingRenderTarget();
    
    
    // initialize sound driver
    
    Z_message("Initializing sound driver... ");
    
    if(install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,NULL))
    {
        //      Z_error(allegro_error);
        Z_message("Sound driver not available.  Sound disabled.\n");
    }
    else
    {
        Z_message("OK\n");
    }
    
    Z_init_sound();
    
    const int wait_ms_on_set_graphics = 20; //formerly 250. -Gleeok
    
    if(used_switch(argc,argv,"-fullscreen") ||
            (!used_switch(argc, argv, "-windowed") && get_config_int("zeldadx","fullscreen",1)==1))
    {
        al_trace("Used switch: -fullscreen\n");
        tempmode = GFX_AUTODETECT_FULLSCREEN;
    }
    else if(used_switch(argc,argv,"-windowed") || get_config_int("zeldadx","fullscreen",1)==0)
    {
        al_trace("Used switch: -windowed\n");
        tempmode=GFX_AUTODETECT_WINDOWED;
    }
    
    //set scale
    if(resx < 320) resx = 320;
    
    if(resy < 240) resy = 240;
    
    screen_scale = zc_max(zc_min(resx / 320, resy / 240), 1);
    
    if(!game_vid_mode(tempmode, wait_ms_on_set_graphics))
    {
        //what we need here is not rightousness but madness!!!
        
#define TRY_SET_VID_MODE(scale) \
	Z_message("Unable to set gfx mode at -%d %dbpp %d x %d \n", tempmode, get_color_depth(), resx, resy); \
	screen_scale=scale; \
	resx=320*scale; \
	resy=240*scale
        
        TRY_SET_VID_MODE(2);
        
        if(!game_vid_mode(tempmode, wait_ms_on_set_graphics))
        {
            TRY_SET_VID_MODE(1);
            
            if(!game_vid_mode(tempmode, wait_ms_on_set_graphics))
            {
                if(tempmode != GFX_AUTODETECT_WINDOWED)
                {
                    tempmode=GFX_AUTODETECT_WINDOWED;
                    al_trace("-fullscreen not supported by your video driver! setting -windowed switch\n");
                    TRY_SET_VID_MODE(2);
                    
                    if(!game_vid_mode(tempmode, wait_ms_on_set_graphics))
                    {
                        TRY_SET_VID_MODE(1);
                        
                        if(!game_vid_mode(tempmode, wait_ms_on_set_graphics))
                        {
                            Z_message("Unable to set gfx mode at -%d %dbpp %d x %d \n", tempmode, get_color_depth(), resx, resy);
                            al_trace("Fatal Error...Zelda Classic could not be initialized. Have a nice day :) \n");
                            Z_error(allegro_error);
                            quit_game();
                        }
                        else Z_message("set gfx mode succsessful at -%d %dbpp %d x %d \n", tempmode, get_color_depth(), resx, resy);
                    }
                    else Z_message("set gfx mode succsessful at -%d %dbpp %d x %d \n", tempmode, get_color_depth(), resx, resy);
                }
                else
                {
                    al_trace("Fatal Error: could not create a window for Zelda Classic.\n");
                    Z_error(allegro_error);
                    quit_game();
                }
            }
            else Z_message("set gfx mode succsessful at -%d %dbpp %d x %d \n", tempmode, get_color_depth(), resx, resy);
        }
        else Z_message("set gfx mode succsessful at -%d %dbpp %d x %d \n", tempmode, get_color_depth(), resx, resy);
    }
    else
    {
        Z_message("set gfx mode succsessful at -%d %dbpp %d x %d \n", tempmode, get_color_depth(), resx, resy);
    }
    
    sbig = (screen_scale > 1);
    set_display_switch_mode(is_windowed_mode()?SWITCH_BACKGROUND:SWITCH_BACKAMNESIA);
    
    real_screen = screen;
    
    set_close_button_callback((void (*)()) hit_close_button);
    set_window_title("Zelda Classic");
    
// load saved games
    Z_message("Loading saved games... ");
    
    if(load_savedgames() != 0)
    {
        Z_error("Insufficient memory");
        quit_game();
    }
    
    Z_message("OK\n");
    
    // play the game
    reset_items(true, &QHeader);
    
    clear_to_color(screen,BLACK);
    Quit = qRESET;
    
    rgb_map = &rgb_table;
    
    // set up an initial game save slot (for the list_saves function)
    game = new gamedata;
    game->Clear();
    
    while(Quit!=qEXIT)
    {
        // this is here to continually fix the keyboard repeat
        set_keyboard_rate(250,33);
        titlescreen(0);
        
        setup_combo_animations();
        setup_combo_animations2();
        
        while(!Quit)
        {
            game_loop();
            advanceframe(true);
        }
        
        tmpscr->flags3=0;
        Playing=Paused=false;
        
        switch(Quit)
        {
        case qQUIT:
        case qGAMEOVER:
        {
            Link.setDontDraw(false);
            show_subscreen_dmap_dots=true;
            show_subscreen_numbers=true;
            show_subscreen_items=true;
            show_subscreen_life=true;
            introclk=intropos=0;
            
            initZScriptGlobalRAM();
            ZScriptVersion::RunScript(SCRIPT_GLOBAL, GLOBAL_SCRIPT_END);
            
            if(!skipcont&&!get_bit(quest_rules,qr_NOCONTINUE)) game_over(get_bit(quest_rules,qr_NOSAVE));
            
            skipcont = 0;
        }
        break;
        
        case qWON:
        {
            Link.setDontDraw(false);
            show_subscreen_dmap_dots=true;
            show_subscreen_numbers=true;
            show_subscreen_items=true;
            show_subscreen_life=true;
            
            initZScriptGlobalRAM();
            ZScriptVersion::RunScript(SCRIPT_GLOBAL, GLOBAL_SCRIPT_END);
            ending();
        }
        break;
        
        }
        
        kill_sfx();
        music_stop();
        clear_to_color(screen,BLACK);
    }
    
    // clean up
    
    music_stop();
    kill_sfx();
    
    show_saving(screen);
    save_savedgames();
    save_game_configs();
    set_gfx_mode(GFX_TEXT,80,25,0,0);
    //rest(250); // ???
    quit_game();
    Z_message("Armageddon Games web site: http://www.armageddongames.com\n");
    Z_message("Zelda Classic web site: http://www.zeldaclassic.com\n");
    Z_message("Zelda Classic wiki: http://www.shardstorm.com/ZCwiki/\n");
    
    allegro_exit();
    return 0;
}
END_OF_MAIN()


void remove_installed_timers()
{
    al_trace("Removing timers. \n");
    remove_int(update_logic_counter);
    Z_remove_timers();
#ifdef _SCRIPT_COUNTER
    remove_int(update_script_counter);
#endif
    
}


void delete_everything_else() //blarg.
{
    delete_combo_aliases();
    reset_subscr_items();
    delete_selectors();
    Sitems.clear();
    
}

void quit_game()
{
    script_drawing_commands.Dispose(); //for allegro bitmaps
    
    remove_installed_timers();
    delete_everything_else();
    
    al_trace("Freeing Data: \n");
    
    if(game) delete game;
    
    if(data) unload_datafile(data);
    
    if(fontsdata) unload_datafile(fontsdata);
    
    if(sfxdata) unload_datafile(sfxdata);
    
    //if(mididata) unload_datafile(mididata);
    //  if(mappic)
    //    destroy_bitmap(mappic);
    
    al_trace("Bitmaps... \n");
    destroy_bitmap(framebuf);
    destroy_bitmap(scrollbuf);
    destroy_bitmap(tmp_scr);
    destroy_bitmap(tmp_bmp);
    destroy_bitmap(prim_bmp);
    set_clip_state(msgdisplaybuf, 1);
    destroy_bitmap(msgdisplaybuf);
    set_clip_state(pricesdisplaybuf, 1);
    destroy_bitmap(pricesdisplaybuf);
    
    al_trace("Subscreens... \n");
    
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<MAXSUBSCREENITEMS; j++)
        {
            switch(custom_subscreen[i].objects[j].type)
            {
            case ssoTEXT:
            case ssoTEXTBOX:
            case ssoCURRENTITEMTEXT:
            case ssoCURRENTITEMCLASSTEXT:
                if(custom_subscreen[i].objects[j].dp1 != NULL) delete[](char *)custom_subscreen[i].objects[j].dp1;
            }
        }
    }
    
    al_trace("SFX... \n");
    zcmusic_exit();
    
    for(int i=0; i<WAV_COUNT; i++)
    {
        delete [] sfx_string[i];
        
        if(customsfxdata[i].data!=NULL)
        {
//      delete [] customsfxdata[i].data;
            free(customsfxdata[i].data);
        }
    }
    
    al_trace("Misc... \n");
    
    for(int i=0; i<WPNCNT; i++)
    {
        delete [] weapon_string[i];
    }
    
    for(int i=0; i<ITEMCNT; i++)
    {
        delete [] item_string[i];
    }
    
    for(int i=0; i<eMAXGUYS; i++)
    {
        delete [] guy_string[i];
    }
    
    al_trace("Script buffers... \n");
    
    for(int i=0; i<512; i++)
    {
        if(ffscripts[i]!=NULL) delete [] ffscripts[i];
    }
    
    for(int i=0; i<256; i++)
    {
        if(itemscripts[i]!=NULL) delete [] itemscripts[i];
    }
    
    for(int i=0; i<256; i++)
    {
        if(guyscripts[i]!=NULL) delete [] guyscripts[i];
    }
    
    for(int i=0; i<256; i++)
    {
        if(wpnscripts[i]!=NULL) delete [] wpnscripts[i];
    }
    
    for(int i=0; i<256; i++)
    {
        if(screenscripts[i]!=NULL) delete [] screenscripts[i];
    }
    
    for(int i=0; i<NUMSCRIPTGLOBAL; i++)
    {
        if(globalscripts[i]!=NULL) delete [] globalscripts[i];
    }
    
    for(int i=0; i<3; i++)
    {
        if(linkscripts[i]!=NULL) delete [] linkscripts[i];
    }
    
    delete zscriptDrawingRenderTarget;
    
    //for(int i=0; i<map_count*MAPSCRS; i++)
    //{
    //if(TheMaps[i].data != NULL) delete [] TheMaps[i].data;
    //if(TheMaps[i].sflag != NULL) delete [] TheMaps[i].sflag;
    //if(TheMaps[i].cset != NULL) delete [] TheMaps[i].cset;
    //}
    al_trace("Screen Data... \n");
    
    al_trace("Deleting quest buffers... \n");
    del_qst_buffers();
    
    if(quest_path) free(quest_path);
    
    //if(TheMaps != NULL) free(TheMaps);
    //if(ZCMaps != NULL) free(ZCMaps);
    //  dumb_exit();
}

/*** end of zelda.cc ***/
