// ドライヤーのファンモータ制御 by鈴木
#define DUTY 255

// 関数
// 吸い込み
void fan_intake() {
  analogWrite(PIN_FAN_1, DUTY);
  analogWrite(PIN_FAN_2, 0);
}
  
// 吐き出し
void fan_exhaust() {
  analogWrite(PIN_FAN_1, 0);
  analogWrite(PIN_FAN_2, DUTY);
}

// ブレーキ
void fan_brake() {
  analogWrite(PIN_FAN_1, DUTY);
  analogWrite(PIN_FAN_2, DUTY);
}

// 解放
void fan_free(){
  analogWrite(PIN_FAN_1, 0);
  analogWrite(PIN_FAN_2, 0);
}
