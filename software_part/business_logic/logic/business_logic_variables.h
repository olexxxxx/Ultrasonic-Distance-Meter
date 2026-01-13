/**
 * @file    business_logic_variables.h
 * @author  Olexandr Makedonskyi
 * @brief   Доменні типи та константи бізнес-логіки
 * @date    13.01.2026
 * @version 1.0
 * Цей файл містить типи даних та константи, які визначають
 * бізнес-правила системи далекоміра. Всі магічні числа
 * винесені у named константи для покращення читабельності.
 */

#ifndef DOMAIN_TYPES_H
#define DOMAIN_TYPES_H



//==================== DEFINES =======================


/** @brief Мінімальне значення порогу (см) */
#define THRESHOLD_MIN           0

/** @brief Максимальне значення порогу (см) */
#define THRESHOLD_MAX           400

/** @brief Крок зміни порогу при натисканні кнопок (см) */
#define THRESHOLD_STEP          10

/** @brief Затримка між вимірюваннями в режимі MEASURE (мс) */
#define MEASUREMENT_DELAY_MS    65

/** @brief Затримка оновлення дисплея в режимі SETUP (мс) */
#define SETUP_DELAY_MS          100

/** @brief Коефіцієнт конверсії см -> дюйми */
#define CM_TO_INCH_FACTOR       2.54f

/** @brief Розмір буфера для форматування чисел */
#define DISPLAY_BUFFER_SIZE     5

/** @brief Значення для відображення помилки вимірювання */
#define DISPLAY_ERROR_VALUE     0

//==================== SYSTEM STATES ===================

/**
 * @brief Стани системної машини станів
 */
typedef enum {
    STATE_MEASURE,      /**< Режим вимірювання відстані */
    STATE_SETUP         /**< Режим налаштування порогу */
} SystemState;

//==================== MEASUREMENT UNITS ===============

/**
 * @brief Одиниці виміру відстані
 */
typedef enum {
    UNIT_INCH = 0,      /**< Дюйми */
    UNIT_CM = 1         /**< Сантиметри */
} MeasurementUnit;

#endif /* DOMAIN_TYPES_H */