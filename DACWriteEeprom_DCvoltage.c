#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c1
#define SDA_PIN  14
#define SCL_PIN  15

#define MCP4725_ADDR 0x60  // デフォルトの7bitアドレス
#define CMD_WRITEDACEEPROM 0x60

// EEPROMに書き込む電圧 [V]3.0f;
#define TARGET_VOLTAGE 3.0f
#define VREF           5.05f

void write_mcp4725_eeprom(i2c_inst_t *i2c, uint8_t addr, uint16_t value);

int main() {
    stdio_init_all();

    // I2C初期化
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    sleep_ms(200);  // 初期化安定化待ち

    // 2.0Vに相当するDAC値を算出
    uint16_t dac_val = (uint16_t)((TARGET_VOLTAGE / VREF) * 4095);
    if (dac_val > 4095) dac_val = 4095;

    write_mcp4725_eeprom(I2C_PORT, MCP4725_ADDR, dac_val);
    // 書き込み完了まで ACK を返すのを待つ
    while (i2c_write_blocking(I2C_PORT, MCP4725_ADDR, NULL, 0, false) == PICO_ERROR_GENERIC) {
       sleep_ms(1);  // 書き込み中はACKなし
    }


    // 完了表示（シリアルなど）
    printf("Wrote %.2f V (DAC=%d) to EEPROM\n", TARGET_VOLTAGE, dac_val);
    
    while (1) {
        tight_loop_contents();  // CPUをアイドル状態にしてループ
    }
    return 0;
}

void write_mcp4725_eeprom(i2c_inst_t *i2c, uint8_t addr, uint16_t value) {
    uint8_t packet[3];
    packet[0] = CMD_WRITEDACEEPROM;
    packet[1] = (value >> 4) & 0xFF;         // 上位8ビット
    packet[2] = (value & 0x0F) << 4;         // 下位4ビットを左詰め

    i2c_write_blocking(i2c, addr, packet, 3, false);
}