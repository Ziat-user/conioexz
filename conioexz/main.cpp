#include <cstdio>
#include <chrono>
//#define CONIOEX
#include "conioexz.h"

// コンソールウィンドウを指定サイズにリサイズする
void resizeConsole(int cols, int rows) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h == INVALID_HANDLE_VALUE) return;

    // バッファを先に大きくしないとウィンドウを広げられない
    COORD bufSize = { static_cast<SHORT>(cols), static_cast<SHORT>(rows) };
    SetConsoleScreenBufferSize(h, bufSize);

    SMALL_RECT winRect = { 0, 0,
        static_cast<SHORT>(cols - 1),
        static_cast<SHORT>(rows - 1)
    };
    SetConsoleWindowInfo(h, TRUE, &winRect);

    // バッファをウィンドウぴったりに再設定（スクロールバーを消す）
    SetConsoleScreenBufferSize(h, bufSize);
}

int main() {
#ifdef CONIOEX
    // CONIOEX モード: Keyboard + Mouse のみ表示
    // rows: 11(keyboard) + 7(mouse) + 1(footer) = 19
    resizeConsole(200, 50);
#else
    // 通常モード: Keyboard + Mouse + Gamepad 表示
    // rows: 11(keyboard) + 7(mouse) + 16(gamepad) + 1(footer) = 35
    resizeConsole(200, 50);
#endif

    hideCursor(false);
    enableMouseInput(true);

    auto fps_s = std::chrono::milliseconds(1000 / 60);
    auto last = std::chrono::steady_clock::now();

    while (!import(VK_ESCAPE)) {
        auto now = std::chrono::steady_clock::now();
        if (now - last < fps_s) continue;
        last = now;

        input_update(); // 毎フレーム先頭で必ず呼ぶ

        gotoxy(0, 0);

        // =====================================================================
        //  キーボード
        // =====================================================================
        printf("=== Keyboard ===\n");
        printf("SPACE  : %d    \n", import(VK_SPACE));
        printf("ENTER  : %d    \n", import(VK_RETURN));
        printf("SHIFT  : %d    \n", import(VK_SHIFT));
        printf("CTRL   : %d    \n", import(VK_CONTROL));
        printf("LEFT   : %d    \n", import(VK_LEFT));
        printf("RIGHT  : %d    \n", import(VK_RIGHT));
        printf("UP     : %d    \n", import(VK_UP));
        printf("DOWN   : %d    \n", import(VK_DOWN));
        printf("A key  : %d    \n", import('A'));
        printf("\n");

        // =====================================================================
        //  マウス
        // =====================================================================
        printf("=== Mouse ===\n");
        printf("POS    : x=%-5d y=%-5d    \n", import(PORT_MOUSE_X), import(PORT_MOUSE_Y));
#ifndef CONIOEX
        printf("DELTA  : x=%-5d y=%-5d    \n", import(PORT_MOUSE_DX), import(PORT_MOUSE_DY));
#else
        printf("DELTA  : (COMPAT mode: N/A)    \n");
#endif
        printf("LEFT   : %d    \n", import(VK_LBUTTON));
        printf("RIGHT  : %d    \n", import(VK_RBUTTON));
        printf("MIDDLE : %d    \n", import(VK_MBUTTON));
        printf("\n");

        // =====================================================================
        //  ゲームパッド（通常モードのみ、未接続なら全部 0）
        // =====================================================================
#ifndef CONIOEX
        const int pad = import(PORT_PAD_BUTTONS);
        printf("=== Gamepad ===\n");
        printf("A      : %d    \n", (pad & PAD_A) ? 1 : 0);
        printf("B      : %d    \n", (pad & PAD_B) ? 1 : 0);
        printf("X      : %d    \n", (pad & PAD_X) ? 1 : 0);
        printf("Y      : %d    \n", (pad & PAD_Y) ? 1 : 0);
        printf("LB     : %d    \n", (pad & PAD_LB) ? 1 : 0);
        printf("RB     : %d    \n", (pad & PAD_RB) ? 1 : 0);
        printf("L3     : %d    \n", (pad & PAD_L3) ? 1 : 0);
        printf("R3     : %d    \n", (pad & PAD_R3) ? 1 : 0);
        printf("START  : %d    \n", (pad & PAD_START) ? 1 : 0);
        printf("BACK   : %d    \n", (pad & PAD_BACK) ? 1 : 0);
        printf("DPAD U : %d    \n", (pad & PAD_DPAD_UP) ? 1 : 0);
        printf("DPAD D : %d    \n", (pad & PAD_DPAD_DOWN) ? 1 : 0);
        printf("DPAD L : %d    \n", (pad & PAD_DPAD_LEFT) ? 1 : 0);
        printf("DPAD R : %d    \n", (pad & PAD_DPAD_RIGHT) ? 1 : 0);
        printf("L-Stick: x=%-7d y=%-7d    \n", import(PORT_PAD_LX), import(PORT_PAD_LY));
        printf("R-Stick: x=%-7d y=%-7d    \n", import(PORT_PAD_RX), import(PORT_PAD_RY));
        printf("LT     : %-3d    \n", import(PORT_PAD_LT));
        printf("RT     : %-3d    \n", import(PORT_PAD_RT));
        printf("PACKED : 0x%04X    \n", pad);
        printf("\n");
#endif

        printf("ESC で終了    \n");
    }

    hideCursor(true);
    return 0;
}