#ifndef _FANTONBO_H_
#define _FANTONBO_H_

/**************/
/* デジタルピン */
/**************/
/* ファン */
#define PIN_FAN_1 2
#define PIN_FAN_2 3

/* カラーセンサ(ライン)用LED */
#define PIN_LED 4

/* 筒の角度制御サーボ */
#define PIN_FAN_SERVO 6

/* カラーセンサ(ボール色)用 */
#define PIN_VCC 8
#define PIN_BALL_COLOR_DOUT 9
#define PIN_BALL_COLOR_RANGE 10
#define PIN_BALL_COLOR_CK 11
#define PIN_BALL_COLOR_GATE 12

/* リミットセンサ */
#define PIN_LIMIT_FRONT_LEFT 22
#define PIN_LIMIT_FRONT_RIGHT 23
#define PIN_LIMIT_BACK_LEFT 24
#define PIN_LIMIT_BACK_RIGHT 25


/*****************/
/* 各種デファイン  */
/*****************/
/* ロボットの状態 */
#define BEGINNING            0
#define ON_START             1
#define SEARCH_BALL_FORWARD  2
#define SEARCH_BALL_BACKWARD 3
#define BACUUM_BALL          4
#define JUDGE_BALL_COLOR     5
#define RETURN_TO_LINE       6
#define RETURN_TO_START      7
#define LAUNCH_BALL          8

/* ボールまでの距離の状態 */
#define NEAR 0
#define FAR 1
/* ボールの閾値 */
#define WALL 140 //170
#define WALL_FAR 130
#define NEAR_THRESHOLD 210 //適正位置
#define TOO_NEAR 270
/* 壁の閾値 */
#define WALL_THRESHOLD 140	//上のPSDが壁と判断する閾値
/* ボールが見つかったかどうか */
#define BALL_FINDED 0
#define BALL_NOT_FINDED 1
#define REACH_TO_WALL 2
#define REACH_TO_START  3
/* ボールか壁か */
#define JUDGE_BALL 0
#define JUDGE_WALL 1
#define JUDGE_NONE 2

/* カラーセンサの値の閾値 */
#define WHITE_LEFT 21
#define WHITE_RIGHT 20
#define WHITE_CENTER 20
// #define GREEN 200

/* ロボットの現在位置 */
#define NONE 0
#define LEFT 1
#define CENTER 2
#define RIGHT 3
#define LEFT_CENTER 4
#define CENTER_RIGHT 5
#define LEFT_RIGHT 6
#define LEFT_CENTER_RIGHT 7

/* ボールの色 */
#define BLUE    0
#define RED     1
#define NO_BALL 2
/* 閾値 */
#define BLUE_SHRESHOLD 100
#define RED_SHRESHOLD 200

/* ジョイスティックで試す用 */
#define JOY_CENTER 0
#define JOY_UP 1
#define JOY_DOWN 2
#define JOY_LEFT 3
#define JOY_RIGHT 4

/* 壁に到達したかどうか */
#define ARRIVED 0
#define NOT_ARRIVED 1

/* ライン上に戻るために向くべき方向 */
#define DIRECT_LEFT 0
#define DIRECT_RIGHT 1

/* ライントレースする速度 */
#define LINE_TRACE_SPEED 0x50

/* コンパスの初期値 */
float compass_base_deg;

/* ロボットが初期基準角度からおおまかにどちらを向いているか */
#define FRONT_DEG 0
#define BACK_DEG  1
#define LEFT_DEG  2
#define RIGHT_DEG 3

/* 適当な時間前進する時間 */
#define TIME_NORMAL 800
#define TIME_SHORT  400

/* 変数定義 */
/* ボール探索のフェーズフラグ */
int search_time = 0;
int search_f = 0;

/* 壁，ボールとの距離 */
int ball_value = 0;
int wall_value = 0;


/* 首振りする時間 */
const int head_swing_time = 1300;

/* ボールを見つけた時の首振り方向 */
int ball_find_direct = NONE;

/* 適正位置に合わせるために後進した値 */
int adapt_backward_value = 0;

/* 首振り開始してからボールを発見するまでに旋回した時間 */
int to_ball_find_time = 0;

#endif // _FANTONBO_H_