#include <pebble.h>
#include "notecardTemplate.h"

//Pointer to window
Window *notecardWindow;
//Card title pointer
static TextLayer *cardTitle;
//Card body pointer
static TextLayer *cardBody;
//Char buffer for title input
static char titleInputBuffer[45];
//Dictation sessions
DictationSession *dictation_session;

void window_load(Window *notecardWindow){
  
  GRect bounds = layer_get_bounds(window_get_root_layer(notecardWindow));
  
  //Make title layer
  cardTitle = text_layer_create(GRect(0, 0, bounds.size.w, 30));
  text_layer_set_background_color(cardTitle, GColorBlack);

  
  //Style
  
  text_layer_set_text_color(cardTitle, GColorWhite);
  //Add to window
  layer_add_child(window_get_root_layer(notecardWindow), text_layer_get_layer(cardTitle));

}
void window_unload(Window *notecardWindow){
  text_layer_destroy(cardTitle);
}

//Gets a human readable dictation status of each of the dictation statuses
char *get_readable_dictation_status(DictationSessionStatus status){
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
void dictation_session_callback(DictationSession *session, DictationSessionStatus status, char *transcription, void *context){
  if(status == DictationSessionStatusSuccess){
    //Insert input string into char buffer as an array of chars
    snprintf(titleInputBuffer, sizeof(titleInputBuffer),"%s", transcription);
    text_layer_set_text(cardTitle, titleInputBuffer);
  }
  //If it doesn't work
  else{
    static char failed_buff[128];
    snprintf(failed_buff, sizeof(failed_buff), "Transcription failed cuz \n%s",get_readable_dictation_status(status)) ;
  }
}

void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    dictation_session = dictation_session_create(sizeof(titleInputBuffer), dictation_session_callback, NULL);
    dictation_session_start(dictation_session);
}
void click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
    window_single_click_subscribe(BUTTON_ID_SELECT, up_click_handler);
}
int main(){
  notecardWindow = window_create();
  window_set_click_config_provider(notecardWindow, click_config_provider);
  window_set_window_handlers(notecardWindow, (WindowHandlers){
    .load = window_load,
    .unload = window_unload
  });
  
  window_stack_push(notecardWindow, true);
  
  app_event_loop();
}