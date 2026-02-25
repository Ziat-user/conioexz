#include <cstdio>
#include <chrono>
#include "conioexz.h"

int main() {
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
        printf("DELTA  : x=%-5d y=%-5d    \n", import(PORT_MOUSE_DX), import(PORT_MOUSE_DY));
        printf("LEFT   : %d    \n", import(VK_LBUTTON));
        printf("RIGHT  : %d    \n", import(VK_RBUTTON));
        printf("MIDDLE : %d    \n", import(VK_MBUTTON));
        printf("\n");

        // =====================================================================
        //  ゲームパッド（未接続なら全部 0 になる）
        // =====================================================================
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

        printf("ESC で終了    \n");
    }

    hideCursor(true);
    return 0;
}