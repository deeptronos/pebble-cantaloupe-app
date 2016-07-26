#include <pebble.h>
#include "main_window.h"
#include "notecardTemplate.h"
#include "splash_window.h"
#include "notesMenu.h"

Window *mainWindow;
MenuLayer *mainMenuLayer;

uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
	return 2;
}

uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    switch(section_index){
		case 0:
			return 1;
		case 1:
			return 1;
		default:
			return 0;
	}
}

int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    return MENU_CELL_BASIC_HEADER_HEIGHT;
}

void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
    switch(section_index){
		case 0:
			menu_cell_basic_header_draw(ctx, cell_layer, "Canteloupe");
			break;
	}
}

void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
    switch(cell_index->section ){
		case 0:
			switch(cell_index->row){
				case 0:
					menu_cell_basic_draw(ctx, cell_layer, "Flashcards", NULL, NULL);
					break;
			}
			break;
		case 1:
			menu_cell_basic_draw(ctx, cell_layer, "Notes", NULL, NULL);
		break;
	}	
}
void menu_select_callback(MenuLayer *main_Menu_layer, MenuIndex *cell_index, void *data) {
  notes_window_load();
}

void setup_menu_layer(Window *window) {
  light_enable(true);
	Layer *window_layer = window_get_root_layer(window);

    mainMenuLayer = menu_layer_create(GRect(0, 0, 144, 168));
    
	menu_layer_set_callbacks(mainMenuLayer, NULL, (MenuLayerCallbacks){
        .get_num_sections = menu_get_num_sections_callback,
        .get_num_rows = menu_get_num_rows_callback,
        .get_header_height = menu_get_header_height_callback,
        .draw_header = menu_draw_header_callback,
        .draw_row = menu_draw_row_callback,
        .select_click = menu_select_callback,
    });

    menu_layer_set_click_config_onto_window(mainMenuLayer, window);

    layer_add_child(window_layer, menu_layer_get_layer(mainMenuLayer));
}


void main_window_load(Window *window){
	setup_menu_layer(window);
}

void main_window_unload(Window *window){
	menu_layer_destroy(mainMenuLayer);
}

void main_window_create(){
	mainWindow = window_create();
	window_set_window_handlers(mainWindow, (WindowHandlers){
		.load = main_window_load,
		.unload = main_window_unload
	});
}

void main_window_destroy(){
	window_destroy(mainWindow);
}

Window *main_window_get_window(){
	return mainWindow;
}