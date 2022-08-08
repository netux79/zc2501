AUDIO_LIBS = -lgme -lalogg -lalmp3
IMAGE_LIBS = 
#LINKOPTS = -pg -g
#OPTS = -pg -g
OPTS = -O3 -pedantic -Wall

ALLEG_LIB = `allegro-config --libs --static`
SFLAG = -s
ZC_PLATFORM = Linux
CC = g++ 
CFLAG = -std=gnu++11

ZELDA_PREFIX = zelda

ZELDA_EXE = $(ZELDA_PREFIX)$(EXEEXT)

ZELDA_OBJECTS = decorations.o defdata.o ending.o ffscript.o gamedata.o guys.o items.o link.o maps.o pal.o particles.o qst.o scriptdraw.o sprite.o subscr.o tiles.o title.o weapons.o custom.o zcsys.o zcmusic.o zelda.o zscriptversion.o text.o

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

decorations.o: decorations.cpp decorations.h maps.h sfx.h sprite.h custom.h zcsys.h zcmusic.h zdefs.h zelda.h zeldadat.h
	$(CC) $(OPTS) $(CFLAG) -c decorations.cpp -o decorations.o $(SFLAG)
defdata.o: defdata.cpp defdata.h guys.h items.h sfx.h sprite.h weapons.h zdefs.h
	$(CC) $(OPTS) $(CFLAG) -c defdata.cpp -o defdata.o $(SFLAG)
ending.o: ending.cpp ending.h guys.h items.h link.h maps.h pal.h qst.h sfx.h sprite.h subscr.h tiles.h title.h weapons.h custom.h zcsys.h zcmusic.h zdefs.h zelda.h zeldadat.h
	$(CC) $(OPTS) $(CFLAG) -c ending.cpp -o ending.o $(SFLAG)
ffasm.o: ffasm.cpp ffasm.h ffscript.h sprite.h zcmusic.h zdefs.h
	$(CC) $(OPTS) $(CFLAG) -c ffasm.cpp -o ffasm.o $(SFLAG)
ffscript.o: ffscript.cpp ffscript.h guys.h items.h link.h maps.h pal.h qst.h sfx.h sprite.h subscr.h tiles.h weapons.h custom.h zcsys.h zcmusic.h zdefs.h zelda.h zeldadat.h
	$(CC) $(OPTS) $(CFLAG) -c ffscript.cpp -o ffscript.o $(SFLAG)
gamedata.o: gamedata.cpp items.h sfx.h sprite.h zcsys.h zcmusic.h zdefs.h zelda.h zeldadat.h
	$(CC) $(OPTS) $(CFLAG) -c gamedata.cpp -o gamedata.o $(SFLAG)
guys.o: guys.cpp defdata.h ffscript.h guys.h items.h link.h maps.h pal.h qst.h sfx.h sprite.h subscr.h tiles.h weapons.h custom.h zcsys.h zcmusic.h zdefs.h zelda.h zeldadat.h
	$(CC) $(OPTS) $(CFLAG) -c guys.cpp -o guys.o $(SFLAG)
items.o: items.cpp items.h maps.h sfx.h sprite.h zcsys.h zcmusic.h zdefs.h zelda.h zeldadat.h
	$(CC) $(OPTS) $(CFLAG) -c items.cpp -o items.o $(SFLAG)
link.o: link.cpp decorations.h ffscript.h guys.h items.h link.h maps.h pal.h qst.h sfx.h sprite.h subscr.h tiles.h title.h weapons.h custom.h zcsys.h zcmusic.h zdefs.h zelda.h zeldadat.h
	$(CC) $(OPTS) $(CFLAG) -c link.cpp -o link.o $(SFLAG)
maps.o: maps.cpp ffscript.h guys.h items.h link.h maps.h pal.h particles.h qst.h sfx.h sprite.h subscr.h tiles.h weapons.h custom.h zcsys.h zcmusic.h zdefs.h zelda.h zeldadat.h
	$(CC) $(OPTS) $(CFLAG) -c maps.cpp -o maps.o $(SFLAG)
pal.o: pal.cpp items.h link.h maps.h pal.h qst.h sfx.h sprite.h subscr.h tiles.h custom.h zcsys.h zcmusic.h zdefs.h zelda.h zeldadat.h
	$(CC) $(OPTS) $(CFLAG) -c pal.cpp -o pal.o $(SFLAG)
particles.o: particles.cpp particles.h sprite.h zdefs.h
	$(CC) $(OPTS) $(CFLAG) -c particles.cpp -o particles.o $(SFLAG)
qst.o: qst.cpp defdata.h guys.h items.h qst.h sfx.h sprite.h subscr.h tiles.h weapons.h custom.h zcmusic.h zdefs.h
	$(CC) $(OPTS) $(CFLAG) -c qst.cpp -o qst.o $(SFLAG)
scriptdraw.o: scriptdraw.cpp ffscript.h maps.h scriptdraw.h tiles.h zelda.h
	$(CC) $(OPTS) $(CFLAG) -c scriptdraw.cpp -o scriptdraw.o $(SFLAG)
sprite.o: sprite.cpp sprite.h tiles.h zdefs.h
	$(CC) $(OPTS) $(CFLAG) -c sprite.cpp -o sprite.o $(SFLAG)
subscr.o: subscr.cpp guys.h items.h link.h maps.h pal.h qst.h sfx.h sprite.h subscr.h tiles.h weapons.h custom.h zcsys.h zcmusic.h zdefs.h zelda.h zeldadat.h
	$(CC) $(OPTS) $(CFLAG) -c subscr.cpp -o subscr.o $(SFLAG)
tiles.o: tiles.cpp tiles.h zdefs.h
	$(CC) $(OPTS) $(CFLAG) -c tiles.cpp -o tiles.o $(SFLAG)
title.o: title.cpp items.h link.h maps.h pal.h qst.h sfx.h sprite.h subscr.h tiles.h title.h custom.h zcsys.h zcmusic.h zdefs.h zelda.h zeldadat.h
	$(CC) $(OPTS) $(CFLAG) -c title.cpp -o title.o $(SFLAG)
weapons.o: weapons.cpp items.h link.h maps.h pal.h qst.h sfx.h sprite.h subscr.h tiles.h weapons.h custom.h zcsys.h zcmusic.h zdefs.h zelda.h zeldadat.h
	$(CC) $(OPTS) $(CFLAG) -c weapons.cpp -o weapons.o $(SFLAG)
custom.o: custom.cpp sfx.h custom.h zcsys.h zcmusic.h zdefs.h zelda.h zeldadat.h
	$(CC) $(OPTS) $(CFLAG) -c custom.cpp -o custom.o $(SFLAG)
zcsys.o: zcsys.cpp guys.h items.h link.h maps.h pal.h particles.h qst.h sfx.h sprite.h subscr.h tiles.h title.h weapons.h custom.h zcsys.h zcmusic.h zdefs.h zelda.h zeldadat.h
	$(CC) $(OPTS) $(CFLAG) -c zcsys.cpp -o zcsys.o $(SFLAG)
zcmusic.o: zcmusic.cpp zcmusic.h
	$(CC) $(OPTS) $(CFLAG) -c zcmusic.cpp -o zcmusic.o $(SFLAG)
zelda.o: zelda.cpp ending.h ffscript.h fontsdat.h guys.h items.h link.h maps.h pal.h particles.h qst.h sfx.h sprite.h subscr.h tiles.h title.h weapons.h custom.h zcsys.h zcmusic.h zdefs.h zelda.h zeldadat.h
	$(CC) $(OPTS) $(CFLAG) -c zelda.cpp -o zelda.o $(SFLAG)
zscriptversion.o: zscriptversion.cpp zscriptversion.h zelda.h link.h zdefs.h
	$(CC) $(OPTS) $(CFLAG) -c zscriptversion.cpp -o zscriptversion.o $(SFLAG)
text.o: text.cpp zcsys.h zdefs.h text.h
	$(CC) $(OPTS) $(CFLAG) -c text.cpp -o text.o $(SFLAG)
