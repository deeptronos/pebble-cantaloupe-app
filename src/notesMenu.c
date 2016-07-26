#include "pebble.h"
#include "notesMenu.h"
#include "nomainNotecard.h"

#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ICONS 3
#define NUM_FIRST_MENU_ITEMS 4
#define NUM_SECOND_MENU_ITEMS 4

static Window *s_main_window;
static MenuLayer *s_menu_layer;
static GBitmap *s_menu_icons[NUM_MENU_ICONS];
static GBitmap *s_background_bitmap;

DictationSession *dictation_session;

static int s_current_icon = 0;
int mostRecentRow;

char *get_readable_dictation_status1(DictationSessionStatus status){
    switch(status){
        case DictationSessionStatusSuccess:
            return "Success";
        case DictationSessionStatusFailureTranscriptionRejected:
            return "User rejected success";
        case DictationSessionStatusFailureTranscriptionRejectedWithError:
            return "User rejected error";
        case DictationSessionStatusFailureSystemAborted:
            return "Too many errors, UI gave up";
        case DictationSessionStatusFailureNoSpeechDetected:
            return "No speech, UI exited";
        case DictationSessionStatusFailureConnectivityError:
            return "No BT/internet connection";
        case DictationSessionStatusFailureDisabled:
            return "Voice dictation disabled";
        case DictationSessionStatusFailureInternalError:
            return "Internal error";
        case DictationSessionStatusFailureRecognizerError:
            return "Failed to transcribe speech";
    }
    return "Unknown";
};

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return NUM_FIRST_MENU_ITEMS;
    case 1:
      return NUM_SECOND_MENU_ITEMS;
    default:
      return 0;
  }
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  switch (section_index) {
    case 0:
      // Draw title text in the section header
      menu_cell_basic_header_draw(ctx, cell_layer, "Note");
      break;
    case 1:
      menu_cell_basic_header_draw(ctx, cell_layer, "One more");
      break;
  }
}

char *strs[2] = {"Title", "Body"};


static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Determine which section we're going to draw in
  switch (cell_index->section) {
    case 0:
      // Use the row to specify which item we'll draw
      switch (cell_index->row) {
        
        default:;
        
         static char buf2[NUM_FIRST_MENU_ITEMS][100];    /* <-- implicit NUL-terminator at the end here */
         persist_read_string(cell_index->row, buf2[cell_index->row], sizeof(buf2[cell_index->row])); 
         APP_LOG(APP_LOG_LEVEL_DEBUG, "String %s at row %d", buf2[cell_index->row], cell_index->row);
         menu_cell_title_draw(ctx, cell_layer, buf2[cell_index->row]);
        
        break;
      }
    
    break;
    
  }
}

void dictation_session_callback1(DictationSession *session, DictationSessionStatus status, char *transcription, void *context){
  if(status == DictationSessionStatusSuccess){
    //Insert input string into char buffer as an array of chars
    
    persist_write_string(mostRecentRow, transcription);
    menu_layer_reload_data(s_menu_layer);
   
  }
  //If it doesn't work
  else{
    static char failed_buff[128];
    snprintf(failed_buff, sizeof(failed_buff), "Transcription failed cuz \n%s",get_readable_dictation_status1(status)) ;
  }
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
  switch (cell_index->row) {
    // This is the menu item with the cycling icon
    
    default:;
      window_load_and_push(cell_index->row);
    
//       mostRecentRow = cell_index->row;
//       dictation_session = dictation_session_create(sizeof(titleInputBuffer), dictation_session_callback1, NULL);
//       dictation_session_start(dictation_session);
    
//       APP_LOG(APP_LOG_LEVEL_DEBUG, "Loop index now %d", cell_index->row);
      
    break;
    
  }

}

void notes_window_load() {
  // Here we load the bitmap assets
//   s_menu_icons[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_BIG_WATCH);
//   s_menu_icons[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_SECTOR_WATCH);
//   s_menu_icons[2] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_BINARY_WATCH);

//   // And also load the background
//   s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND_BRAINS);

  // Now we prepare to initialize the menu layer
  s_main_window = window_create();
  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_frame(window_layer);


  // Create the menu layer
  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(s_menu_layer, s_main_window);

  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
  
  window_stack_push(s_main_window, true);
}

void notes_window_unload() {
  // Destroy the menu layer
  menu_layer_destroy(s_menu_layer);

  // Cleanup the menu icons
  for (int i = 0; i < NUM_MENU_ICONS; i++) {
    gbitmap_destroy(s_menu_icons[i]);
  }

  gbitmap_destroy(s_background_bitmap);
}

static void init() {
  
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = notes_window_load,
    .unload = notes_window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
}

Window *note_window_get_window(){
	return s_main_window;
}