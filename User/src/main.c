#include "stm32f4xx.h"
#include "uart.h"
#include "lcd.h"
#include "exti.h"
#include "led.h"
#include "queue.h"
#include "string.h"
#include "mylib.h"

// Define Macros
#define STR_MAX_LENGTH 100

// Global variables
volatile char *gp_main_menu = "_Choose your option(1,2,..):\r\n\
1. Student info\r\n\
2. Basic operation\r\n\
3. Simple led\r\n\
4. Advance led\r\n\
5. Caro 3x3\r\n\
6. Audio\r\n\
Select: ";

volatile char *gp_operator_option = "_Choose your option(1,2,..):\r\n\
a. Plus\r\n\
b. Subtract\r\n\
c. Multiple\r\n\
d. Divide\r\n\
e. Module";

volatile char *gp_simple_led_option = "_Choose your option(1,2,..):\r\n\
a. Green On\r\n\
b. Green Off\r\n\
c. Red On\r\n\
d. Red Off";

volatile char *gp_student_info = "Student 1:\r\n\
ID: 14520674\r\n\
Fullname: Thai Viet Phong\r\n\
Student 2:\r\n\
ID: 14520578 \r\n\
Fullname : Cap Doan Nghia.";

volatile char *gp_new_line = "\r\n";
volatile char *gp_esc_option = "ESC: return previous menu";
volatile char *gp_advanced_led = "Press Button to toggle LED";
volatile char *gp_1st_number = "Number 1st: ";
volatile char *gp_2nd_number = "Number 2nd: ";
volatile char *gp_add = ">>Plus\r\n";
volatile char *gp_sub = ">>Subtract\r\n";
volatile char *gp_mul = ">>Multiple\r\n";
volatile char *gp_div = ">>Divide\r\n";
volatile char *gp_mod = ">>Module\r\n";
volatile char *gp_result = "Result: ";
volatile char *gp_input_invalid = ">>Input invalid, please input again!";
volatile char *gp_press_any = "\r\n>>Press anykey to continue...\r\n";
volatile char *gp_nan = "NaN";
volatile char *gp_seperator = "\r\n=============================\r\n";
volatile char *gp_tic_tac_toe = "Tic tac toe\r\n\
1 2 3\r\n\
4 5 6\r\n\
7 8 9\r\n\
Input position to draw";
volatile char *gp_caro_next_turn = "The next turn is ";
volatile char *gp_caro_winner = "The winner is ";

uint8_t gp_board[3][3];
uint8_t gp_caro_turn = 0;

uint8_t state = 0;

volatile char g_char_input = 0;
char g_task_sel = 0;
char g_sub_task = 0;

uint8_t x, y, check_win;
int64_t g_i64_num1 = 0;
int64_t g_i64_num2 = 0;
int64_t g_i64_result = 0;

// Declare data structure
queue_t g_queue_send;
queue_t g_queue_receive;

//prototype func
int64_t readNum(int64_t *number);
void printNum(int64_t number);
void invalidNotification(void);

int main()
{
    // Initialize LCD
    lcd_init();
    //Init ext
    exti_init();
    // Led init
    led_init();

    //init gp_board
    initBoard((uint8_t(*)[3])gp_board);

    // Initialize UART
    uart3_init_interrupt();

    // Initialize queue
    queue_init(&g_queue_send);
    queue_init(&g_queue_receive);

    //loop main task
    while (state != (uint8_t)-1)
    {
        //print main menu to terminal
        queue_push_string(&g_queue_send, (char *)gp_seperator, strlen((char *)gp_seperator));
        queue_push_string(&g_queue_send, (char *)gp_main_menu, strlen((char *)gp_main_menu));
        while (!queue_is_empty(&g_queue_send))
            USART_ITConfig(USART3, USART_IT_TXE, ENABLE);

        //read selection
        while (queue_is_empty(&g_queue_receive))
            USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
        g_task_sel = queue_pop(&g_queue_receive);

        if (g_task_sel > '6' || g_task_sel < '1')
        {
            invalidNotification();
        }
        else
        {
            state = (uint8_t)1;
        }

        while (state == (uint8_t)1) //loop for sub task
        {
            if (g_task_sel == '1') // student info
            {
                //print result
                if (g_sub_task == 0)
                {
                    queue_push_string(&g_queue_send, (char *)gp_seperator, strlen((char *)gp_seperator));
                    queue_push_string(&g_queue_send, (char *)gp_student_info, strlen((char *)gp_student_info));
                    queue_push_string(&g_queue_send, (char *)gp_new_line, strlen((char *)gp_new_line));
                    queue_push_string(&g_queue_send, (char *)gp_esc_option, strlen((char *)gp_esc_option));
                }
            }
            else if (g_task_sel == '2') //basic operation
            {
                if (g_sub_task != 0 && (g_sub_task < 'a' || g_sub_task > 'e'))
                {
                    invalidNotification();
                    queue_push_string(&g_queue_send, (char *)gp_operator_option, strlen((char *)gp_operator_option));
                    queue_push_string(&g_queue_send, (char *)gp_new_line, strlen((char *)gp_new_line));
                    queue_push_string(&g_queue_send, (char *)gp_esc_option, strlen((char *)gp_esc_option));
                }
                else if (g_sub_task == 0)
                {
                    //print sub menu
                    queue_push_string(&g_queue_send, (char *)gp_seperator, strlen((char *)gp_seperator));
                    queue_push_string(&g_queue_send, (char *)gp_operator_option, strlen((char *)gp_operator_option));
                    queue_push_string(&g_queue_send, (char *)gp_new_line, strlen((char *)gp_new_line));
                    queue_push_string(&g_queue_send, (char *)gp_esc_option, strlen((char *)gp_esc_option));
                }
                else
                {
                    //print title
                    queue_push_string(&g_queue_send, (char *)gp_seperator, strlen((char *)gp_seperator));
                    if (g_sub_task == 'a')
                    {
                        queue_push_string(&g_queue_send, (char *)gp_add, strlen((char *)gp_add));
                    }
                    else if (g_sub_task == 'b')
                    {
                        queue_push_string(&g_queue_send, (char *)gp_sub, strlen((char *)gp_sub));
                    }
                    else if (g_sub_task == 'c')
                    {
                        queue_push_string(&g_queue_send, (char *)gp_mul, strlen((char *)gp_mul));
                    }
                    else if (g_sub_task == 'd')
                    {
                        queue_push_string(&g_queue_send, (char *)gp_div, strlen((char *)gp_div));
                    }
                    else if (g_sub_task == 'e')
                    {
                        queue_push_string(&g_queue_send, (char *)gp_mod, strlen((char *)gp_mod));
                    }

                    queue_push_string(&g_queue_send, (char *)gp_1st_number, strlen((char *)gp_1st_number));
                    //read the 1st number
                    readNum(&g_i64_num1);

                    queue_push_string(&g_queue_send, (char *)gp_new_line, strlen((char *)gp_new_line));
                    queue_push_string(&g_queue_send, (char *)gp_2nd_number, strlen((char *)gp_2nd_number));
                    //read the 2nd number
                    readNum(&g_i64_num2);

                    if (g_sub_task == 'a')
                    {
                        g_i64_result = g_i64_num1 + g_i64_num2;
                    }
                    else if (g_sub_task == 'b')
                    {
                        g_i64_result = g_i64_num1 - g_i64_num2;
                    }
                    else if (g_sub_task == 'c')
                    {
                        g_i64_result = g_i64_num1 * g_i64_num2;
                    }
                    else if (g_sub_task == 'd' && (g_i64_num2 != 0))
                    {
                        g_i64_result = g_i64_num1 / g_i64_num2;
                    }
                    else if (g_sub_task == 'e')
                    {
                        g_i64_result = g_i64_num1 % g_i64_num2;
                    }
                    //print result
                    queue_push_string(&g_queue_send, (char *)gp_new_line, strlen((char *)gp_new_line));
                    queue_push_string(&g_queue_send, (char *)gp_result, strlen((char *)gp_result));
                    if (g_sub_task == 'd' && (g_i64_num2 == 0))
                    {
                        queue_push_string(&g_queue_send, (char *)gp_nan, strlen((char *)gp_nan));
                    }
                    else if (g_i64_result == 0)
                    {
                        queue_push(&g_queue_send, '0');
                    }
                    else
                    { //print sign
                        if (g_i64_result < 0)
                        {
                            queue_push(&g_queue_send, '-');
                            g_i64_result = -g_i64_result;
                        }
                        //backtrack print ^_^ lazy
                        printNum(g_i64_result);
                    }
                    //press any key to continue
                    queue_push_string(&g_queue_send, (char *)gp_press_any, strlen((char *)gp_press_any));
                    clear_queue(&g_queue_receive);
                    while (queue_is_empty(&g_queue_receive))
                        USART_ITConfig(USART3, USART_IT_TXE, ENABLE);

                    //print sub menu
                    queue_push_string(&g_queue_send, (char *)gp_seperator, strlen((char *)gp_seperator));
                    queue_push_string(&g_queue_send, (char *)gp_operator_option, strlen((char *)gp_operator_option));
                    queue_push_string(&g_queue_send, (char *)gp_new_line, strlen((char *)gp_new_line));
                    queue_push_string(&g_queue_send, (char *)gp_esc_option, strlen((char *)gp_esc_option));
                }
            }
            else if (g_task_sel == '3') // simple led
            {
                //print sub menu
                if (g_sub_task != 0 && (g_sub_task < 'a' || g_sub_task > 'd'))
                {
                    invalidNotification();
                    queue_push_string(&g_queue_send, (char *)gp_simple_led_option, strlen((char *)gp_simple_led_option));
                    queue_push_string(&g_queue_send, (char *)gp_new_line, strlen((char *)gp_new_line));
                    queue_push_string(&g_queue_send, (char *)gp_esc_option, strlen((char *)gp_esc_option));
                }
                else if (g_sub_task == 0)
                {
                    queue_push_string(&g_queue_send, (char *)gp_seperator, strlen((char *)gp_seperator));
                    queue_push_string(&g_queue_send, (char *)gp_simple_led_option, strlen((char *)gp_simple_led_option));
                    queue_push_string(&g_queue_send, (char *)gp_new_line, strlen((char *)gp_new_line));
                    queue_push_string(&g_queue_send, (char *)gp_esc_option, strlen((char *)gp_esc_option));
                }
                else if (g_sub_task == 'a')
                {
                    led_on(LED_GREEN);
                }
                else if (g_sub_task == 'b')
                {
                    led_off(LED_GREEN);
                }
                else if (g_sub_task == 'c')
                {
                    led_on(LED_RED);
                }
                else if (g_sub_task == 'd')
                {
                    led_off(LED_RED);
                }
            }
            else if (g_task_sel == '4') // advanced led
            {
                //todo
                queue_push_string(&g_queue_send, (char *)gp_seperator, strlen((char *)gp_seperator));
                queue_push_string(&g_queue_send, (char *)gp_advanced_led, strlen((char *)gp_advanced_led));
                queue_push_string(&g_queue_send, (char *)gp_new_line, strlen((char *)gp_new_line));
                queue_push_string(&g_queue_send, (char *)gp_esc_option, strlen((char *)gp_esc_option));
                led_on(LED_GREEN);
                led_off(LED_RED);
            }
            else if (g_task_sel == '5') //caro
            {
                //todo
                if (g_sub_task == 0)
                {
                    gp_caro_turn = 1;
                    check_win = 0;
                    initBoard(gp_board);
                }
                queue_push_string(&g_queue_send, (char *)gp_seperator, strlen((char *)gp_seperator));
                queue_push_string(&g_queue_send, (char *)gp_tic_tac_toe, strlen((char *)gp_tic_tac_toe));
                queue_push_string(&g_queue_send, (char *)gp_new_line, strlen((char *)gp_new_line));
                queue_push_string(&g_queue_send, (char *)gp_caro_next_turn, strlen((char *)gp_caro_next_turn));
                queue_push(&g_queue_send, gp_caro_turn == 1 ? 'X' : 'O');
                queue_push_string(&g_queue_send, (char *)gp_new_line, strlen((char *)gp_new_line));
                queue_push_string(&g_queue_send, (char *)gp_esc_option, strlen((char *)gp_esc_option));
                if (g_sub_task > '0' && g_sub_task <= '9' && check_win == 0)
                {
                    x = ((uint8_t)(g_sub_task - '0') - 1) / 3;
                    y = ((uint8_t)(g_sub_task - '0') - 1) % 3;
                    if (gp_board[x][y] == 0)
                    {
                        gp_board[x][y] = gp_caro_turn;
                        gp_caro_turn = gp_caro_turn == 1 ? 2 : 1;
                        check_win = checkWinner(gp_board);
                    }
                }
                if (check_win)
                {
                    queue_push_string(&g_queue_send, (char *)gp_new_line, strlen((char *)gp_new_line));
                    queue_push_string(&g_queue_send, (char *)gp_caro_winner, strlen((char *)gp_caro_winner));
                    queue_push(&g_queue_send, check_win == 1 ? 'X' : 'O');
                }
                //clear LCD
                LCD_Clear(LCD_COLOR_WHITE);
                drawBoard((uint8_t(*)[3])gp_board);
            }

            //read selection for sub task
            clear_queue(&g_queue_receive);
            while (queue_is_empty(&g_queue_receive))
                USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
            if (!queue_is_empty(&g_queue_receive))
            {
                g_char_input = queue_pop(&g_queue_receive);
                if (g_char_input == (char)27) // esc is pressed
                {
                    state = 0;      // return to main menu
                    g_sub_task = 0; //reset sub task sel
                    //LCD_DisplayStringLine(0, (uint8_t *)"main menu");
                }
                else
                {
                    g_sub_task = g_char_input; // read sub sel from inp
                }
            }
        }
    }
}
// define functions
void printNum(int64_t number)
{
    if (number == 0)
        return;
    printNum(number / 10);
    queue_push(&g_queue_send, (char)(number % (int64_t)10 + (int64_t)'0'));
}

int64_t readNum(int64_t *number)
{
    *number = 0;
    uint8_t _sign = 0;
    g_char_input = 0;

    while (g_char_input != (char)'\n')
    {
        USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
        if (!queue_is_empty(&g_queue_receive))
        {
            g_char_input = queue_pop(&g_queue_receive);
            if (g_char_input >= (char)'0' && g_char_input <= (char)'9')
            {
                *number = (*number) * 10 + (int64_t)(g_char_input - (char)'0');
                queue_push(&g_queue_send, g_char_input);
            }
            else if ((*number == 0) && (g_char_input == (char)'-'))
            {
                _sign = 1;
                queue_push(&g_queue_send, g_char_input);
            }
        }
    }
    if (_sign)
    {
        *number = -(*number);
    }
    return *number;
}
void invalidNotification()
{
    queue_push_string(&g_queue_send, (char *)gp_seperator, strlen((char *)gp_seperator));
    queue_push_string(&g_queue_send, (char *)gp_input_invalid, strlen((char *)gp_input_invalid));
    queue_push_string(&g_queue_send, (char *)gp_seperator, strlen((char *)gp_seperator));
}
//--------------------------INTERRUPT HANDLER----------------------------
void EXTI0_IRQHandler(void)
{

    exti_isr();
}
void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_TXE) == SET)
    {
        // Send characters until queue is empty
        if (!queue_is_empty(&g_queue_send))
        {
            USART_SendData(USART3, queue_pop(&g_queue_send));
        }
        else
        {
            USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
            USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
        }
    }

    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {
        USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
        queue_push(&g_queue_receive, (char)USART_ReceiveData(USART3));
    }
}

/* End of file */
