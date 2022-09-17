#include "defines.h"

extern void start(void);

// 関数へのポインタの配列を定義
// start は startup.s で定義された _start のことである。(リンカスクリプトはあくまでも各オブジェクトファイルを結合しているだけ。)
// マイコンボードの電源を ON にした時に、CPU が一番最初に実行を開始する位置なのが start() という関数である。
// start 関数では sp の設定と、main 関数が実行される。
void (*vectors[])(void) = { start, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
			    NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
			    NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
			    NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
			    NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
			    NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
			    NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
			    NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL };
