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
OBJS += lib.o serial.o xmodem.o elf.o

TARGET = kzload

# -mh : H8/300H 用のコードを出力するためのオプション
# -nostdinc : システムのヘッダファイルを使用しないためのオプション
# -nostdlib : システムのライブラリを使用しないためのオプション
# -fno-buildin : コンパイラのビルドイン関数を使用しないためのオプション
# -mint32 : int を 32bit とするためのオプション
CFLAGS = -Wall -mh -nostdinc -nostdlib -fno-builtin
# CLFAGS += -mint32
# -I : ヘッダファイルのの検索先を指定するためのオプション
CLFAGS += -I.
# CLFAGS += -g
# -Os : サイズ制約の最適化を行うためのオプション
CLFAGS += -Os
CLFAGS += -DKZLOAD

# リンクオプション
# -static : 全て静的リンクするためのオプション
# -T : リンカスクリプトを指定するためのオプション
# -L : ライブラリの検索先を指定するためのオプション
LFLAGS = -static -T ld.scr -L.

# なんのオプションか全くわからん。
.SUFFIXES: .c .o
.SUFFIXES: .s .o

all :		$(TARGET)

# この操作がどのような目的で行なっているかはわかっていない。
$(TARGET): $(OBJS)
		$(CC) $(OBJS) -o $(TARGET) $(CFLAGS) $(LFLAGS)
		cp $(TARGET) $(TARGET).elf
		$(STRIP) $(TARGET)

.c.o : $<
		@echo "In .c.o, $< is " $<
		$(CC) -c $(CFLAGS) $<

.s.o : $<
		@echo "In .s.o, $< is " $<
		$(CC) -c $(CFLAGS) $<

$(TARGET).mot: $(TARGET)
		$(OBJCOPY) -O srec $(TARGET) $(TARGET).mot

image: $(TARGET).mot

write: $(TARGET).mot
	$(H8WRITE) -3069 -f20 $(TARGET).mot $(H8WRITE_SERDEV)

clean:
		rm -f $(OBJS) $(TARGET) $(TARGET).elf $(TARGET).mot core.*
