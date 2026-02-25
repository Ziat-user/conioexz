# conioexz.h

HAL1年次に配布されるconioexが割と不便なので使う部分だけ作り直してみた

---

## 要件

- Windows
- C++17 以上
- Visual Studio（または MSVC 互換環境）

> **複数の .cpp から include しても安全です。**  
> `inline` 変数（C++17）を使用しているため、TU をまたいでも内部状態が正しく共有されます。

---

## 導入

`conioexz.h` をプロジェクトにコピーして include するだけです。

```cpp
#include "conioexz.h"
```

`winmm.lib` と `xinput.lib` は `#pragma comment` で自動リンクされます。

---

## コンソール操作

```cpp
// カーソルを (x, y) に移動。左上が (0, 0)
gotoxy(10, 5);

// カーソルの表示/非表示
hideCursor(false); // 非表示
hideCursor(true);  // 表示

// マウス入力の有効/無効（クイック編集モードの制御）
// クリックがコンソール選択に奪われる場合は true にする
// 推奨: main の先頭で一度だけ呼ぶ
enableMouseInput(true);
```

---

## サウンド

WAV ファイルの非同期再生に対応しています。

```cpp
// ファイルパス指定で再生
playsound_a("se.wav");

// メモリ上の WAV データを再生（配列化した WAV など）
playsound_mem(my_wav_data);

// 再生停止
stopsound();
```

> PlaySound は同時に1音しか鳴らせません。複数音の同時再生には別の方法が必要です。

---

## 入力: import()

```cpp
int import(int port);
```

ポート番号を渡すと入力値を返します。**毎フレーム先頭で `input_update()` を呼ぶ必要があります。**

```cpp
// メインループ
while (true) {
    input_update(); // 毎フレーム先頭で必ず呼ぶ

    if (import(VK_ESCAPE)) break;
}
```

---

### キーボード / マウスボタン

ポートに Windows 仮想キーコード (`VK_*`) をそのまま渡します。押されていれば `1`、離されていれば `0` を返します。

```cpp
import(VK_SPACE)    // スペース
import(VK_ESCAPE)   // ESC
import(VK_LEFT)     // ←
import('A')         // A キー
import(VK_LBUTTON)  // マウス左クリック
import(VK_RBUTTON)  // マウス右クリック
import(VK_MBUTTON)  // マウス中クリック
```

**主な VK コード一覧**

| 定数 | 値 | キー |
|------|----|------|
| `VK_LBUTTON` | `0x01` | マウス左 |
| `VK_RBUTTON` | `0x02` | マウス右 |
| `VK_MBUTTON` | `0x04` | マウス中 |
| `VK_BACK` | `0x08` | Backspace |
| `VK_RETURN` | `0x0D` | Enter |
| `VK_SHIFT` | `0x10` | Shift |
| `VK_CONTROL` | `0x11` | Ctrl |
| `VK_MENU` | `0x12` | Alt |
| `VK_ESCAPE` | `0x1B` | Esc |
| `VK_SPACE` | `0x20` | Space |
| `VK_LEFT` / `VK_RIGHT` / `VK_UP` / `VK_DOWN` | `0x25〜0x28` | 矢印キー |
| `'A'〜'Z'` | `0x41〜0x5A` | アルファベット |
| `'0'〜'9'` | `0x30〜0x39` | 数字 |
| `VK_F1〜VK_F12` | `0x70〜0x7B` | ファンクションキー |

---

### マウス座標 / 移動量

| ポート | 内容 | 戻り値 |
|--------|------|--------|
| `PORT_MOUSE_X` | マウス X 座標（スクリーン絶対座標） | `int` |
| `PORT_MOUSE_Y` | マウス Y 座標（スクリーン絶対座標） | `int` |
| `PORT_MOUSE_DX` | 前フレームからの X 移動量 | `int` |
| `PORT_MOUSE_DY` | 前フレームからの Y 移動量 | `int` |

```cpp
int mx = import(PORT_MOUSE_X);
int my = import(PORT_MOUSE_Y);
int dx = import(PORT_MOUSE_DX); // 前フレームからの移動量
int dy = import(PORT_MOUSE_DY);
```

---

### ゲームパッド アナログ

XInput 対応ゲームパッドのスティック・トリガー値を返します。未接続の場合は `0` を返します。

| ポート | 内容 | 戻り値 |
|--------|------|--------|
| `PORT_PAD_LX` | 左スティック X 軸 | `-32767〜32767` |
| `PORT_PAD_LY` | 左スティック Y 軸 | `-32767〜32767` |
| `PORT_PAD_RX` | 右スティック X 軸 | `-32767〜32767` |
| `PORT_PAD_RY` | 右スティック Y 軸 | `-32767〜32767` |
| `PORT_PAD_LT` | 左トリガー | `0〜255` |
| `PORT_PAD_RT` | 右トリガー | `0〜255` |

```cpp
int lx = import(PORT_PAD_LX);
int lt = import(PORT_PAD_LT);
```

---

### ゲームパッド ボタン

`import(PORT_PAD_BUTTONS)` で全ボタンをビットパックした値が返ります。  
`PAD_*` マスクで各ボタンを取り出します。

```cpp
int pad = import(PORT_PAD_BUTTONS);

if (pad & PAD_A)     { /* A ボタン */ }
if (pad & PAD_LB)    { /* LB */ }
if (pad & PAD_L3)    { /* 左スティック押し込み */ }

// 複数ボタン同時判定
if ((pad & PAD_LB) && (pad & PAD_RB)) { /* LB + RB 同時押し */ }
```

**ビットレイアウト**

| マスク | bit | ボタン |
|--------|-----|--------|
| `PAD_DPAD_UP` | 0 | 十字キー上 |
| `PAD_DPAD_DOWN` | 1 | 十字キー下 |
| `PAD_DPAD_LEFT` | 2 | 十字キー左 |
| `PAD_DPAD_RIGHT` | 3 | 十字キー右 |
| `PAD_START` | 4 | START |
| `PAD_BACK` | 5 | BACK / SELECT |
| `PAD_L3` | 6 | 左スティック押し込み |
| `PAD_R3` | 7 | 右スティック押し込み |
| `PAD_LB` | 8 | LB |
| `PAD_RB` | 9 | RB |
| `PAD_A` | 10 | A |
| `PAD_B` | 11 | B |
| `PAD_X` | 12 | X |
| `PAD_Y` | 13 | Y |

---

## ポート番号一覧

| 範囲 | デバイス |
|------|---------|
| `0x00〜0xFF` | キーボード / マウスボタン（VK コードそのまま） |
| `0x1000〜0x10FF` | マウス座標・移動量 |
| `0x2000〜0x20FF` | ゲームパッド アナログ |
| `0x3000` | ゲームパッド ボタン パック |

---

## 使用例

```cpp
#include <cstdio>
#include <chrono>
#include "conioziat.h"

int main() {
    hideCursor(false);
    enableMouseInput(true);

    auto fps_s = std::chrono::milliseconds(1000 / 60);
    auto last  = std::chrono::steady_clock::now();

    while (!import(VK_ESCAPE)) {
        auto now = std::chrono::steady_clock::now();
        if (now - last < fps_s) continue;
        last = now;

        input_update(); // 毎フレーム先頭で必ず呼ぶ

        gotoxy(0, 0);

        // キーボード
        printf("SPACE : %d\n", import(VK_SPACE));

        // マウス
        printf("MOUSE : x=%-5d y=%-5d\n", import(PORT_MOUSE_X), import(PORT_MOUSE_Y));

        // ゲームパッド
        int pad = import(PORT_PAD_BUTTONS);
        printf("A     : %d\n", (pad & PAD_A) ? 1 : 0);
        printf("LX    : %d\n", import(PORT_PAD_LX));
    }

    hideCursor(true);
    return 0;
}
```

---
### 具体的変更点  
* inportが誤字だと思ったので直した
* playsoundが重くなるので、windows標準のAPIを使用して軽くなるようにした
* マウスの有効化on offを追加した
* マウス25*80じゃない範囲も取れるようにした
* gotoxyの初期値を0にした
---
おそらく現状様々な不具合があるよ  
マウスの有効範囲とかを25*80にしたほうがいいか迷っているよ  
