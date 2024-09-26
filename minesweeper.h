
#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main();

void setup_grid();
void init_curses();
void run();
void run_loop();
void lose();
void win();

void reveal_bombs();
void place_bomb(int exclx, int excly);
void place_bombs();
int count_bombs(int x, int y);

int get_cell(int x, int y);
void set_cell(int x, int y, int value);

void process_input(char c);
void move_cursor(int x, int y);
void reveal_cell(int x, int y);
void place_flag();
void check_win();
void toggle_borders();

bool do_bold(int x, int y);
void draw_top(float timer);
void draw_grid();
int get_cell_color(int x, int y);
char *get_cell_char(int x, int y);

#endif
