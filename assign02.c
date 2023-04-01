#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "hardware/watchdog.h"

#define IS_RGBW true        // Will use RGBW format
#define NUM_PIXELS 1        // There is 1 WS2812 device in the chain
#define WS2812_PIN 28       // The GPIO pin that the WS2812 connected to


/**
 * @brief Welcome message
 * 
 */
void welcome_message() {
    printf("* -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- *\n");
    printf("|          ASSIGNMENT #2 - GROUP 14                     |\n");
    printf("* -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- *\n");
    printf("|       * * * *   *               *      *     *        |\n");
    printf("|       *     *   *              * *      *   *         |\n");
    printf("|       * * * *   *             *   *      * *          |\n");
    printf("|       *         *            * * * *      *           |\n");
    printf("|       *         *           *       *     *           |\n");
    printf("|       *         *          *         *    *           |\n");
    printf("|       *         * * * *   *           *   *           |\n");
    printf("|                                                       |\n");
    printf("|   *         *    * * *    * * *     * * *    * * * *  |\n");
    printf("|   *  *   *  *  *       *  *   *    *     *   *        |\n");
    printf("|   *    *    *  *       *  * * *    *         *        |\n");
    printf("|   *         *  *       *  *  *       * * *   * * *    |\n");
    printf("|   *         *  *       *  *   *           *  *        |\n");
    printf("|   *         *  *       *  *    *    *     *  *        |\n");
    printf("|   *         *    * * *    *     *    * * *   * * * *  |\n");
    printf("* -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- *\n");
    printf("|              ARE YOU READY TO PLAY ?                  |\n");
    printf("|           USE GP21 TO PICK YOUR LEVEL!                |\n");
    printf("|           \"----\" - LEVEL 1 - CHARS (EASY)             |\n");
    printf("|           \".---\" - LEVEL 2 - CHARS (HARD)             |\n");
    printf("|                  GOODLUCK ! ! !                       |\n");
    printf("* -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- *\n");

}

/*              VARIABLES               */

int number_of_wins = 0;     // Number of wins the player has achieved
int player_lives = 3;       // Number of lives the player has
int level_selected = 0;     // Level selected by player
int correct_answers = 0;    // Number of correct answers the player has entered
int lives_added = 0;        // Player has 0 lives added to start with
int lives_lost = 0;         // Player has 0 lives lost to start with

int correct_sequences = 5;  // Number of correct sequences the player needs to enter to win the game
char user_input[30];        // Array to store user input
int length_of_input = 0;    // Length of user input
int input_entered = 0;      // Flag to indicate if user has entered input
bool select_level = false;  // Level selected by player
int input_index = 0;        // Index of user input

/*              WELCOME               */
//TODO - Della's code


/*              ARM FUNCTIONS               */

// Declare the main assembly code entry point.
void main_asm();

// Initialise a GPIO pin – see SDK for detail on gpio_init()
void asm_gpio_init(uint pin)
{
    gpio_init(pin);
}

// Set direction of a GPIO pin – see SDK for detail on gpio_set_dir()
void asm_gpio_set_dir(uint pin, bool out)
{
    gpio_set_dir(pin, out);
}

// Get the value of a GPIO pin – see SDK for detail on gpio_get()
bool asm_gpio_get(uint pin)
{
    return gpio_get(pin);
}

// Set the value of a GPIO pin – see SDK for detail on gpio_put()
void asm_gpio_put(uint pin, bool value)
{
    gpio_put(pin, value);
}

// Enable rising and falling-edge interrupt – see SDK for detail on gpio_set_irq_enabled()
void asm_gpio_set_irq(uint pin)
{
    gpio_set_irq_enabled(pin, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(pin, GPIO_IRQ_EDGE_RISE, true);
}


/*              WATCHDOG TIMER              */
void watchdog_init(){
    // Check for reboot due to watchdog timeout
    if(watchdog_caused_reboot()){
        printf("TIMEOUT");
    }
    // Check for reboot due to watchdog enable
    if(watchdog_enable_caused_reboot()){
        printf("ENABLED");
    }
    // Enable watchdog timer
    watchdog_enable(0x7fffff, 1);
    watchdog_update();
}


/*              RGB LED             */
/**
 * @brief Wrapper function used to call the underlying PIO
 *        function that pushes the 32-bit RGB colour value
 *        out to the LED serially using the PIO0 block. The
 *        function does not return until all of the data has
 *        been written out.
 * 
 * @param pixel_grb The 32-bit colour value generated by urgb_u32()
 */

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}


/**
 * @brief Function to generate an unsigned 32-bit composit GRB
 *        value by combining the individual 8-bit parameters for
 *        red, green and blue together in the right order.
 * 
 * @param r     The 8-bit intensity value for the red component
 * @param g     The 8-bit intensity value for the green component
 * @param b     The 8-bit intensity value for the blue component
 * @return uint32_t Returns the resulting composite 32-bit RGB value
 */
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return  ((uint32_t) (r) << 8)  |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

/**
 * @brief start_RGB_Colour()
 *  Ensures that the RGB is blue at start of the game
 */
void start_RGB_Colour(){
    put_pixel(urgb_u32(0x00, 0x00, 0xFF));
    printf("The RGB LED is Blue to show the game has not started");

// Update RGB LED colour  
void RGB_update(int number_of_lives)
{
    switch (number_of_lives)                                // Switch statement to update RGB LED colour based on number of lives remaining
    {
        case 0:                                             // If player has 0 lives remaining
            // Black
            put_pixel(urgb_u32(0x00, 0x00, 0x00));
            printf("0 lives remaining. GAME OVER");
            break;

        case 1:                                             // If player has 1 life remaining
            // Red
            put_pixel(urgb_u32(0x7F, 0x00, 0x00));
            printf("1 life remaining");
            break;

        case 2:                                             // If player has 2 lives remaining
            // Yellow
            put_pixel(urgb_u32(0xFF, 0xFF, 0x00));
            printf("2 lives remaining");
            break;

        case 3:                                             // If player has 3 lives remaining (Max)
            // Green
            put_pixel(urgb_u32(0x00, 0xFF, 0x00));
            printf("3 lives remaining");
            break;

        default:
            printf("");
            break;
    }

}


/*              BUTTON DURATION             */
int get_time(){
    absolute_time_t time = get_absolute_time();
    return to_ms_since_boot(time);
}

int get_time_diff(int start_time, int end_time){
    return end_time - start_time;
}



/*              MORSE CODE              */
#define alphanumeric_chars 36       // (A-Z)(0-9)
typedef struct morsecode{           // Creating morsecode datatype
    char * morsecode;
    char alphanum;
}
morse;

morse alphabet[alphanumeric_chars];

/**
 * @brief Initialises the alphabhet array with Morse code representation for
 *        letters A-Z and numbers 0-9.
 * 
 */
void init_morse(){

    alphabet[0].alphanum='A';
    alphabet[1].alphanum='B';
    alphabet[2].alphanum='C';
    alphabet[3].alphanum='D';
    alphabet[4].alphanum='E';
    alphabet[5].alphanum='F';
    alphabet[6].alphanum='G';
    alphabet[7].alphanum='H';
    alphabet[8].alphanum='I';
    alphabet[9].alphanum='J';
    alphabet[10].alphanum='K';
    alphabet[11].alphanum='L';
    alphabet[12].alphanum='M';
    alphabet[13].alphanum='N';
    alphabet[14].alphanum='O';
    alphabet[15].alphanum='P';
    alphabet[16].alphanum='Q';
    alphabet[17].alphanum='R';
    alphabet[18].alphanum='S';
    alphabet[19].alphanum='T';
    alphabet[20].alphanum='U';
    alphabet[21].alphanum='V';
    alphabet[22].alphanum='W';
    alphabet[23].alphanum='X';
    alphabet[24].alphanum='Y';
    alphabet[25].alphanum='Z';
    alphabet[26].alphanum='0';
    alphabet[27].alphanum='1';
    alphabet[28].alphanum='2';
    alphabet[29].alphanum='3';
    alphabet[30].alphanum='4';
    alphabet[31].alphanum='5';
    alphabet[32].alphanum='6';
    alphabet[33].alphanum='7';
    alphabet[34].alphanum='8';
    alphabet[35].alphanum='9';

    alphabet[0].morsecode=".-";
    alphabet[1].morsecode="-...";
    alphabet[2].morsecode="-.-.";
    alphabet[3].morsecode="-..";
    alphabet[4].morsecode=".";
    alphabet[5].morsecode="..-.";
    alphabet[6].morsecode="--.";
    alphabet[7].morsecode="....";
    alphabet[8].morsecode="..";
    alphabet[9].morsecode=".---";
    alphabet[10].morsecode="-.-";
    alphabet[11].morsecode=".-..";
    alphabet[12].morsecode="--";
    alphabet[13].morsecode="-.";
    alphabet[14].morsecode="---";
    alphabet[15].morsecode=".--.";
    alphabet[16].morsecode="--.-";
    alphabet[17].morsecode=".-.";
    alphabet[18].morsecode="...";
    alphabet[19].morsecode="-";
    alphabet[20].morsecode="..-";
    alphabet[21].morsecode="...-";
    alphabet[22].morsecode=".--";
    alphabet[23].morsecode="-..-";
    alphabet[24].morsecode="-.--";
    alphabet[25].morsecode="--..";
    alphabet[26].morsecode="-----";
    alphabet[27].morsecode=".----";
    alphabet[28].morsecode="..---";
    alphabet[29].morsecode="...--";
    alphabet[30].morsecode="....-";
    alphabet[31].morsecode=".....";
    alphabet[32].morsecode="-....";
    alphabet[33].morsecode="--...";
    alphabet[34].morsecode="---..";
    alphabet[35].morsecode="----.";

}


/*              GAME BEGIN              */

void begin_game(){
    //TODO - call Della's welcome function
    player_lives = 3;       // Player has 3 lives to start with
    correct_answers = 0;    // Player has 0 correct answers to start with
    correct_sequences = 5;  // Player needs to enter 5 correct sequences to win the game
}


/*              LEVEL SELECT                */
int level_select(){
    // Level 1 selected .----
    if(strcmp(user_input, alphabet[27].morsecode) == 0){
        printf("LEVEL 1");
        return 1;
    }
    // Level 2 selected ..---
    else if(strcmp(user_input, alphabet[28].morsecode) == 0){
        printf("LEVEL 2");
        return 2;
    }
    //Level invalid
    else{
        printf("Invalid input. Please try again.");
        return -1;
    }
}

// Player can select level
void player_select(){
    select_level = true;;
}
// Player cannot select level
void player_select2(){
    select_level = false;;
}

// Returns the level selected
int get_level(){
    return level_selected;
}

// Sets the level selected
int set_level(int level){
    level_selected = level;
}


/**
 * @brief Level #1: Individual characters with their equivalent Morse code provided.
 *        Asks the user to enter the morse code equivalent of a given alphanumeric
 *        charcter.
 * 
 * @return int 
 */
int level_1(){
    level_selected = 1;
    int num = rand() %36;
    input_index = num;
    printf("Enter equivalent Morse code for the following charcter:\n");
    printf("%c \n",alphabet[num].alphanum);
    printf("%s \n",alphabet[num].morsecode);
    return num;
}

/**
 * @brief Level #2: Individual characters without their equivalent Morse code provided.
 *        Same function as level_1 except without giving the morse code equivalent.
 * 
 * @return int 
 */
int level_2(){
    level_selected = 2;
    int num = rand() %36;
    input_index = num;
    printf("Enter equivalent Morse code for the following charcter:\n");
    printf("%c \n",alphabet[num].alphanum);
    return num;
}

// Check if player can move on to next level
int progress_next(){
    if(correct_sequences == 5){
        return 1;
    }
    if(player_lives == 0){
        printf("GAME OVER");
        return 2;
    }
    return 0;
}


/*              INPUT HANDLING              */

// Intialize input array
void init_input(){
    int max = 30;
    for(int i = 0; i < max; i++){
        user_input[i] = '\0';
    }
    input_entered = 0;
    length_of_input = 0;
}

// Check for dot, dash, space, or enter
void input_handler(int player_input){
    // If player enters dot
    if(player_input == 1){
        user_input[length_of_input] = '.';
        length_of_input++;
        printf(".");
    }
    // If player enters dash
    else if(player_input == 2){
        user_input[length_of_input] = '-';
        length_of_input++;
        printf("-");
    }
    // If player enters space
    else if(player_input == 3){
        user_input[length_of_input] = ' ';
        length_of_input++;
        printf(" ");
    }
    // If player enters enter (select level)
    else if(player_input == 4 && select_level == true){
        user_input[length_of_input - 1] = '\0';
        input_entered = 1;
        level_select();
    }
    // If player enters enter (input morse code)
    else if(player_input == 4 && select_level == false){
        user_input[length_of_input - 1] = '\0';
        input_entered = 1;
        display_input();
        RGB_update(player_lives);
    }
}


/*              VALIDATE ANSWERS                */
int validate_input_sequence(){
    if(strcmp(user_input, alphabet[input_index].morsecode) == 0){
        return 1;
    }
    else{
        return 0;
    }
}

int validate_morse_code(){
    for(int i = 0; i < alphanumeric_chars; i++){
        if(user_input == alphabet[i].morsecode){
            return i;
        }
    }
    return -1;
}


/*              MESSAGES                */
bool continue_game(){
    if(player_lives <= 0){
        return 0;
    }
    return 1;
}

void progress_level(){
    printf("Completed level %d", level_selected);
}

void winner(){
    printf("Congratulations! You have won the game!");
}

void loser(){
    printf("You have lost the game. Better luck next time!");
}

void level_choice(){
    printf("Please select a level to play:");
    printf("Level 1: .----");
    printf("Level 2: ..---");
}

void invalid_input_entered(){
    printf("Invalid input. Please try again.");
}

void wrong_sequence(){
    printf("You entered %s", user_input);
    printf("The correct sequence is %s", alphabet[input_index].morsecode);
    player_lives--;
    printf("You have %d lives left", player_lives);
}

void correct_sequence(){
    printf("You entered %s", user_input);
    printf("The correct sequence is %s", alphabet[input_index].morsecode);
    correct_answers++;
    printf("You have %d correct sequences", correct_sequences);
    if(player_lives < 3){
        player_lives++;
        printf("You have gained a life! You now have %d lives", player_lives);
    }
}

void display_input(){
    if(input_index >= 0 && input_index <= 35){
        int ans = validate_input_sequence();
        if(input_entered == 1){
            if(ans == 1){
                correct_sequence();
            }
            else{
                wrong_sequence();
            }
        }
    }
    else{
        printf("Invalid input. Please try again.");
    }
}


// //Code for part 4
// void print_alphanum_morse(struct morsecode *this, char *input) {

//     int i = 0;
    
//     //check if morse code
//     while (strcmp(input, this[i]->morsecode) != 0 && i < 36) {
//         i++;
//     }

//     char output = malloc(char);

//     if (i >= 36)
//         output = "?";
//     else
//         output = this[i]->alphanum;

    
//     printf("Morse: %s\nAlphanumerical: %s\n", input, output);
    
//     free(output);
// }


int main() {
    // Initialise all STDIO as we will be using the GPIOs
    stdio_init_all();
    init_morse();
    watchdog_init();
 
    // Initialise the PIO interface with the WS2812 code
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, 0, offset, WS2812_PIN, 800000, IS_RGBW);
    put_pixel(urgb_u32(0x00,0x00,0x7f)); // Blue

    //TODO - Call Della's welcome screen function
    watchdog_update(); 

    // Should never get here due to the infinite while-loop.
    return 0;
}

