PREFIX = /usr/local
ARHCH = h8300-elf
BINDIR = $(PREFIX)/bin
ADDNAME = $(ARHCH)-

# root@3056ea74c634:/# /usr/local/bin/h8300-elf-gcc --version
# h8300-elf-gcc (GCC) 3.4.6
AR = $(BINDIR)/$(ADDNAME)ar
AS = $(BINDIR)/$(ADDNAME)as
CC = $(BINDIR)/$(ADDNAME)gcc
LD = $(BINDIR)/$(ADDNAME)ld
NM = $(BINDIR)/$(ADDNAME)nm
OBJCOPY = $(BINDIR)/$(ADDNAME)objcopy
OBJDUMP = $(BINDIR)/$(ADDNAME)objdump
RANLIB = $(BINDIR)/$(ADDNAME)ranlib
STRIP = $(BINDIR)/$(ADDNAME)strip

H8WRITE = ../../tools/h8write/h8write

H8WRITE_SERDEV = /dev/ttyUSB0

OBJS = vector.o startup.o main.o
OBJS += lib.o serial.o

TARGET = kzload

CFLAGS = -Wall -mh -nostdinc -nostdlib -fno-builtin
# CLFAGS += -mint32
CLFAGS += -I.
# CLFAGS += -g
CLFAGS += -Os
CLFAGS += -DKZLOAD

# リンクオプション
LFLAGS = -static -T ld.scr -L.

# なんのオプションか全くわからん。
.SUFFIXES: .c .o
.SUFFIXES: .s .o

all :		$(TARGET)

# cp $(TARGET) $(TARGET).elf の意味がわからん。
# $(STRIP) $(TARGET)
$(TARGET): $(OBJS)
		$(CC) $(OBJS) -o $(TARGET) $(CFLAGS) $(LFLAGS)
		cp $(TARGET) $(TARGET).elf
		$(STRIP) $(TARGET)

# $< ってなんやったっけ？
.c.o : $<
		$(CC) -c $(CFLAGS) $<

.s.o : $<
		$(CC) -c $(CFLAGS) $<

# この箇所の意味がわからん。
$(TARGET).mot: $(TARGET)
		$(OBJCOPY) -O srec $(TARGET) $(TARGET).mot

image: $(TARGET).mot

write: $(TARGET).mot
	$(H8WRITE) -3069 -f20 $(TARGET).mot $(H8WRITE_SERDEV)

clean:
		rm -f $(OBJS) $(TARGET) $(TARGET).elf $(TARGET).mot

# 一応ビルドできるとこまで実装できた。
# マイコンにバイナリを書き込んで検証 / 疑問点の解消 / 書籍のサラッと見直し
