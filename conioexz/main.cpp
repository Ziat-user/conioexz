// =============================================================================
//  test_conioexz.cpp
//  conioexz.h の全機能テストプログラム
//
//  ビルド方法（Visual Studio Developer Command Prompt）:
//    cl /std:c++17 /EHsc /W3 test_conioexz.cpp
//
//  または Visual Studio でプロジェクトに追加してそのままビルド可能。
//  conioexz.h と同じフォルダに置くこと。
//
//  操作方法:
//    [Tab] で次のページへ
//    [ESC] で終了
// =============================================================================

#include "conioexz.h"
#include <cstdio>
#include <cstring>

// =============================================================================
//  共通定数
// =============================================================================
static const int W = 80;   // コンソール幅（列数）
static const int H = 30;   // コンソール高さ（行数）

// =============================================================================
//  テストページ
// =============================================================================
enum Page {
    PAGE_INVALID = -1,    // 強制再描画トリガー用（(Page)-1 の代わり）
    PAGE_MENU = 0,
    PAGE_COLOR,           // setColor / putStr / putInt / putChar
    PAGE_COLOR_RGB,       // setColorRGB
    PAGE_CURSOR,          // gotoxy / clrscr / clrline / hideCursor
    PAGE_KEYBOARD,        // import(VK_*)
    PAGE_MOUSE,           // import(PORT_MOUSE_*)
    PAGE_PAD,             // import(PORT_PAD_*) マルチパッド
    PAGE_COUNT
};

static const char* PAGE_NAMES[] = {
    "MENU",
    "1. Color (setColor / putStr / putInt / putChar)",
    "2. Color RGB (setColorRGB)",
    "3. Cursor / Clear (gotoxy / clrscr / clrline)",
    "4. Keyboard (import VK)",
    "5. Mouse (import PORT_MOUSE)",
    "6. Gamepad (import PORT_PAD, multi-pad)",
};

// =============================================================================
//  共通ユーティリティ
// =============================================================================

static void drawHLine(int y, char ch = '-', WORD fg = CON_WHITE) {
    gotoxy(0, y);
    setColor(fg);
    for (int i = 0; i < W; ++i) putchar(ch);
    resetColor();
}

static void drawHeader(const char* title) {
    gotoxy(0, 0);
    setColor(CON_BLACK, CON_BG_CYAN | CON_BG_BRIGHT);
    for (int i = 0; i < W; ++i) putchar(' ');
    putStr(1, 0, "conioexz.h TEST", CON_BLACK, CON_BG_CYAN | CON_BG_BRIGHT);
    putStr(20, 0, title, CON_BLACK, CON_BG_CYAN | CON_BG_BRIGHT);
    resetColor();

    putStr(0, 1, " [Tab] 次のページ   [ESC] 終了", CON_YELLOW);
    drawHLine(2, '=', CON_CYAN);
}

// =============================================================================
//  PAGE_MENU
// =============================================================================
static void testMenu() {
    drawHeader("-- MENU --");
    putStr(2, 4, "このプログラムは conioexz.h の全機能をテストします。", CON_WHITE | CON_BRIGHT);
    putStr(2, 6, "[Tab] キーでページを切り替えてください。", CON_YELLOW);
    drawHLine(8, '-', CON_WHITE);
    for (int i = 1; i < PAGE_COUNT; ++i) {
        putStr(4, 8 + i, PAGE_NAMES[i], CON_CYAN);
    }
    drawHLine(H - 2, '-', CON_WHITE);
    putStr(2, H - 1, "ESC で終了", CON_RED | CON_BRIGHT);
}

// =============================================================================
//  PAGE_COLOR
// =============================================================================
static void testColor() {
    drawHeader(PAGE_NAMES[PAGE_COLOR]);

    putStr(2, 4, "■ 前景色 (setColor / putStr)", CON_WHITE | CON_BRIGHT);
    struct FgSample { WORD fg; const char* name; };
    static const FgSample fgList[] = {
        { CON_BLACK | CON_BRIGHT, "BLACK+BRIGHT " },
        { CON_RED,                  "RED          " },
        { CON_RED | CON_BRIGHT, "RED+BRIGHT   " },
        { CON_GREEN,                "GREEN        " },
        { CON_GREEN | CON_BRIGHT, "GREEN+BRIGHT " },
        { CON_YELLOW,               "YELLOW       " },
        { CON_YELLOW | CON_BRIGHT, "YELLOW+BRIGHT" },
        { CON_BLUE,                 "BLUE         " },
        { CON_BLUE | CON_BRIGHT, "BLUE+BRIGHT  " },
        { CON_MAGENTA,              "MAGENTA      " },
        { CON_CYAN,                 "CYAN         " },
        { CON_WHITE,                "WHITE        " },
        { CON_WHITE | CON_BRIGHT, "WHITE+BRIGHT " },
    };
    for (int i = 0; i < (int)(sizeof(fgList) / sizeof(fgList[0])); ++i) {
        putStr(4, 5 + i, fgList[i].name, fgList[i].fg);
    }

    putStr(25, 4, "■ 背景色 (setColor fg+bg)", CON_WHITE | CON_BRIGHT);
    struct BgSample { WORD fg; WORD bg; const char* name; };
    static const BgSample bgList[] = {
        { CON_WHITE, CON_BG_RED,                  " BG_RED     " },
        { CON_WHITE, CON_BG_GREEN,                " BG_GREEN   " },
        { CON_BLACK, CON_BG_YELLOW,               " BG_YELLOW  " },
        { CON_WHITE, CON_BG_BLUE,                 " BG_BLUE    " },
        { CON_WHITE, CON_BG_MAGENTA,              " BG_MAGENTA " },
        { CON_BLACK, CON_BG_CYAN,                 " BG_CYAN    " },
        { CON_BLACK, CON_BG_WHITE,                " BG_WHITE   " },
        { CON_WHITE, CON_BG_RED | CON_BG_BRIGHT, " BG_RED+B   " },
        { CON_WHITE, CON_BG_GREEN | CON_BG_BRIGHT, " BG_GRN+B   " },
        { CON_BLACK, CON_BG_CYAN | CON_BG_BRIGHT, " BG_CYN+B   " },
    };
    for (int i = 0; i < (int)(sizeof(bgList) / sizeof(bgList[0])); ++i) {
        putStr(25, 5 + i, bgList[i].name, bgList[i].fg, bgList[i].bg);
    }

    putStr(2, 19, "■ putChar", CON_WHITE | CON_BRIGHT);
    const char chars[] = "ABCDEFGHIJKLMNOP";
    for (int i = 0; i < 16; ++i) {
        putChar(4 + i * 2, 20, chars[i], static_cast<WORD>(i + 1));
    }

    putStr(2, 22, "■ putInt", CON_WHITE | CON_BRIGHT);
    for (int i = 0; i < 10; ++i) {
        putInt(4 + i * 5, 23, i * 111, CON_YELLOW | CON_BRIGHT);
    }
}

// =============================================================================
//  PAGE_COLOR_RGB
// =============================================================================
static void testColorRGB() {
    drawHeader(PAGE_NAMES[PAGE_COLOR_RGB]);
    putStr(2, 4, "■ setColorRGB — 24bit RGB グラデーション (Win10 v1511以降)",
        CON_WHITE | CON_BRIGHT);

    putStr(2, 6, "赤->青 前景色グラデーション:", CON_WHITE);
    for (int i = 0; i < 60; ++i) {
        int r = 255 - i * 4; if (r < 0)   r = 0;
        int b = i * 4;       if (b > 255) b = 255;
        setColorRGB(r, 0, b);
        gotoxy(2 + i, 7); putchar('#');
    }
    resetColorANSI();

    putStr(2, 9, "緑->黄 前景色グラデーション:", CON_WHITE);
    for (int i = 0; i < 60; ++i) {
        int r = i * 4; if (r > 255) r = 255;
        setColorRGB(r, 255, 0);
        gotoxy(2 + i, 10); putchar('#');
    }
    resetColorANSI();

    putStr(2, 12, "背景色グラデーション (文字は ' '):", CON_WHITE);
    for (int i = 0; i < 60; ++i) {
        int r = i * 4;       if (r > 255) r = 255;
        int g = 255 - i * 4; if (g < 0)   g = 0;
        setColorRGB(0, 0, 0, r, g, 50);
        gotoxy(2 + i, 13); putchar(' ');
    }
    resetColorANSI();

    putStr(2, 15, "※ ANSI 非対応環境では正しく表示されません", CON_YELLOW);
}

// =============================================================================
//  PAGE_CURSOR
// =============================================================================
static void testCursor() {
    drawHeader(PAGE_NAMES[PAGE_CURSOR]);

    putStr(2, 4, "■ gotoxy — 指定座標に文字を描画", CON_WHITE | CON_BRIGHT);
    for (int y = 6; y <= 14; y += 2) {
        for (int x = 4; x <= 60; x += 4) {
            putChar(x, y, '+', CON_CYAN);
        }
    }
    putStr(2, 16, "上の格子は gotoxy で1文字ずつ描画しています", CON_YELLOW);

    drawHLine(18, '-', CON_WHITE);
    putStr(2, 19, "■ clrline / clrscr のテスト", CON_WHITE | CON_BRIGHT);
    putStr(2, 20, "[C] clrline(20) を実行  |  [X] clrscr() を実行",
        CON_RED | CON_BRIGHT);
    putStr(2, 22, "■ hideCursor(false) でカーソルが消えています", CON_WHITE | CON_BRIGHT);
    putStr(2, 23, "  次のページに移動すると hideCursor(true) で戻ります", CON_YELLOW);
    drawHLine(H - 2, '-', CON_WHITE);
}

// =============================================================================
//  PAGE_KEYBOARD
// =============================================================================
struct KeyEntry { int vk; const char* label; int x; int y; };
static const KeyEntry KEY_LIST[] = {
    { VK_SPACE,   "SPACE",    2,  8 },
    { VK_RETURN,  "ENTER",    2, 10 },
    { VK_ESCAPE,  "ESC",      2, 12 },
    { VK_SHIFT,   "SHIFT",    2, 14 },
    { VK_CONTROL, "CTRL",    20,  8 },
    { VK_MENU,    "ALT",     20, 10 },
    { VK_LEFT,    "LEFT",    20, 12 },
    { VK_RIGHT,   "RIGHT",   20, 14 },
    { VK_UP,      "UP",      38,  8 },
    { VK_DOWN,    "DOWN",    38, 10 },
    { VK_LBUTTON, "LClick",  38, 12 },
    { VK_RBUTTON, "RClick",  38, 14 },
    { 'W',        "W",       56,  8 },
    { 'A',        "A",       56, 10 },
    { 'S',        "S",       56, 12 },
    { 'D',        "D",       56, 14 },
};
static const int KEY_LIST_COUNT = (int)(sizeof(KEY_LIST) / sizeof(KEY_LIST[0]));

static void testKeyboard() {
    drawHeader(PAGE_NAMES[PAGE_KEYBOARD]);
    putStr(2, 4, "■ import(VK_*) — キーボード / マウスボタン入力テスト",
        CON_WHITE | CON_BRIGHT);
    putStr(2, 5, "下記のキーを押すと [ON] に変わります", CON_YELLOW);
    drawHLine(6, '-', CON_WHITE);
    for (int i = 0; i < KEY_LIST_COUNT; ++i) {
        putStr(KEY_LIST[i].x, KEY_LIST[i].y, KEY_LIST[i].label, CON_WHITE);
        putStr(KEY_LIST[i].x + 8, KEY_LIST[i].y, "[   ]", CON_WHITE);
    }
    drawHLine(17, '-', CON_WHITE);
    putStr(2, 18, "■ 押されているキーの VK コード:", CON_WHITE | CON_BRIGHT);
    putStr(4, 19, "---", CON_WHITE);
}

static void updateKeyboard() {
    for (int i = 0; i < KEY_LIST_COUNT; ++i) {
        bool on = import(KEY_LIST[i].vk) != 0;
        putStr(KEY_LIST[i].x + 8, KEY_LIST[i].y,
            on ? "[ON ]" : "[   ]",
            on ? (CON_GREEN | CON_BRIGHT) : CON_WHITE);
    }
    char buf[40];
    bool found = false;
    for (int i = 1; i < 0x100; ++i) {
        if (GetAsyncKeyState(i) & 0x8000) {
            sprintf_s(buf, sizeof(buf), "VK=0x%02X (%3d)   ", i, i);
            putStr(4, 19, buf, CON_CYAN | CON_BRIGHT);
            found = true;
            break;
        }
    }
    if (!found) putStr(4, 19, "                     ", CON_WHITE);
}

// =============================================================================
//  PAGE_MOUSE
// =============================================================================
static void testMouse() {
    drawHeader(PAGE_NAMES[PAGE_MOUSE]);
    putStr(2, 4, "■ import(PORT_MOUSE_*) — マウス座標・移動量テスト",
        CON_WHITE | CON_BRIGHT);
    putStr(2, 5, "マウスを動かすと値が更新されます", CON_YELLOW);
    drawHLine(6, '-', CON_WHITE);
    putStr(2, 8, "PORT_MOUSE_X  (X座標)  :", CON_WHITE);
    putStr(2, 10, "PORT_MOUSE_Y  (Y座標)  :", CON_WHITE);
    putStr(2, 12, "PORT_MOUSE_DX (X移動量):", CON_WHITE);
    putStr(2, 14, "PORT_MOUSE_DY (Y移動量):", CON_WHITE);
    drawHLine(16, '-', CON_WHITE);
    putStr(2, 17, "■ マウスボタン:", CON_WHITE | CON_BRIGHT);
    putStr(2, 18, "左クリック (VK_LBUTTON):", CON_WHITE);
    putStr(2, 19, "右クリック (VK_RBUTTON):", CON_WHITE);
    putStr(2, 20, "中クリック (VK_MBUTTON):", CON_WHITE);
}

// ボタン表示ヘルパー（静的メソッド → MSVC のラムダ制限を回避）
struct BtnHelper {
    static void show(int vk, int x, int y) {
        bool on = import(vk) != 0;
        putStr(x, y,
            on ? " PRESSED " : "         ",
            on ? CON_BLACK : CON_WHITE,
            on ? (CON_BG_GREEN | CON_BG_BRIGHT) : 0);
    }
};

static void updateMouse() {
    char buf[32];

    sprintf_s(buf, sizeof(buf), "%6d", import(PORT_MOUSE_X));
    putStr(28, 8, buf, CON_CYAN | CON_BRIGHT);

    sprintf_s(buf, sizeof(buf), "%6d", import(PORT_MOUSE_Y));
    putStr(28, 10, buf, CON_CYAN | CON_BRIGHT);

    int dx = import(PORT_MOUSE_DX);
    int dy = import(PORT_MOUSE_DY);
    sprintf_s(buf, sizeof(buf), "%+6d", dx);
    putStr(28, 12, buf, (dx != 0) ? (CON_GREEN | CON_BRIGHT) : CON_WHITE);
    sprintf_s(buf, sizeof(buf), "%+6d", dy);
    putStr(28, 14, buf, (dy != 0) ? (CON_GREEN | CON_BRIGHT) : CON_WHITE);

    BtnHelper::show(VK_LBUTTON, 28, 18);
    BtnHelper::show(VK_RBUTTON, 28, 19);
    BtnHelper::show(VK_MBUTTON, 28, 20);
}

// =============================================================================
//  PAGE_PAD
// =============================================================================
struct BtnDraw {
    static void show(int btns, int mask, int x, int y) {
        bool on = (btns & mask) != 0;
        putStr(x, y, on ? "ON " : "-- ",
            on ? (CON_GREEN | CON_BRIGHT) : CON_WHITE);
    }
};

static void testPad() {
    drawHeader(PAGE_NAMES[PAGE_PAD]);
    putStr(2, 4, "■ XInput マルチパッド (最大4台)", CON_WHITE | CON_BRIGHT);
    putStr(2, 5, "コントローラーを接続して各値を確認してください", CON_YELLOW);
    drawHLine(6, '-', CON_WHITE);
    putStr(2, 7, "PAD", CON_WHITE | CON_BRIGHT);
    putStr(8, 7, "接続", CON_WHITE | CON_BRIGHT);
    putStr(14, 7, "LX      LY      RX      RY      LT   RT",
        CON_WHITE | CON_BRIGHT);
    drawHLine(16, '-', CON_WHITE);
    putStr(2, 17, "PAD  A    B    X    Y    LB   RB   L3   R3   ST   BK   U  D  L  R",
        CON_WHITE | CON_BRIGHT);
    drawHLine(H - 4, '-', CON_WHITE);
    putStr(2, H - 3, "スティック: -32767~32767  トリガー: 0~255", CON_CYAN);
}

static void updatePad() {
    char buf[64];
    for (int id = 0; id < PAD_MAX_COUNT; ++id) {
        int row = 8 + id * 2;
        XINPUT_STATE state;
        bool connected = (XInputGetState(id, &state) == ERROR_SUCCESS);

        sprintf_s(buf, sizeof(buf), "%dP", id + 1);
        putStr(2, row, buf, CON_YELLOW | CON_BRIGHT);

        if (connected) {
            putStr(8, row, "OK  ", CON_GREEN | CON_BRIGHT);
            int lx = import(PORT_PAD_LX + PAD_ID(id));
            int ly = import(PORT_PAD_LY + PAD_ID(id));
            int rx = import(PORT_PAD_RX + PAD_ID(id));
            int ry = import(PORT_PAD_RY + PAD_ID(id));
            int lt = import(PORT_PAD_LT + PAD_ID(id));
            int rt = import(PORT_PAD_RT + PAD_ID(id));
            sprintf_s(buf, sizeof(buf), "%7d %7d %7d %7d %4d %4d",
                lx, ly, rx, ry, lt, rt);
            putStr(14, row, buf, CON_CYAN);

            int btns = import(PORT_PAD_BUTTONS + id);
            int brow = 18 + id;
            sprintf_s(buf, sizeof(buf), "%dP", id + 1);
            putStr(2, brow, buf, CON_YELLOW | CON_BRIGHT);
            BtnDraw::show(btns, PAD_A, 6, brow);
            BtnDraw::show(btns, PAD_B, 11, brow);
            BtnDraw::show(btns, PAD_X, 16, brow);
            BtnDraw::show(btns, PAD_Y, 21, brow);
            BtnDraw::show(btns, PAD_LB, 26, brow);
            BtnDraw::show(btns, PAD_RB, 31, brow);
            BtnDraw::show(btns, PAD_L3, 36, brow);
            BtnDraw::show(btns, PAD_R3, 41, brow);
            BtnDraw::show(btns, PAD_START, 46, brow);
            BtnDraw::show(btns, PAD_BACK, 51, brow);
            BtnDraw::show(btns, PAD_DPAD_UP, 56, brow);
            BtnDraw::show(btns, PAD_DPAD_DOWN, 59, brow);
            BtnDraw::show(btns, PAD_DPAD_LEFT, 62, brow);
            BtnDraw::show(btns, PAD_DPAD_RIGHT, 65, brow);
        }
        else {
            putStr(8, row, "----", CON_RED);
            putStr(14, row, "  (未接続)                              ", CON_RED);
            int brow = 18 + id;
            sprintf_s(buf, sizeof(buf), "%dP", id + 1);
            putStr(2, brow, buf, CON_YELLOW | CON_BRIGHT);
            putStr(6, brow,
                "  (未接続)                                            ", CON_RED);
        }
    }
}

// =============================================================================
//  メインループ
// =============================================================================
int main() {
    resizeConsole(W, H);                    // ★ resizeConsole を使用
    setTitle("conioexz.h テストプログラム");
    hideCursor(false);
    enableMouseInput(true);

    Page page = PAGE_MENU;
    Page prevPage = PAGE_INVALID;           // ★ PAGE_INVALID を使用

    bool clrlineReq = false;
    bool clrscrReq = false;

    while (true) {
        input_update();

        // ── ページ切り替え（Tab）─────────────────────────────────────────
        if (import(VK_TAB)) {
            Sleep(150);
            page = static_cast<Page>((static_cast<int>(page) + 1) % PAGE_COUNT);
            prevPage = PAGE_INVALID;
        }

        // ── 終了（ESC）──────────────────────────────────────────────────
        if (import(VK_ESCAPE)) break;

        // ── 静的部分の再描画 ─────────────────────────────────────────────
        if (page != prevPage) {
            clrscr();
            switch (page) {
            case PAGE_MENU:      testMenu();      break;
            case PAGE_COLOR:     testColor();     break;
            case PAGE_COLOR_RGB: testColorRGB();  break;
            case PAGE_CURSOR:    testCursor();    break;
            case PAGE_KEYBOARD:  testKeyboard();  break;
            case PAGE_MOUSE:     testMouse();     break;
            case PAGE_PAD:       testPad();       break;
            default:                              break;
            }
            prevPage = page;
        }

        // ── 毎フレーム更新 ───────────────────────────────────────────────
        switch (page) {
        case PAGE_KEYBOARD:
            updateKeyboard();
            break;

        case PAGE_MOUSE:
            updateMouse();
            break;

        case PAGE_PAD:
            updatePad();
            break;

        case PAGE_CURSOR:
            if (import('C') && !clrlineReq) {
                clrlineReq = true;
                putStr(2, 20, ">>> clrline(20) 実行！ <<<  ", CON_GREEN | CON_BRIGHT);
                Sleep(800);
                clrline(20);
                putStr(2, 20, "(クリアされました)          ", CON_CYAN);
                Sleep(400);
                clrlineReq = false;
            }
            if (import('X') && !clrscrReq) {
                clrscrReq = true;
                Sleep(200);
                clrscr();
                putStr(2, 4, "clrscr() を実行しました。[Tab] で次のページへ。",
                    CON_GREEN | CON_BRIGHT);
                clrscrReq = false;
                prevPage = PAGE_INVALID;
            }
            break;

        default:
            break;
        }

        Sleep(16);   // 約 60 fps
    }

    // ── 終了処理 ────────────────────────────────────────────────────────
    clrscr();
    hideCursor(true);
    resetColor();
    enableMouseInput(false);
    setColor(CON_WHITE | CON_BRIGHT);
    printf("テストを終了しました。\n");
    resetColor();
    return 0;
}