#include "lcd/lcd.h"
#include "utils.h"
#include "assembly/example.h"
#include <stdbool.h>
#include <stdlib.h>
void Inp_init(void) {
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);

    gpio_init(GPIOA, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    gpio_init(GPIOC, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ,
                GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
}

void IO_init(void) {
    Inp_init(); // inport init
    Lcd_Init(); // LCD init
}


#define SEED 1503

// Config
const u16 FPS = 40;
const u16 FPS_delay = 25;

void rand_reset() { srand(SEED); }
u16 rand_u16_wallpos_easy() { return (u16)rand()%40 + 60; }
u16 rand_u16_wallpos_normal() { return (u16)rand()%65 + 55; }
u16 rand_u16_wallpos_hard() { return (u16)rand()%90 + 50; }
enum difficulty_level {EASY, NORMAL, HARD};
u8* difficulty_str[5] = {(u8*)"EASY", (u8*)"NORMAL", (u8*)"HARD"};
u16 difficulty = HARD;

// origin pos of player
const u16 player_x = 30, player_y = 90;
bool new_life = true;

// current pos, only y changes
u16 player_current_y = player_y;

u16 score = 0, life = 999;
const u16 score_x = 1, score_y = 5;
const u8 score_str[10] = "Score:";

const u16 life_x = 1, life_y = 25;
const u8 life_str[10] = "Life:";

const u16 score_num_x = 48;
const u16 life_num_x = 40;

const u16 board_x1 = 5, board_y1 = 45, board_x2 = 75, board_y2 = 155;


// go down is positive direction
const u16 gravity = 1;
bool is_upward = false;
const u16 upward_easy = -3;
const u16 upward_normal = -5;
const u16 upward_hard = -7;
u16 speed = 0;


u16 upward(){
    switch (difficulty)
    {
    case EASY:
        return upward_easy;
        break;
    case NORMAL:
        return upward_normal;
        break;
    }
    return upward_hard;
}
// if no constrains, max_speed is sqrt(220)
const u16 max_fall_speed_easy = 10;
const u16 max_fall_speed_normal = 13;
const u16 max_fall_speed_hard = 15;

u16 max_fall_speed(){
    switch (difficulty)
    {
    case EASY:
        return max_fall_speed_easy;
        break;
    case NORMAL:
        return max_fall_speed_normal;
        break;
    }
    return max_fall_speed_hard;
}

struct wall
{ 
    u16 x;
    // the end of upper wall, begin of lower wall
    // for each new wall, random the location of upper wall
    u16 y_1, y_2;
    // as one time, there are at most 2 wall in the board, implement it as a queue?
    struct wall* next;
};

// right is positive direction
const u16 wall_speed = -1;

const u16 wall_width = 1;
const u16 wall_horizonal_space = 30;
const u16 wall_vertical_space_easy = 40;
const u16 wall_vertical_space_normal = 25;
const u16 wall_vertical_space_hard = 10;

// store all existing wall
struct wall* first_wall= NULL;

#define MAX_TRACE_POINTS 100
typedef struct sampled_point{
    u16 current_x, current_y;
    u16 origin_y;
}sampled_point;
sampled_point player_trace[MAX_TRACE_POINTS];
u16 trace_len = 0;

void move_curve_left() {
    // Move each point to the left
    for (int i = 0; i < trace_len; i++) {
        player_trace[i].current_x += wall_speed;
    }

    // Remove points that are off the screen
    while (trace_len > 0 && player_trace[0].current_x < board_x1) {
        for (int i = 0; i < trace_len - 1; i++) {
            player_trace[i+1].origin_y = player_trace[i].current_y;
            player_trace[i] = player_trace[i + 1];
        }
        trace_len--;
    }

    // Draw lines between adjacent points
    for (int i = 1; i < trace_len; i++) {
        LCD_DrawLine(player_trace[i - 1].current_x - wall_speed, player_trace[i - 1].current_y, player_trace[i].current_x - wall_speed, player_trace[i].current_y, BLACK);
        if(player_trace[i-1].current_y >= board_y2 || player_trace[i-1].current_y <= board_y1){
            break;
        }
        if(player_trace[i].current_y >= board_y2){
            LCD_DrawLine(player_trace[i - 1].current_x, player_trace[i - 1].current_y, player_trace[i].current_x, board_y2, GREEN);
            break;
        }
        else if(player_trace[i].current_y <= board_y1){
            LCD_DrawLine(player_trace[i - 1].current_x, player_trace[i - 1].current_y, player_trace[i].current_x, board_y1, GREEN);
            break;
        }

        LCD_DrawLine(player_trace[i - 1].current_x, player_trace[i - 1].current_y, player_trace[i].current_x, player_trace[i].current_y, GREEN);
    }
    if(player_current_y >= board_y2 || player_current_y <= board_y1){
        return;
    }
    LCD_DrawLine(player_trace[trace_len - 1].current_x, player_trace[trace_len - 1].current_y, player_x, player_current_y, GREEN);
}

bool get_button(){
    return (Get_Button(JOY_LEFT) || Get_Button(JOY_DOWN) || Get_Button(JOY_UP) || Get_Button(JOY_RIGHT) || Get_Button(JOY_CTR) || Get_Button(BUTTON_1) || Get_Button(BUTTON_2));
}

u16 get_num_len(u16 num){
    u16 len = 0;
    if(num == 0){
        return 1;
    }
    while(num){
        num/=10;
        len++;
    }
    return len;
}

void initialization(){
    score = 0;
    life = 999;
    if(first_wall -> next != NULL){
        free(first_wall -> next);
    }
    if(first_wall != NULL){
        free(first_wall);
    }

    trace_len = 0;

    new_life = true;
    return;
}


bool pressed_RIGHT = false;
bool pressed_LEFT = false;
u16 RIGHT_count = 0;
u16 LEFT_count = 0;

void difficulty_choose(){
    LCD_Clear(BLACK);
    for(u16 i = 0; i <= HARD; i++){
        if(i == 0){
            LCD_ShowString(10, 5 + 20*i, difficulty_str[i], 0XBC40);
        }
        else{
            LCD_ShowString(10, 5 + 20*i, difficulty_str[i], 0X01CF);
        }
    }
    u16 diff_num = 0;
    uint64_t start_time, delta_time;
    while(!Get_Button(JOY_CTR)){
        start_time = get_timer_value();
        u16 pre_diff = diff_num;
        // key de-bouncing
        if(pressed_RIGHT){
            RIGHT_count -= 20;
            pressed_RIGHT = (RIGHT_count == 0)? false:true;
        }
        if(pressed_LEFT){
            LEFT_count -= 20;
            pressed_LEFT = (LEFT_count == 0)? false:true;
        }

        if(Get_Button(JOY_RIGHT) && !pressed_RIGHT){
            pressed_RIGHT = true;
            RIGHT_count = 300;
            diff_num = (diff_num + 1) % (HARD+1);
        }
        else if(Get_Button(JOY_LEFT) && !pressed_LEFT){
            pressed_LEFT = true;
            LEFT_count = 300;
            diff_num = (diff_num + HARD) % (HARD+1);
        }
        for(u16 i = 0; i <= HARD; i++){
            if(i == diff_num){
                LCD_ShowString(10, 5 + 20*i, difficulty_str[i], 0XBC40);
            }
            else if(i == pre_diff){
                LCD_ShowString(10, 5 + 20*i, difficulty_str[i], 0X01CF);
            }
        }

        // stable FPS, actually compute time in this function -> 0?
        delta_time = get_timer_value() - start_time;
        delay_1ms(FPS_delay - (delta_time*4000.0)/SystemCoreClock);
    }
    difficulty = diff_num;
    LCD_Clear(BLACK);
    return;
}

struct wall* build_wall(u16 chose_difficulty){
    struct wall* new_wall = (struct wall*)malloc(sizeof(struct wall));
    new_wall -> x = board_x2 - 1;
    switch (chose_difficulty)
    {
    case EASY:
        new_wall -> y_1 = rand_u16_wallpos_easy();
        new_wall -> y_2 = new_wall -> y_1 + wall_vertical_space_easy;
        new_wall -> next = NULL;
        break;
    case NORMAL:
        new_wall -> y_1 = rand_u16_wallpos_normal();
        new_wall -> y_2 = new_wall -> y_1 + wall_vertical_space_normal;
        new_wall -> next = NULL;
        break;
    case HARD:
        new_wall -> y_1 = rand_u16_wallpos_hard();
        new_wall -> y_2 = new_wall -> y_1 + wall_vertical_space_hard;
        new_wall -> next = NULL;
        break;
    }
    return new_wall;
}

void set_environment(){
    LCD_ShowString(score_x, score_y, score_str, WHITE);
    LCD_ShowString(life_x, life_y, life_str, WHITE);
    LCD_DrawRectangle(board_x1, board_y1, board_x2, board_y2, WHITE);

    if(!first_wall){
        first_wall = build_wall(difficulty);
    }
    return;
}

void set_environment_new(){
    // handle the case: score exceed display length
    if(score > 999){
        score %= 1000;
    }
    LCD_ShowNum(score_num_x, score_y, score, get_num_len(score), YELLOW);
    LCD_ShowNum(life_num_x, life_y, life, get_num_len(life), YELLOW);
    if(new_life){
        LCD_DrawPoint_big(player_x, player_current_y, BLACK);
        player_current_y = player_y;
        speed = 0;
        new_life = false;
        for (int i = 1; i < trace_len; i++) {
            LCD_DrawLine(player_trace[i - 1].current_x, player_trace[i - 1].current_y, player_trace[i].current_x, player_trace[i].current_y, BLACK);
        }
        trace_len = 0;
    }
    LCD_DrawRectangle(board_x1, board_y1, board_x2, board_y2, WHITE);
}

void display_wall(struct wall* current_wall){
    if(current_wall -> x <= board_x1){
        return;
    }
    LCD_DrawRectangle(current_wall -> x, board_y1, current_wall -> x + wall_width, current_wall -> y_1, WHITE);
    LCD_DrawRectangle(current_wall -> x, current_wall -> y_2, current_wall -> x + wall_width, board_y2, WHITE);
    return;
}

void erase_wall(struct wall* current_wall){
    if(current_wall -> x != board_x2 - 1 && current_wall -> x != board_x1){
        LCD_DrawRectangle(current_wall -> x, board_y1 + 1, current_wall -> x + wall_width, current_wall -> y_1, BLACK);
        LCD_DrawRectangle(current_wall -> x, current_wall -> y_2, current_wall -> x + wall_width, board_y2, BLACK);        
    }
    else if(current_wall -> x == board_x2 - 1){
        LCD_DrawLine(current_wall -> x, board_y1 + 1, current_wall -> x, current_wall -> y_1+1, BLACK);
        LCD_DrawLine(current_wall -> x, current_wall -> y_2-1, current_wall -> x, board_y2, BLACK);
    }
    current_wall -> x += wall_speed;
    return;
}

void check_wall_valid(struct wall* current_wall){
    if(current_wall -> x <= board_x1){
        struct wall* current = current_wall;
        first_wall = current -> next;
        free(current);
    }
    return;
}

void update(){
    // 1. PLAYER
    // 1.1 Upward speed
    if(get_button()){
        if (is_upward == false) {
            speed = upward(); // Apply upward speed
            is_upward = true;
        }
    } 
    else if(is_upward){
        is_upward = false;
        speed = 0;
    }
    else{
        // Button is not pressed, reset upward flag
        if(speed + gravity < max_fall_speed()){
            speed += gravity;
        }
    }

    // 1.2 Life & Score
    LCD_DrawPoint_big(player_x, player_current_y, BLACK);
    player_current_y += speed;    
    if(player_current_y >= board_y2 ||player_current_y <= board_y1){
        life--;
        new_life = true;
    }
    if(!new_life){
        LCD_DrawPoint_big(player_x, player_current_y, GREEN);
    }
    if(abs(player_x - (first_wall -> x)) < 1){
        if(player_current_y < first_wall -> y_2 && player_current_y > first_wall -> y_1){
            score ++;
        }
        else{
            life--;
            new_life = true;
        }
    }
    

    // 2. WALL
    // 2.1 Display wall
    erase_wall(first_wall);
    display_wall(first_wall);
    if(first_wall -> next != NULL){
        erase_wall(first_wall -> next);
    }
    if(first_wall -> next != NULL){
        display_wall(first_wall -> next);
    }

    check_wall_valid(first_wall);

    // 2.3 Build the second wall
    if(!(first_wall -> next) && (board_x2 - (first_wall -> x)) >= wall_horizonal_space){
        first_wall -> next = build_wall(difficulty);
    }



    // 3. SCORE
    
    // 4. PLAYER TRACE
    // 4.1 Sample the player's current position and add it to the trace
    if (trace_len < MAX_TRACE_POINTS) {
        player_trace[trace_len].current_x = player_x;
        player_trace[trace_len].current_y = player_current_y;
        trace_len++;
    } else {
        // If the trace array is full, shift all elements left and add the new point at the end
        for (int i = 0; i < MAX_TRACE_POINTS - 1; i++) {
            player_trace[i] = player_trace[i + 1];
        }
        player_trace[MAX_TRACE_POINTS - 1].current_x = player_x;
        player_trace[MAX_TRACE_POINTS - 1].current_y = player_current_y;
    }


    // 4.2
    move_curve_left();


    return;
}


int main(void) {
    IO_init();
    
    u16 ret_value = splash_screen(CYAN, LIGHTGREEN, BLUE);
    delay_1ms(ret_value);

    difficulty_choose();
    set_environment();
    while(life){
        uint64_t delta, start = 0;
        uint64_t tmp = get_timer_value();
        do{
            start = get_timer_value();
        }while(start == tmp);

        set_environment_new();
        update();
    
        // If lose all life, begin a new game
        if(!life){
            initialization();
        }

        // Actual delay = second per frame - compute time
        // FPS = 40, delay = 25ms
        do{
            delta = get_timer_value() - start;
        }while(delta < (SystemCoreClock/4000.0 * FPS_delay));
    }
}
