SET(SOURCES screens/ui_data.c
    screens/ui_control1.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_pattern_png.c
    images/ui_img_weather_1_png.c
    fonts/ui_font_Number.c)

add_library(ui ${SOURCES})
