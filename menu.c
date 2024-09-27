#include "menu.h"

void draw_button(struct button b, struct menu m, int col, int scrw) {

  attron(COLOR_PAIR(col));

  char top_bar[100] = "";
  char bot_bar[100] = "";

  if (m.draw_borders) {

    for (int i = 0; i < strlen(m.top_left); i++) {
      strcat(top_bar, " ");
    }
    strcat(top_bar, m.top_left);
    for (int i = 0; i < strlen(b.text); i++) {
      strcat(top_bar, m.top);
    }
    strcat(top_bar, m.top_right);
    for (int i = 0; i < strlen(m.top_right); i++) {
      strcat(top_bar, " ");
    }

    for (int i = 0; i < strlen(m.bot_left); i++) {
      strcat(bot_bar, " ");
    }
    strcat(bot_bar, m.bot_left);
    for (int i = 0; i < strlen(b.text); i++) {
      strcat(bot_bar, m.bot);
    }
    strcat(bot_bar, m.bot_right);
    for (int i = 0; i < strlen(m.bot_right); i++) {
      strcat(bot_bar, " ");
    }

    to_center(strlen(top_bar), scrw);
    printw("%s\n\r", top_bar);
  }
  to_center(strlen(m.draw_borders ? top_bar : b.text), scrw);
  printw("%s%s%s\n\r", m.left, b.text, m.right);

  if (!m.draw_borders) {

    attroff(COLOR_PAIR(col));
    return;
  }

  to_center(strlen(bot_bar), scrw);
  printw("%s\n\r", bot_bar);

  attroff(COLOR_PAIR(col));
}

void draw_buttons(struct menu m, int scrw) {
  for (int i = 0; i < m.num_buttons; i++) {
    struct button b = m.buttons[i];
    draw_button(b, m, m.selected == i ? m.selected_col : m.normal_col, scrw);
  }
}

struct button get_button(struct menu m, int i) { return m.buttons[i]; }

void handle_input(struct menu *m) {

  switch (getch()) {
  case 'k':
  case 'w':
    m->selected -= m->selected <= 0 ? 0 : 1;
    break;

  case 'j':
  case 's':
    m->selected += m->selected >= m->num_buttons - 1 ? 0 : 1;
    break;

  case ' ':
    m->has_selected = true;
    break;
  }
}

void to_center(int len, int scrw) {

  int fwidth = (scrw / 2) - len / 2;
  char empty[100] = "";
  for (int i = 0; i < fwidth; i++) {
    strcat(empty, " ");
  }
  printw("%s", empty);
}
