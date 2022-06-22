//--------------------------------------------------------
//  Zelda Classic
//  by Jeremy Craner, 1999-2000
//
//  title.h
//
//  Title screen and intro for zelda.cc
//  Also has game loading and select screen code.
//
//--------------------------------------------------------

#ifndef _ZC_TITLE_H_
#define _ZC_TITLE_H_

int  load_savedgames();
int  save_savedgames();
void load_game_icon(gamedata *g, bool forceDefault, int index);
void load_game_icon_to_buffer(bool forceDefault, int index);
void titlescreen();
void game_over(int type);
void save_game(bool savepoint);
bool save_game(bool savepoint, int type);
#endif

