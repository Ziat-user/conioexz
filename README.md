# conioexz.h

コンソールゲーム向けユーティリティヘッダ。  
HAL 1年次で配布される `conioex.h` をベースに、使いづらい部分を作り直してみた。

---

## 特徴

- ヘッダオンリー（`.h` をコピーして `#include` するだけ）
- `winmm.lib` / `xinput.lib` は `#pragma comment` で自動リンク
- `inline` 変数（C++17）を使用しているため、複数の `.cpp` から include しても内部状態が正しく共有される
- コンソール操作・サウンド再生・キーボード/マウス/ゲームパッド入力をまとめて提供
- `#define CONIOEX` で元の `conioex.h` の `inport()` と互換する動作に切り替え可能

---

## 要件

- Windows
- C++17 以上
- Visual Studio（または MSVC 互換環境）

---

## 導入

`conioexz.h` をプロジェクトにコピーして include するだけです。

```cpp
#include "conioexz.h"
```

---

## 機能一覧

### コンソール操作

```cpp
gotoxy(10, 5);         // カーソルを (x, y) に移動。左上が (0, 0)
hideCursor(false);     // カーソル非表示
hideCursor(true);      // カーソル表示
enableMouseInput(true); // マウス入力を有効化（クイック編集モードを無効化）
resizeConsole(80, 25); // コンソールを cols x rows にリサイズ
```

### 描画ユーティリティ

```cpp
setColor(CON_YELLOW | CON_BRIGHT, CON_BG_BLUE); // テキスト色・背景色を設定
resetColor();                                    // 白文字・黒背景にリセット
setColorRGB(255, 128, 0);                        // 24bit RGB で色を設定（Win10 v1511 以降）
clrscr();                                        // 画面全体をクリア
clrline(3);                                      // 指定行をクリア
setTitle("My Game");                             // ウィンドウタイトルを設定
putChar(5, 3, '@', CON_CYAN);                    // 座標に色付き1文字を描画
putStr(0, 0, "Hello", CON_WHITE);                // 座標に色付き文字列を描画
putInt(10, 5, score, CON_YELLOW);                // 座標に色付き整数を描画
```

**テキストカラー定数**

| 定数 | 説明 |
|------|------|
| `CON_BLACK` `CON_RED` `CON_GREEN` `CON_YELLOW` | 前景色 |
| `CON_BLUE` `CON_MAGENTA` `CON_CYAN` `CON_WHITE` | 前景色 |
| `CON_BRIGHT` | 前景色に OR して明るくする |
| `CON_BG_RED` `CON_BG_GREEN` ... `CON_BG_WHITE` | 背景色 |
| `CON_BG_BRIGHT` | 背景色に OR して明るくする |

### サウンド

WAV ファイルの非同期再生に対応しています（同時に1音のみ）。

```cpp
playsound_a("se.wav");      // ファイルパス指定で再生
playsound_mem(my_wav_data); // メモリ上の WAV データを再生
stopsound();                // 再生停止
```

### 入力: `import()`

毎フレーム先頭で `input_update()` を呼んだあと、ポート番号を渡して入力値を取得します。

```cpp
while (true) {
    input_update(); // 毎フレーム先頭で必ず呼ぶ

    if (import(VK_ESCAPE)) break;
}
```

#### キーボード / マウスボタン

Windows 仮想キーコード (`VK_*`) をそのまま渡します。押されていれば `1`、離されていれば `0`。

```cpp
import(VK_SPACE)    // スペース
import(VK_LEFT)     // ←
import('A')         // A キー
import(VK_LBUTTON)  // マウス左クリック
import(VK_RBUTTON)  // マウス右クリック
```

**主な VK コード**

| 定数 | キー |
|------|------|
| `VK_LBUTTON` / `VK_RBUTTON` / `VK_MBUTTON` | マウスボタン |
| `VK_BACK` / `VK_RETURN` / `VK_ESCAPE` / `VK_SPACE` | 基本キー |
| `VK_SHIFT` / `VK_CONTROL` / `VK_MENU` | 修飾キー |
| `VK_LEFT` / `VK_RIGHT` / `VK_UP` / `VK_DOWN` | 矢印キー |
| `'A'`〜`'Z'` / `'0'`〜`'9'` | 英数字 |
| `VK_F1`〜`VK_F12` | ファンクションキー |

#### マウス座標 / 移動量

| ポート | 内容 |
|--------|------|
| `PORT_MOUSE_X` / `PORT_MOUSE_Y` | スクリーン絶対座標 |
| `PORT_MOUSE_DX` / `PORT_MOUSE_DY` | 前フレームからの移動量 |

```cpp
int mx = import(PORT_MOUSE_X);
int dy = import(PORT_MOUSE_DY);
```

#### ゲームパッド（XInput）

最大4台に対応しています。未接続の場合は `0` を返します。

**アナログ軸**

| ポート | 内容 | 戻り値 |
|--------|------|--------|
| `PORT_PAD_LX` / `PORT_PAD_LY` | 左スティック | `-32767〜32767` |
| `PORT_PAD_RX` / `PORT_PAD_RY` | 右スティック | `-32767〜32767` |
| `PORT_PAD_LT` / `PORT_PAD_RT` | トリガー | `0〜255` |

2P以降は `PAD_ID(id)` をポートに加算します。

```cpp
import(PORT_PAD_LX);              // 1P 左スティックX
import(PORT_PAD_LX + PAD_ID(1)); // 2P 左スティックX
```

**ボタン**

`import(PORT_PAD_BUTTONS)` でビットパックされた値を取得し、`PAD_*` マスクで各ボタンを判定します。

```cpp
int pad = import(PORT_PAD_BUTTONS); // 1P
if (pad & PAD_A)  { /* A ボタン */ }
if (pad & PAD_LB) { /* LB */ }
if ((pad & PAD_LB) && (pad & PAD_RB)) { /* LB + RB 同時押し */ }

int pad2 = import(PORT_PAD_BUTTONS + 1); // 2P
```

| マスク | ボタン |
|--------|--------|
| `PAD_DPAD_UP` / `DOWN` / `LEFT` / `RIGHT` | 十字キー |
| `PAD_START` / `PAD_BACK` | START / BACK |
| `PAD_L3` / `PAD_R3` | スティック押し込み |
| `PAD_LB` / `PAD_RB` | ショルダーボタン |
| `PAD_A` / `PAD_B` / `PAD_X` / `PAD_Y` | フェイスボタン |

---

## ポート番号一覧

| 範囲 | デバイス |
|------|---------|
| `0x00〜0xFF` | キーボード / マウスボタン（VK コードそのまま） |
| `0x1000〜0x10FF` | マウス座標・移動量 |
| `0x2000〜0x20FF` | ゲームパッド アナログ |
| `0x3000〜0x3003` | ゲームパッド ボタン パック（パッドID 0〜3） |

---

## 互換モード（`CONIOEX`）

`#define CONIOEX` を include より前に定義すると、元の `conioex.h` の `inport()` と互換する動作になります。

- マウス座標が 1 始まりになる
- ゲームパッドが XInput ではなく `joyGetPosEx`（MM 系 API）になる
- ゲームパッドのポート体系が `inport` と同じになる

```cpp
#define CONIOEX
#include "conioexz.h"
```

---

## 使用例

```cpp
#include <cstdio>
#include <chrono>
#include "conioexz.h"

int main() {
    hideCursor(false);
    enableMouseInput(true);

    auto fps_s = std::chrono::milliseconds(1000 / 60);
    auto last  = std::chrono::steady_clock::now();

    while (!import(VK_ESCAPE)) {
        auto now = std::chrono::steady_clock::now();
        if (now - last < fps_s) continue;
        last = now;

        input_update();
        gotoxy(0, 0);

        printf("SPACE : %d\n", import(VK_SPACE));
        printf("MOUSE : x=%-5d y=%-5d\n", import(PORT_MOUSE_X), import(PORT_MOUSE_Y));

        int pad = import(PORT_PAD_BUTTONS);
        printf("A     : %d\n", (pad & PAD_A) ? 1 : 0);
        printf("LX    : %d\n", import(PORT_PAD_LX));
    }

    hideCursor(true);
    return 0;
}
```

---

## 元の `conioex.h` からの変更点

- 関数名 `inport` → `import` に修正（`inport` は誤字と判断）
- `playsound` を Windows 標準 API（`PlaySound`）に置き換え、軽量化
- `enableMouseInput()` でマウス入力の有効/無効を切り替え可能に
- マウス座標を 25×80 の範囲に限定せず、スクリーン全体から取得できるように変更
- `gotoxy()` の座標の基準値を 0 始まりに統一
- 描画ユーティリティ（`setColor`・`putChar`・`clrscr` など）を追加
- XInput によるマルチパッド（最大4台）対応を追加

---

## 既知の問題・TODO

- マウスの有効範囲を 25×80 に制限したほうがよいか検討中
- その他さまざまな不具合が存在する可能性があります

---

## ライセンス
ライセンスなんて存在しません。
自由に使用・改変・再配布できます。
