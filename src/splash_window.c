#include <pebble.h>
#include "splash_window.h"


Window *splashWindow;
GBitmap *splashImage;
BitmapLayer *splashImageLayer;
TextLayer *splashImageTitle; 

void splash_window_load(Window *window){
  Layer *window_layer = window_get_root_layer(window);
  
  splashImage = gbitmap_create_with_resource(RESOURCE_ID_CANTALOUPE_SPLASH);
  splashImageLayer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(splashImageLayer, splashImage);
  bitmap_layer_set_compositing_mode(splashImageLayer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(splashImageLayer));
  
  splashImageTitle = text_layer_create(GRect(0, 120, 144, 34));
  text_layer_set_text(splashImageTitle, "CANTALOUPE");
  text_layer_set_text_color(splashImageTitle, GColorRajah);
  text_layer_set_background_color(splashImageTitle, GColorCadetBlue);
  text_layer_set_text_alignment(splashImageTitle, GTextAlignmentCenter);
  text_layer_set_font(splashImageTitle, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  layer_add_child(window_layer, text_layer_get_layer(splashImageTitle));
}  

void splash_window_unload(Window *window){
  
}

void splash_window_create(){
  splashWindow = window_create();
  window_set_window_handlers(splashWindow, (WindowHandlers){
    .load = splash_window_load,
    .unload = splash_window_unload
  });
}

void splash_window_destroy(){
  window_destroy(splashWindow);
}

Window *splash_window_get_window(){
  return splashWindow;
}