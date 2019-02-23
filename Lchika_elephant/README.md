エルチカエレファント
2019年社内ロボコン大会ボール運び部門出場ロボット

コード概要

#Lchika_elephant.ino  
①Arduino setup関数，loop関数を定義．  
loop関数内で状態遷移を管理．  
②各状態で行う動作の関数を定義．  


#Lchika_elephant.h

各種デファイン，グローバル変数定義．
要チューニングパラメータ：
#define WALL 140  ・・・下のPSDがこれ以上の値を検知したらボールがあると判断
#define WALL_FAR 130　・・・上記パラのゆるい版
#define NEAR_THRESHOLD 210 ・・・ボールを吸い込む適正位置
#define TOO_NEAR 270　・・・ボールが近すぎる距離の閾値
#define WALL_THRESHOLD 140　・・・上のPSDが壁と判断する閾値
#define WHITE_LEFT 21　・・・左のラインセンサが白ラインと判断する閾値
#define WHITE_RIGHT 20　・・・右のラインセンサが白ラインと判断する閾値
#define WHITE_CENTER 20　・・・中央のラインセンサが白ラインと判断する閾値
#define BLUE_SHRESHOLD 100　・・・ボールが青と判断する閾値
#define RED_SHRESHOLD 200　・・・ボールが赤と判断する閾値
#define LINE_TRACE_SPEED 0x50　・・・ライントレースする速度
const int head_swing_time = 1300　・・・首振りする時間

#L6470.ino, L6470.h
モータドライバへの指令に関する関数定義

#sensor.ino
各種センサの値を読む関数定義

#motor_driver_fan.ino
ファンを回すサーボに関する関数定義

#line_trace.ino
ライントレース関係の関数定義

#VarSpeedServo.cpp, VerSpeedServo.h
サーボを回転させる速度を指定して動作させる関数のライブラリ

#joy_stick.ino
ジョイスティックで足回りをテストするときに使用
