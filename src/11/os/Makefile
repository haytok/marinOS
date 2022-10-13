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

OBJS = startup.o main.o interrupt.o
OBJS += lib.o serial.o
# Sources of marinOS
OBJS += marinos.o syscall.o memory.o test11_1.o test11_2.o

TARGET = marinos

# -mh : H8/300H 用のコードを出力するためのオプション
# -nostdinc : システムのヘッダファイルを使用しないためのオプション
# -nostdlib : システムのライブラリを使用しないためのオプション
# -fno-buildin : コンパイラのビルドイン関数を使用しないためのオプション
# -mint32 : int を 32bit とするためのオプション
CFLAGS = -Wall -mh -nostdinc -nostdlib -fno-builtin
# CFLAGS += -mint32
# -I : ヘッダファイルのの検索先を指定するためのオプション
CFLAGS += -I.
# CFLAGS += -g
# -Os : サイズ制約の最適化を行うためのオプション
CFLAGS += -Os
CFLAGS += -DMARINOS

# リンクオプション
# -static : 全て静的リンクするためのオプション
# -T : リンカスクリプトを指定するためのオプション
# -L : ライブラリの検索先を指定するためのオプション
LFLAGS = -static -T ld.ld -L.

# なんのオプションか全くわからん。
.SUFFIXES: .c .o
.SUFFIXES: .s .o
# いらん気がするねんけど、いるんかな ...
.SUFFIXES: .S .o

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

# いらん気がするねんけど、いるんかな ...
.S.o : $<
		@echo "In .S.o, $< is " $<
		$(CC) -c $(CFLAGS) $<

clean:
		rm -f $(OBJS) $(TARGET) $(TARGET).elf core.*
