#pragma output REGISTER_SP = 0xD000

#include <arch/zx.h>
#include <arch/zx/sp1.h>
#include <input.h>
#include <stdlib.h>
#include <intrinsic.h>
#include "int.h"
#include <stdio.h>

struct sp1_Rect full_screen = {0, 0, 32, 24};

#define MAX_STARS 10
#define MAX_SIMPLE_STARS 100

// Bubble GFX
extern unsigned char bubble_col1[];
extern unsigned char bubble_col2[];

// Star tiles animation
extern unsigned char star_gfx[];
extern unsigned char star1_gfx[];
extern unsigned char star2_gfx[];
extern unsigned char star3_gfx[];
extern unsigned char star4_gfx[];

// Bubble sprite structure and variable declaration
struct my_sprite {
    struct sp1_ss *s;
    uint8_t x, y;
};
struct my_sprite bubble1;

// Stars tiles structure and variable declaration
struct my_stars {    
    uint8_t star_frame;                     // The current animation frame
    unsigned char star_animation_dir;       // The animation direction (increment or decrement)
    unsigned char star_animate;             // Can we animate it?
    uint16_t row;                           // star row
    uint16_t col;                           // star column
    uint8_t color;                          // star color
    uint16_t tile_char;                     // assigned character
};
struct my_stars stars[MAX_STARS];                  // declare the start object

struct sp1_update *star_list[MAX_STARS + 1];           // declare the star regions array to update (must end with 0)

int i;
int c = 66; // characters assigned start at 66 ('B')
unsigned char star_color[7] = {INK_BLUE, INK_RED, INK_MAGENTA, INK_GREEN, INK_CYAN, INK_YELLOW, INK_WHITE};

// get the player keys input and update our sprite x/y
void get_movement(){

    if (in_key_pressed( IN_KEY_SCANCODE_q )) {
        --bubble1.y;
    }
    if (in_key_pressed( IN_KEY_SCANCODE_a )) {
        ++bubble1.y;
    }
    if (in_key_pressed( IN_KEY_SCANCODE_o )) {
        --bubble1.x;
    }
    if (in_key_pressed( IN_KEY_SCANCODE_p )) {
        ++bubble1.x;
    }

}

// create sprite bubbles
void create_bubbles() {

    bubble1.s = sp1_CreateSpr(SP1_DRAW_MASK2LB, SP1_TYPE_2BYTE, 3, (int)bubble_col1, 10);
    sp1_AddColSpr(bubble1.s, SP1_DRAW_MASK2,    SP1_TYPE_2BYTE, (int)bubble_col2, 10);
    sp1_AddColSpr(bubble1.s, SP1_DRAW_MASK2RB,  SP1_TYPE_2BYTE, 0, 10);
    bubble1.x = 110;
    bubble1.y = 90;

}

// So let's create the stars
void init_star() {

    // create a couple of single stars, taht will not be animated (single dot actualy)
    sp1_TileEntry(65, star_gfx);    // assign to char 65 ('A')
    i = 0;
    while(i<MAX_SIMPLE_STARS) {
        sp1_PrintAt(rand() % 23, rand() % 31, star_color[rand() % 7], 65); //Print the simple stars on screen
        i++;
    }

    // Let's create some complex start. Those will be animated
    i = 0;
    while(i<MAX_STARS) {

        stars[i].star_frame = 3;
        stars[i].star_animation_dir = 0;
        stars[i].star_animate = 0;
        stars[i].row = rand() % 23;
        stars[i].col = rand() % 31;
        stars[i].color = star_color[rand() % 7];
        stars[i].tile_char = c;
        sp1_TileEntry(c, star4_gfx);    // Assign character to the star4 gfx (they always start wide). C contains the charcater code value (66 up)

        star_list[i]=sp1_GetUpdateStruct(stars[i].row, stars[i].col); // Let's keep the tile of where this star is so we can invalidate in the main loop

        sp1_PrintAt(stars[i].row, stars[i].col, stars[i].color, stars[i].tile_char); // print it on the screen

        c++;
        i++;
    }
    star_list[MAX_STARS]=0; // Remember, the array we feed to sp1_IterateUpdateArr needs to end in 0!!!
    
}

// let's animate the stars
void animate_stars() {

    // Every 25frame
    if(interrupt_500ms == 1) {

        i = 0;
        while(i<MAX_STARS) {

            // If the star is currently in animation, see it will start
            if(stars[i].star_animate==0) {
                stars[i].star_animate = rand() % 10;
            }

            i++;
        }

        interrupt_500ms = 0;
    }

    // Every ~12frames, animate the stars
    if(interrupt_250ms == 1) {

        i = 0;
        while(i<MAX_STARS) {

            if(stars[i].star_animate > 8) { // Can we animate this one (~20% chance)?
                
                // If we can, let's find in which frame it is and change the tile to the correct GFX
                // Note that we do not PRINT anything to screen, we have already done that in the init.
                if(stars[i].star_frame == 0)
                    sp1_TileEntry(stars[i].tile_char, star1_gfx);
                if(stars[i].star_frame == 1)
                    sp1_TileEntry(stars[i].tile_char, star2_gfx);
                if(stars[i].star_frame == 2)
                    sp1_TileEntry(stars[i].tile_char, star3_gfx);
                if(stars[i].star_frame == 3)
                    sp1_TileEntry(stars[i].tile_char, star4_gfx);

                // control in which way the animation flows
                if(stars[i].star_animation_dir == 1) {
                    if(stars[i].star_frame==3) {
                        stars[i].star_animation_dir = 0;
                        stars[i].star_animate = 0;
                    } else {
                        stars[i].star_frame++;
                    }
                } else {
                    if(stars[i].star_frame==0) {
                        stars[i].star_animation_dir = 1;
                    } else {
                        stars[i].star_frame--;
                    }
                }
                            
            } else {

                // This one can not animate, let's reset and try next time
                stars[i].star_animate = 0;
            }

            i++;
        }

        // So after all the GFX has been updated to the new graphics with sp1_TileEntry
        // We now invalidate the tiles (remember the list built with sp1_GetUpdateStruct?)
        sp1_IterateUpdateArr(star_list, sp1_InvUpdateStruct);

        interrupt_250ms = 0;

    }
}

int main() {

    zx_border(INK_WHITE);

    setup_int();

    sp1_Initialize(SP1_IFLAG_MAKE_ROTTBL | SP1_IFLAG_OVERWRITE_TILES | SP1_IFLAG_OVERWRITE_DFILE,
                   INK_WHITE | PAPER_BLACK, ' ' );
            
    sp1_Invalidate(&full_screen);

    create_bubbles();

    init_star();

    srand(ticker);

    while(1) {

        get_movement();

        sp1_MoveSprPix(bubble1.s, &full_screen, 0, bubble1.x, bubble1.y);

        animate_stars();

        intrinsic_halt();
        sp1_UpdateNow();
    }
}