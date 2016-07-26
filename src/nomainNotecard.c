#include <pebble.h>
#include "nomainNotecard.h"

//Pointer to window
Window *notecardWindow;
//Card title pointer
static TextLayer *cardTitle;
//Card body pointer
static TextLayer *cardBody;
//Char buffer for title input
static char titleInputBuffer[100];
//Make title key for persistance
uint32_t titleKey = 0;
//Char buffer for body input
static char bodyInputBuffer[1200];
//Make body key for persistance
uint32_t bodyKey1 = 1;


bool isUp = false;
bool isDown = false;

//Dictation sessions
DictationSession *dictation_session;

//Gets a human readable dictation status of each of the dictation statuses
char *get_readable_dictation_status2(DictationSessionStatus status){
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

//Called when the dication API has something
void dictation_session_callback2(DictationSession *session, DictationSessionStatus status, char *transcription, void *context){
  if(status == DictationSessionStatusSuccess){
    //Insert input string into char buffer as an array of chars
    
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%s %d %d", transcription, isUp, isDown);
    
    if(isUp == true ){
      snprintf(titleInputBuffer, sizeof(titleInputBuffer),"%s", transcription);
      persist_write_string( titleKey, titleInputBuffer);
    }
    if(isDown == true ){
      snprintf(bodyInputBuffer, sizeof(bodyInputBuffer),"%s", transcription);
      persist_write_string(bodyKey1, bodyInputBuffer);
    }
    text_layer_set_text(cardTitle, titleInputBuffer);
    text_layer_set_text(cardBody, bodyInputBuffer);
    
  }
  //If it doesn't work
  else{
    static char failed_buff[128];
    snprintf(failed_buff, sizeof(failed_buff), "Transcription failed cuz \n%s",get_readable_dictation_status2(status)) ;
  }
}

void up_click_handler1(ClickRecognizerRef recognizer, void *context) {
    dictation_session = dictation_session_create(sizeof(titleInputBuffer), dictation_session_callback2, NULL);
    dictation_session_start(dictation_session);
    isUp = true;
    isDown = false;
}
void down_click_handler1(ClickRecognizerRef recognizer, void *context) {
    dictation_session = dictation_session_create(sizeof(bodyInputBuffer), dictation_session_callback2, NULL);
    dictation_session_start(dictation_session);
    isDown = true;
    isUp = false;
  
}

void select_click_handler1(ClickRecognizerRef recognizer, void *context) {
  
}

void click_config_provider1(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler1);
    window_single_click_subscribe(BUTTON_ID_UP, up_click_handler1);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler1);
}


void window_load_and_push(int index){
  
  notecardWindow = window_create();
  titleKey = index;
  
  //Check if anything is previously stored in keys
  if (persist_exists(titleKey)) {
  // Read persisted value
    persist_read_string(titleKey, titleInputBuffer, sizeof(titleInputBuffer));

  }
  //Check if anything is previously stored in keys
  if (persist_exists(bodyKey1)) {
  // Read persisted value
    persist_read_string(bodyKey1, bodyInputBuffer, sizeof(bodyInputBuffer));

  }
  
  //Make GRect bounds based on bounds of window
  GRect bounds = layer_get_bounds(window_get_root_layer(notecardWindow));
  
  //Make title layer
  cardTitle = text_layer_create(GRect(0, 0, bounds.size.w, 30));
  text_layer_set_background_color(cardTitle, GColorWhite);

  
  //Title Style
  text_layer_set_text_color(cardTitle, GColorBlack);
  text_layer_set_font(cardTitle, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  //Fonts
 
  //Add to window
  text_layer_set_text(cardTitle, titleInputBuffer);
  layer_add_child(window_get_root_layer(notecardWindow), text_layer_get_layer(cardTitle));
     
  //Make body layer
  cardBody = text_layer_create(GRect(0, 30, bounds.size.w, 138));
  text_layer_set_background_color(cardBody, GColorWhite);
  text_layer_set_font(cardBody, fonts_get_system_font(FONT_KEY_GOTHIC_18));
   //Body Style
  text_layer_set_text_color(cardBody, GColorBlack);
 
  //Add to window
  text_layer_set_text(cardBody, bodyInputBuffer);
  layer_add_child(window_get_root_layer(notecardWindow), text_layer_get_layer(cardBody));
  
  
  window_set_click_config_provider(notecardWindow, click_config_provider1);
  
  window_stack_push(notecardWindow, true);
   
  

}
void window_unload_notecard(Window *notecardWindow){
  text_layer_destroy(cardTitle);
  text_layer_destroy(cardBody);
}








Window *mynote_window_get_window(){
  return notecardWindow;
}