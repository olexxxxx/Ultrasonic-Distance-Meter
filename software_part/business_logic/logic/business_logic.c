#include "business_logic.h"
#include "system_init.h"
#include "logger.h"

//==================== PRIVATE STATE ===================

/**
 * @brief Приватний стан системи (інкапсульований)
 */
static struct {
    SystemState state;          /**< Поточний стан машини станів */
    MeasurementUnit unit;       /**< Поточна одиниця виміру */
    uint16_t threshold_cm;      /**< Порогове значення в см */
} app_state;

//============= STATIC INTERNAL FUNCTIONS PROTOTYPES ==============

static void handle_measure_state(ButtonID button);
static void handle_setup_state(ButtonID button);


static void perform_distance_measurement(void);
static void update_threshold_display(void);
static void adjust_threshold(int16_t delta);
static uint16_t convert_threshold_to_current_unit(void);
static uint16_t convert_distance_to_current_unit(uint16_t distance_cm);

static void business_logic_init(void);
static void business_logic_run(void);


//============== PUBLIC FUNCTION IMPLEMENTATIONS ===============

void business_logic_loop(void){
    initialize_hardware();
    business_logic_init();
    
    while (1) {
        business_logic_run();
    }

}

//============= STATIC INTERNAL FUNCTION IMPLEMENTATIONS =======

/**
 * @brief Ініціалізація бізнес-логіки
 * 
 * Встановлює початковий стан системи:
 * - STATE_MEASURE
 * - UNIT_CM
 * - threshold = 0 (індикація вимкнена)
 * 
 * @note Викликається ОДИН РАЗ після hardware_init()
 * @note НЕ містить нескінченного циклу
 */
static void business_logic_init(void) {
    app_state.state = STATE_MEASURE;
    app_state.unit = UNIT_CM;
    app_state.threshold_cm = THRESHOLD_MIN;
}

/**
 * @brief Виконання одної ітерації машини станів
 * 
 * Функція виконує:
 * 1. Читання кнопки
 * 2. Обробку поточного стану
 * 3. Оновлення виходів (дисплей, LED)
 * 
 * @note Викликається циклічно з main loop
 * @note Час виконання: ~70 мс (STATE_MEASURE) або ~100 мс (STATE_SETUP)
 */
static void business_logic_run(void) {
    ButtonID button = button_read();
    
    switch (app_state.state) {
        case STATE_MEASURE:
            handle_measure_state(button);
            break;
            
        case STATE_SETUP:
            handle_setup_state(button);
            break;
            
        default:
            /* Невідомий стан - повертаємось до MEASURE */
            app_state.state = STATE_MEASURE;
            break;
    }
}


/**
 * @brief Обробка стану MEASURE (вимірювання)
 */
static void handle_measure_state(ButtonID button) {
    switch (button) {
        case BTN_MODE:
            /* Перемикання одиниць виміру */
            app_state.unit = (app_state.unit == UNIT_CM) ? UNIT_INCH : UNIT_CM;
            break;
            
        case BTN_UP:
        case BTN_DOWN:
            /* Перехід до режиму налаштування */
            app_state.state = STATE_SETUP;
            break;
            
        case BTN_NONE:
        default:
            /* Виконання вимірювання */
            perform_distance_measurement();
            _delay_ms(MEASUREMENT_DELAY_MS);
            break;
    }
}

/**
 * @brief Обробка стану SETUP (налаштування порогу)
 */
static void handle_setup_state(ButtonID button) {
    update_threshold_display();
    
    switch (button) {
        case BTN_MODE:
            /* Повернення до режиму вимірювання */
            app_state.state = STATE_MEASURE;
            break;
            
        case BTN_UP:
            /* Збільшення порогу */
            adjust_threshold(THRESHOLD_STEP);
            break;
            
        case BTN_DOWN:
            /* Зменшення порогу */
            adjust_threshold(-THRESHOLD_STEP);
            break;
            
        case BTN_NONE:
        default:
            /* Просто відображаємо поточне значення */
            break;
    }
    
    _delay_ms(SETUP_DELAY_MS);
}

/**
 * @brief Виконання вимірювання відстані
 */
static void perform_distance_measurement(void) {
    uint16_t raw_time;
    uint16_t distance_cm;
    uint16_t distance_display;
    uint16_t threshold_display;
    /*Отримання вимірів в необробленому вигляді*/
    raw_time = distance_sensor_measure_raw();
    
    /* Перевірка валідності */
    if (raw_time == 0) {
        /* Об'єкт не виявлено */
        display_show_number(DISPLAY_ERROR_VALUE);
        led_indication_update(0, 0);  /* Вимкнути LED */
        return;
    }
    
    /* Конвертація в сантиметри (базова одиниця) */
    distance_cm = distance_sensor_convert_to_cm(raw_time);
    
    /* Конвертація для відображення */
    distance_display = convert_distance_to_current_unit(distance_cm);
    threshold_display = convert_threshold_to_current_unit();
    
    /* Оновлення виходів */
    display_show_number(distance_display);
    led_indication_update(distance_display, threshold_display);
}

/**
 * @brief Відображення порогу на дисплеї
 */
static void update_threshold_display(void) {
    uint16_t threshold_display = convert_threshold_to_current_unit();
    display_show_threshold(threshold_display);
}

/**
 * @brief Зміна порогового значення
 * 
 * @param delta Зміна (може бути від'ємною)
 */
static void adjust_threshold(int16_t delta) {
    int16_t new_threshold = (int16_t)app_state.threshold_cm + delta;
    
    /* Обмеження діапазону */
    if (new_threshold < THRESHOLD_MIN) {
        new_threshold = THRESHOLD_MIN;
    } else if (new_threshold > THRESHOLD_MAX) {
        new_threshold = THRESHOLD_MAX;
    }
    
    app_state.threshold_cm = (uint16_t)new_threshold;
}

/**
 * @brief Конвертація порогу в поточну одиницю виміру
 * 
 * @return Порогове значення в поточних одиницях
 */
static uint16_t convert_threshold_to_current_unit(void) {
    if (app_state.unit == UNIT_CM) {
        return app_state.threshold_cm;
    }
    
    /* Конвертація в дюйми */
    return (uint16_t)((float)app_state.threshold_cm / CM_TO_INCH_FACTOR);
}

/**
 * @brief Конвертація відстані в поточну одиницю виміру
 * 
 * @param distance_cm Відстань в сантиметрах
 * @return Відстань в поточних одиницях
 */
static uint16_t convert_distance_to_current_unit(uint16_t distance_cm) {
    uint16_t raw_time;
    if (app_state.unit == UNIT_CM) {
        return distance_cm;
    }
    
    /* Конвертація в дюйми через драйвер */
    raw_time = distance_cm * 58;  /* Reverse conversion */
    return distance_sensor_convert_to_inch(raw_time);
}