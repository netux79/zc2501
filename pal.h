#ifndef _ZC_PAL_H_
#define _ZC_PAL_H_

#include "zdefs.h"

extern int CSET_SIZE;                                       // this is only changed to 4 in the NES title screen
extern int CSET_SHFT;                                       // log2 of CSET_SIZE
extern byte nes_pal[];

#define CSET(x)         ((x)<<CSET_SHFT)
#define csBOSS          14
#define csICON          12  /* Colors used for link icons in main menu 12 13 14 */

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

extern RGB _RGB(byte *si);
extern RGB _RGB(int r,int g,int b);

extern void loadfullpal();
extern void loadlvlpal(int level);
extern void loadpalset(int cset,int dataset);
extern void loadfadepal(int dataset);
extern void interpolatedfade();

extern void ringcolor(bool forceDefault);
extern void fade(int level,bool blackall,bool fromblack);
extern void lighting(bool existslight, bool setnaturaldark = false);
extern void lightingInstant();
extern void dryuplake();
extern void rehydratelake(bool instant);
extern void reset_pal_cycling();
extern void cycle_palette();

extern void create_zc_trans_table(COLOR_MAP *table, AL_CONST PALETTE pal, int r, int g, int b);
extern void create_rgb_table_range(RGB_MAP *table, AL_CONST PALETTE pal, unsigned char start, unsigned char end);

INLINE RGB NESpal(int i)
{
    return _RGB(nes_pal+i*3);
}

extern int reverse_NESpal(RGB c);

#endif                                                      // _ZC_PAL_H_
