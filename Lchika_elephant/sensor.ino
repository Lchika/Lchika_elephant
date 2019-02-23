/* 各種センサの処理 */

/* 距離センサの値を取得(ボール) */
int get_diststate_ball(void) {
  int value = analogRead(A3);
  Serial.println(value);

  return value;
}

/* 距離センサの値を取得(壁) */
int get_diststate_wall(void) {
  int value = analogRead(A4);
  Serial.println(value);

  return value;
}

/* ボールか壁か判断する */
int judge_ball_or_wall(void) {
  ball_value = get_diststate_ball();
  wall_value = get_diststate_wall();
  int result;
  char s;

  if(ball_value > WALL) {
    if(wall_value > WALL_THRESHOLD) {
      result = JUDGE_WALL;
      s = 'W';
    } else {
      ball_value = get_diststate_ball();
      if(ball_value > WALL) {
        ball_value = get_diststate_ball();
        if(ball_value > WALL) {
          result = JUDGE_BALL;
          s = 'B';
        } else {
          s = 'N';
          result = JUDGE_NONE;
        }
      } else {
        s = 'N';
        result = JUDGE_NONE;
      }
    }
  }
  else {
    result = JUDGE_NONE;
    s = 'N';
  }

  Serial.print("JUDGE: ");
  Serial.println(s);
  return result;

}

/* ラインセンサ調整用 */
void setting_line_sensor(void) {
    Serial.print(analogRead(A0));
    Serial.print(",");
    Serial.print(analogRead(A1));
    Serial.print(",");
    Serial.println(analogRead(A2));
}

/* ラインセンサの値を取得し，現在位置を決定 */
int get_position() {
  digitalWrite(PIN_LED, HIGH);
  int left_val = analogRead(A0);
  digitalWrite(PIN_LED, LOW);
  digitalWrite(PIN_LED, HIGH);
  int center_val = analogRead(A1);
  digitalWrite(PIN_LED, LOW);
  digitalWrite(PIN_LED, HIGH);
  int right_val = analogRead(A2);
  digitalWrite(PIN_LED, LOW);

  /* 全部ライン上でない */
  if(left_val >= WHITE_LEFT && center_val >= WHITE_CENTER && right_val >= WHITE_RIGHT) {
    Serial.println("NONE");
    return NONE;
  }
  /* 真ん中だけライン上 */
  if(left_val >= WHITE_LEFT && center_val < WHITE_CENTER && right_val >= WHITE_RIGHT) {
    Serial.println("CENTER");
    return CENTER;
  }
  /* 右だけライン上 */
  if(left_val >= WHITE_LEFT && center_val >= WHITE_CENTER && right_val < WHITE_RIGHT) {
    Serial.println("RIGHT");
    return RIGHT;
  }
  /* 真ん中・右だけライン上 */
  if(left_val >= WHITE_LEFT && center_val < WHITE_CENTER && right_val < WHITE_RIGHT) {
    Serial.println("CENTER_RIGHT");
    return CENTER_RIGHT;
  }
  /* 左だけライン上 */
  if(left_val < WHITE_LEFT && center_val >= WHITE_CENTER && right_val >= WHITE_RIGHT) {
    Serial.println("LEFT");
    return LEFT;
  }
  /* 左・真ん中だけライン上 */
  if(left_val < WHITE_LEFT && center_val < WHITE_CENTER && right_val >= WHITE_RIGHT) {
    Serial.println("LEFT_CENTER");
    return LEFT_CENTER;
  }
  /* 左・右だけライン上 */
  if(left_val < WHITE_LEFT && center_val >= WHITE_CENTER && right_val < WHITE_RIGHT) {
    Serial.println("LEFT_RIGHT");
    return LEFT_RIGHT;
  }
  /* 全部ライン上 */
  if(left_val < WHITE_LEFT && center_val < WHITE_CENTER && right_val < WHITE_RIGHT) {
    Serial.println("LEFT_CENTER_RIGHT");
    return LEFT_CENTER_RIGHT;  
  }
}

//12ビットの値を読み込む関数(LSBから送信されるデータを想定)
int shiftIn12(int dataPin, int clockPin) {
  int value = 0;
   
  for (int i = 0; i < 12; i++) {
    digitalWrite(clockPin, HIGH);           // クロックをHIGHにする
    value |= digitalRead(dataPin) << i;     // データピンの値を読み取り、所定のビットを設定する。
    digitalWrite(clockPin, LOW);            // クロックピンをLOWにする。
  }
  
  return value;
}

/* 前向きに壁についているか判断する */
int arrive_wall_front(void) {
  if(digitalRead(PIN_LIMIT_FRONT_LEFT) == HIGH ||
     digitalRead(PIN_LIMIT_FRONT_RIGHT) == HIGH) {
       return ARRIVED;
     } else {
       return NOT_ARRIVED;
     }
}

/* 後ろ向きに壁についているか判断する */
int arrive_wall_back(void) {
  if(digitalRead(PIN_LIMIT_BACK_LEFT) == HIGH &&
     digitalRead(PIN_LIMIT_BACK_RIGHT) == HIGH) {
       return ARRIVED;
     } else {
       return NOT_ARRIVED;
     }
}

/* ボールの色を判定 */
int get_ball_color(void) {
  int red, green, blue;
  char s[64]; 

  fanServo.write(0, 20, true);
  delay(500);

  //digitalWrite(PIN_VCC, HIGH);
  digitalWrite(PIN_BALL_COLOR_GATE, LOW);
  digitalWrite(PIN_BALL_COLOR_CK, LOW);
  digitalWrite(PIN_BALL_COLOR_RANGE, HIGH);

  digitalWrite(PIN_BALL_COLOR_GATE, HIGH);
  delay(50);
  digitalWrite(PIN_BALL_COLOR_GATE, LOW);
  delayMicroseconds(4);
  red = shiftIn12(PIN_BALL_COLOR_DOUT, PIN_BALL_COLOR_CK);
  delayMicroseconds(3);
  green = shiftIn12(PIN_BALL_COLOR_DOUT, PIN_BALL_COLOR_CK);
  delayMicroseconds(3);
  blue = shiftIn12(PIN_BALL_COLOR_DOUT, PIN_BALL_COLOR_CK);
  //digitalWrite(PIN_VCC, LOW);

  sprintf(s, "Red = %4d, Green = %4d, Blue = %4d", red, green, blue);
  Serial.println(s);

  if(green > red && green > blue) {
    return NO_BALL;
  }
  if(red >= green) {
    if(red >= blue*2) {
      return RED;
    }
  } else {
      return BLUE;
  }
}

/* コンパスの値を読む */
float read_compass(void) {
  Vector norm = compass.readNormalize();

  // Calculate heading
  float heading = atan2(norm.YAxis, norm.XAxis);

  // Set declination angle on your location and fix heading
  // You can find your declination on: http://magnetic-declination.com/
  // (+) Positive or (-) for negative
  // For Bytom / Poland declination angle is 4'26E (positive)
  // Formula: (deg + (min / 60.0)) / (180 / PI);
  float declinationAngle = (4.0 + (26.0 / 60.0)) / (180 / PI);
  heading += declinationAngle;

  // Correct for heading < 0deg and heading > 360deg
  if (heading < 0){
    heading += 2 * PI;
  }

  if (heading > 2 * PI){
    heading -= 2 * PI;
  }

  // Convert to degrees
  float headingDegrees = heading * 180/PI; 

  //Output
  // Serial.print(" Heading = ");
  // Serial.print(heading);
  // Serial.print(" Degress = ");
  // Serial.print(headingDegrees);
  // Serial.println();

  return headingDegrees;
}

/* ロボットの現在角度を初期基準値からの差で求める */
/* 初期基準値から時計回りに何度回転しているかを求める */
float get_degree(void) {
  float now_deg;
  float deg_sub;
  float deg;
  now_deg = read_compass();

  deg_sub = compass_base_deg - now_deg;

  /* 負の値になる場合，基準角度から359度までの間に現在角度があるため，
     符号を変えればそのまま角度になる */
  if(deg_sub < 0) {
    deg = -1 * deg_sub;
  } 
  /* 正の値になる場合，360度 = 0度 の処理が必要 */
  else {
    deg = 360 - deg_sub;
  }

  return deg;
}
