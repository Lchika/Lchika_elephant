/* ジョイスティックでの制御 */
void joy_control(void) {
  int state = get_joystate();

  if(state > 0) {
    forward(0xFF);
  } else 
  if(state < 0) {
    backward(0xFF);
  } else {
    stp(SOFT_STOP);
  }
}

/* ジョイスティックの状態 */
int get_joystate() {
  int value = analogRead(A5) - 643;
  if(value < 0) {
    return -192;//int(-1 * float(192/609) * float(value));
  } else if(value > 0) {
    return 192;//int(float(192/376) * float(value));
  } else {
    return 0;
  }
}
