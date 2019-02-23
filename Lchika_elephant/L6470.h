#ifndef _L6470_H_
#define _L6470_H_

// ピン設定
#define PIN_SPI_MOSI 51
#define PIN_SPI_MISO 50
#define PIN_SPI_SCK 52
#define PIN_SPI_SS_LEFT 48
#define PIN_SPI_SS_RIGHT 49

// SPI SLAVE選択時に使用
#define SPI_SLV_LEFT 0
#define SPI_SLV_RIGHT 1

//停止方法
#define SOFT_STOP 0
#define HARD_STOP 1

#endif // _L6470_H_