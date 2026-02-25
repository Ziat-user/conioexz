#pragma once
#include <windows.h>
#include <xinput.h>
#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "winmm.lib")

// =============================================================================
//  conioziat.h  （C++17 以上が必要）
//  コンソールゲーム向けユーティリティヘッダ
//  複数 .cpp から include しても安全（inline 変数により TU 間で状態を共有）
// =============================================================================

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

// =============================================================================
//  サウンド
// =============================================================================

// ファイルパス指定で再生  例: playsound_a("se.wav")
inline void playsound_a(const char* filename) {
    PlaySoundA(filename, NULL, SND_FILENAME | SND_ASYNC);
}

// メモリ上の WAV データを再生  例: playsound_mem(my_wav_data)
inline void playsound_mem(const void* filedata) {
    PlaySoundA(reinterpret_cast<LPCSTR>(filedata), NULL, SND_MEMORY | SND_ASYNC);
}

// 再生停止
inline void stopsound() {
    PlaySound(NULL, NULL, 0);
}

// =============================================================================
//  import() ポート定数
//
//  【キーボード / マウスボタン】 0x00〜0xFF
//    Windows 仮想キーコード (VK_*) をそのまま渡す
//    例: import(VK_SPACE), import(VK_LEFT), import(VK_LBUTTON)
//    主な VK コード:
//      VK_LBUTTON  0x01  左クリック
//      VK_RBUTTON  0x02  右クリック
//      VK_MBUTTON  0x04  中クリック
//      VK_BACK     0x08  Backspace
//      VK_TAB      0x09  Tab
//      VK_RETURN   0x0D  Enter
//      VK_SHIFT    0x10  Shift
//      VK_CONTROL  0x11  Ctrl
//      VK_MENU     0x12  Alt
//      VK_ESCAPE   0x1B  Esc
//      VK_SPACE    0x20  Space
//      VK_LEFT     0x25  ←
//      VK_UP       0x26  ↑
//      VK_RIGHT    0x27  →
//      VK_DOWN     0x28  ↓
//      'A'〜'Z'    0x41〜0x5A
//      '0'〜'9'    0x30〜0x39
//      VK_F1〜F12  0x70〜0x7B
//
//  【マウス座標 / 移動量】 0x1000〜0x10FF
//
//  【ゲームパッドアナログ】 0x2000〜0x20FF
//    スティック: -32767〜32767、トリガー: 0〜255
//
//  【ゲームパッドボタン パック】 0x3000
//    全ボタンをビットパックして返す
//    取り出し方: import(PORT_PAD_BUTTONS) & PAD_A
//
//    ビットレイアウト:
//      bit 0  PAD_DPAD_UP
//      bit 1  PAD_DPAD_DOWN
//      bit 2  PAD_DPAD_LEFT
//      bit 3  PAD_DPAD_RIGHT
//      bit 4  PAD_START
//      bit 5  PAD_BACK
//      bit 6  PAD_L3
//      bit 7  PAD_R3
//      bit 8  PAD_LB
//      bit 9  PAD_RB
//      bit 10 PAD_A
//      bit 11 PAD_B
//      bit 12 PAD_X
//      bit 13 PAD_Y
// =============================================================================

// マウス座標・移動量
#define PORT_MOUSE_X        0x1000  // マウス X 座標（スクリーン絶対座標）
#define PORT_MOUSE_Y        0x1001  // マウス Y 座標（スクリーン絶対座標）
#define PORT_MOUSE_DX       0x1002  // 前フレームからの X 移動量
#define PORT_MOUSE_DY       0x1003  // 前フレームからの Y 移動量

// ゲームパッドアナログ
#define PORT_PAD_LX         0x2000  // 左スティック X軸  -32767〜32767
#define PORT_PAD_LY         0x2001  // 左スティック Y軸  -32767〜32767
#define PORT_PAD_RX         0x2002  // 右スティック X軸  -32767〜32767
#define PORT_PAD_RY         0x2003  // 右スティック Y軸  -32767〜32767
#define PORT_PAD_LT         0x2004  // 左トリガー        0〜255
#define PORT_PAD_RT         0x2005  // 右トリガー        0〜255

// ゲームパッドボタン パック（全ボタンをビットパックして返す）
#define PORT_PAD_BUTTONS    0x3000

// ゲームパッドボタン ビットマスク（PORT_PAD_BUTTONS の戻り値に & で使う）
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

// =============================================================================
//  内部状態
//  inline 変数（C++17）: ヘッダに書いても全 TU で同一インスタンスを共有する
//  複数 .cpp から include しても前フレーム情報が正しく共有される
// =============================================================================
inline POINT        _prev_mouse = {};
inline XINPUT_STATE _prev_pad = {};
inline bool         _input_initialized = false;

// 毎フレーム先頭で呼ぶ（前フレームの状態を保存する）
inline void input_update() {
    GetCursorPos(&_prev_mouse);
    XInputGetState(0, &_prev_pad);
    _input_initialized = true;
}

// =============================================================================
//  import() — ポートから入力値を読み取る
// =============================================================================
inline int import(int port) {
    // 初回は自動で初期化（起動直後の差分が異常値になるのを防ぐ）
    if (!_input_initialized) input_update();

    // ── キーボード / マウスボタン（VK コード: 0x00〜0xFF）────────────────
    if (port >= 0x00 && port <= 0xFF) {
        return (GetAsyncKeyState(port) & 0x8000) ? 1 : 0;
    }

    // ── マウス座標 / 移動量（0x1000〜0x10FF）─────────────────────────────
    if (port >= 0x1000 && port <= 0x10FF) {
        POINT now;
        GetCursorPos(&now);
        switch (port) {
        case PORT_MOUSE_X:  return static_cast<int>(now.x);
        case PORT_MOUSE_Y:  return static_cast<int>(now.y);
        case PORT_MOUSE_DX: return static_cast<int>(now.x - _prev_mouse.x);
        case PORT_MOUSE_DY: return static_cast<int>(now.y - _prev_mouse.y);
        }
        return 0;
    }

    // ── ゲームパッドアナログ（0x2000〜0x20FF）────────────────────────────
    if (port >= 0x2000 && port <= 0x20FF) {
        XINPUT_STATE state;
        if (XInputGetState(0, &state) != ERROR_SUCCESS) return 0;
        switch (port) {
        case PORT_PAD_LX: return state.Gamepad.sThumbLX;
        case PORT_PAD_LY: return state.Gamepad.sThumbLY;
        case PORT_PAD_RX: return state.Gamepad.sThumbRX;
        case PORT_PAD_RY: return state.Gamepad.sThumbRY;
        case PORT_PAD_LT: return state.Gamepad.bLeftTrigger;
        case PORT_PAD_RT: return state.Gamepad.bRightTrigger;
        }
        return 0;
    }

    // ── ゲームパッドボタン パック（0x3000）───────────────────────────────
    // 全ボタンをビットパックして返す
    // 取り出し方: import(PORT_PAD_BUTTONS) & PAD_A
    if (port == PORT_PAD_BUTTONS) {
        XINPUT_STATE state;
        if (XInputGetState(0, &state) != ERROR_SUCCESS) return 0;
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

    return 0;
}