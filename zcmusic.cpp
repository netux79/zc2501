#include <string.h>
#include <alogg.h>
#include <almp3.h>
#include <gme/Nsf_Emu.h>
#include <gme/Gbs_Emu.h>
#include <gme/Spc_Emu.h>

#include "zcsys.h"
#include "zcmusic.h"

#define GME_SAMPLES     44100 //Hz
#define ZCMUSIC_BUFSZ   64

typedef struct OGGFILE : public ZCMUSICBASE
{
    ALOGG_OGGSTREAM *s;
    PACKFILE *f;
    char *fname;
    int vol;
} OGGFILE;

typedef struct MP3FILE : public ZCMUSICBASE
{
    ALMP3_MP3STREAM *s;
    PACKFILE *f;
    char *fname;
    int vol;
} MP3FILE;

typedef struct GMEFILE : public ZCMUSICBASE
{
    AUDIOSTREAM *stream;
    class Music_Emu* emu;
    int samples;
} GMEFILE;

static ZCMUSIC *cur_zcm;

// forward declarations
OGGFILE *load_ogg_file(char *filename);
int poll_ogg_file(OGGFILE *ogg);
void unload_ogg_file(OGGFILE *ogg);
bool ogg_pause(OGGFILE *ogg);
bool ogg_resume(OGGFILE *ogg);
bool ogg_reset(OGGFILE *ogg);
void ogg_stop(OGGFILE *ogg);

MP3FILE *load_mp3_file(char *filename);
int poll_mp3_file(MP3FILE *mp3);
void unload_mp3_file(MP3FILE *mp3);
bool mp3_pause(MP3FILE *mp3);
bool mp3_resume(MP3FILE *mp3);
bool mp3_reset(MP3FILE *mp3);
void mp3_stop(MP3FILE *mp3);

Music_Emu* gme_load_file(char* filename, char* ext);
int poll_gme_file(GMEFILE *gme);
int unload_gme_file(GMEFILE* gme);
int gme_play(GMEFILE *gme, int vol);


bool zcmusic_init(void)
{
    /* For now there is nothing to do */
    return true;
}

bool zcmusic_poll(void)
{
    if (!cur_zcm)
        return false;

    if(cur_zcm->playing==ZCM_PLAYING)
    {
        cur_zcm->position++;
        
        switch(cur_zcm->type)
        {
            case ZCMF_OGG:
                poll_ogg_file((OGGFILE*)cur_zcm);
                break;
                
            case ZCMF_MP3:
                poll_mp3_file((MP3FILE*)cur_zcm);
                break;
                
            case ZCMF_GME:
                if(((GMEFILE*)cur_zcm)->emu)
                    poll_gme_file((GMEFILE*)cur_zcm);
                    
                break;
        }
    }
    
    return true;
}

void zcmusic_exit(void)
{
    if (!cur_zcm)
        return;

    zcmusic_unload_file(cur_zcm);
    cur_zcm = NULL;
}

ZCMUSIC const * zcmusic_load_file(char *filename)
{
    if(filename == NULL)
    {
        return NULL;
    }
    
    Z_message("Loading %s\n", filename);
    
    if(strlen(filename)>255)
    {
        Z_message("Music file '%s' not loaded: filename too long\n", filename);
        return NULL;
    }
    
    char *ext=get_extension(filename);
    
    if(stricmp(ext,"ogg")==0)
    {
        OGGFILE *p = load_ogg_file(filename);
        
        if(!p)
        {
            Z_message("OGG file '%s' not found.\n",filename);
            goto error;
        }
        
        p->fname = (char*)malloc(strlen(filename)+1);
        
        if(!p->fname)
        {
            unload_ogg_file(p);
            goto error;
        }
        
        strcpy(p->fname, filename);
        p->type = ZCMF_OGG;
        p->playing = ZCM_STOPPED;
        ZCMUSIC *music=(ZCMUSIC*)p;
        strncpy(music->filename, get_filename(filename), sizeof(music->filename)-1);
        music->track=0;
        return music;
    }
    
    if(stricmp(ext,"mp3")==0)
    {
        MP3FILE *p = load_mp3_file(filename);
        
        if(!p)
        {
            Z_message("MP3 file '%s' not found.\n",filename);
            goto error;
        }
        
        p->fname = (char*)malloc(strlen(filename)+1);
        
        if(!p->fname)
        {
            unload_mp3_file(p);
            goto error;
        }
        
        strcpy(p->fname, filename);
        p->type = ZCMF_MP3;
        p->playing = ZCM_STOPPED;
        ZCMUSIC *music=(ZCMUSIC*)p;
        strncpy(music->filename, get_filename(filename), sizeof(music->filename)-1);
        music->track=0;
        return music;
    }
    
    if((stricmp(ext,"spc")==0) || (stricmp(ext,"gbs")==0) || (stricmp(ext,"vgm")==0)|| (stricmp(ext,"gym")==0)|| (stricmp(ext,"nsf")==0))
    {
    
        Music_Emu *emu;
        
        emu=gme_load_file(filename, ext);
        
        if(emu)
        {
            GMEFILE *p=(GMEFILE*)malloc(sizeof(GMEFILE));
            
            if(!p) return NULL;
            
            p->type = ZCMF_GME;
            p->playing = ZCM_STOPPED;
            p->emu = emu;
            ZCMUSIC *music=(ZCMUSIC*)p;
            strncpy(music->filename, get_filename(filename), sizeof(music->filename)-1);
            music->track=0;
            return music;
        }
        else Z_message("%s file '%s' not found.\n",ext,filename);
        
    }
    
error:
    return NULL;
}

bool zcmusic_play(ZCMUSIC* zcm, int vol) /* = FALSE */
{
    // the libraries require polling
    // of individual streams, so here we must keep
    // a pointer to the current ZCMUSIC playing.
    //
    // In addition, any music library which actually
    // has a 'play' function or similar will be
    // called from here.
    
    if(zcm == NULL) return FALSE;
    
    int ret = TRUE;
    
    if(zcm->playing != ZCM_STOPPED)                         // adjust volume
    {
        switch(zcm->type)
        {
        case ZCMF_OGG:
            if(((OGGFILE*)zcm)->s != NULL)
            {
                /*pan*/
                alogg_adjust_oggstream(((OGGFILE*)zcm)->s, vol, 128, 1000/*speed*/);
                ((OGGFILE*)zcm)->vol = vol;
            }
            
            break;
            
        case ZCMF_MP3:
            if(((MP3FILE*)zcm)->s != NULL)
            {
                /*pan*/
                almp3_adjust_mp3stream(((MP3FILE*)zcm)->s, vol, 128, 1000/*speed*/);
                ((MP3FILE*)zcm)->vol = vol;
            }
            
            break;
            
        case ZCMF_GME:
            // need to figure out volume switch
            break;
        }
    }
    else
    {
        switch(zcm->type)
        {
        case ZCMF_OGG:
            if(((OGGFILE*)zcm)->s != NULL)
            {
                if(alogg_play_oggstream(((OGGFILE*)zcm)->s, (ZCMUSIC_BUFSZ*1024), vol, 128) != ALOGG_OK)
                    ret = FALSE;
                    
                ((OGGFILE*)zcm)->vol = vol;
            }
            else
            {
                ret = FALSE;
            }
            
            break;
            
        case ZCMF_MP3:
            if(((MP3FILE*)zcm)->s != NULL)
            {
                if(almp3_play_mp3stream(((MP3FILE*)zcm)->s, (ZCMUSIC_BUFSZ*1024), vol, 128) != ALMP3_OK)
                    ret = FALSE;
                    
                ((MP3FILE*)zcm)->vol = vol;
            }
            else
            {
                ret = FALSE;
            }
            
            break;
            
        case ZCMF_GME:
            if(((GMEFILE*)zcm)->emu != NULL)
            {
                gme_play((GMEFILE*) zcm, vol);
            }
            
            break;
        }
        
        if(ret != FALSE)
        {
            zcm->position=0;
            zcm->playing = ZCM_PLAYING;
            cur_zcm=zcm;
        }
    }
    
    return ret!=0;
}

bool zcmusic_pause(ZCMUSIC* zcm, int pause = -1)
{
    // This function suspends play of the music indicated
    // by 'zcm'. Passing 0 for pause will resume; passing
    // -1 (or if the default argument is invoked) will
    // toggle the current state; passing 1 will pause.
    if(zcm == NULL) return FALSE;
    
    if(zcm->playing != ZCM_STOPPED)
    {
        int p = ZCM_PLAYING;
        
        switch(pause)
        {
        case ZCM_TOGGLE:
            p = (zcm->playing == ZCM_PAUSED) ? ZCM_PLAYING : ZCM_PAUSED;
            break;
            
        case ZCM_RESUME:
            p = ZCM_PLAYING;
            break;
            
        case ZCM_PAUSE:
            p = ZCM_PAUSED;
            break;
        }
        
        if(p != zcm->playing)                                 // if the state has actually changed
        {
            zcm->playing = p;
            
            switch(zcm->type)
            {
            case ZCMF_OGG:
                if(p == ZCM_PAUSED)
                    ogg_pause((OGGFILE*)zcm);
                else
                    ogg_resume((OGGFILE*)zcm);
                    
                break;
                
            case ZCMF_MP3:
                if(p == ZCM_PAUSED)
                    mp3_pause((MP3FILE*)zcm);
                else
                    mp3_resume((MP3FILE*)zcm);
                    
                break;
                
            case ZCMF_GME:
                if(((GMEFILE*)zcm)->emu != NULL)
                {
                    if(p == ZCM_PAUSED)
                    {
                        voice_stop(((GMEFILE*)zcm)->stream->voice);
                    }
                    else
                    {
                        voice_start(((GMEFILE*)zcm)->stream->voice);
                    }
                    
                    break;
                }
                
            }
        }
    }
    
    return TRUE;
}

bool zcmusic_stop(ZCMUSIC* zcm)
{
    // this function will stop playback of 'zcm' and reset
    // the stream position to the beginning.
    if(zcm == NULL) return FALSE;
    
    switch(zcm->type)
    {
    case ZCMF_OGG:
        ogg_stop((OGGFILE*)zcm);
        break;
        
    case ZCMF_MP3:
        mp3_stop((MP3FILE*)zcm);
        break;
        
    case ZCMF_GME:
        if(((GMEFILE*)zcm)->emu != NULL)
        {
            if(zcm->playing != ZCM_STOPPED) stop_audio_stream(((GMEFILE*)zcm)->stream);
            
            zcm->playing = ZCM_STOPPED;
        }
        
        break;
        
    }
    
    return TRUE;
}

void zcmusic_unload_file(ZCMUSIC* zcm)
{
    // this will unload and destroy all of the data/etc.
    // associated with 'zcm'.

    if(zcm == NULL) return;
    
    switch(zcm->type)
    {
    case ZCMF_OGG:
        unload_ogg_file((OGGFILE*)zcm);
        break;
        
    case ZCMF_MP3:
        unload_mp3_file((MP3FILE*)zcm);
        break;
        
    case ZCMF_GME:
        unload_gme_file((GMEFILE*)zcm);
        break;
    }

    return;
}

int zcmusic_get_tracks(ZCMUSIC* zcm)
{
    if(zcm == NULL) return 0;
    
    switch(zcm->type)
    {
    case ZCMF_OGG:
    case ZCMF_MP3:
        return 0;
        break;
        
    case ZCMF_GME:
        if(((GMEFILE*)zcm)->emu != NULL)
        {
            int t=((GMEFILE*)zcm)->emu->track_count();
            return (t>1)?t:0;
        }
        else
        {
            return 0;
        }
        
        break;
    }
    
    return 0;
}

int zcmusic_change_track(ZCMUSIC* zcm, int tracknum)
{
    if(zcm == NULL) return -1;
    
    switch(zcm->type)
    {
    case ZCMF_OGG:
    case ZCMF_MP3:
        return -1;
        break;
        
    case ZCMF_GME:
        if(((GMEFILE*)zcm)->emu != NULL)
        {
            int t=((GMEFILE*)zcm)->emu->track_count();
            
            if(tracknum<0 || tracknum>=t)
            {
                tracknum=0;
            }
            
            ((GMEFILE*)zcm)->emu->start_track(tracknum);
            zcm->track=tracknum;
            return tracknum;
        }
        else
        {
            return -1;
        }
        
        break;
    }
    
    return 0;
}


MP3FILE *load_mp3_file(char *filename)
{
    MP3FILE *p = NULL;
    PACKFILE *f = NULL;
    ALMP3_MP3STREAM *s = NULL;
    char *data = new char[(ZCMUSIC_BUFSZ*512)];
    int len;
    
    if((p = (MP3FILE *)malloc(sizeof(MP3FILE)))==NULL)
        goto error;
        
    if((f = pack_fopen(filename, F_READ))==NULL)
        goto error;
    
    // ID3 tags sometimes cause problems with almp3, so try to skip them
    if((len = pack_fread(data, 10, f)) <= 0)
        goto error;
    
    if(strncmp(data, "ID3", 3)==0)
    {
        int id3Size=10;
        
        id3Size=((data[6]&0x7F)<<21)|((data[7]&0x7F)<<14)|((data[8]&0x7F)<<7)|(data[9]&0x7F);
        pack_fseek(f, id3Size-10);
        if((len = pack_fread(data, (ZCMUSIC_BUFSZ*512), f)) <= 0)
            goto error;
    }
    else // no ID3
    {
        if((len = pack_fread(data+10, (ZCMUSIC_BUFSZ*512)-10, f)) <= 0)
            goto error;
    }
    
    if(len < (ZCMUSIC_BUFSZ*512))
    {
        if((s = almp3_create_mp3stream(data, len, TRUE))==NULL)
            goto error;
    }
    else
    {
        if((s = almp3_create_mp3stream(data, (ZCMUSIC_BUFSZ*512), FALSE))==NULL)
            goto error;
    }
    
    p->f = f;
    p->s = s;
    delete[] data;
    return p;
    
error:

    if(f)
        pack_fclose(f);
        
    if(p)
        free(p);
        
    delete[] data;
    return NULL;
}

int poll_mp3_file(MP3FILE *mp3)
{
    if(mp3 == NULL) return ALMP3_POLL_NOTPLAYING;
    
    if(mp3->s == NULL) return ALMP3_POLL_NOTPLAYING;
    
    char *data = (char *)almp3_get_mp3stream_buffer(mp3->s);
    
    if(data)
    {
        long len = pack_fread(data, (ZCMUSIC_BUFSZ*512), mp3->f);
        
        if(len < (ZCMUSIC_BUFSZ*512))
            almp3_free_mp3stream_buffer(mp3->s, len);
        else
            almp3_free_mp3stream_buffer(mp3->s, -1);
    }
    
    int ret = almp3_poll_mp3stream(mp3->s);
    
    if(ret != ALMP3_OK)
    {
        mp3_reset(mp3);
        almp3_play_mp3stream(mp3->s, (ZCMUSIC_BUFSZ*1024), mp3->vol, 128);
        mp3->playing = ZCM_PLAYING;
    }
    
    return ret;
}

void unload_mp3_file(MP3FILE *mp3)
{
    if(mp3 != NULL)
    {
        if(mp3->f != NULL)
        {
            pack_fclose(mp3->f);
            mp3->f = NULL;
        }
        
        if(mp3->s != NULL)
        {
            AUDIOSTREAM* a = almp3_get_audiostream_mp3stream(mp3->s);
            
            if(a != NULL)
                voice_stop(a->voice);
                
            almp3_destroy_mp3stream(mp3->s);
            mp3->s = NULL;
        }
        
        if(mp3->fname != NULL)
        {
            free(mp3->fname);
            free(mp3);
        }
    }
}

bool mp3_pause(MP3FILE *mp3)
{
    AUDIOSTREAM* a = NULL;
    
    if(mp3->s != NULL)
        a = almp3_get_audiostream_mp3stream(mp3->s);
        
    if(a != NULL)
    {
        voice_stop(a->voice);
        return true;
    }
    
    return false;
}

bool mp3_resume(MP3FILE *mp3)
{
    AUDIOSTREAM* a = NULL;
    
    if(mp3->s != NULL)
        a = almp3_get_audiostream_mp3stream(mp3->s);
        
    if(a != NULL)
    {
        voice_start(a->voice);
        return true;
    }
    
    return false;
}

bool mp3_reset(MP3FILE *mp3)
{
    if(mp3->fname != NULL)
    {
        if(mp3->f != NULL)
        {
            pack_fclose(mp3->f);
            mp3->f = NULL;
        }
        
        if(mp3->s != NULL)
        {
            AUDIOSTREAM* a = almp3_get_audiostream_mp3stream(mp3->s);
            
            if(a != NULL)
                voice_stop(a->voice);
                
            almp3_destroy_mp3stream(mp3->s);
            mp3->s = NULL;
        }
        
        MP3FILE* tmp3 = load_mp3_file(mp3->fname);
        
        if(tmp3 != NULL)
        {
            mp3->playing = ZCM_STOPPED;
            mp3->s = tmp3->s;
            mp3->f = tmp3->f;
            free(tmp3);
            return true;
        }
    }
    
    return false;
}

void mp3_stop(MP3FILE *mp3)
{
    if(mp3->fname != NULL)
    {
        if(mp3->f != NULL)
        {
            pack_fclose(mp3->f);
            mp3->f = NULL;
        }
        
        if(mp3->s != NULL)
        {
            AUDIOSTREAM* a = almp3_get_audiostream_mp3stream(mp3->s);
            
            if(a != NULL)
                voice_stop(a->voice);
                
            almp3_destroy_mp3stream(mp3->s);
            mp3->s = NULL;
        }
    }
}

OGGFILE *load_ogg_file(char *filename)
{
    OGGFILE *p = NULL;
    PACKFILE *f = NULL;
    ALOGG_OGGSTREAM *s = NULL;
    char *data = new char[(ZCMUSIC_BUFSZ*512)];
    int len;
    
    if((p = (OGGFILE *)malloc(sizeof(OGGFILE)))==NULL)
    {
        goto error;
    }
    
    if((f = pack_fopen(filename, F_READ))==NULL)
    {
        goto error;
    }
    
    if((len = pack_fread(data, (ZCMUSIC_BUFSZ*512), f)) <= 0)
    {
        goto error;
    }
    
    if(len < (ZCMUSIC_BUFSZ*512))
    {
        if((s = alogg_create_oggstream(data, len, TRUE))==NULL)
        {
            goto error;
        }
    }
    else
    {
        if((s = alogg_create_oggstream(data, (ZCMUSIC_BUFSZ*512), FALSE))==NULL)
        {
            goto error;
        }
    }
    
    p->f = f;
    p->s = s;
    delete[] data;
    return p;
    
error:

    if(f)
        pack_fclose(f);
        
    if(p)
        free(p);
        
    delete[] data;
    return NULL;
}

int poll_ogg_file(OGGFILE *ogg)
{
    if(ogg == NULL) return ALOGG_POLL_NOTPLAYING;
    
    if(ogg->s == NULL) return ALOGG_POLL_NOTPLAYING;
    
    char *data = (char *)alogg_get_oggstream_buffer(ogg->s);
    
    if(data)
    {
        long len = pack_fread(data, (ZCMUSIC_BUFSZ*512), ogg->f);
        
        if(len < (ZCMUSIC_BUFSZ*512))
            alogg_free_oggstream_buffer(ogg->s, len);
        else
            alogg_free_oggstream_buffer(ogg->s, -1);
    }
    
    int ret = alogg_poll_oggstream(ogg->s);
    
    if(ret != ALOGG_OK)
    {
        ogg_reset(ogg);
        alogg_play_oggstream(ogg->s, (ZCMUSIC_BUFSZ*1024), ogg->vol, 128);
        ogg->playing = ZCM_PLAYING;
    }
    
    return ret;
}

void unload_ogg_file(OGGFILE *ogg)
{
    if(ogg != NULL)
    {
        if(ogg->f != NULL)
        {
            pack_fclose(ogg->f);
            ogg->f = NULL;
        }
        
        if(ogg->s != NULL)
        {
            AUDIOSTREAM* a = alogg_get_audiostream_oggstream(ogg->s);
            
            if(a != NULL)
                voice_stop(a->voice);
                
            alogg_destroy_oggstream(ogg->s);
            ogg->s = NULL;
        }
        
        if(ogg->fname != NULL)
        {
            free(ogg->fname);
            free(ogg);
        }
    }
}

bool ogg_pause(OGGFILE *ogg)
{
    AUDIOSTREAM* a = NULL;
    
    if(ogg->s != NULL)
        a = alogg_get_audiostream_oggstream(ogg->s);
        
    if(a != NULL)
    {
        voice_stop(a->voice);
        return true;
    }
    
    return false;
}

bool ogg_resume(OGGFILE *ogg)
{
    AUDIOSTREAM* a = NULL;
    
    if(ogg->s != NULL)
        a = alogg_get_audiostream_oggstream(ogg->s);
        
    if(a != NULL)
    {
        voice_start(a->voice);
        return true;
    }
    
    return false;
}

bool ogg_reset(OGGFILE *ogg)
{
    if(ogg->fname != NULL)
    {
        if(ogg->f != NULL)
        {
            pack_fclose(ogg->f);
            ogg->f = NULL;
        }
        
        if(ogg->s != NULL)
        {
            AUDIOSTREAM* a = alogg_get_audiostream_oggstream(ogg->s);
            
            if(a != NULL)
                voice_stop(a->voice);
                
            alogg_destroy_oggstream(ogg->s);
            ogg->s = NULL;
        }
        
        OGGFILE* togg = load_ogg_file(ogg->fname);
        
        if(togg != NULL)
        {
            ogg->playing = ZCM_STOPPED;
            ogg->s = togg->s;
            ogg->f = togg->f;
            free(togg);
            return true;
        }
    }
    
    return false;
}

void ogg_stop(OGGFILE *ogg)
{
    if(ogg->fname != NULL)
    {
        if(ogg->f != NULL)
        {
            pack_fclose(ogg->f);
            ogg->f = NULL;
        }
        
        if(ogg->s != NULL)
        {
            AUDIOSTREAM* a = alogg_get_audiostream_oggstream(ogg->s);
            
            if(a != NULL)
                voice_stop(a->voice);
                
            alogg_destroy_oggstream(ogg->s);
            ogg->s = NULL;
        }
    }
}

int poll_gme_file(GMEFILE* gme)
{
    unsigned char *p;
    p = (unsigned char*) get_audio_stream_buffer(gme->stream);
    
    if(p)
    {
        int samples=gme->samples;
        memset(p,0,4*samples);
        unsigned short *q=(unsigned short*) p;
        gme->emu->play((long) 2*samples,(short*)p);
        
        // Allegro only uses UNSIGNED samples ...
        for(int j=0; j<2*samples; ++j)
        {
            *q ^= 0x8000;
            q++;
        }
        
        free_audio_stream_buffer(gme->stream);
        return true;
    }
    
    return false;
}


Music_Emu* gme_load_file(char* filename, char* ext)
{
    Music_Emu* emu=NULL;
    
    if(stricmp(ext,"spc")==0) emu = new Spc_Emu;
    
    if(stricmp(ext,"gbs")==0) emu = new Gbs_Emu;
    
    if(stricmp(ext,"nsf")==0) emu = new Nsf_Emu;
    
    if(!emu) return NULL;
    
    Std_File_Reader in;
    
    const char* err = emu->set_sample_rate(GME_SAMPLES);
    
    if(!err) err = in.open(filename);
    
    if(!err) err = emu->load(in);
    
    if(err)
    {
        delete emu;
        return NULL;
    }
    
    return emu;
}

int gme_play(GMEFILE *gme, int vol)
{
    gme->emu->start_track(0);
    int samples=512;
    int buf_size=2*GME_SAMPLES/50;
    
    while(samples < buf_size) samples *= 2;
    
    gme->samples=samples;
    
    if(gme->playing != ZCM_STOPPED) stop_audio_stream(gme->stream);
    
    gme->stream = play_audio_stream(samples, 16, TRUE, GME_SAMPLES, vol, 128);
    return true;
}

int unload_gme_file(GMEFILE* gme)
{
    if(gme!=NULL)
    {
        if(gme->emu != NULL)
        {
            zcmusic_stop(gme);
            delete gme->emu;
            gme->emu=NULL;
            free(gme);
        }
    }
    
    return true;
}
