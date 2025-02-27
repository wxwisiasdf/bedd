#ifndef __BEDD_H__
#define __BEDD_H__

#include <syntax.h>
#include <time.h>
#include <io.h>

typedef struct bd_config_t bd_config_t;
typedef struct bd_cursor_t bd_cursor_t;
typedef struct bd_view_t bd_view_t;

struct bd_config_t {
  int indent_width;  // anything other than 2 or 4 would be cringe af, but we must still provide room for customization and blah blah blah :p
  int indent_spaces; // afects saving/loading, as in editing they're still shown as spaces no matter what
  int scroll_step;   // lines to move per scroll step
  
  const char *syntax_colors[st_color_count];
};

struct bd_cursor_t {
  int x, y;
};

#define BD_CURSOR_MIN(a, b)   ((a).y < (b).y ? (a) : ((a).y == (b).y ? ((a).x < (b).x ? (a) : (b)) : (b)))
#define BD_CURSOR_MAX(a, b)   ((a).y > (b).y ? (a) : ((a).y == (b).y ? ((a).x > (b).x ? (a) : (b)) : (b)))
#define BD_CURSOR_LESS(a, b)  ((a).y < (b).y || ((a).y == (b).y && (a).x < (b).x))
#define BD_CURSOR_GREAT(a, b) ((a).y > (b).y || ((a).y == (b).y && (a).x > (b).x))

enum {
  bd_view_welcome,
  bd_view_text,
  bd_view_explore,
};

struct bd_view_t {
  char title[256];
  int title_dirty;
  
  int type;
  
  bd_cursor_t cursor;
  void *data;
};

extern bd_config_t bd_config;

extern bd_view_t *bd_views;
extern int bd_view_count, bd_view;

extern int bd_width, bd_height;
extern time_t bd_time;

bd_view_t *bd_view_add(const char *title, int type, ...);
void       bd_view_remove(bd_view_t *view);
void       bd_view_draw(bd_view_t *view);
int        bd_view_event(bd_view_t *view, io_event_t event);

void bd_welcome_draw(bd_view_t *view);

void bd_text_draw(bd_view_t *view);
int  bd_text_event(bd_view_t *view, io_event_t event);
void bd_text_load(bd_view_t *view, const char *path);
int  bd_text_save(bd_view_t *view, int closing); // returns 1 if should close (when closing)

void bd_explore_draw(bd_view_t *view);
int  bd_explore_event(bd_view_t *view, io_event_t event);
void bd_explore_load(bd_view_t *view, const char *path);

void bd_global_draw(void);
int  bd_global_click(int x, int y);

// Dialog formatting:
// - "(1-9)[...]" -> Informational text (with number specifying length)
// - "i[...]" -> Text input (with prompt inside) -> Expects (char *)
// - "b[count;button_1;button_2;...]" -> Button line (with button count and titles inside, returned number corresponds to button index)
// 
// NOTE: Ctrl+Q returns 0, while Enter returns the index of the selected button

int bd_dialog(const char *title, int width, const char *format, ...);

#endif
