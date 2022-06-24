AUDIO_LIBS = -lgme -lalogg -lalmp3 -laldmb -ldumb
IMAGE_LIBS = 
#LINKOPTS = -pg -g
#LINKOPTS = -pg
#OPTS = -pg -g
#OPTS = -pg
OPTS = -O3 -pedantic -Wall
#OPTS = -DPTW32_STATIC_LIB -O3

ALLEG_LIB = `allegro-config --libs --static`
SFLAG = -s
ZC_PLATFORM = Linux
CC = g++ 
CFLAG =

ZELDA_PREFIX = zelda

ZELDA_EXE = $(ZELDA_PREFIX)$(EXEEXT)

ZELDA_OBJECTS = colors.o decorations.o defdata.o ending.o ffscript.o gamedata.o guys.o items.o link.o maps.o pal.o particles.o qst.o script_drawing.o sprite.o subscr.o tiles.o title.o weapons.o zc_custom.o zc_items.o zc_sprite.o zc_subscr.o zc_sys.o zcmusic.o zelda.o zscriptversion.o zsys.o

.PHONY: default veryclean clean all msg linux done

default: all
msg:
	@echo Compiling Zelda Classic for $(ZC_PLATFORM)...
done:
	@echo Done!
clean:
	rm -f $(ZELDA_OBJECTS)
veryclean: clean
	rm -f $(ZELDA_EXE)

all: msg $(ZELDA_EXE) done

$(ZELDA_EXE): $(ZELDA_OBJECTS)
	$(CC) $(LINKOPTS) -o $(ZELDA_EXE) $(ZELDA_OBJECTS) $(IMAGE_LIBS) $(AUDIO_LIBS) $(ALLEG_LIB) $(SFLAG)

colors.o: colors.cpp colors.h zdefs.h
	$(CC) $(OPTS) $(CFLAG) -c colors.cpp -o colors.o $(SFLAG)
decorations.o: decorations.cpp decorations.h maps.h sfx.h sprite.h zc_custom.h zc_sys.h zcmusic.h zdefs.h zelda.h zeldadat.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c decorations.cpp -o decorations.o $(SFLAG)
defdata.o: defdata.cpp defdata.h guys.h items.h sfx.h sprite.h weapons.h zdefs.h
	$(CC) $(OPTS) $(CFLAG) -c defdata.cpp -o defdata.o $(SFLAG)
ending.o: ending.cpp colors.h ending.h guys.h items.h link.h maps.h pal.h qst.h sfx.h sprite.h subscr.h tiles.h title.h weapons.h zc_custom.h zc_sys.h zcmusic.h zdefs.h zelda.h zeldadat.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c ending.cpp -o ending.o $(SFLAG)
ffasm.o: ffasm.cpp ffasm.h ffscript.h sprite.h zcmusic.h zdefs.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c ffasm.cpp -o ffasm.o $(SFLAG)
ffscript.o: ffscript.cpp colors.h ffscript.h guys.h items.h link.h maps.h pal.h qst.h sfx.h sprite.h subscr.h tiles.h weapons.h zc_custom.h zc_sys.h zcmusic.h zdefs.h zelda.h zeldadat.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c ffscript.cpp -o ffscript.o $(SFLAG)
gamedata.o: gamedata.cpp items.h sfx.h sprite.h zc_sys.h zcmusic.h zdefs.h zelda.h zeldadat.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c gamedata.cpp -o gamedata.o $(SFLAG)
guys.o: guys.cpp colors.h defdata.h ffscript.h guys.h items.h link.h maps.h pal.h qst.h sfx.h sprite.h subscr.h tiles.h weapons.h zc_custom.h zc_sys.h zcmusic.h zdefs.h zelda.h zeldadat.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c guys.cpp -o guys.o $(SFLAG)
items.o: items.cpp items.h maps.h sfx.h sprite.h zc_sys.h zcmusic.h zdefs.h zelda.h zeldadat.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c items.cpp -o items.o $(SFLAG)
link.o: link.cpp colors.h decorations.h ffscript.h guys.h items.h link.h maps.h pal.h qst.h sfx.h sprite.h subscr.h tiles.h title.h weapons.h zc_custom.h zc_subscr.h zc_sys.h zcmusic.h zdefs.h zelda.h zeldadat.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c link.cpp -o link.o $(SFLAG)
maps.o: maps.cpp colors.h ffscript.h guys.h items.h link.h maps.h pal.h particles.h qst.h sfx.h sprite.h subscr.h tiles.h weapons.h zc_custom.h zc_subscr.h zc_sys.h zcmusic.h zdefs.h zelda.h zeldadat.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c maps.cpp -o maps.o $(SFLAG)
pal.o: pal.cpp colors.h items.h link.h maps.h pal.h qst.h sfx.h sprite.h subscr.h tiles.h zc_custom.h zc_sys.h zcmusic.h zdefs.h zelda.h zeldadat.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c pal.cpp -o pal.o $(SFLAG)
particles.o: particles.cpp particles.h sprite.h zdefs.h
	$(CC) $(OPTS) $(CFLAG) -c particles.cpp -o particles.o $(SFLAG)
qst.o: qst.cpp colors.h defdata.h guys.h items.h qst.h sfx.h sprite.h subscr.h tiles.h weapons.h zc_custom.h zcmusic.h zdefs.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c qst.cpp -o qst.o $(SFLAG)
script_drawing.o: script_drawing.cpp ffscript.h maps.h rendertarget.h script_drawing.h tiles.h zelda.h
	$(CC) $(OPTS) $(CFLAG) -c script_drawing.cpp -o script_drawing.o $(SFLAG)
sprite.o: sprite.cpp sprite.h tiles.h zdefs.h
	$(CC) $(OPTS) $(CFLAG) -c sprite.cpp -o sprite.o $(SFLAG)
subscr.o: subscr.cpp colors.h guys.h items.h link.h maps.h pal.h qst.h sfx.h sprite.h subscr.h tiles.h weapons.h zc_custom.h zc_sys.h zcmusic.h zdefs.h zelda.h zeldadat.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c subscr.cpp -o subscr.o $(SFLAG)
tiles.o: tiles.cpp tiles.h zdefs.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c tiles.cpp -o tiles.o $(SFLAG)
title.o: title.cpp colors.h items.h link.h maps.h pal.h qst.h sfx.h sprite.h subscr.h tiles.h title.h zc_custom.h zc_sys.h zcmusic.h zdefs.h zelda.h zeldadat.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c title.cpp -o title.o $(SFLAG)
weapons.o: weapons.cpp colors.h items.h link.h maps.h pal.h qst.h sfx.h sprite.h subscr.h tiles.h weapons.h zc_custom.h zc_sys.h zcmusic.h zdefs.h zelda.h zeldadat.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c weapons.cpp -o weapons.o $(SFLAG)
zc_custom.o: zc_custom.cpp sfx.h zc_custom.h zc_sys.h zcmusic.h zdefs.h zelda.h zeldadat.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c zc_custom.cpp -o zc_custom.o $(SFLAG)
zc_items.o: zc_items.cpp guys.h maps.h sfx.h sprite.h weapons.h zc_sys.h zcmusic.h zdefs.h zelda.h zeldadat.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c zc_items.cpp -o zc_items.o $(SFLAG)
zc_sprite.o: zc_sprite.cpp maps.h sfx.h sprite.h tiles.h zc_sys.h zcmusic.h zdefs.h zelda.h zeldadat.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c zc_sprite.cpp -o zc_sprite.o $(SFLAG)
zc_subscr.o: zc_subscr.cpp colors.h guys.h items.h link.h maps.h pal.h qst.h sfx.h sprite.h subscr.h tiles.h weapons.h zc_custom.h zc_subscr.h zc_sys.h zcmusic.h zdefs.h zelda.h zeldadat.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c zc_subscr.cpp -o zc_subscr.o $(SFLAG)
zc_sys.o: zc_sys.cpp colors.h guys.h items.h link.h maps.h pal.h particles.h qst.h sfx.h sprite.h subscr.h tiles.h title.h weapons.h zc_custom.h zc_sys.h zcmusic.h zdefs.h zelda.h zeldadat.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c zc_sys.cpp -o zc_sys.o $(SFLAG)
zcmusic.o: zcmusic.cpp zcmusic.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c zcmusic.cpp -o zcmusic.o $(SFLAG)
zelda.o: zelda.cpp colors.h ending.h ffscript.h fontsdat.h guys.h items.h link.h maps.h pal.h particles.h qst.h sfx.h sprite.h subscr.h tiles.h title.h weapons.h zc_custom.h zc_sys.h zcmusic.h zdefs.h zelda.h zeldadat.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c zelda.cpp -o zelda.o $(SFLAG)
zscriptversion.o: zscriptversion.cpp zscriptversion.h zelda.h link.h zdefs.h
	$(CC) $(OPTS) $(CFLAG) -c zscriptversion.cpp -o zscriptversion.o $(SFLAG)
zsys.o: zsys.cpp zc_sys.h zdefs.h zsys.h
	$(CC) $(OPTS) $(CFLAG) -c zsys.cpp -o zsys.o $(SFLAG)
