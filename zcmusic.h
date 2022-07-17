#ifndef _ZCMUSIC_H_
#define _ZCMUSIC_H_

#define ZCMF_OGG      0x00000001
#define ZCMF_MP3      0x00000002
#define ZCMF_GME      0x00000004

#define ZCM_PLAYING 1
#define ZCM_STOPPED 0
#define ZCM_PAUSED -1

#define ZCM_PAUSE   1
#define ZCM_RESUME  0
#define ZCM_TOGGLE -1

extern bool zcmusic_init(void);
extern bool zcmusic_poll(void);
extern void zcmusic_exit(void);

typedef struct ZCMUSICBASE
{
    int type;                                               // uses ZCMF defines
    int playing;                                            // -1 = paused, 0 = stopped, 1 = playing
    int position;                                           // Only needed to sync Triforce jingle
    char filename[256];
    int track;
} ZCMUSIC;

extern ZCMUSIC const * zcmusic_load_file(char *filename);
extern bool zcmusic_play(ZCMUSIC* zcm, int vol);
extern bool zcmusic_pause(ZCMUSIC* zcm, int pause);
extern bool zcmusic_stop(ZCMUSIC* zcm);
extern void zcmusic_unload_file(ZCMUSIC* &zcm);
extern int zcmusic_get_tracks(ZCMUSIC* zcm);
extern int zcmusic_change_track(ZCMUSIC* zcm, int tracknum);

#endif
