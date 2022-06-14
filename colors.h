//--------------------------------------------------------
//  Zelda Classic
//  by Jeremy Craner, 1999-2000
//
//  colors.h
//
//  Palette data for Zelda Classic.
//
//--------------------------------------------------------

#ifndef _ZC_COLORS_H_
#define _ZC_COLORS_H_

#include "zdefs.h"

extern byte *colordata;
extern void create_zc_trans_table(COLOR_MAP *table, AL_CONST PALETTE pal, int r, int g, int b);
void create_rgb_table_range(RGB_MAP *table, AL_CONST PALETTE pal, unsigned char start, unsigned char end, void (*callback)(int pos));

// offsets in "data sets"
#define poLEVEL  15                                         // first level palette
#define poFADE1  15+4                                       //
#define poFADE2  15+7                                       //
#define poFADE3  15+10                                      //
#define oldpoSPRITE 210                                     // first sprite pal
#define newpoSPRITE 3343                                    // first sprite pal
#define newerpoSPRITE 6671
#define pSprite(x) ((x)+newerpoSPRITE)

// palette data sizes in "data sets"
#define pdLEVEL  13                                         // complete level pal (normal & fade pals)
#define pdFADE   3                                          // size of a fade pal
#define oldpdTOTAL  240                                     // total data sets
#define newpdTOTAL  3373                                    // total data sets
#define newerpdTOTAL 6701

// palette data sizes in bytes
#define newerpsTOTAL ((newerpdTOTAL<<4)*3)

extern byte nes_pal[];

#endif                                                      // _ZC_COLORS_H_
 
