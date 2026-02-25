#pragma once
#include <windows.h>
#include <cstdio>
#pragma comment(lib, "winmm.lib")

// =============================================================================
//  conioexz.h  （C++17 以上が必要）
//  コンソールゲーム向けユーティリティヘッダ
//  複数 .cpp から include しても安全（inline 変数により TU 間で状態を共有）
//
//  【互換モード】
//  #define CONIOEX を include 前に定義すると、
//  元の conioex.h の inport() と互換する動作になります。
//    - マウス座標が 1 始まりになる（通常は 0 始まり）
//    - ゲームパッドが joyGetPosEx（MM系 API）になる（通常は XInput）
//    - ゲームパッドのポート体系が inport と同じになる
//        port & 0xfe00 == 0x0200 → ゲームパッド
//        id   = (port & 0x01f0) >> 4  （ジョイスティック ID）
//        func = port & 0x0f
//          0: X軸  1: Y軸  2: Z軸  3: ボタン
//          4: R軸  5: U軸  6: V軸  7: POV
// =============================================================================

#ifndef CONIOEX
#include <xinput.h>
#pragma comment(lib, "xinput.lib")
#endif

// =============================================================================
//  コンソール操作
// =============================================================================

// カーソルを (x, y) に移動。左上が (0, 0)
inline void gotoxy(int x, int y) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h == INVALID_HANDLE_VALUE) return;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(h, &csbi)) return;

    COORD coord;
    coord.X = static_cast<SHORT>(x);
    coord.Y = static_cast<SHORT>(y);

    if (coord.X < 0) coord.X = 0;
    if (coord.Y < 0) coord.Y = 0;
    if (coord.X >= csbi.dwSize.X) coord.X = csbi.dwSize.X - 1;
    if (coord.Y >= csbi.dwSize.Y) coord.Y = csbi.dwSize.Y - 1;

    SetConsoleCursorPosition(h, coord);
}

// true で表示、false で非表示
inline void hideCursor(bool visible) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h == INVALID_HANDLE_VALUE) return;

    CONSOLE_CURSOR_INFO info;
    if (!GetConsoleCursorInfo(h, &info)) info.dwSize = 25;
    info.bVisible = visible ? TRUE : FALSE;
    SetConsoleCursorInfo(h, &info);
}

// コンソールのマウス入力を有効/無効にする
// true  : マウス入力を有効化（クイック編集モードを無効化）
// false : マウス入力を無効化（クイック編集モードを元に戻す）
// 推奨  : main の先頭で一度だけ呼ぶ
inline void enableMouseInput(bool enable) {
    HANDLE hin = GetStdHandle(STD_INPUT_HANDLE);
    if (hin == INVALID_HANDLE_VALUE || hin == nullptr) return;

    DWORD mode = 0;
    if (!GetConsoleMode(hin, &mode)) return;

    if (enable) {
        mode |= ENABLE_MOUSE_INPUT;
        mode |= ENABLE_EXTENDED_FLAGS;
        mode &= ~ENABLE_QUICK_EDIT_MODE;
    }
    else {
        mode &= ~ENABLE_MOUSE_INPUT;
        mode |= ENABLE_QUICK_EDIT_MODE;
    }

    SetConsoleMode(hin, mode);
}

// コンソールウィンドウとバッファを cols x rows にリサイズする
// スクロールバーが出ないようバッファもウィンドウぴったりに設定する
inline void resizeConsole(int cols, int rows) {
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

// =============================================================================
//  コンソール描画ユーティリティ
// =============================================================================

// ── テキストカラー定数 ────────────────────────────────────────────────────────
//  setColor() の fg / bg 引数に使う
//  組み合わせ例: setColor(CON_YELLOW | CON_BRIGHT, CON_BG_BLUE)
#define CON_BLACK        0
#define CON_BLUE         FOREGROUND_BLUE
#define CON_GREEN        FOREGROUND_GREEN
#define CON_CYAN         (FOREGROUND_GREEN | FOREGROUND_BLUE)
#define CON_RED          FOREGROUND_RED
#define CON_MAGENTA      (FOREGROUND_RED  | FOREGROUND_BLUE)
#define CON_YELLOW       (FOREGROUND_RED  | FOREGROUND_GREEN)
#define CON_WHITE        (FOREGROUND_RED  | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define CON_BRIGHT       FOREGROUND_INTENSITY   // 前景色に OR して明るくする
#define CON_BG_BLUE      BACKGROUND_BLUE
#define CON_BG_GREEN     BACKGROUND_GREEN
#define CON_BG_CYAN      (BACKGROUND_GREEN | BACKGROUND_BLUE)
#define CON_BG_RED       BACKGROUND_RED
#define CON_BG_MAGENTA   (BACKGROUND_RED  | BACKGROUND_BLUE)
#define CON_BG_YELLOW    (BACKGROUND_RED  | BACKGROUND_GREEN)
#define CON_BG_WHITE     (BACKGROUND_RED  | BACKGROUND_GREEN | BACKGROUND_BLUE)
#define CON_BG_BRIGHT    BACKGROUND_INTENSITY   // 背景色に OR して明るくする

// テキスト色・背景色を設定する
// fg : 前景色（CON_* 定数）  bg : 背景色（CON_BG_* 定数）
inline void setColor(WORD fg, WORD bg = 0) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h == INVALID_HANDLE_VALUE) return;
    SetConsoleTextAttribute(h, fg | bg);
}

// 白文字・黒背景にリセット
inline void resetColor() {
    setColor(CON_WHITE);
}

// ANSI エスケープで 24bit RGB 色を設定する（Win10 v1511 以降）
// fg_r/g/b : 前景色（0〜255）  bg_r/g/b : 背景色（負の値を渡すとスキップ）
inline void setColorRGB(int fg_r, int fg_g, int fg_b,
    int bg_r = -1, int bg_g = -1, int bg_b = -1) {
    static bool vt_enabled = false;
    if (!vt_enabled) {
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        if (GetConsoleMode(h, &mode)) {
            SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
            vt_enabled = true;
        }
    }
    if (fg_r >= 0) printf("\x1b[38;2;%d;%d;%dm", fg_r, fg_g, fg_b);
    if (bg_r >= 0) printf("\x1b[48;2;%d;%d;%dm", bg_r, bg_g, bg_b);
}

// ANSI エスケープで色をリセット
inline void resetColorANSI() {
    printf("\x1b[0m");
}

// 画面全体をクリアしてカーソルを (0,0) に移動する
inline void clrscr() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h == INVALID_HANDLE_VALUE) return;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(h, &csbi)) return;

    DWORD size = csbi.dwSize.X * csbi.dwSize.Y;
    COORD origin = { 0, 0 };
    DWORD written;

    FillConsoleOutputCharacterA(h, ' ', size, origin, &written);
    FillConsoleOutputAttribute(h, csbi.wAttributes, size, origin, &written);
    SetConsoleCursorPosition(h, origin);
}

// 指定行だけをクリアしてカーソルをその行の先頭に移動する
inline void clrline(int y) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h == INVALID_HANDLE_VALUE) return;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(h, &csbi)) return;

    COORD origin = { 0, static_cast<SHORT>(y) };
    DWORD written;
    FillConsoleOutputCharacterA(h, ' ', csbi.dwSize.X, origin, &written);
    FillConsoleOutputAttribute(h, csbi.wAttributes, csbi.dwSize.X, origin, &written);
    SetConsoleCursorPosition(h, origin);
}

// コンソールウィンドウのタイトルを設定する
inline void setTitle(const char* title) {
    SetConsoleTitleA(title);
}

// 指定座標に色付き1文字を描画する
inline void putChar(int x, int y, char ch, WORD fg = CON_WHITE, WORD bg = 0) {
    gotoxy(x, y);
    setColor(fg, bg);
    putchar(ch);
    resetColor();
}

// 指定座標に色付き文字列を描画する
inline void putStr(int x, int y, const char* str, WORD fg = CON_WHITE, WORD bg = 0) {
    gotoxy(x, y);
    setColor(fg, bg);
    printf("%s", str);
    resetColor();
}

// 指定座標に色付き整数を描画する
inline void putInt(int x, int y, int value, WORD fg = CON_WHITE, WORD bg = 0) {
    gotoxy(x, y);
    setColor(fg, bg);
    printf("%d", value);
    resetColor();
}

// =============================================================================
//  サウンド
// =============================================================================

// ファイルパス指定で WAV を非同期再生
inline void playsound_a(const char* filename) {
    PlaySoundA(filename, NULL, SND_FILENAME | SND_ASYNC);
}

// メモリ上の WAV データを非同期再生
inline void playsound_mem(const void* filedata) {
    PlaySoundA(reinterpret_cast<LPCSTR>(filedata), NULL, SND_MEMORY | SND_ASYNC);
}

// 再生停止
inline void stopsound() {
    PlaySound(NULL, NULL, 0);
}

// =============================================================================
//  import() ポート定数
// =============================================================================

// ── 通常モード ──────────────────────────────────────────────────────────────
//
//  【キーボード / マウスボタン】 0x00〜0xFF
//    Windows 仮想キーコード (VK_*) をそのまま渡す
//
//  【マウス座標 / 移動量】 0x1000〜0x10FF
//    マウス座標は 0 始まり（CONIOEX 時は 1 始まり）
//
//  【ゲームパッドアナログ】 0x2000〜0x20FF  ※通常モードのみ
//    スティック: -32767〜32767、トリガー: 0〜255
//    ポート = ベース + PAD_ID(id)
//    例: 2P の左スティックX → import(PORT_PAD_LX + PAD_ID(1))
//
//  【ゲームパッドボタン パック】 0x3000〜0x3003  ※通常モードのみ
//    ポート = PORT_PAD_BUTTONS + パッドID（0〜3）
//
// ── CONIOEX モード ────────────────────────────────────────────────────
//  【ゲームパッド】 port & 0xfe00 == 0x0200
//    id   = (port & 0x01f0) >> 4
//    func = port & 0x0f

#define PORT_MOUSE_X        0x1000
#define PORT_MOUSE_Y        0x1001
#define PORT_MOUSE_DX       0x1002
#define PORT_MOUSE_DY       0x1003

#ifndef CONIOEX

// ゲームパッドアナログ軸（通常モードのみ）
#define PORT_PAD_LX         0x2000  // 左スティック X軸  -32767〜32767
#define PORT_PAD_LY         0x2001  // 左スティック Y軸  -32767〜32767
#define PORT_PAD_RX         0x2002  // 右スティック X軸  -32767〜32767
#define PORT_PAD_RY         0x2003  // 右スティック Y軸  -32767〜32767
#define PORT_PAD_LT         0x2004  // 左トリガー        0〜255
#define PORT_PAD_RT         0x2005  // 右トリガー        0〜255

// マルチパッド用 ID オフセット
// 例: 2P（id=1）の左スティックX → import(PORT_PAD_LX + PAD_ID(1))
#define PAD_ID(id)          ((id) * 0x10)

// ゲームパッドボタン パック（通常モードのみ）
// 例: 1P → import(PORT_PAD_BUTTONS + 0)
//     2P → import(PORT_PAD_BUTTONS + 1)
#define PORT_PAD_BUTTONS    0x3000

// ゲームパッドボタン ビットマスク
#define PAD_DPAD_UP         (1 << 0)
#define PAD_DPAD_DOWN       (1 << 1)
#define PAD_DPAD_LEFT       (1 << 2)
#define PAD_DPAD_RIGHT      (1 << 3)
#define PAD_START           (1 << 4)
#define PAD_BACK            (1 << 5)
#define PAD_L3              (1 << 6)
#define PAD_R3              (1 << 7)
#define PAD_LB              (1 << 8)
#define PAD_RB              (1 << 9)
#define PAD_A               (1 << 10)
#define PAD_B               (1 << 11)
#define PAD_X               (1 << 12)
#define PAD_Y               (1 << 13)

#define PAD_MAX_COUNT       4

#endif // !CONIOEX

// =============================================================================
//  内部状態（inline 変数: C++17 / 全 TU で同一インスタンスを共有）
// =============================================================================
inline POINT _prev_mouse = {};
inline bool  _input_initialized = false;

#ifndef CONIOEX
inline XINPUT_STATE _prev_pad[PAD_MAX_COUNT] = {};
#endif

// 毎フレーム先頭で呼ぶ（前フレームの状態を保存する）
inline void input_update() {
    GetCursorPos(&_prev_mouse);
#ifndef CONIOEX
    for (int i = 0; i < PAD_MAX_COUNT; ++i) {
        XInputGetState(i, &_prev_pad[i]);
    }
#endif
    _input_initialized = true;
}

// =============================================================================
//  import() — ポートから入力値を読み取る
// =============================================================================
inline int import(int port) {
    if (!_input_initialized) input_update();

    // ── キーボード / マウスボタン（VK コード: 0x00〜0xFF）────────────────
    if (port >= 0x00 && port <= 0xFF) {
        return (GetAsyncKeyState(port) & 0x8000) ? 1 : 0;
    }

    // ── マウス座標 / 移動量（0x1000〜0x10FF）─────────────────────────────
    if (port >= 0x1000 && port <= 0x10FF) {
        POINT now;
        GetCursorPos(&now);
#ifdef CONIOEX
        switch (port) {
        case PORT_MOUSE_X: return static_cast<int>(now.x) + 1;
        case PORT_MOUSE_Y: return static_cast<int>(now.y) + 1;
        }
#else
        switch (port) {
        case PORT_MOUSE_X:  return static_cast<int>(now.x);
        case PORT_MOUSE_Y:  return static_cast<int>(now.y);
        case PORT_MOUSE_DX: return static_cast<int>(now.x - _prev_mouse.x);
        case PORT_MOUSE_DY: return static_cast<int>(now.y - _prev_mouse.y);
        }
#endif
        return 0;
    }

#ifdef CONIOEX
    // ── ゲームパッド（CONIOEX: joyGetPosEx / inport 互換）─────────
    if ((port & 0xfe00) == 0x0200) {
        int id = (port & 0x01f0) >> 4;
        int func = port & 0x0f;
        JOYINFOEX ji;
        ji.dwSize = sizeof(JOYINFOEX);
        ji.dwFlags = JOY_RETURNALL;
        if (joyGetPosEx(id, &ji) != JOYERR_NOERROR) return -1;
        switch (func) {
        case 0: return static_cast<int>(ji.dwXpos);
        case 1: return static_cast<int>(ji.dwYpos);
        case 2: return static_cast<int>(ji.dwZpos);
        case 3: return static_cast<int>(ji.dwButtons);
        case 4: return static_cast<int>(ji.dwRpos);
        case 5: return static_cast<int>(ji.dwUpos);
        case 6: return static_cast<int>(ji.dwVpos);
        case 7: return static_cast<int>(ji.dwPOV);
        }
        return 0;
    }

#else
    // ── ゲームパッドアナログ（0x2000〜0x20FF）マルチパッド対応 ──────────
    // ポートレイアウト: 0x2[id * 0x10 + axis]
    //   id   = (port - 0x2000) / 0x10  （パッド番号 0〜3）
    //   axis = (port - 0x2000) % 0x10  （軸の種類）
    if (port >= 0x2000 && port <= 0x20FF) {
        int offset = port - 0x2000;
        int id = offset / 0x10;
        int axis = offset % 0x10;

        if (id < 0 || id >= PAD_MAX_COUNT) return 0;

        XINPUT_STATE state;
        if (XInputGetState(id, &state) != ERROR_SUCCESS) return 0;

        switch (0x2000 + axis) {
        case PORT_PAD_LX: return state.Gamepad.sThumbLX;
        case PORT_PAD_LY: return state.Gamepad.sThumbLY;
        case PORT_PAD_RX: return state.Gamepad.sThumbRX;
        case PORT_PAD_RY: return state.Gamepad.sThumbRY;
        case PORT_PAD_LT: return state.Gamepad.bLeftTrigger;
        case PORT_PAD_RT: return state.Gamepad.bRightTrigger;
        }
        return 0;
    }

    // ── ゲームパッドボタン パック（0x3000〜0x3003）マルチパッド対応 ──────
    if (port >= PORT_PAD_BUTTONS && port < PORT_PAD_BUTTONS + PAD_MAX_COUNT) {
        int id = port - PORT_PAD_BUTTONS;

        XINPUT_STATE state;
        if (XInputGetState(id, &state) != ERROR_SUCCESS) return 0;

        const WORD w = state.Gamepad.wButtons;
        int packed = 0;
        if (w & XINPUT_GAMEPAD_DPAD_UP)       packed |= PAD_DPAD_UP;
        if (w & XINPUT_GAMEPAD_DPAD_DOWN)      packed |= PAD_DPAD_DOWN;
        if (w & XINPUT_GAMEPAD_DPAD_LEFT)      packed |= PAD_DPAD_LEFT;
        if (w & XINPUT_GAMEPAD_DPAD_RIGHT)     packed |= PAD_DPAD_RIGHT;
        if (w & XINPUT_GAMEPAD_START)          packed |= PAD_START;
        if (w & XINPUT_GAMEPAD_BACK)           packed |= PAD_BACK;
        if (w & XINPUT_GAMEPAD_LEFT_THUMB)     packed |= PAD_L3;
        if (w & XINPUT_GAMEPAD_RIGHT_THUMB)    packed |= PAD_R3;
        if (w & XINPUT_GAMEPAD_LEFT_SHOULDER)  packed |= PAD_LB;
        if (w & XINPUT_GAMEPAD_RIGHT_SHOULDER) packed |= PAD_RB;
        if (w & XINPUT_GAMEPAD_A)              packed |= PAD_A;
        if (w & XINPUT_GAMEPAD_B)              packed |= PAD_B;
        if (w & XINPUT_GAMEPAD_X)              packed |= PAD_X;
        if (w & XINPUT_GAMEPAD_Y)              packed |= PAD_Y;
        return packed;
    }

#endif // CONIOEX

    return 0;
}