/**
 * @file    tm1637.c
 * @author  Olexandr Makedonskyi
 * @brief   Реалізація HAL драйвера TM1637
 * @date    13.01.2026
 * @version 1.0
 * 
 * Модуль реалізує низькорівневі функції для керування
 * контролером дисплея TM1637 через 2-wire протокол.
 * 
 * Особливості реалізації:
 * - Програмна реалізація протоколу (bit-banging)
 * - Open-Drain режим для DIO (підтримка ACK)
 * - Мікросекундні затримки для синхронізації
 * - Перевірка ACK від контролера
 * 
 * Тайминг операцій:
 * - START: 6 мкс
 * - STOP: 6 мкс
 * - Байт (8 біт + ACK): ~100 мкс
 * - Повна транзакція (4 байти): ~500 мкс
 */

//==================== INCLUDES ========================
#include "tm1637.h"

//============= STATIC INTERNAL FUNCTIONS PROTOTYPES ==============
static uint8_t tm1637_write_byte(uint8_t data);
static void    tm1637_stop(void);
static void    tm1637_start(void);
static uint8_t tm1637_check_brightness_boundary(uint8_t brightness);
//============ PUBLIC FUNCTION IMPLEMENTATIONS =========


void tm1637_gpio_init(void) {
    // 1. Встановлення початкового стану HIGH для обох ліній
    TM1637_PORT->ODR |= (TM1637_CLK_MASK | TM1637_DIO_MASK);

    // 2. Налаштування CLK (PC3) як вихід Push-Pull High Speed
    // DDR=1: вихід
    TM1637_PORT->DDR |= TM1637_CLK_MASK;
    // CR1=1: Push-Pull
    TM1637_PORT->CR1 |= TM1637_CLK_MASK;
    // CR2=1: High Speed (10 МГц)
    TM1637_PORT->CR2 |= TM1637_CLK_MASK;

    // 3. Налаштування DIO (PC4) як вихід Open-Drain High Speed
    // DDR=1: вихід
    TM1637_PORT->DDR |= TM1637_DIO_MASK;
    // CR1=0: Open-Drain (для підтримки ACK від TM1637)
    TM1637_PORT->CR1 &= ~TM1637_DIO_MASK;
    // CR2=1: High Speed (10 МГц)
    TM1637_PORT->CR2 |= TM1637_DIO_MASK;
}


void tm1637_set_brightness(uint8_t brightness) {
    // Обмеження яскравості до діапазону 0-7
    brightness =  tm1637_check_brightness_boundary(brightness);
    
    // Відправка команди керування дисплеєм
    tm1637_start();
    tm1637_write_byte(TM1637_CMD_DISPLAY_ON | (brightness & TM1637_BRIGHTNESS_MASK)); 
    tm1637_stop();
}


void tm1637_write_segments(const uint8_t *segments) {
    int i;
    
    // 1. Відправка команди автоматичного інкременту адреси
    tm1637_start();
    tm1637_write_byte(TM1637_CMD_DATA_AUTO);
    tm1637_stop();

    // 2. Відправка даних для всіх 4 розрядів
    tm1637_start();
    
    // Встановлення початкової адреси (розряд 0)
    tm1637_write_byte(TM1637_CMD_ADDR_START);
    
    // Відправка 4 байт сегментів
    for (i = 0; i < TM1637_DIGITS_COUNT; ++i) {
        tm1637_write_byte(segments[i]);
    }
    
    tm1637_stop();
}

//============= STATIC INTERNAL FUNCTION IMPLEMENTATIONS =======

/**
 * @brief Генерація сигналу START для протоколу TM1637
 * 
 * Формує умову START згідно з протоколом TM1637:
 * 1. DIO та CLK в HIGH
 * 2. DIO падає до LOW (при HIGH CLK)
 * 3. CLK падає до LOW
 * 
 * Тайминг:
 * - Затримка між станами: 2 мкс
 * 
 * Після START можна починати передачу байтів.
 * 
 * @note Перед START обидва сигнали мають бути в HIGH
 * @note Після START контролер очікує команду або дані
 * 
 * @see tm1637_stop()
 * @see tm1637_write_byte()
 */
static void tm1637_start(void) {
    // 1. Встановлення DIO як вихід Open-Drain
    TM1637_PORT->DDR |= TM1637_DIO_MASK;
    TM1637_PORT->CR1 &= ~TM1637_DIO_MASK;

    // 2. Обидві лінії в HIGH (idle стан)
    TM1637_PORT->ODR |= TM1637_DIO_MASK;
    TM1637_PORT->ODR |= TM1637_CLK_MASK;
    _delay_us(2);

    // 3. START умова: DIO падає при HIGH CLK
    TM1637_PORT->ODR &= ~TM1637_DIO_MASK;
    _delay_us(2);

    // 4. CLK падає після DIO
    TM1637_PORT->ODR &= ~TM1637_CLK_MASK;
    _delay_us(2);
}
/**
 * @brief Генерація сигналу STOP для протоколу TM1637
 * 
 * Формує умову STOP згідно з протоколом TM1637:
 * 1. CLK в LOW, DIO в LOW
 * 2. CLK зростає до HIGH
 * 3. DIO зростає до HIGH (при HIGH CLK)
 * 
 * Тайминг:
 * - Затримка між станами: 2 мкс
 * 
 * Після STOP транзакція завершена, шина вільна.
 * 
 * @note Після STOP можна починати нову транзакцію
 * @note STOP має викликатись після кожної послідовності команд
 * 
 * @see tm1637_start()
 * @see tm1637_write_byte()
 */
static void tm1637_stop(void) {
    // 1. Встановлення DIO як вихід Open-Drain
    TM1637_PORT->DDR |= TM1637_DIO_MASK;
    TM1637_PORT->CR1 &= ~TM1637_DIO_MASK;

    // 2. Обидві лінії в LOW (перед STOP)
    TM1637_PORT->ODR &= ~TM1637_CLK_MASK;
    _delay_us(2);
    TM1637_PORT->ODR &= ~TM1637_DIO_MASK;
    _delay_us(2);

    // 3. CLK зростає першим
    TM1637_PORT->ODR |= TM1637_CLK_MASK;
    _delay_us(2);

    // 4. STOP умова: DIO зростає при HIGH CLK
    TM1637_PORT->ODR |= TM1637_DIO_MASK;
    _delay_us(2);
}


/**
 * @brief Запис байта даних до TM1637
 * 
 * Передає один байт до контролера згідно з протоколом:
 * 
 * Послідовність передачі:
 * 1. Для кожного біта (0-7, LSB first):
 *    a. CLK = LOW
 *    b. Встановити DIO (біт даних)
 *    c. Затримка (setup time)
 *    d. CLK = HIGH
 *    e. Затримка (hold time)
 * 2. Очікування ACK від TM1637:
 *    a. CLK = LOW
 *    b. DIO перемкнути на вхід з pull-up
 *    c. CLK = HIGH
 *    d. Зчитати ACK з DIO (0 = ACK, 1 = NACK)
 *    e. CLK = LOW
 * 3. Відновлення DIO як вихід Open-Drain
 * 
 * Тайминг:
 * - CLK LOW: 1 мкс
 * - Setup time: 3 мкс
 * - CLK HIGH: 5 мкс
 * - ACK read: 1-3 мкс
 * 
 * @param[in] data Байт для передачі (0x00-0xFF)
 * 
 * @return Статус ACK від TM1637
 * @retval 0 ACK отримано (успішна передача)
 * @retval 1 NACK або помилка (TM1637 не відповідає)
 * 
 * @note Функція має викликатись між START та STOP
 * @note ACK=0 (LOW) означає успішну передачу
 * @note ACK=1 (HIGH) означає помилку або відсутність TM1637
 * 
 * @warning Не викликати без попереднього tm1637_start()
 * 
 * @see tm1637_start()
 * @see tm1637_stop()
 */
static uint8_t tm1637_write_byte(uint8_t data) {
    int i;
    uint8_t ack;
    
    // 1. Передача 8 біт даних (LSB first)
    for (i = 0; i < 8; ++i) {
        // a. CLK = LOW (підготовка до зміни даних)
        TM1637_PORT->ODR &= ~TM1637_CLK_MASK;
        _delay_us(1);

        // b. Встановлення DIO як вихід Open-Drain
        TM1637_PORT->DDR |= TM1637_DIO_MASK;
        TM1637_PORT->CR1 &= ~TM1637_DIO_MASK;

        // c. Встановлення біта даних на DIO
        if (data & 0x01) {
            TM1637_PORT->ODR |= TM1637_DIO_MASK;
        } else {
            TM1637_PORT->ODR &= ~TM1637_DIO_MASK;
        }
        _delay_us(3);  // Setup time

        // d. CLK = HIGH (дані захоплюються на rising edge)
        TM1637_PORT->ODR |= TM1637_CLK_MASK;
        _delay_us(5);  // Hold time

        // e. Зсув даних для наступного біта
        data >>= 1;
    }

    // 2. Очікування ACK від TM1637
    // a. CLK = LOW
    TM1637_PORT->ODR &= ~TM1637_CLK_MASK;

    // b. Перемикання DIO на вхід з pull-up
    TM1637_PORT->DDR &= ~TM1637_DIO_MASK;  // Вхід
    TM1637_PORT->CR1 |= TM1637_DIO_MASK;   // Pull-up
    _delay_us(1);

    // c. CLK = HIGH (TM1637 виставляє ACK на DIO)
    TM1637_PORT->ODR |= TM1637_CLK_MASK;
    _delay_us(3);

    // d. Зчитування ACK (0 = ACK, 1 = NACK)
    ack = (TM1637_PORT->IDR & TM1637_DIO_MASK) ? 1 : 0;

    // e. CLK = LOW (завершення ACK циклу)
    TM1637_PORT->ODR &= ~TM1637_CLK_MASK;
    _delay_us(1);

    // 3. Відновлення DIO як вихід Open-Drain
    TM1637_PORT->DDR |= TM1637_DIO_MASK;
    TM1637_PORT->CR1 &= ~TM1637_DIO_MASK;

    return ack;
}

/**
 * @brief Перевірка валідності введеного значення яскравості.
 * 
 */
static uint8_t tm1637_check_brightness_boundary(uint8_t brightness){
        if (brightness > 7) {
            brightness = 7;  
            return brightness;
        } else {
            return brightness;
        }
}
