#include <Arduino.h>
#include <SPI.h>
#include "L6470.h"
#include "fantonbo.h"
#include "VarSpeedServo.h"
VarSpeedServo fanServo;
#include <DFRobot_QMC5883.h>
DFRobot_QMC5883 compass;
#include <math.h>

void setup()
{
  //ピン設定
  pinMode(PIN_SPI_MOSI, OUTPUT);
  pinMode(PIN_SPI_MISO, INPUT);
  pinMode(PIN_SPI_SCK, OUTPUT);
  pinMode(PIN_SPI_SS_LEFT, OUTPUT);
  digitalWrite(PIN_SPI_SS_LEFT, HIGH);
  pinMode(PIN_SPI_SS_RIGHT, OUTPUT);
  digitalWrite(PIN_SPI_SS_RIGHT, HIGH);

  pinMode(PIN_VCC, OUTPUT);
  digitalWrite(PIN_VCC, LOW);
  pinMode(PIN_BALL_COLOR_DOUT, INPUT);
  pinMode(PIN_BALL_COLOR_RANGE, OUTPUT);
  pinMode(PIN_BALL_COLOR_CK, OUTPUT);
  pinMode(PIN_BALL_COLOR_GATE, OUTPUT);

  pinMode(PIN_LIMIT_FRONT_LEFT, INPUT);
  pinMode(PIN_LIMIT_FRONT_RIGHT, INPUT);
  pinMode(PIN_LIMIT_BACK_LEFT, INPUT);
  pinMode(PIN_LIMIT_BACK_RIGHT, INPUT);


  //SPI通信
  SPI.begin();
  
  // シリアル通信
  Serial.begin(9600);
  SPI.setDataMode(SPI_MODE3);//SCKの立上りでテータを送受信、アイドル時はpinをＨＩＧＨに設定
  SPI.setBitOrder(MSBFIRST);//MSBから送信

  //前コマンドをＣＬＳ
  L6470_send(0x00, SPI_SLV_LEFT);
  L6470_send(0x00, SPI_SLV_LEFT);
  L6470_send(0x00, SPI_SLV_LEFT);
  L6470_send(0x00, SPI_SLV_LEFT);
  L6470_send(0x00, SPI_SLV_RIGHT);
  L6470_send(0x00, SPI_SLV_RIGHT);
  L6470_send(0x00, SPI_SLV_RIGHT);
  L6470_send(0x00, SPI_SLV_RIGHT);
  stp(HARD_STOP);
   
  //デバイスリセットコマンド
  L6470_send(0xc0, SPI_SLV_LEFT);//ResetRevice
  L6470_setup(SPI_SLV_LEFT);//L6470を設定
  L6470_send(0xc0, SPI_SLV_RIGHT);//ResetRevice
  L6470_setup(SPI_SLV_RIGHT);//L6470を設定

  /* カラーセンサ用LED */
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  // /* コンパス初期化 */
  // compass.begin();
  // if(compass.isHMC()){
  //     compass.setRange(HMC5883L_RANGE_1_3GA);
  //     compass.setMeasurementMode(HMC5883L_CONTINOUS);
  //     compass.setDataRate(HMC5883L_DATARATE_15HZ);
  //     compass.setSamples(HMC5883L_SAMPLES_8);
  // }
  // else if(compass.isQMC()){
  //     compass.setRange(QMC5883_RANGE_2GA);
  //     compass.setMeasurementMode(QMC5883_CONTINOUS); 
  //     compass.setDataRate(QMC5883_DATARATE_50HZ);
  //     compass.setSamples(QMC5883_SAMPLES_8);
  // }

  /* サーボ初期化 */
  fanServo.attach(PIN_FAN_SERVO);
  fanServo.write(0, 20, true);
  delay(500);

  // /* 初期角度のコンパスの値を基準値として記憶しておく */
  // compass_base_deg = read_compass();
}

void loop(){
  static char state = ON_START;
  static int ball_color = RED;
  static int search_result;
  int pos;
  int count;
  int ball_dist;
  int forward_time;
  int center_f;
  static int launch_f = 0;

  count = 0;
  
#if 1
  switch (state) {
    /* スタート位置にいる状態 */
    case ON_START :
      center_f = 0;
      search_time++;
      /* 十回目以降閾値を緩くして探索する */
      if(search_time > 10) {
        search_f= 1;
      }
      launch_f = 0;
      /* センターラインまで前進する */
      // while(center_f < 3) { 
      //   forward_on_line();
      //   if(get_position() != LEFT_CENTER_RIGHT) {
      //     center_f++;
      //   }
      // }
      count = 0;
      forward(0x50);
      delay(1000);
      forward_on_line();
      delay(2500);

      forward_time = 800;
      if(search_time > 9) {
        forward_time = 400;
      }
      while(count < forward_time) {
        forward_on_line();
        delay(100);
        count += 100;
      }
      stp(SOFT_STOP);
      delay(500);
      state = SEARCH_BALL_FORWARD;
      break;

    /* ライン上を前進しながらボール探索する */
    case SEARCH_BALL_FORWARD :
      search_result = search_ball_forward();
      delay(100);

      /* 壁に到達してしまった場合 */
      if(search_result == REACH_TO_WALL) {
        /* 少し後進する */
        backward(0x20);
        delay(1000);
        stp(HARD_STOP);
        launch_f = 0;
        state = RETURN_TO_START;
        break;
      }

      /* ボールが見つかった場合 */
      else if(search_result == BALL_FINDED) {
        state = BACUUM_BALL;
      } 
      
      break;

    /* ライン上をスタート位置に戻りながらボールを探索する */
    case SEARCH_BALL_BACKWARD:
      turn_left_90deg();
      turn_left_90deg();

      /* 前方のリミットスイッチが反応するまで */
      search_result = search_ball_forward();
      delay(500);

      /* 壁に到達してしまった場合 */
      if(search_result == REACH_TO_WALL) {
        backward(0x20);
        delay(1000);
        stp(HARD_STOP);
        state = ON_START;
        break;
      } 

      /* ボールが見つかった場合 */
      if(search_result == BALL_FINDED) {
        state = BACUUM_BALL;
      } 
      break;

    /* ボール吸い込み */
    case BACUUM_BALL :
      bacuum_ball();
      state = JUDGE_BALL_COLOR;
      break;
      
    /* ボールの色を判定する */
    case JUDGE_BALL_COLOR :
      ball_color = get_ball_color();
      delay(100);
      state = RETURN_TO_LINE;
      break;

    /* ライン上に戻る */
    case RETURN_TO_LINE :
      return_to_line(ball_color);
      if(ball_color != NO_BALL) {
        launch_f = 1;
        state = RETURN_TO_START;
      } else {
        state = SEARCH_BALL_FORWARD;
      }
      break;

    /* スタート位置に戻る */
    case RETURN_TO_START :
      if(launch_f == 0) {
        turn_right_90deg();
        turn_right_90deg();
        forward(0x50);
        delay(1000);
      }
      return_to_start();
      if(launch_f == 1){
        state = LAUNCH_BALL;
      } else {
        turn_left_90deg();
        turn_left_90deg();
        state = ON_START;
      }
      break;

    /* ボールを射出する */
    case LAUNCH_BALL :
      launch_ball(ball_color);

      /* スタートに戻る */
      state = ON_START;
      break;

    /* ここにきたらバグ */
    default :
      break;
  }
#endif
}

/* 首振りしながらボールを探す */
int head_swing(void) {
  int ball_dist;
  int count = 0;
  int position;

  /* まずは左方向を探す */
  to_ball_find_time = 0;
  ball_dist = turn_left_with_find();
  stp(HARD_STOP);
  delay(100);

  /* ボールが見つかった場合 */
  if(ball_dist != 0) {
    /* ボールまでの距離を返す */
    return ball_dist;
  } 
  /* ボールが見つからなかった場合 */
  else {
    /* 正面を向く */
    count = 0;

    turn_right(0x30);
    delay(2000);
    do{
      turn_right(0x30);
      position = get_position();
      } while(position != CENTER && position != LEFT_CENTER && position != CENTER_RIGHT);
    stp(HARD_STOP);

    /* 次に右方向を探す */
    to_ball_find_time = 0;
    ball_dist = turn_right_with_find();
    stp(HARD_STOP);
    delay(100);

    /* ボールが見つかった場合 */
    if(ball_dist != 0) {
      return ball_dist;
    }
    /* ボールが見つからなかった場合 */
    else {
      /* 正面を向く */
      count = 0;
      turn_left(0x30);
      delay(2000);
      do{
        turn_left(0x30);
        position = get_position();
      } while(position != CENTER && position != LEFT_CENTER && position != CENTER_RIGHT);
    
      return ball_dist;
    }
  }
}

/* ボールと適切な距離を保つ */
void keep_dist_with_ball(int ball_dist) {
  adapt_backward_value = 0;
  /* ボールに近すぎる場合，一旦後進する */
  while(get_diststate_ball() > NEAR_THRESHOLD) {
    backward(0x20);
    delay(1);
    /* 後進した時間を残しておく */
    adapt_backward_value++;
  }
  stp(HARD_STOP);
  // Serial.print("VALUE1");
  // Serial.println(adapt_backward_value);
}

/* 前進しながらボールを探す */
int search_ball_forward(void) {
  int ball_find_f = BALL_NOT_FINDED;
  int ball_dist;
  int count = 0;
  int pos;
  int threshold;
  int forward_time;

  if(search_f == 0) {
    threshold = WALL;
  } else {
    threshold = WALL_FAR;
  }

  forward_time = TIME_NORMAL;
  if(search_time > 9) {
    forward_time = TIME_SHORT;
  }
  /* 適当な時間前進 */
  while(count < forward_time) {
    /* 壁に到達するまで首振りしながらライン上を前進する */
    forward_on_line();
    pos = arrive_wall_front();
    delay(10);
    /* 壁に到達するまで */
    if(pos == ARRIVED) {
      return REACH_TO_WALL;
    }
    count += 10;
  }
  stp(HARD_STOP);
  delay(100);
    
  /* 首振り */
  ball_dist = head_swing();

  /* ボールが見つかった場合 */
  if(ball_dist > threshold) {
    keep_dist_with_ball(ball_dist);
    ball_find_f = BALL_FINDED;
  }

  return ball_find_f;
}

/* 前進しながらボールを探す2 */
int search_ball_forward_2(void) {
  int reach_to_wall_f = 0; //壁到達フラグ
  int ball_dist;
  int count = 10000;
  
  /* 左方向に90度旋回 */
  turn_left_90deg();
  delay(100);

  ball_dist = head_swing();

  /* ボールが見つかった場合 */
  if(ball_dist > WALL) {
    keep_dist_with_ball(ball_dist);
    ball_find_direct = LEFT;
    return BALL_FINDED;
  }

  /* ボールが見つからなかった場合 */
  /* STEP4: 右方向に180度旋回 */
  turn_right_90deg();
  turn_right_90deg();
  delay(100);

  ball_dist = head_swing();

  /* ボールが見つかった場合 */
  if(ball_dist > WALL) {
    keep_dist_with_ball(ball_dist);
    ball_find_direct = RIGHT;
    return BALL_FINDED;
  }

  /* ボールが見つからなかった場合 */
  /* 正面を向く */
  turn_left_90deg();

  return BALL_NOT_FINDED;
}

/*  ファンで吸い込みながらボールに接近する */
int bacuum_ball(void) {
  int color_val;

  /* 筒を下ろす */
  fanServo.write(70,30, true);
  fanServo.write(80,20, true);
  fanServo.write(90,10, true);
  delay(100);

  /* ファンを回転させる */
  fan_intake();
  delay(300);

  /* 前進する */
  forward(0x30);
  delay(3000);
  stp(HARD_STOP);

  /* 吸い込み切るまで待機 */
  //delay(2000);
  delay(100);

  /* 後進する */
  backward(0x30);
  delay(3000);
  stp(HARD_STOP);

  /* 筒を上げる */
  fanServo.write(0, 30, true);
  // delay(100);

  /* ファンを停止する */
  fan_free();

  /* ボールが停止するまで待機 */
  delay(1000);
}

/* スタート位置に戻る */
void return_to_start(void) {
  /* 前方のリミットセンサが反応するまでライン上を後進する */
  while(arrive_wall_front() == NOT_ARRIVED) {
    forward_on_line();
  }
  backward(0x20);
  delay(1000);
  stp(HARD_STOP);
}

/* ボール発射 */
void launch_ball(int ball_color) {
  /* 台の方向に旋回する */
  /* ボールの色によって左右を切り替える */
  if(ball_color == BLUE) {
    turn_left_90deg();
    turn_right(0x30);
    delay(300);
    stp(HARD_STOP);
  } else {
    turn_right_90deg();
    turn_left(0x30);
    delay(300);
    stp(HARD_STOP);
  }

  /* 筒の角度を調整する */
  fanServo.write(40, 20, true);
  delay(500);

  /* ファンを回転させる */
  fan_exhaust();
  delay(300);

  /* ファンを停止する */
  fan_free();

  /* 筒を元に戻す */
  fanServo.write(0, 20, true);
  delay(100);

  /* 正面を向く */
  if(ball_color == BLUE) {
    turn_left_90deg();
    turn_left(0x30);
    delay(300);
    stp(HARD_STOP);
  } else {
    turn_right_90deg();
    turn_right(0x30);
    delay(300);
    stp(HARD_STOP);
  }
}
