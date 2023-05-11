include ../../EMULib/Rules.Unix

# Depending on your CPU endianess, use either -DLSB_FIRST or -DMSB_FIRST.
# Depending on your X11 display mode, use -DBPP8, -DBPP16, or -DBPP32.
# If you do not use -DBPPx, ColEm will determine mode automatically.
DEFINES+= -DCOLEM -DLSB_FIRST -DZLIB -DDEBUG -DCONDEBUG
CFLAGS += -Wall -I$(LIBZ80)
OBJECTS+= $(Z80) $(SN76489) $(TMS9918) $(AY8910) $(C24XX) $(CRC32) $(HUNT) \
	  $(FDIDISK) ../Coleco.o ../AdamNet.o ../ColEm.o ../Menu.o Unix.o

all:    colem

colem:	Makefile $(OBJECTS)
	$(CC) -o $@ $(CFLAGS) $(OBJECTS) $(LIBS)

clean:
	rm -f $(OBJECTS)

go:
	./colem -ntfs -adam reversi.ddp