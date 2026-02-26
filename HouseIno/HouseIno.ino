#include <lvgl.h>
#include <TFT_eSPI.h>

LV_IMG_DECLARE(house_portrait);

#define PIN_LED_RED 4
#define PIN_LED_GREEN 16
#define PIN_LED_BLUE 17

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define TERMINAL_GREEN lv_color_make(50, 255, 50)
#define TERMINAL_RED   lv_color_make(255, 50, 50)
#define TERMINAL_BG    lv_color_make(0, 0, 0)

TFT_eSPI tft = TFT_eSPI();
static lv_color_t buf[SCREEN_WIDTH * 10];
static lv_disp_draw_buf_t draw_buf;

int current_screen_id = 0;
bool glitch_active = false;
uint32_t glitch_end_time = 0;
uint32_t last_glitch_update = 0;

String g_uptime = "0:00:00";
String g_net = "IN: 0 B/s\nOUT: 0 B/s";
int g_cpu = 0;
int g_ram = 0;
bool g_data_fresh = false;

lv_obj_t *screen_face_cont;
lv_obj_t *label_uptime;
lv_obj_t *label_net;
lv_obj_t *label_status;

lv_obj_t *screen_stats_cont;
lv_obj_t *chart_cpu;
lv_chart_series_t *ser_cpu;
lv_obj_t *bar_ram;
lv_obj_t *label_cpu_val;
lv_obj_t *label_ram_val;

lv_obj_t *screen_ctrl_cont;

void set_led(bool r, bool g, bool b) {
    digitalWrite(PIN_LED_RED,   r ? LOW : HIGH);
    digitalWrite(PIN_LED_GREEN, g ? LOW : HIGH);
    digitalWrite(PIN_LED_BLUE,  b ? LOW : HIGH);
}

void my_touch_read(lv_indev_drv_t * drv, lv_indev_data_t * data) {
    uint16_t x, y;
    if (tft.getTouch(&x, &y)) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = x;
        data->point.y = y;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t*)&color_p->full, w * h, true);
    tft.endWrite();
    lv_disp_flush_ready(disp);
}

void switch_screen() {
    current_screen_id++;
    if (current_screen_id > 2) current_screen_id = 0;

    lv_obj_add_flag(screen_face_cont, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(screen_stats_cont, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(screen_ctrl_cont, LV_OBJ_FLAG_HIDDEN);

    if (current_screen_id == 0) lv_obj_clear_flag(screen_face_cont, LV_OBJ_FLAG_HIDDEN);
    else if (current_screen_id == 1) lv_obj_clear_flag(screen_stats_cont, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_clear_flag(screen_ctrl_cont, LV_OBJ_FLAG_HIDDEN);
}

static void screen_click_event_handler(lv_event_t * e) {
    switch_screen();
}

static void btn_action_handler(lv_event_t * e) {
    int id = (int)(intptr_t)lv_event_get_user_data(e);

    set_led(false, false, true);
    delay(100);
    set_led(false, true, false);

    if (id == 1) Serial.println("BTN:LOCK");
    if (id == 2) Serial.println("BTN:MUTE");
    if (id == 3) Serial.println("BTN:KILL");
}

void create_ui() {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, TERMINAL_BG, 0);

    screen_face_cont = lv_obj_create(scr);
    lv_obj_set_size(screen_face_cont, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_set_style_bg_color(screen_face_cont, TERMINAL_BG, 0);
    lv_obj_set_style_border_width(screen_face_cont, 0, 0);
    lv_obj_set_scrollbar_mode(screen_face_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_align(screen_face_cont, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(screen_face_cont, screen_click_event_handler, LV_EVENT_CLICKED, NULL);

    label_status = lv_label_create(screen_face_cont);
    lv_label_set_text(label_status, "LINK: ACTIVE");
    lv_obj_set_style_text_color(label_status, TERMINAL_GREEN, 0);
    lv_obj_align(label_status, LV_ALIGN_TOP_LEFT, 5, 5);

    label_uptime = lv_label_create(screen_face_cont);
    lv_label_set_text(label_uptime, "0:00:00");
    lv_obj_set_style_text_color(label_uptime, TERMINAL_GREEN, 0);
    lv_obj_align(label_uptime, LV_ALIGN_TOP_RIGHT, -5, 5);

    label_net = lv_label_create(screen_face_cont);
    lv_label_set_text(label_net, "IN: 0 B/s\nOUT: 0 B/s");
    lv_obj_set_style_text_color(label_net, TERMINAL_GREEN, 0);
    lv_obj_align(label_net, LV_ALIGN_TOP_LEFT, 5, 25);

    lv_obj_t *img = lv_img_create(screen_face_cont);
    lv_img_set_src(img, &house_portrait);
    lv_obj_align(img, LV_ALIGN_BOTTOM_MID, 0, 0);

    screen_stats_cont = lv_obj_create(scr);
    lv_obj_set_size(screen_stats_cont, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_set_style_bg_color(screen_stats_cont, TERMINAL_BG, 0);
    lv_obj_set_style_border_width(screen_stats_cont, 0, 0);
    lv_obj_set_scrollbar_mode(screen_stats_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_align(screen_stats_cont, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(screen_stats_cont, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(screen_stats_cont, screen_click_event_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *t1 = lv_label_create(screen_stats_cont);
    lv_label_set_text(t1, "CPU HISTORY");
    lv_obj_set_style_text_color(t1, TERMINAL_GREEN, 0);
    lv_obj_align(t1, LV_ALIGN_TOP_LEFT, 10, 10);

    label_cpu_val = lv_label_create(screen_stats_cont);
    lv_label_set_text(label_cpu_val, "0%");
    lv_obj_set_style_text_color(label_cpu_val, TERMINAL_GREEN, 0);
    lv_obj_align(label_cpu_val, LV_ALIGN_TOP_RIGHT, -10, 10);

    chart_cpu = lv_chart_create(screen_stats_cont);
    lv_obj_set_size(chart_cpu, 220, 120);
    lv_obj_align(chart_cpu, LV_ALIGN_TOP_MID, 0, 35);
    lv_chart_set_type(chart_cpu, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart_cpu, 20);
    lv_obj_set_style_bg_color(chart_cpu, lv_color_make(10,10,10), 0);
    lv_obj_set_style_line_color(chart_cpu, lv_color_make(40,40,40), LV_PART_MAIN);
    ser_cpu = lv_chart_add_series(chart_cpu, TERMINAL_GREEN, LV_CHART_AXIS_PRIMARY_Y);

    lv_obj_t *t2 = lv_label_create(screen_stats_cont);
    lv_label_set_text(t2, "RAM USAGE");
    lv_obj_set_style_text_color(t2, TERMINAL_GREEN, 0);
    lv_obj_align(t2, LV_ALIGN_TOP_LEFT, 10, 170);

    label_ram_val = lv_label_create(screen_stats_cont);
    lv_label_set_text(label_ram_val, "0%");
    lv_obj_set_style_text_color(label_ram_val, TERMINAL_GREEN, 0);
    lv_obj_align(label_ram_val, LV_ALIGN_TOP_RIGHT, -10, 170);

    bar_ram = lv_bar_create(screen_stats_cont);
    lv_obj_set_size(bar_ram, 220, 20);
    lv_obj_align(bar_ram, LV_ALIGN_TOP_MID, 0, 195);
    lv_obj_set_style_bg_color(bar_ram, lv_color_make(20,20,20), LV_PART_MAIN);
    lv_obj_set_style_bg_color(bar_ram, TERMINAL_GREEN, LV_PART_INDICATOR);
    lv_bar_set_range(bar_ram, 0, 100);

    screen_ctrl_cont = lv_obj_create(scr);
    lv_obj_set_size(screen_ctrl_cont, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_set_style_bg_color(screen_ctrl_cont, TERMINAL_BG, 0);
    lv_obj_set_style_border_width(screen_ctrl_cont, 0, 0);
    lv_obj_align(screen_ctrl_cont, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(screen_ctrl_cont, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *header = lv_label_create(screen_ctrl_cont);
    lv_label_set_text(header, "OVERRIDE PROTOCOLS");
    lv_obj_set_style_text_color(header, TERMINAL_RED, 0);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_add_flag(header, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(header, screen_click_event_handler, LV_EVENT_CLICKED, NULL);

    static lv_style_t style_btn;
    lv_style_init(&style_btn);
    lv_style_set_bg_color(&style_btn, lv_color_black());
    lv_style_set_border_color(&style_btn, TERMINAL_GREEN);
    lv_style_set_border_width(&style_btn, 2);
    lv_style_set_radius(&style_btn, 5);

    lv_obj_t *btn1 = lv_btn_create(screen_ctrl_cont);
    lv_obj_add_style(btn1, &style_btn, 0);
    lv_obj_set_size(btn1, 200, 50);
    lv_obj_align(btn1, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_add_event_cb(btn1, btn_action_handler, LV_EVENT_CLICKED, (void*)1);
    lv_obj_t *lbl1 = lv_label_create(btn1);
    lv_label_set_text(lbl1, "LOCK TERMINAL");
    lv_obj_center(lbl1);

    lv_obj_t *btn2 = lv_btn_create(screen_ctrl_cont);
    lv_obj_add_style(btn2, &style_btn, 0);
    lv_obj_set_size(btn2, 200, 50);
    lv_obj_align(btn2, LV_ALIGN_TOP_MID, 0, 120);
    lv_obj_add_event_cb(btn2, btn_action_handler, LV_EVENT_CLICKED, (void*)2);
    lv_obj_t *lbl2 = lv_label_create(btn2);
    lv_label_set_text(lbl2, "TOGGLE AUDIO");
    lv_obj_center(lbl2);

    lv_obj_t *btn3 = lv_btn_create(screen_ctrl_cont);
    lv_obj_add_style(btn3, &style_btn, 0);
    lv_obj_set_style_border_color(btn3, TERMINAL_RED, 0);
    lv_obj_set_size(btn3, 200, 50);
    lv_obj_align(btn3, LV_ALIGN_TOP_MID, 0, 190);
    lv_obj_add_event_cb(btn3, btn_action_handler, LV_EVENT_CLICKED, (void*)3);
    lv_obj_t *lbl3 = lv_label_create(btn3);
    lv_label_set_text(lbl3, "KILL HIGH CPU");
    lv_obj_set_style_text_color(lbl3, TERMINAL_RED, 0);
    lv_obj_center(lbl3);
}

void update_data_ui() {
    if (!g_data_fresh) return;

    if (!glitch_active) {
        lv_label_set_text(label_uptime, g_uptime.c_str());
        lv_label_set_text(label_net, g_net.c_str());
        if (g_cpu > 90) set_led(true, false, false);
        else set_led(false, true, false);
    }

    lv_chart_set_next_value(chart_cpu, ser_cpu, g_cpu);
    lv_label_set_text_fmt(label_cpu_val, "%d%%", g_cpu);
    lv_bar_set_value(bar_ram, g_ram, LV_ANIM_ON);
    lv_label_set_text_fmt(label_ram_val, "%d%%", g_ram);

    g_data_fresh = false;
}

void handle_glitch() {
    if (!glitch_active) return;

    if (millis() % 100 > 50) set_led(true, false, false);
    else set_led(false, false, false);

    if (millis() - last_glitch_update > 40) {
        int x = random(-3, 4);
        int y = random(-2, 8);

        lv_obj_t *t = current_screen_id == 0 ? screen_face_cont :
                      current_screen_id == 1 ? screen_stats_cont :
                      screen_ctrl_cont;

        lv_obj_align(t, LV_ALIGN_CENTER, x, y);

        if (current_screen_id == 0 && random(0,10) > 7) {
            lv_label_set_text(label_status, "ERRO#_!");
            lv_obj_set_style_text_color(label_status, lv_color_make(20,50,20), 0);
        }

        last_glitch_update = millis();
    }

    if (millis() > glitch_end_time) {
        glitch_active = false;

        lv_obj_align(screen_face_cont, LV_ALIGN_CENTER, 0, 0);
        lv_obj_align(screen_stats_cont, LV_ALIGN_CENTER, 0, 0);
        lv_obj_align(screen_ctrl_cont, LV_ALIGN_CENTER, 0, 0);

        lv_label_set_text(label_status, "LINK: ACTIVE");
        lv_obj_set_style_text_color(label_status, TERMINAL_GREEN, 0);
        set_led(false, true, false);
    }
}

void handleSerial() {
    if (!Serial.available()) return;

    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.length() < 4) return;

    bool updated = false;

    if (cmd.startsWith("UPTIME:")) {
        g_uptime = cmd.substring(7);
        updated = true;
        Serial.println("ACK");
    } else if (cmd.startsWith("NET:")) {
        String d = cmd.substring(4);
        int s = d.indexOf('|');
        if (s != -1) {
            g_net = "IN: " + d.substring(0, s) + "\nOUT: " + d.substring(s + 1);
            updated = true;
        }
    } else if (cmd.startsWith("STATS:")) {
        String d = cmd.substring(6);
        int s = d.indexOf('|');
        if (s != -1) {
            g_cpu = d.substring(0, s).toInt();
            g_ram = d.substring(s + 1).toInt();
            updated = true;
        }
    } else if (cmd.startsWith("GLITCH:1") && !glitch_active) {
        glitch_active = true;
        glitch_end_time = millis() + 500;
    }

    if (updated) g_data_fresh = true;
}

void setup() {
    Serial.begin(115200);

    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_BLUE, OUTPUT);
    set_led(false, false, true);

    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);

    uint16_t calData[5] = { 275, 3620, 264, 3532, 1 };
    tft.setTouch(calData);

    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, SCREEN_WIDTH * 10);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touch_read;
    lv_indev_drv_register(&indev_drv);

    create_ui();
    Serial.println("READY");
    set_led(false, true, false);
}

void loop() {
    handleSerial();

    static uint32_t last_ui = 0;
    if (millis() - last_ui > 100) {
        update_data_ui();
        last_ui = millis();
    }

    handle_glitch();

    static uint32_t last_tick = 0;
    uint32_t now = millis();
    lv_tick_inc(now - last_tick);
    last_tick = now;

    lv_timer_handler();
    delay(5);
}
