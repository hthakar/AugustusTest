#include "custom_messages.h"

#include "core/string.h"
#include "editor/editor.h"
#include "graphics/button.h"
#include "graphics/generic_button.h"
#include "graphics/graphics.h"
#include "graphics/image.h"
#include "graphics/lang_text.h"
#include "graphics/panel.h"
#include "graphics/screen.h"
#include "graphics/scrollbar.h"
#include "graphics/text.h"
#include "graphics/window.h"
#include "input/input.h"
#include "scenario/property.h"
#include "scenario/custom_media.h"
#include "scenario/custom_messages_import_xml.h"
#include "scenario/custom_messages.h"
#include "scenario/message_media_text_blob.h"
#include "window/city.h"
#include "window/editor/attributes.h"
#include "window/editor/map.h"
#include "window/message_dialog.h"
#include "window/file_dialog.h"
#include "window/numeric_input.h"

#define MESSAGES_Y_OFFSET 146
#define MESSAGES_ROW_HEIGHT 32
#define MAX_VISIBLE_ROWS 8
#define BUTTON_WIDTH 320


static void on_scroll(void);
static void button_click(int type, int param2);
static void button_event(int button_index, int param2);
static void populate_list(int offset);

static scrollbar_type scrollbar = {
    368, MESSAGES_Y_OFFSET, MESSAGES_ROW_HEIGHT * MAX_VISIBLE_ROWS, BUTTON_WIDTH - 16, MAX_VISIBLE_ROWS, on_scroll, 0, 4
};

static generic_button buttons[] = {
    {48, MESSAGES_Y_OFFSET + (0 * MESSAGES_ROW_HEIGHT), BUTTON_WIDTH, MESSAGES_ROW_HEIGHT - 2, button_event, button_none, 1, 0},
    {48, MESSAGES_Y_OFFSET + (1 * MESSAGES_ROW_HEIGHT), BUTTON_WIDTH, MESSAGES_ROW_HEIGHT - 2, button_event, button_none, 2, 0},
    {48, MESSAGES_Y_OFFSET + (2 * MESSAGES_ROW_HEIGHT), BUTTON_WIDTH, MESSAGES_ROW_HEIGHT - 2, button_event, button_none, 3, 0},
    {48, MESSAGES_Y_OFFSET + (3 * MESSAGES_ROW_HEIGHT), BUTTON_WIDTH, MESSAGES_ROW_HEIGHT - 2, button_event, button_none, 4, 0},
    {48, MESSAGES_Y_OFFSET + (4 * MESSAGES_ROW_HEIGHT), BUTTON_WIDTH, MESSAGES_ROW_HEIGHT - 2, button_event, button_none, 5, 0},
    {48, MESSAGES_Y_OFFSET + (5 * MESSAGES_ROW_HEIGHT), BUTTON_WIDTH, MESSAGES_ROW_HEIGHT - 2, button_event, button_none, 6, 0},
    {48, MESSAGES_Y_OFFSET + (6 * MESSAGES_ROW_HEIGHT), BUTTON_WIDTH, MESSAGES_ROW_HEIGHT - 2, button_event, button_none, 7, 0},
    {48, MESSAGES_Y_OFFSET + (7 * MESSAGES_ROW_HEIGHT), BUTTON_WIDTH, MESSAGES_ROW_HEIGHT - 2, button_event, button_none, 8, 0},
    {48, MESSAGES_Y_OFFSET + (9 * MESSAGES_ROW_HEIGHT), BUTTON_WIDTH, MESSAGES_ROW_HEIGHT - 2, button_click, button_none, 9, 0},
    {48, MESSAGES_Y_OFFSET + (10 * MESSAGES_ROW_HEIGHT), BUTTON_WIDTH, MESSAGES_ROW_HEIGHT - 2, button_click, button_none, 10, 0},
    {48, MESSAGES_Y_OFFSET + (11 * MESSAGES_ROW_HEIGHT), BUTTON_WIDTH, MESSAGES_ROW_HEIGHT - 2, button_click, button_none, 11, 0}
};

#define MAX_BUTTONS (sizeof(buttons) / sizeof(generic_button))

static struct {
    int focus_button_id;

    int total_messages;
    custom_message_t *list[MAX_VISIBLE_ROWS];
} data;

static void init(void)
{
    data.total_messages = custom_messages_count();
    populate_list(0);
    scrollbar_init(&scrollbar, 0, data.total_messages);
}

static void populate_list(int offset)
{
    // Ensure we dont offset past the end or beginning of the list.
    if (data.total_messages - offset < MAX_VISIBLE_ROWS) {
        offset = data.total_messages - MAX_VISIBLE_ROWS;
    }
    if (offset < 0) {
        offset = 0;
    }
    for (int i = 0; i < MAX_VISIBLE_ROWS; i++) {
        int target_id = i + offset + 1; // Skip entry zero custom message
        if (target_id <= data.total_messages) {
            data.list[i] = custom_messages_get(target_id);
        } else {
            data.list[i] = 0;
        }
    }
}

static void draw_background(void)
{
    window_editor_map_draw_all();
}

static void draw_foreground(void)
{
    graphics_in_dialog();

    outer_panel_draw(16, 16, 26, 38);

    text_draw_centered(translation_for(TR_EDITOR_CUSTOM_MESSAGES_TITLE), 48, 58, BUTTON_WIDTH, FONT_LARGE_BLACK, 0);
    text_draw_label_and_number(translation_for(TR_EDITOR_CUSTOM_MESSAGES_COUNT), data.total_messages, "", 48, 106, FONT_NORMAL_PLAIN, COLOR_BLACK);

    int y_offset = MESSAGES_Y_OFFSET;
    for (int i = 0; i < MAX_VISIBLE_ROWS; i++) {
        if (data.list[i]) {
            large_label_draw(buttons[i].x, buttons[i].y, buttons[i].width / BLOCK_SIZE,
                data.focus_button_id == i + 1 ? 1 : 0);

            text_draw_label_and_number(0, data.list[i]->id, "", 48, y_offset + 8, FONT_NORMAL_GREEN, COLOR_MASK_NONE);
            if (data.list[i] && data.list[i]->linked_uid && data.list[i]->linked_uid->text) {
                text_draw_centered(data.list[i]->linked_uid->text, 100, y_offset + 8, 250, FONT_NORMAL_GREEN, COLOR_MASK_NONE);
            }
        }

        y_offset += MESSAGES_ROW_HEIGHT;
    }

    for (size_t i = 8; i < MAX_BUTTONS; i++) {
        large_label_draw(buttons[i].x, buttons[i].y, buttons[i].width / BLOCK_SIZE,
            data.focus_button_id == i + 1 ? 1 : 0);
    }

    y_offset += MESSAGES_ROW_HEIGHT;
    lang_text_draw_centered(CUSTOM_TRANSLATION, TR_EDITOR_SCENARIO_EVENTS_IMPORT, 48, y_offset + 8, BUTTON_WIDTH, FONT_NORMAL_GREEN);

    y_offset += MESSAGES_ROW_HEIGHT;
    lang_text_draw_centered(CUSTOM_TRANSLATION, TR_EDITOR_SCENARIO_EVENTS_EXPORT, 48, y_offset + 8, BUTTON_WIDTH, FONT_NORMAL_GREEN);

    y_offset += MESSAGES_ROW_HEIGHT;
    lang_text_draw_centered(CUSTOM_TRANSLATION, TR_EDITOR_CUSTOM_MESSAGES_CLEAR, 48, y_offset + 8, BUTTON_WIDTH, FONT_NORMAL_GREEN);

    y_offset += MESSAGES_ROW_HEIGHT;
    lang_text_draw_centered(13, 3, 48, y_offset, BUTTON_WIDTH, FONT_NORMAL_BLACK);

    scrollbar_draw(&scrollbar);
    graphics_reset_dialog();
}

static void button_event(int button_index, int param2)
{
    int target_index = button_index - 1;
    if (!data.list[target_index]) {
        return;
    };
    window_message_dialog_show_custom_message(data.list[target_index]->id, 0, 0);
}

static void on_scroll(void)
{
    window_request_refresh();
}

static void close_window(void)
{
    if (editor_is_active()) {
        window_editor_attributes_show();
    } else {
        window_city_show();
    }
}

static void handle_input(const mouse *m, const hotkeys *h)
{
    const mouse *m_dialog = mouse_in_dialog(m);
    if (scrollbar_handle_mouse(&scrollbar, m_dialog, 1) ||
        generic_buttons_handle_mouse(m_dialog, 0, 0, buttons, MAX_BUTTONS, &data.focus_button_id)) {
        return;
    }
    if (input_go_back_requested(m, h)) {
        close_window();
    }
    populate_list(scrollbar.scroll_position);
}

static void button_click(int type, int param2)
{
    if (type == 9) {
        window_file_dialog_show(FILE_TYPE_CUSTOM_MESSAGES, FILE_DIALOG_LOAD);
    } else if (type == 10) {
        window_file_dialog_show(FILE_TYPE_CUSTOM_MESSAGES, FILE_DIALOG_SAVE);
    } else if (type == 11) {
        custom_messages_clear_all();
        init();
    }
}

void window_editor_custom_messages_show(void)
{
    window_type window = {
        WINDOW_EDITOR_CUSTOM_MESSAGES,
        draw_background,
        draw_foreground,
        handle_input
    };
    init();
    window_show(&window);
}
