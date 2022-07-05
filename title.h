#ifndef _ZC_TITLE_H_
#define _ZC_TITLE_H_

int  load_savedgames();
int  save_savedgames();
void load_game_icon(gamedata *g);
void titlescreen();
void game_over(int type);
void save_game(bool savepoint);
bool save_game(bool savepoint, int type);
#endif
