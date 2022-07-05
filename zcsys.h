#ifndef _ZCSYS_H_
#define _ZCSYS_H_

#include "zdefs.h"

bool game_vid_mode(int mode,int wait);
int Z_init_timers();
void Z_remove_timers();
void Z_init_sound();

void load_game_configs();
void save_game_configs();

void draw_lens_under(BITMAP *dest, bool layer);
void draw_lens_over();
void f_Quit(int type);
void advanceframe(bool allowwavy, bool sfxcleaup = true);
void updatescr(bool allowwavy);
void syskeys();
void  onQuit();

bool ReadKey(int k);
void eat_buttons();

extern bool control_state[18];
extern bool button_press[18];

void load_control_state();

bool Up();
bool Down();
bool Left();
bool Right();
bool DrunkUp();
bool DrunkDown();
bool DrunkLeft();
bool DrunkRight();

bool rUp();
bool rDown();
bool rLeft();
bool rRight();
bool DrunkrUp();
bool DrunkrDown();
bool DrunkrLeft();
bool DrunkrRight();

bool cAbtn();
bool cBbtn();
bool cSbtn();
bool cMbtn();
bool cLbtn();
bool cRbtn();
bool cPbtn();
bool cEx1btn();
bool cEx2btn();
bool cEx3btn();
bool cEx4btn();
bool AxisUp();
bool AxisDown();
bool AxisLeft();
bool AxisRight();
bool DrunkcAbtn();
bool DrunkcBbtn();
bool DrunkcSbtn();
bool DrunkcMbtn();
bool DrunkcLbtn();
bool DrunkcRbtn();
bool DrunkcPbtn();

bool rAbtn();
bool rBbtn();
bool rSbtn();
bool rMbtn();
bool rLbtn();
bool rRbtn();
bool rPbtn();
bool rEx1btn();
bool rEx2btn();
bool rEx3btn();
bool rEx4btn();
bool rAxisUp();
bool rAxisDown();
bool rAxisLeft();
bool rAxisRight();
bool DrunkrAbtn();
bool DrunkrBbtn();
bool DrunkrSbtn();
bool DrunkrMbtn();
bool DrunkrLbtn();
bool DrunkrRbtn();
bool DrunkrPbtn();

enum {bosCIRCLE=0, bosOVAL, bosTRIANGLE, bosSMAS, bosMAX};

void show_fps(BITMAP *target);
void show_saving(BITMAP *target);
bool game_vid_mode(int mode,int wait);

extern int black_opening_count;
extern int black_opening_x,black_opening_y;
extern int black_opening_shape;

void zapout();
void zapin();
void wavyout(bool showlink);
void wavyin();
void blackscr(int fcnt,bool showsubscr);
void black_opening(BITMAP *dest,int x,int y,int a,int max_a);
void close_black_opening(int x, int y, bool wait);
void open_black_opening(int x, int y, bool wait);
void openscreen();
int  TriforceCount();

bool item_disabled(int item);
bool can_use_item(int item_type, int item);
bool has_item(int item_type, int item);
//int high_item(int jmax, int item_type, bool consecutive, int itemcluster, bool usecluster);
int current_item(int item_type);
int current_item(int item_type, bool checkenabled);
int current_item_power(int item_type);
int current_item_id(int item_type, bool checkmagic = true);
int item_tile_mod(bool);
int dmap_tile_mod();

bool try_zcmusic(char *filename, int track, int midi);
void jukebox(int index);
void play_DmapMusic();
void music_pause();
void music_resume();
void music_stop();
void master_volume(int dv,int mv);
int  sfx_count();
void sfx_cleanup();
bool sfx_init(int index);
void sfx(int index,int pan,bool loop, bool restart = true);
bool sfx_allocated(int index);
void cont_sfx(int index);
void stop_sfx(int index);
void adjust_sfx(int index,int pan,bool loop);
void pause_sfx(int index);
void resume_sfx(int index);
void pause_all_sfx();
void resume_all_sfx();
void stop_sfx(int index);
void stop_item_sfx(int family);
void kill_sfx();
int  pan(int x);

char *time_str_short(dword time);
char *time_str_med(dword time);

int  vbound(int x,int low,int high);
float vbound(float x,float low,float high);
bool isinRect(int x,int y,int rx1,int ry1,int rx2,int ry2);

int encode_file_007(const char *srcfile, const char *destfile, int key, const char *header, int method);
int decode_file_007(const char *srcfile, const char *destfile, const char *header, int method);

int  get_bit(byte *bitstr,int bit);
void set_bit(byte *bitstr,int bit,byte val);

void Z_error(const char *format,...);
void Z_message(const char *format,...);

int anim_3_4(int clk, int speed);

int zc_trace_handler(const char *);
void zc_trace_clear();

#endif                                                      // _ZCSYS_H_
