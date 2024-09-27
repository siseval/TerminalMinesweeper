#include "minesweeper.h"

int *grid;
size_t width = 10;
size_t height = 10;

int scrw, scrh;

int cur_cell[2];
bool first_reveal = true;
int num_bombs = 15;
int num_flags = 0;
bool lost = false;
bool won = false;

char prev_input = '1';

int difficulties[6] = {9, 10, 16, 40, 21, 99};
int cur_difficulty = 0;

char *h_line = "-";
char *v_line = "|";
char *corner = "+";

bool show_borders = true;

char *empty_cell = " ";
char *flag_cell = "P";
char *cursor_cell[2] = {"=", "#"};

char *empty_cell_nb = ".";
char *flag_cell_nb = "P";
char *cursor_cell_nb[2] = {"=", "#"};

char *nums[11] = {"+", "1", "2", "3", "4", "5", "6", "7", "8", "*", "+"};

clock_t start_time;
float prev_time;
float refresh_ms = 100;

clock_t last_flash;
float flash_time = 0.1f;
bool flashing = false;

int main() {

  setlocale(LC_ALL, "");
  init_curses();

  run();
  return 0;
}

void setup_grid() {

  free(grid);
  grid = malloc(width * height * sizeof *grid);
}

void init_curses() {

  initscr();
  cbreak();
  noecho();
  timeout(refresh_ms);
  curs_set(0);
  start_color();
  use_default_colors();

  init_pair(1, COLOR_CYAN, -1);
  init_pair(2, COLOR_BLUE, -1);
  init_pair(3, COLOR_GREEN, -1);
  init_pair(4, COLOR_MAGENTA, -1);
  init_pair(5, COLOR_RED, -1);
  init_pair(6, COLOR_RED, -1);
  init_pair(7, COLOR_RED, -1);
  init_pair(8, COLOR_RED, -1);
  init_pair(9, COLOR_RED, -1);
  init_pair(10, COLOR_RED, -1);
  init_pair(11, COLOR_WHITE, -1);
  init_pair(12, COLOR_YELLOW, -1);
}

void set_difficulty(int difficulty) {

  width = difficulties[2 * difficulty];
  height = difficulties[2 * difficulty];
  num_bombs = difficulties[2 * difficulty + 1];

  cur_difficulty = difficulty;
}

void run() {

  attron(A_BOLD);

  char *top_text = "=: Select difficulty :=";

  struct button easy = {"Easy"};
  struct button medium = {"Medium"};
  struct button hard = {"Hard"};
  struct button quit = {"Quit"};

  int gaps[4] = {0, 0, 0, 2};

  struct menu m = {
      true,      " ",         " ",       ":: ",    " ::", " ", " ",
      " ",       " ",         false,     12,       11,    4,   cur_difficulty,
      easy.text, medium.text, hard.text, quit.text};

  while (!m.has_selected) {
    getmaxyx(stdscr, scrh, scrw);
    clear();
    printw("\n\n\n");
    to_center(strlen(top_text), scrw);
    printw(top_text);
    printw("\n\n\n\r");
    draw_buttons(m, scrw, gaps);
    handle_input(&m);
  }

  if (m.selected == 3) {
    exit(0);
  }

  set_difficulty(m.selected);

  cur_cell[0] = width / 2;
  cur_cell[1] = height / 2;

  setup_grid();
  first_reveal = true;
  num_flags = 0;
  lost = false;
  won = false;
  start_time = clock();
  last_flash = clock();
  run_loop();
}

void run_loop() {

  while (true) {

    process_input(getch());

    if ((float)(clock() - last_flash) / CLOCKS_PER_SEC > flash_time / 100) {
      flashing = !flashing;
      last_flash = clock();
    }

    clear();

    getmaxyx(stdscr, scrh, scrw);

    bool over = (lost || won);
    draw_top(over ? prev_time : (float)(clock() - start_time) / CLOCKS_PER_SEC);
    draw_grid();
  }
}

void lose() {

  lost = true;
  reveal_bombs();
  draw_grid();
}
void win() {

  won = true;
  draw_grid();
}
void reveal_bombs() {
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      if (get_cell(j, i) == 9) {
        reveal_cell(j, i);
      }
    }
  }
}

void place_bomb(int exclx, int excly) {

  bool placed = false;
  srand(time(NULL));
  while (!placed) {

    int x = rand() % width;
    int y = rand() % height;

    if (x >= exclx - 1 && x <= exclx + 1 && y >= excly - 1 && y <= excly + 1) {
      continue;
    }

    if (get_cell(x, y) != 9) {
      set_cell(x, y, 9);
      placed = true;
    }
  }
}

void place_bombs() {

  for (int i = 0; i < num_bombs; i++) {
    place_bomb(cur_cell[0], cur_cell[1]);
  }

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {

      if (get_cell(j, i) != 9) {
        set_cell(j, i, count_bombs(j, i));
      }
    }
  }
}
int count_bombs(int x, int y) {
  int count = 0;
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      count += get_cell(x + j, y + i) == 9 ? 1 : 0;
    }
  }
  return count;
}

int get_cell(int x, int y) {

  if (x < 0 || x >= width || y < 0 || y >= height) {
    return 0;
  }
  return grid[y * width + x];
}
void set_cell(int x, int y, int value) {

  if (x < 0 || x >= width || y < 0 || y >= height) {
    return;
  }
  grid[y * width + x] = value;
}

bool is_cursor(int x, int y) { return x == cur_cell[0] && y == cur_cell[1]; }

void process_input(char input) {

  switch (input) {
  case 'k':
  case 'w':
    move_cursor(0, -1);
    break;
  case 'h':
  case 'a':
    move_cursor(-1, 0);
    break;
  case 'j':
  case 's':
    move_cursor(0, 1);
    break;
  case 'l':
  case 'd':
    move_cursor(1, 0);
    break;

  case ' ':
    reveal_cell(cur_cell[0], cur_cell[1]);
    break;

  case 'f':
    place_flag();
    break;

  case 'b':
    toggle_borders();
    break;

  case 'r':
    run();
    break;

  case 'q':
    exit(0);
    break;
  }

  prev_input = isdigit(input) ? input : prev_input;
}

void move_cursor(int dx, int dy) {

  if (lost || won) {
    return;
  }

  dx = dx * (prev_input - '0');
  dy = dy * (prev_input - '0');

  prev_input = '1';

  if (cur_cell[0] + dx < 0) {
    cur_cell[0] = 0;
    return;
  }
  if (cur_cell[0] + dx >= width) {
    cur_cell[0] = width - 1;
    return;
  }
  if (cur_cell[1] + dy < 0) {
    cur_cell[1] = 0;
    return;
  }
  if (cur_cell[1] + dy >= height) {
    cur_cell[1] = height - 1;
    return;
  }

  cur_cell[0] += dx;
  cur_cell[1] += dy;
}

void reveal_cell(int x, int y) {

  if (x < 0 || x >= width || y < 0 || y >= height) {
    return;
  }
  if (get_cell(x, y) < 0) {
    return;
  }

  if (first_reveal) {
    place_bombs();
    first_reveal = false;
  }

  if (get_cell(x, y) == 9) {
    set_cell(x, y, -10);
    if (!lost) {
      lose();
    }
    return;
  }

  if (get_cell(x, y) >= 20) {
    set_cell(x, y, get_cell(x, y) - 20);
  }

  set_cell(x, y, -1 - get_cell(x, y));

  if (get_cell(x, y) != -1) {
    return;
  }

  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      if (i == 0 && j == 0) {
        continue;
      }
      reveal_cell(x + j, y + i);
    }
  }
}

void place_flag() {

  int cur = get_cell(cur_cell[0], cur_cell[1]);
  if (cur < 0) {
    return;
  }
  set_cell(cur_cell[0], cur_cell[1], cur < 20 ? cur + 20 : cur - 20);
  num_flags += cur < 20 ? 1 : -1;
  check_win();
}

void check_win() {

  if (num_flags != num_bombs) {
    return;
  }
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      if (get_cell(j, i) >= 20 && get_cell(j, i) != 29) {
        return;
      }
    }
  }
  win();
}

void toggle_borders() { show_borders = !show_borders; }

bool do_bold(int x, int y) {

  return is_cursor(x, y) || get_cell(x, y) >= 20 || get_cell(x, y) < -1;
}

int num_length(int value) {

  int l = 1;
  while (value > 9) {
    l++;
    value /= 10;
  }
  return l;
}

void draw_top(float timer) {

  attron(A_BOLD);
  printw("\n\r");

  if (won) {
    attron(COLOR_PAIR(12));
  } else if (lost) {
    attron(COLOR_PAIR(9));
  } else {
    attron(COLOR_PAIR(11));
  }

  char *top = "Timer: %.0fs <*> Bombs: %d/%d";
  int timer_len = num_length((int)timer * 100);
  int flags_len = num_length(num_flags);
  int bombs_len = num_length(num_bombs);

  int n_length = timer_len + flags_len + bombs_len - 7;

  to_center(strlen(top) + n_length, scrw);
  printw(top, timer * 100, num_flags, num_bombs);

  prev_time = timer;

  printw("\n\r");
  attroff(A_BOLD);
}

void draw_grid() {

  if (!show_borders) {
    for (int i = 0; i < height; i++) {
      to_center(width * 2, scrw);
      for (int j = 0; j < width; j++) {

        do_bold(j, i) ? attron(A_BOLD) : attroff(A_BOLD);

        attron(COLOR_PAIR(get_cell_color(j, i)));
        printw("%s ", get_cell_char(j, i));
      }
      printw("\n\r");
    }
    return;
  }

  for (int i = 0; i < height * 2 + 1; i++) {
    to_center(width * 4, scrw);
    if (i % 2 == 0) {

      attron(COLOR_PAIR(11));

      for (int j = 0; j < width * 2 + 1; j++) {
        printw(j % 2 == 0 ? "%s" : " %s ", j % 2 == 0 ? corner : h_line);
      }
      printw("\n\r");
      continue;
    }

    for (int j = 0; j < width * 2 + 1; j++) {

      bool even = j % 2 == 0;

      do_bold(j / 2, i / 2) && !even ? attron(A_BOLD) : attroff(A_BOLD);
      attron(COLOR_PAIR(even ? 11 : get_cell_color(j / 2, i / 2)));
      printw(even ? "%s" : " %s ", even ? v_line : get_cell_char(j / 2, i / 2));
    }
    printw("\n\r");
  }
}

int get_cell_color(int x, int y) {

  int cur = get_cell(x, y);

  if (cur >= 20) {
    return won ? 12 : 10;
  }
  if (cur < -1) {
    return abs(cur) - 1;
  }
  return 11;
}
char *get_cell_char(int x, int y) {

  if (is_cursor(x, y) && !(lost || won)) {
    return show_borders ? cursor_cell[flashing] : cursor_cell_nb[flashing];
  }

  int cur = get_cell(x, y);

  if (cur >= 20) {
    return show_borders ? flag_cell : flag_cell_nb;
  }

  if (cur < 0) {
    return nums[-cur - 1 + (cur == -10 ? flashing : 0)];
  }

  return show_borders ? empty_cell : empty_cell_nb;
}
