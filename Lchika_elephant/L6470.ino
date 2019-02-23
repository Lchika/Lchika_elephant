/* SPI SLAVEに送信する関数 */
/* add_or_val : 送信内容(コマンドor値) */
/* spi_slv : 送信相手のスレーブ選択 */
void L6470_send(unsigned char add_or_val, unsigned char spi_slv){
  if(spi_slv == SPI_SLV_LEFT) {
    digitalWrite(PIN_SPI_SS_LEFT, LOW); 
    SPI.transfer(add_or_val); // アドレスもしくはデータ送信
    digitalWrite(PIN_SPI_SS_LEFT, HIGH);
  } else {
    digitalWrite(PIN_SPI_SS_RIGHT, LOW); 
    SPI.transfer(add_or_val); // アドレスもしくはデータ送信
    digitalWrite(PIN_SPI_SS_RIGHT, HIGH); 
  }
}

/* L6470 の初期設定 */
/* spi_slv : 初期設定するスレーブ選択 */
void L6470_setup(unsigned char spi_slv){
  //加速度
//  L6470_send(0x05, spi_slv);//レジスタアドレス
//  L6470_send(0x4A, spi_slv);//値(12bit),デフォルト0x8A
//  L6470_send(0x06, spi_slv);//レジスタアドレス
//  L6470_send(0x4A, spi_slv);//値(12bit),デフォルト0x8A
  
  //最大回転スピード
  L6470_send(0x07, spi_slv);//レジスタアドレス
  L6470_send(0xFF, spi_slv);//値(10bit),デフォルト0x41
  
  //モータの電圧設定
  L6470_send(0x09, spi_slv);//レジスタアドレス
  L6470_send(0x29, spi_slv);//値(8bit),デフォルト0x29
  L6470_send(0x0A, spi_slv);//レジスタアドレス
  L6470_send(0x29, spi_slv);//値(8bit),デフォルト0x29
  L6470_send(0x0B, spi_slv);//レジスタアドレス
  L6470_send(0x29, spi_slv);//値(8bit),デフォルト0x29
  L6470_send(0x0C, spi_slv);//レジスタアドレス
  L6470_send(0x29, spi_slv);//値(8bit),デフォルト0x29
  
  //フ ル ス テ ッ プ,ハ ー フ ス テ ッ プ,1/4, 1/8,…,1/128 ステップの設定
  L6470_send(0x16, spi_slv);//レジスタアドレス
  L6470_send(0x00, spi_slv);//値(8bit)
} 

/* 両輪を停止する */
void stp(unsigned char kind) {
  if(kind == SOFT_STOP) {
    //減速して停止
    L6470_send(0xB0, SPI_SLV_LEFT);
    L6470_send(0x00, SPI_SLV_LEFT);
    L6470_send(0xB0, SPI_SLV_RIGHT);
    L6470_send(0x00, SPI_SLV_RIGHT);
  } else {
  //急停止
    L6470_send(0xB8, SPI_SLV_LEFT);
    L6470_send(0x00, SPI_SLV_LEFT);
    L6470_send(0xB8, SPI_SLV_RIGHT);
    L6470_send(0x00, SPI_SLV_RIGHT);
  }
}

/* 指定した速度で前進する */
void forward(unsigned int spd){
  L6470_send(0x50, SPI_SLV_LEFT);
  L6470_send(0x00, SPI_SLV_LEFT);
  L6470_send(spd, SPI_SLV_LEFT);
  L6470_send(0x00, SPI_SLV_LEFT);
  L6470_send(0x51, SPI_SLV_RIGHT);
  L6470_send(0x00, SPI_SLV_RIGHT);
  L6470_send(spd, SPI_SLV_RIGHT);
  L6470_send(0x00, SPI_SLV_RIGHT);
}

/* 指定した速度で後進する */
void backward(unsigned int spd){
  L6470_send(0x51, SPI_SLV_LEFT);
  L6470_send(0x00, SPI_SLV_LEFT);
  L6470_send(spd, SPI_SLV_LEFT);
  L6470_send(0x00, SPI_SLV_LEFT);
  L6470_send(0x50, SPI_SLV_RIGHT);
  L6470_send(0x00, SPI_SLV_RIGHT);
  L6470_send(spd, SPI_SLV_RIGHT);
  L6470_send(0x00, SPI_SLV_RIGHT);
}

/* 指定した速度で左方向に旋回する */
void turn_left(unsigned int spd){
  L6470_send(0x50, SPI_SLV_LEFT);
  L6470_send(0x00, SPI_SLV_LEFT);
  L6470_send(spd, SPI_SLV_LEFT);
  L6470_send(0x00, SPI_SLV_LEFT);
  L6470_send(0x50, SPI_SLV_RIGHT);
  L6470_send(0x00, SPI_SLV_RIGHT);
  L6470_send(spd, SPI_SLV_RIGHT);
  L6470_send(0x00, SPI_SLV_RIGHT);
}

/* 指定した速度で右方向に旋回する */
void turn_right(unsigned int spd){
  L6470_send(0x51, SPI_SLV_LEFT);
  L6470_send(0x00, SPI_SLV_LEFT);
  L6470_send(spd, SPI_SLV_LEFT);
  L6470_send(0x00, SPI_SLV_LEFT);
  L6470_send(0x51, SPI_SLV_RIGHT);
  L6470_send(0x00, SPI_SLV_RIGHT);
  L6470_send(spd, SPI_SLV_RIGHT);
  L6470_send(0x00, SPI_SLV_RIGHT);
}

void turn_right_one_side(unsigned int spd){
  L6470_send(0x51, SPI_SLV_LEFT);
  L6470_send(0x00, SPI_SLV_LEFT);
  L6470_send(spd, SPI_SLV_LEFT);
  L6470_send(0x00, SPI_SLV_LEFT);
  // L6470_send(0x50, SPI_SLV_RIGHT);
  // L6470_send(0x00, SPI_SLV_RIGHT);
  // L6470_send(spd, SPI_SLV_RIGHT);
  // L6470_send(0x00, SPI_SLV_RIGHT);
}

/* 指定した速度で左方向に旋回する */
void turn_left_one_side(unsigned int spd){
  // L6470_send(0x51, SPI_SLV_LEFT);
  // L6470_send(0x00, SPI_SLV_LEFT);
  // L6470_send(spd, SPI_SLV_LEFT);
  // L6470_send(0x00, SPI_SLV_LEFT);
  L6470_send(0x51, SPI_SLV_RIGHT);
  L6470_send(0x00, SPI_SLV_RIGHT);
  L6470_send(spd, SPI_SLV_RIGHT);
  L6470_send(0x00, SPI_SLV_RIGHT);
}

/* ボールを探しながら左に旋回する */
int turn_left_with_find(void) {
  int count = 0;

  while(count < head_swing_time) {
    turn_left(0x30);
    delay(10);
    to_ball_find_time += 10;

    if(judge_ball_or_wall() == JUDGE_BALL) {
      stp(HARD_STOP);
      ball_find_direct = LEFT;
      return ball_value;
    }
    count += 10;
  }
  stp(HARD_STOP);
  
  return 0;
}

/* ボールを探しながら右に旋回する */
int turn_right_with_find(void) {
  int count = 0;

  while(count < head_swing_time) {
    turn_right(0x30);
    delay(10);
    to_ball_find_time += 10;
    
    if(judge_ball_or_wall() == JUDGE_BALL) {
      stp(HARD_STOP);
      ball_find_direct = RIGHT;
      return ball_value;
    }
    count += 10;
  }
  stp(HARD_STOP);

  return 0;
}

/* 左に90度旋回する */
void turn_left_90deg(void) {
    turn_left(0x30);
    delay(1350);
    stp(HARD_STOP);
}

/* 右に90度旋回する */
void turn_right_90deg(void) {
    turn_right(0x30);
    delay(1350);
    stp(HARD_STOP);
}
