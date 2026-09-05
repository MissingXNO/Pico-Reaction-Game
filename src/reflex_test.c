/**
 * @file main.c
 * @brief This is the main and only file for the Reflex-testing game developed for RP2040 hardware in C language, By Santiago Giraldo Tabares and Ana María Velasco Montenegro, Electronics Dept, Faculty of Engineering, UdeA.
 */

#include <stdio.h>
#include "pico/stdlib.h" 
#include "hardware/gpio.h"
#include <time.h>
#include <stdbool.h>



#define FIRST_GPIO 2 /**< First GPIO pin */
#define BUTTON_GPIO (FIRST_GPIO+7) /**< 7 segment first GPIO pin (A) */
#define START_BUTTON 19            /**< Start button GPIO pin */

#define YELLOW_BUTTON 22 /**< Yellow button GPIO pin */
#define BLUE_BUTTON 21 /**< Blue button GPIO pin */
#define RED_BUTTON 20 /**< Red button GPIO pin */

#define YELLOW_LED 18   /**< Yellow LED GPIO pin */
#define BLUE_LED 17  /**< Blue LED GPIO pin */
#define RED_LED 16 /**< Red LED GPIO pin */

#define TRANSISTOR_1 12 /**< Transistor 1, base activation pin */
#define TRANSISTOR_2 13 /**< Transistor 2, base activation pin */
#define TRANSISTOR_3 14 /**< Transistor 3, base activation pin */
#define TRANSISTOR_4 15 /**< Transistor 4, base activation pin */

// 
int bits[10] = {
        0x3f,  // 0
        0x06,  // 1
        0x5b,  // 2
        0x4f,  // 3
        0x66,  // 4
        0x6d,  // 5
        0x7d,  // 6
        0x07,  // 7
        0x7f,  // 8
        0x6f   // 9
};/*!< Inverted bit patterns for common cathode display */


int counting = 0; /*!< State variable. 1 if the program is counting miliseconds, 0 if not counting so i whould be doing other stuff*/
int val = 0; /*!< stores the time as a whole number. Used to check if player takes more than 10000ms  */
int timer = 0; /*!< UNUSED*/
int uni = 0; /*!< Store the time (Units of a second) */
int dec = 0; /*!< Store the time (tens of a second) */
int cen = 0; /*!< Store the time (hundreds of a second) */
int mil = 0; /*!< Store the time (thousandths of a second) */
int i = 0; /*!< serves as addend while selecting transistor bases. With this, you can activate or deactivate any transistor using the lowest transistor pin number and adding i. Only works if transistor pins are connected consequently  */
int flag = 0; /*!< If this flag is 1, the program is in the counting section. If flag is 0, the program executes the "wait for start button" which also includes de 3 LED countdown, the random time waiting and random LED selection*/
int32_t mask; /*!< Mask used to activate or diactive segments of the displays according to the number to represent on them*/

bool prev_blue_state = true; /*!< Previous state for Blue button. Used to act against bouncing and detecting rising/falling edges */
bool prev_red_state = true;  /*!< Previous state for Red button. Used to act against bouncing and detecting rising/falling edges*/
bool prev_yellow_state = true; /*!< Previous state for Yellow button. Used to act against bouncing and detecting rising/falling edges*/

/**
 * @brief Main function of the program. Here is where everything takes place. Inside the main, Predefined pin ports are initialized. also, the while loop takes place until power is off. 
 * @return 0 on successful execution.
 */
int main() {
    stdio_init_all();
    //printf("Hello, 7segment - press button to count down!\n");


    //Initialize every segment of the 7 segment displays
    for (int gpio = FIRST_GPIO; gpio < FIRST_GPIO + 7; gpio++) { //init segments from A to G
        gpio_init(gpio);
        gpio_set_dir(gpio, GPIO_OUT);
    }
    // Initialize Start button
    gpio_init(START_BUTTON);
    gpio_set_dir(START_BUTTON, GPIO_IN);
    gpio_pull_up(START_BUTTON);


    // Initialize three color buttons
    gpio_init(YELLOW_BUTTON);
    gpio_set_dir(YELLOW_BUTTON, GPIO_IN);
    gpio_pull_up(YELLOW_BUTTON);

    gpio_init(BLUE_BUTTON);
    gpio_set_dir(BLUE_BUTTON, GPIO_IN);
    gpio_pull_up(BLUE_BUTTON);

    gpio_init(RED_BUTTON);
    gpio_set_dir(RED_BUTTON, GPIO_IN);
    gpio_pull_up(RED_BUTTON);

    gpio_init(BUTTON_GPIO);
    gpio_set_dir(BUTTON_GPIO, GPIO_IN);
    gpio_pull_up(BUTTON_GPIO);


    // Initialize three color LEDs
    gpio_init(YELLOW_LED);
    gpio_set_dir(YELLOW_LED, GPIO_OUT);

    gpio_init(BLUE_LED);
    gpio_set_dir(BLUE_LED, GPIO_OUT);

    gpio_init(RED_LED);
    gpio_set_dir(RED_LED, GPIO_OUT);

    // Initialize pin ports for transistor bases.
    gpio_init(TRANSISTOR_1);
    gpio_set_dir(TRANSISTOR_1, GPIO_OUT);
    
    gpio_init(TRANSISTOR_2);
    gpio_set_dir(TRANSISTOR_2, GPIO_OUT);

    gpio_init(TRANSISTOR_3);
    gpio_set_dir(TRANSISTOR_3, GPIO_OUT);

    gpio_init(TRANSISTOR_4);
    gpio_set_dir(TRANSISTOR_4, GPIO_OUT);
    



    while (true) {

      if(flag == 0){
        if(!gpio_get(START_BUTTON)){
          counting = 0;
          val = 0;
          timer = 0;
          uni = 0;
          dec = 0;
          cen = 0;
          mil = 0;
          i = 0;
          flag = 0;

          printf("GET READY!\n");
          //turn on 3 leds
          gpio_put(YELLOW_LED,1);
          gpio_put(BLUE_LED,1);
          gpio_put(RED_LED,1);
          sleep_ms(1000);
          //turn off led 3
          gpio_put(YELLOW_LED,0);
          sleep_ms(1000);
          //turn off led 2
          gpio_put(BLUE_LED,0);
          sleep_ms(1000);
          //turn off led 1
          gpio_put(RED_LED,0);
          sleep_ms(1000);

          //Wait a random time (from 1000 ms to 10000 ms)
          int random_time;
          srand(time(NULL));
          random_time = (rand() % (10000 - 1000 + 1)) + 1000;
          printf("Random time: %d\n", random_time);
          sleep_ms(random_time);

          //once random_time passed, select a random LED.
          int random_led;
          //srand(time(NULL));
          random_led = (rand() % (3)) + 1;
          printf("Random led: %d\n", random_led);
          
          //once random led selected, put random led on.
          gpio_put(random_led+15,1);
          val = 0;
          flag = 1;
          counting = 1;
        }
      }

      else{   //if counting...


        gpio_put(TRANSISTOR_1+i, 1); // Turn on transistor1 +i
            
            if (!gpio_get(YELLOW_BUTTON) && prev_yellow_state) { // Rising edge on yellow button
                printf("(Yellow Button)\n");
                prev_yellow_state = false;
                if(gpio_get(YELLOW_LED)){
                  printf("YELLOW MATCH\n");
                  counting = 0;
                }
                else{
                  printf("PENALIZACION!!!!!!!!!!!!!!!!!\n");
                  if(val>=9999  || uni>=10){
                    printf("PERDISTE\n");
                    val = 9999;   //If after penalty, player reaches more than 9999, stops counting and set every display to 9.
                    uni = 9;
                    dec = 9;
                    cen = 9;
                    mil = 9;
                    counting = 0;
                  }
                  else{
                    val = val + 1000;
                    uni++;
                  }
                }
            } else if (gpio_get(YELLOW_BUTTON) && !prev_yellow_state) {
                prev_yellow_state = true;
            } else if (!gpio_get(BLUE_BUTTON) && prev_blue_state) { // Rising edge on blue button
                printf("(Blue Button)\n");
                prev_blue_state = false;
                if(gpio_get(BLUE_LED)){
                  printf("BLUE MATCH\n");
                  counting = 0;
                }
                else{
                  printf("PENALIZACION!!!!!!!!!!!!!!!!!\n");
                  if(val>=9999  || uni>=10){
                    printf("PERDISTE\n");
                    val = 9999;
                    uni = 9;
                    dec = 9;
                    cen = 9;
                    mil = 9;
                    counting = 0;
                  }
                  else{
                    val = val + 1000;
                    uni++;
                  }
                }
            } else if (gpio_get(BLUE_BUTTON) && !prev_blue_state) {
                prev_blue_state = true;
            } else if (!gpio_get(RED_BUTTON) && prev_red_state) { // Rising edge on red button
                printf("(Red Button)\n");
                prev_red_state = false;
                if(gpio_get(RED_LED)){
                  printf("RED MATCH\n");
                  counting = 0;
                }
                else{
                  printf("PENALIZACION!!!!!!!!!!!!!!!!!\n");
                  if(val>=9999  || uni>=10){
                    printf("PERDISTE\n");
                    val = 9999;
                    uni = 9;
                    dec = 9;
                    cen = 9;
                    mil = 9;
                    counting = 0;
                  }
                  else{
                    val = val + 1000;
                    uni++;
                  }
                }
            } else if (gpio_get(RED_BUTTON) && !prev_red_state) {
                prev_red_state = true;
            }

            if (!gpio_get(START_BUTTON)) {
              flag = 0;        
              counting = 0;
            }

        if (counting) {
            if (val >= 9999) {
                printf("PERDISTE\n");
                counting = 0;
            } else {
                val++;
                // Update units
                mil++;
                if (mil >= 10) {
                    mil = 0;
                    // Update tens
                    cen++;
                    if (cen >= 10) {
                        cen = 0;
                        // Update hundreds
                        dec++;
                        if (dec >= 10) {
                            dec = 0;
                            // Update thousands
                            uni++;
                            if (uni >= 10) {
                                uni = 0;
                            }
                        }
                    }
                }
            }
        }

      // Select a mask according to which transistor is ON. i.e, if the Display at the very left is on, mask should correspond to milliseconds.
        if(i==0){
          mask = bits[mil] << FIRST_GPIO;
        }

        if(i==1){
          mask = bits[cen] << FIRST_GPIO;
        }

        if(i==2){
          mask = bits[dec] << FIRST_GPIO;
        }

        if(i==3){
          mask = bits[uni] << FIRST_GPIO;
        }        
        

        gpio_set_mask(mask);
        sleep_ms(1);
        gpio_clr_mask(mask);
        printf("%d %d %d %d \n", uni,dec,cen,mil);


      }
      //change the active transistor addend i. if the addend reaches 4, start from 0 again.
      gpio_put(TRANSISTOR_1+i,0);
      i++;
      if(i==4){
        i=0;
      }
      //if Start button is pressed in the counting routine, resets the game.
      if(!gpio_get(START_BUTTON)){
        flag=0;
        counting=0;
        printf("TERMINADA LA OPERACION\n");
        //counting=0;
      }  

    }


}