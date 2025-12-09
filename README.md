
---

# 📘 MCP4725 EEPROM 電圧書き込みプログラム（Raspberry Pi Pico / C SDK）

このプロジェクトは、**Raspberry Pi Pico** を用いて
I2C 接続の **12bit DAC（MCP4725）** に対し、
**任意の出力電圧を EEPROM に書き込むサンプル**です。

MCP4725 の EEPROM に書き込むことで、
電源を入れ直しても **自動的にその電圧で起動**できるようになります。

---

## ✨ 主な機能

* Raspberry Pi Pico（C SDK）から **MCP4725 に I2C 書き込み**
* EEPROM 書き込みコマンド (`CMD_WRITEDACEEPROM`)
* 電圧値（例：3.0V）から **DAC値(0〜4095)** を計算
* EEPROM 書き込み完了まで **ACK 返答待ち処理** あり
* 完了後、UART に結果を表示

---

## 🔧 使用デバイス

| デバイス                  | 説明                        |
| --------------------- | ------------------------- |
| **Raspberry Pi Pico** | RP2040 マイコン               |
| **MCP4725**           | 12bit DAC（I2C接続・EEPROM内蔵） |
| **I2C接続（SDA/SCL）**    | SDA=GPIO14 / SCL=GPIO15   |

---

## 🔌 配線

| Pico GPIO        | MCP4725 |
| ---------------- | ------- |
| **GPIO 14（SDA）** | SDA     |
| **GPIO 15（SCL）** | SCL     |
| 3.3V             | VCC     |
| GND              | GND     |

Pico の I2C1 を使用しています。

---

## 🔧 パラメータ設定

### 📌 書き込む電圧（ユーザーが変更可能）

```c
#define TARGET_VOLTAGE 3.0f
```

### 📌 MCP4725 の基準電圧（Vref）

```c
#define VREF 5.05f
```

MCP4725 の電源電圧（通常 5V）に合わせて調整してください。

---

## 🧮 DAC 値の計算式

```c
uint16_t dac_val = (uint16_t)((TARGET_VOLTAGE / VREF) * 4095);
```

12bit DAC なので出力範囲は **0〜4095**。

---

## 🧠 EEPROM 書き込みの流れ

1. 書き込みコマンド `0x60` を送信
2. 3byte のパケットで DAC値 + EEPROM 書き込み指示
3. 書き込み中はデバイスが **ACK を返さない（NACKになる）**
4. ACK が返ってくるまでループ（書き込み完了待ち）

コード：

```c
while (i2c_write_blocking(I2C_PORT, MCP4725_ADDR, NULL, 0, false)
       == PICO_ERROR_GENERIC) {
    sleep_ms(1);
}
```

---

## 📄 ビルド方法（Pico SDK）

```bash
mkdir build
cd build
cmake ..
make
```

生成された `.uf2` ファイルを Pico にコピーすると実行できます。

---

## 📤 実行結果の例（UART出力）

```
Wrote 3.00 V (DAC=2431) to EEPROM
```

---

## 📁 プロジェクト構成例

```
├── CMakeLists.txt
├── mcp4725_write_eeprom.c
└── README.md
```

---

## 🧩 カスタマイズのヒント

* `TARGET_VOLTAGE` を変更すると書き込む電圧が変わります
* EEPROM ではなく通常動作レジスタに書き込みたい場合は
  コマンド `CMD_WRITEDACEEPROM` を変更するだけで可能
* 電圧範囲外（0〜Vref）にならないよう安全処理を追加しても良い

---

