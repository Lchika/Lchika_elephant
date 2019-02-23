/* ライントレース関係の関数定義 */

/* ライン上を前進 */
int forward_on_line(void) {
    int position;
    static int before_position = CENTER;
    position = get_position();
  
    switch (position) {
        case NONE :
            search_line();
            position = get_position();
            break;
        case LEFT :
            while(position != CENTER && position != LEFT_CENTER ) {
                turn_left(0x30);
                position = get_position();
            }
            break;
        case CENTER :
            forward(LINE_TRACE_SPEED);
            break;
        case LEFT_CENTER_RIGHT :
            forward(LINE_TRACE_SPEED);
            break;
        case RIGHT :
            while(position != CENTER && position != CENTER_RIGHT ) {
                turn_right(0x30);
                position = get_position();
            }
            break;
        case CENTER_RIGHT :
            forward(LINE_TRACE_SPEED);
            break;
        case LEFT_CENTER :
            forward(LINE_TRACE_SPEED);
            break;
        case LEFT_RIGHT :
            forward(LINE_TRACE_SPEED);
            break;
        default :
            stp(HARD_STOP);
            break;
    }

    before_position = position;

    return position;
}

/* 首振りしてラインを探す */
void search_line(void) {
    int position;
    int count;
    int time = 1;
    while(1) {
        count = time * 50;
        do {
            turn_right(0x30);
            position = get_position();
            count--;
            } while(position == NONE and count > 0);
        stp(HARD_STOP);
        if(position != NONE) {
            return;
        }

        count = time * 100;
        do {
            turn_left(0x30);
            position = get_position();
            count--;
            } while(position == NONE and count > 0);
        stp(HARD_STOP);
        if(position != NONE) {
            return;
        }

        count = time * 50;
        do {
            turn_right(0x30);
            position = get_position();
            count--;
            } while(position == NONE and count > 0);
        stp(HARD_STOP);
        if(position != NONE) {
            return;
        }
        time++;
    }

    return;
}

/* ライン外からライン上に戻る */
void return_to_line(int ball_color) {
    volatile int position;
    float deg;
    int flag = 0;
    /* 吸い込みのために前進した分後進する */
    // backward(0x30);
    // delay(2000);
    // stp(HARD_STOP);

    adapt_backward_value = int(adapt_backward_value*2.0);
    /* 適正位置に合わせるために後進した分前進する */
    while(adapt_backward_value > 0) {
      forward(0x20);
      delay(1);
      adapt_backward_value--;
    }
    delay(200);
    stp(HARD_STOP);

    to_ball_find_time = int(to_ball_find_time*1.7);
    /* 左方向への首振り中にボールを見つけた場合 */
    if(ball_find_direct == LEFT) {
        Serial.println("LEFT FIND");
        while(to_ball_find_time > 0) {
            turn_right(0x30);
            delay(10);
            to_ball_find_time -= 10;
        }

        #if 0
        do{
          turn_right(0x30);
          position = get_position();
        } while(position != CENTER);
        stp(HARD_STOP);
        backward(0x30);
        delay(200);
        do{
          backward(0x30);
          position = get_position();
        } while(position != CENTER);
        #endif

        /* 発見したラインが縦ラインであるかどうかを確認する */
        // deg = get_degree();
        // /* 横ラインを発見してしまった場合，90度右に旋回する */
        // if((deg > 60 and deg < 120) or (deg > 240 and deg < 300)) {
        //     turn_right_90deg();
        // } 

        if(ball_color != NO_BALL) {
            /* 180度旋回 */
            turn_left_90deg();
            turn_left_90deg();
        }

        // while(get_position() != CENTER) {
        //     turn_right(0x05);
        // }
    } 
    /* 右方向への首振り中にボールを見つけた場合 */
    else {
        Serial.println("RIGHT FIND");
        while(to_ball_find_time > 0) {
            turn_left(0x30);
            delay(10);
            to_ball_find_time -= 10;
        }
        /* 左方向に旋回しながらラインを探す */

        #if 0
        do{
          turn_left(0x30);
          position = get_position();
        } while(position != CENTER);
        stp(HARD_STOP);
        backward(0x30);
        delay(200);
        do{
          backward(0x30);
          position = get_position();
        } while(position != CENTER);
        #endif

         /* 発見したラインが縦ラインであるかどうかを確認する */
        // deg = get_degree();
        // /* 横ラインを発見してしまった場合，90度左に旋回する */
        // if((deg > 60 and deg < 120) or (deg > 240 and deg < 300)) {
        //     turn_left_90deg();
        // } 

        if(ball_color != NO_BALL) {
            /* 180度旋回 */
            turn_left_90deg();
            turn_left_90deg();
        }

        // while(get_position() != CENTER) {
        //     turn_left(0x05);
        // }
    }
}