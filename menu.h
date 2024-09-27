#ifndef MENU_H
#define MENU_H

#include <curses.h>
#include <stdlib.h>
#include <string.h>

struct button {

  char *text;
};

struct menu {

  bool draw_borders;
  char *top;
  char *bot;
  char *left;
  char *right;
  char *top_left;
  char *top_right;
  char *bot_left;
  char *bot_right;
  bool has_selected;
  int selected_col;
  int normal_col;
  int num_buttons;
  int selected;
  struct button buttons[20];
};

struct button get_button(struct menu m, int i);

void draw_button(struct button b, struct menu m, int col, int scrw);
void draw_buttons(struct menu m, int scrw, int gaps[]);

void handle_input(struct menu *m);

void to_center(int len, int scrw);

#endif
