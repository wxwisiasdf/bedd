#include <string.h>
#include <bedd.h>
#include <io.h>

bd_view_t *bd_views = NULL;
int bd_view_count = 0, bd_view = 0;

int bd_width, bd_height;
time_t bd_time;

int main(int argc, const char **argv) {
  io_init();
  
  for (int i = 1; i < argc; i++) {
    char buffer[256];
    strcpy(buffer, argv[i]);
    
    while (buffer[0] == '.' && buffer[1] == '/') {
      memmove(buffer, buffer + 2, sizeof(buffer) - 2);
    }
    
    if (buffer[0] == '.' && buffer[1] == '.' && (!buffer[2] || buffer[2] == '/')) {
      io_dsolve(argv[i], buffer);
    }
    
    io_file_t file = io_dopen(buffer);
    
    if (io_dvalid(file)) {
      io_dclose(file);
      bd_view_add(buffer, bd_view_explore, buffer);
      
      continue;
    }
    
    file = io_fopen(buffer, 0);
    
    if (io_fvalid(file)) {
      io_fclose(file);
      bd_view_add(buffer, bd_view_text, buffer);
      
      continue;
    }
  }
  
  if (!bd_view_count) {
    bd_view_add("Welcome", bd_view_welcome);
  }
  
  int global_draw = 1;
  int view_draw = 1;
  
  int do_exit = 0;
  
  while (!do_exit) {
    io_event_t event;
    int handled = 0;
    
    if ((event = io_get_event()).type) {
      if (event.type == IO_EVENT_TIME_SECOND) {
        bd_time = event.time;
        handled = 1;
        
        global_draw = 1;
      } else if (event.type == IO_EVENT_RESIZE) {
        bd_width = event.size.width;
        bd_height = event.size.height;
        
        handled = 1;
        
        global_draw = 1;
        view_draw = 1;
      } else if (event.type == IO_EVENT_KEY_PRESS) {
        if (event.key == IO_CTRL('Q')) {
          bd_view_remove(bd_views + bd_view);
          if (!bd_view_count) do_exit = 1;
          
          handled = 1;
          
          global_draw = 1;
          view_draw = 1;
        } else if (event.key == IO_CTRL('W')) {
          bd_view = bd_view_add("Welcome", bd_view_welcome) - bd_views;
          handled = 1;
          
          global_draw = 1;
          view_draw = 1;
        } else if (event.key == IO_CTRL('N')) {
          bd_view = bd_view_add("", bd_view_text, NULL) - bd_views;
          handled = 1;
          
          global_draw = 1;
          view_draw = 1;
        } else if (event.key == IO_CTRL('O')) {
          char buffer[256];
          buffer[0] = '\0';
          
          for (;;) {
            int result = bd_dialog("Open file (Ctrl+Q to cancel)", -16, "i[Path:]b[1;Open]", buffer);
            
            if (result) {
              while (buffer[0] == '.' && buffer[1] == '/') {
                memmove(buffer, buffer + 2, sizeof(buffer) - 2);
              }
              
              if (buffer[0] == '.' && buffer[1] == '.' && (!buffer[2] || buffer[2] == '/')) {
                char temp[256];
                strcpy(temp, buffer);
                
                io_dsolve(temp, buffer);
              }
              
              io_file_t file = io_dopen(buffer);
              
              if (io_dvalid(file)) {
                io_dclose(file);
                
                bd_view = bd_view_add(buffer, bd_view_explore, buffer) - bd_views;
                break;
              }
              
              file = io_fopen(buffer, 0);
              
              if (io_fvalid(file)) {
                io_fclose(file);
                
                bd_view = bd_view_add(buffer, bd_view_text, buffer) - bd_views;
                break;
              }
            } else {
              break;
            }
          }
          
          handled = 1;
          
          global_draw = 1;
          view_draw = 1;
        } else if (event.key == IO_CTRL(IO_ARROW_LEFT)) {
          if (bd_view) {
            bd_view--;
            
            global_draw = 1;
            view_draw = 1;
          }
          
          handled = 1;
        } else if (event.key == IO_CTRL(IO_ARROW_RIGHT)) {
          if (bd_view < bd_view_count - 1) {
            bd_view++;
            
            global_draw = 1;
            view_draw = 1;
          }
          
          handled = 1;
        }
      } else if (event.type == IO_EVENT_MOUSE_DOWN) {
        if (event.mouse.y < 2) {
          if (bd_global_click(event.mouse.x, event.mouse.y)) {
            global_draw = 1;
            view_draw = 1;
          }
          
          handled = 1;
        }
      }
      
      int old_view = bd_view;
      
      if (!handled && bd_view_event(bd_views + bd_view, event)) {
        view_draw = 1;
      }
      
      if (bd_view != old_view) {
        global_draw = 1;
        view_draw = 1;
      }
    }
    
    for (int i = 0; !global_draw && i < bd_view_count; i++) {
      if (bd_views[i].title_dirty) global_draw = 1;
    }
    
    if (global_draw) {
      bd_global_draw();
      global_draw = 0;
      
      io_cursor(bd_views[bd_view].cursor.x, bd_views[bd_view].cursor.y);
    }
    
    if (view_draw) {
      bd_view_draw(bd_views + bd_view);
      view_draw = 0;
      
      io_cursor(bd_views[bd_view].cursor.x, bd_views[bd_view].cursor.y);
    }
  }
  
  io_exit();
  return 0;
}
