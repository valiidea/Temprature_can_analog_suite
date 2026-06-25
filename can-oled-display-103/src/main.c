#include "firmware_types.h"

#define PERIPH_BASE        0x40000000UL
#define APB1PERIPH_BASE    PERIPH_BASE
#define APB2PERIPH_BASE    (PERIPH_BASE + 0x00010000UL)
#define AHBPERIPH_BASE     (PERIPH_BASE + 0x00020000UL)

#define RCC_BASE           (AHBPERIPH_BASE + 0x00001000UL)
#define FLASH_BASE         (AHBPERIPH_BASE + 0x00002000UL)
#define AFIO_BASE          (APB2PERIPH_BASE + 0x00000000UL)
#define GPIOA_BASE         (APB2PERIPH_BASE + 0x00000800UL)
#define GPIOB_BASE         (APB2PERIPH_BASE + 0x00000C00UL)
#define GPIOC_BASE         (APB2PERIPH_BASE + 0x00001000UL)
#define CAN1_BASE          (APB1PERIPH_BASE + 0x00006400UL)
#define USART1_BASE        (APB2PERIPH_BASE + 0x00003800UL)
#define SYSTICK_BASE       0xE000E010UL

#define BIT(n)             (1UL << (n))

#define CAN_BITRATE        500000UL
#define CAN_TEMP_STD_ID    0x321UL
#define CAN_LINK_PING_STD_ID 0x700UL
#define CAN_LINK_PONG_STD_ID 0x701UL
#define USART1_BAUDRATE    115200UL
#define SYSCLK_HZ          72000000UL
#define PCLK1_HZ           36000000UL
#define PCLK2_HZ           72000000UL
#define CAN_RX_DRAIN_LIMIT 16U
#define OLED_UPDATE_INTERVAL_MS 100U
#define TEMP_SERIAL_INTERVAL_MS 200U

typedef struct {
    volatile uint32_t CRL;
    volatile uint32_t CRH;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t BRR;
    volatile uint32_t LCKR;
} GPIO_TypeDef;

typedef struct {
    volatile uint32_t EVCR;
    volatile uint32_t MAPR;
    volatile uint32_t EXTICR[4];
    volatile uint32_t MAPR2;
} AFIO_TypeDef;

typedef struct {
    volatile uint32_t CR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t APB2RSTR;
    volatile uint32_t APB1RSTR;
    volatile uint32_t AHBENR;
    volatile uint32_t APB2ENR;
    volatile uint32_t APB1ENR;
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
} RCC_TypeDef;

typedef struct {
    volatile uint32_t ACR;
    volatile uint32_t KEYR;
    volatile uint32_t OPTKEYR;
    volatile uint32_t SR;
    volatile uint32_t CR;
    volatile uint32_t AR;
    volatile uint32_t RESERVED;
    volatile uint32_t OBR;
    volatile uint32_t WRPR;
} FLASH_TypeDef;

typedef struct {
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t GTPR;
} USART_TypeDef;

typedef struct {
    volatile uint32_t TIR;
    volatile uint32_t TDTR;
    volatile uint32_t TDLR;
    volatile uint32_t TDHR;
} CAN_TxMailBox_TypeDef;

typedef struct {
    volatile uint32_t RIR;
    volatile uint32_t RDTR;
    volatile uint32_t RDLR;
    volatile uint32_t RDHR;
} CAN_FIFOMailBox_TypeDef;

typedef struct {
    volatile uint32_t FR1;
    volatile uint32_t FR2;
} CAN_FilterRegister_TypeDef;

typedef struct {
    volatile uint32_t MCR;
    volatile uint32_t MSR;
    volatile uint32_t TSR;
    volatile uint32_t RF0R;
    volatile uint32_t RF1R;
    volatile uint32_t IER;
    volatile uint32_t ESR;
    volatile uint32_t BTR;
    volatile uint32_t RESERVED0[88];
    CAN_TxMailBox_TypeDef sTxMailBox[3];
    CAN_FIFOMailBox_TypeDef sFIFOMailBox[2];
    volatile uint32_t RESERVED1[12];
    volatile uint32_t FMR;
    volatile uint32_t FM1R;
    volatile uint32_t RESERVED2;
    volatile uint32_t FS1R;
    volatile uint32_t RESERVED3;
    volatile uint32_t FFA1R;
    volatile uint32_t RESERVED4;
    volatile uint32_t FA1R;
    volatile uint32_t RESERVED5[8];
    CAN_FilterRegister_TypeDef sFilterRegister[14];
} CAN_TypeDef;

typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} SysTick_TypeDef;

#define RCC                ((RCC_TypeDef *)RCC_BASE)
#define FLASH              ((FLASH_TypeDef *)FLASH_BASE)
#define AFIO               ((AFIO_TypeDef *)AFIO_BASE)
#define GPIOA              ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB              ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC              ((GPIO_TypeDef *)GPIOC_BASE)
#define CAN1               ((CAN_TypeDef *)CAN1_BASE)
#define USART1             ((USART_TypeDef *)USART1_BASE)
#define SYSTICK            ((SysTick_TypeDef *)SYSTICK_BASE)

#define RCC_CR_HSEON       BIT(16)
#define RCC_CR_HSERDY      BIT(17)
#define RCC_CR_PLLON       BIT(24)
#define RCC_CR_PLLRDY      BIT(25)

#define RCC_CFGR_SW_PLL    BIT(1)
#define RCC_CFGR_SWS_PLL   BIT(3)
#define RCC_CFGR_PPRE1_DIV2 BIT(10)
#define RCC_CFGR_PLLSRC    BIT(16)
#define RCC_CFGR_PLLMUL9   (7UL << 18)

#define RCC_APB2ENR_AFIOEN BIT(0)
#define RCC_APB2ENR_IOPAEN BIT(2)
#define RCC_APB2ENR_IOPBEN BIT(3)
#define RCC_APB2ENR_IOPCEN BIT(4)
#define RCC_APB2ENR_USART1EN BIT(14)
#define RCC_APB1ENR_CAN1EN BIT(25)
#define RCC_APB1RSTR_CAN1RST BIT(25)

#define FLASH_ACR_LATENCY_2 2UL
#define FLASH_ACR_PRFTBE   BIT(4)

#define AFIO_MAPR_CAN_REMAP_MASK (3UL << 13)
#define AFIO_MAPR_CAN_REMAP_PB8_PB9 BIT(14)

#define GPIO_MODE_INPUT_FLOATING 0x4UL
#define GPIO_MODE_INPUT_PULL     0x8UL
#define GPIO_MODE_OUTPUT_PP_2MHZ 0x2UL
#define GPIO_MODE_OUTPUT_OD_50MHZ 0x7UL
#define GPIO_MODE_AF_PP_50MHZ    0xBUL

#define USART_SR_TXE       BIT(7)
#define USART_CR1_RE       BIT(2)
#define USART_CR1_TE       BIT(3)
#define USART_CR1_UE       BIT(13)

#define CAN_MCR_INRQ       BIT(0)
#define CAN_MCR_SLEEP      BIT(1)
#define CAN_MCR_ABOM       BIT(6)
#define CAN_MCR_AWUM       BIT(5)
#define CAN_MSR_INAK       BIT(0)
#define CAN_FMR_FINIT      BIT(0)
#define CAN_RF0R_FMP0_MASK 0x3UL
#define CAN_RF0R_RFOM0     BIT(5)
#define CAN_RIR_RTR        BIT(1)
#define CAN_RIR_IDE        BIT(2)
#define CAN_TIR_TXRQ       BIT(0)
#define CAN_TSR_RQCP0      BIT(0)
#define CAN_TSR_TXOK0      BIT(1)
#define CAN_TSR_ABRQ0      BIT(7)
#define CAN_TSR_RQCP1      BIT(8)
#define CAN_TSR_TXOK1      BIT(9)
#define CAN_TSR_ABRQ1      BIT(15)
#define CAN_TSR_RQCP2      BIT(16)
#define CAN_TSR_TXOK2      BIT(17)
#define CAN_TSR_ABRQ2      BIT(23)
#define CAN_TSR_TME0       BIT(26)
#define CAN_TSR_TME1       BIT(27)
#define CAN_TSR_TME2       BIT(28)
#define CAN_ESR_BOFF       BIT(2)

#define CAN_RX_PIN         8U
#define CAN_TX_PIN         9U
#define STATUS_LED_PIN     13U
#define STATUS_LED_BLINK_INTERVAL_MS 250UL
#define CAN_LINK_OK_TIMEOUT_MS 10000UL
#define CAN_TX_TIMEOUT     1000000UL
#define CAN_BTR_BRP        4UL
#define CAN_BTR_TS1        15UL
#define CAN_BTR_TS2        2UL
#define CAN_BTR_SJW        1UL

#define SYSTICK_CTRL_ENABLE BIT(0)
#define SYSTICK_CTRL_TICKINT BIT(1)
#define SYSTICK_CTRL_CLKSOURCE BIT(2)
#define SYSTICK_CTRL_COUNTFLAG BIT(16)

#define OLED_I2C_WRITE_ADDRESS_3C 0x78U
#define OLED_I2C_WRITE_ADDRESS_3D 0x7AU
#define OLED_ACK_3C        BIT(0)
#define OLED_ACK_3D        BIT(1)
#define OLED_WIDTH         128U
#define OLED_PAGES         8U
#define OLED_SCL_PIN       6U
#define OLED_SDA_PIN       7U
#define OLED_VALUE_COLUMN  18U
#define OLED_CHAR_WIDTH    6U
#define OLED_TEMP_FIELD_LEN 8U
#define OLED_CURRENT_FIELD_LEN 8U
#define OLED_VOLTAGE_FIELD_LEN 7U

#define TEMP_MIN_DECIC     (-400)
#define TEMP_MAX_DECIC     3000
#define TEMP_SPAN_DECIC    3400
#define CURRENT_MIN_CENTIMA 400
#define CURRENT_SPAN_CENTIMA 1600
#define VOLTAGE_MIN_CENTIV (-500)
#define VOLTAGE_SPAN_CENTIV 1000

typedef struct {
    uint32_t id;
    uint8_t is_extended;
    uint8_t is_remote;
    uint8_t dlc;
    uint8_t data[8];
} CanFrame;

static void clock_init(void);
static void gpio_init(void);
static void can_rx_pin_poll(void);
static void systick_init(void);
static uint8_t systick_elapsed(uint32_t *last_ms, uint32_t interval_ms);
static uint32_t millis(void);
static void usart1_init(void);
static uint8_t can1_init(void);
static uint8_t can1_receive(CanFrame *frame);
static uint8_t can1_transmit(uint32_t id, const uint8_t *data, uint8_t dlc);
static void can_handle_link_frame(const CanFrame *frame);
static uint8_t can_frame_temperature_decic(const CanFrame *frame, int16_t *temperature_decic);
static void print_latest_temperature(int16_t temperature_decic, uint32_t age_ms);
static void print_can_status(void);
static void print_oled_status(void);
static void usart1_write_char(char value);
static void usart1_write_string(const char *value);
static void usart1_write_unsigned(uint32_t value);
static void usart1_write_temperature(int16_t temperature_decic);
static void usart1_write_hex_u32(uint32_t value, uint8_t digits);
static void usart1_write_hex_u8(uint8_t value);
static void oled_delay(void);
static void oled_scl(uint8_t high);
static void oled_sda(uint8_t high);
static void oled_i2c_start(void);
static void oled_i2c_stop(void);
static uint8_t oled_i2c_write(uint8_t value);
static uint8_t oled_probe_address(uint8_t address);
static void oled_select_address(void);
static uint8_t oled_write_control(uint8_t control, uint8_t value);
static void oled_command(uint8_t command);
static void oled_data(uint8_t data);
static void oled_init(void);
static void oled_clear(void);
static void oled_fill(uint8_t pattern);
static void oled_set_cursor(uint8_t column, uint8_t page);
static const uint8_t *oled_glyph(char value);
static void oled_write_char(char value);
static void oled_write_string(const char *value);
static void field_write_unsigned_padded(char *field, uint8_t *position, uint32_t value, uint8_t width);
static void field_write_signed_integer(char *field, uint8_t *position, uint32_t magnitude,
                                       uint8_t integer_width, uint8_t negative);
static void field_write_signed_fixed_1(char *field, uint8_t *position, int16_t value_deci,
                                       uint8_t integer_width);
static void field_write_unsigned_fixed_2(char *field, uint8_t *position, uint32_t value_centi,
                                         uint8_t integer_width);
static void field_write_signed_fixed_2(char *field, uint8_t *position, int16_t value_centi,
                                       uint8_t integer_width);
static void format_temperature_field(char *field, int16_t temperature_decic);
static void format_current_field(char *field, uint32_t current_centima);
static void format_voltage_field(char *field, int16_t voltage_centiv);
static void oled_reset_temperature_field_cache(void);
static void oled_update_field(uint8_t column, uint8_t page, char *cache, const char *next, uint8_t length);
static int16_t clamp_temperature_decic(int16_t temperature_decic);
static uint32_t temperature_to_current_centima(int16_t temperature_decic);
static int16_t temperature_to_voltage_centiv(int16_t temperature_decic);
static void oled_show_waiting(void);
static void oled_show_can_error(void);
static void oled_show_temperature_layout(void);
static void oled_show_temperature(int16_t temperature_decic);
static void status_led_set(uint8_t on);
void SysTick_Handler(void);
static void delay_cycles(uint32_t cycles);

static volatile uint32_t g_millis;
static uint32_t g_can_rx_count;
static uint32_t g_can_tx_count;
static uint32_t g_can_link_count;
static volatile uint32_t g_can_link_last_ms;
static uint32_t g_can_rx_pin_edges;
static volatile uint32_t g_status_led_last_toggle_ms;
static uint8_t g_last_can_rx_pin;
static uint8_t g_oled_i2c_write_address = OLED_I2C_WRITE_ADDRESS_3C;
static uint8_t g_oled_ack_mask;
static uint8_t g_oled_temperature_layout_ready;
static uint32_t g_can_temperature_count;
static uint32_t g_can_temperature_last_ms;
static int16_t g_latest_temperature_decic;
static uint8_t g_temperature_pending;
static char g_oled_temp_cache[OLED_TEMP_FIELD_LEN];
static char g_oled_current_cache[OLED_CURRENT_FIELD_LEN];
static char g_oled_voltage_cache[OLED_VOLTAGE_FIELD_LEN];
static volatile uint8_t g_can_ready;
static volatile uint8_t g_can_link_ok;
static volatile uint8_t g_status_led_on;

int main(void)
{
    clock_init();
    gpio_init();
    systick_init();
    usart1_init();
    oled_init();
    oled_show_waiting();
    g_can_ready = can1_init();

    if (g_can_ready == 0U) {
        oled_show_can_error();
    }

    usart1_write_string("\r\nCanOledDisplay103 CAN receiver\r\n");
    usart1_write_string("CAN receiver: 500 kbit/s on PB8/PB9, accept all IDs\r\n");
    usart1_write_string("Link check: host sends std id 0x700, F103 replies std id 0x701\r\n");
    usart1_write_string("OLED: SSD1306 on PB6/PB7, temperature from CAN ID 0x321 DATA[0..3]\r\n");
    usart1_write_string("OLED update: latest temp at 10Hz max, char-diff refresh\r\n");
    usart1_write_string("USART1: 115200 8N1\r\n");
    print_oled_status();
    usart1_write_string("Waiting for CAN frames...\r\n");

    uint32_t last_status_ms = millis();
    uint32_t last_oled_update_ms = 0U;
    uint32_t last_temp_serial_ms = 0U;

    while (1) {
        CanFrame frame;
        uint8_t drained_count = 0U;

        can_rx_pin_poll();

        while ((g_can_ready != 0U) && (can1_receive(&frame) != 0U)) {
            int16_t temperature_decic;

            ++g_can_rx_count;
            can_handle_link_frame(&frame);

            if (can_frame_temperature_decic(&frame, &temperature_decic) != 0U) {
                g_latest_temperature_decic = temperature_decic;
                g_temperature_pending = 1U;
                ++g_can_temperature_count;
                g_can_temperature_last_ms = millis();
            }

            ++drained_count;
            if (drained_count >= CAN_RX_DRAIN_LIMIT) {
                break;
            }
        }

        const uint32_t now_ms = millis();
        if ((g_temperature_pending != 0U)
            && ((g_oled_temperature_layout_ready == 0U)
                || ((uint32_t)(now_ms - last_oled_update_ms) >= OLED_UPDATE_INTERVAL_MS))) {
            oled_show_temperature(g_latest_temperature_decic);
            last_oled_update_ms = now_ms;
            g_temperature_pending = 0U;

            if ((last_temp_serial_ms == 0U)
                || ((uint32_t)(now_ms - last_temp_serial_ms) >= TEMP_SERIAL_INTERVAL_MS)) {
                print_latest_temperature(g_latest_temperature_decic,
                                         (uint32_t)(now_ms - g_can_temperature_last_ms));
                last_temp_serial_ms = now_ms;
            }
        }

        if (systick_elapsed(&last_status_ms, 1000U) != 0U) {
            print_can_status();
        }
    }
}

static void clock_init(void)
{
    RCC->CR |= RCC_CR_HSEON;
    while ((RCC->CR & RCC_CR_HSERDY) == 0U) {
    }

    FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2;

    RCC->CFGR = RCC_CFGR_PLLSRC | RCC_CFGR_PLLMUL9 | RCC_CFGR_PPRE1_DIV2;

    RCC->CR |= RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) == 0U) {
    }

    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS_PLL) != RCC_CFGR_SWS_PLL) {
    }
}

static void gpio_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;
    AFIO->MAPR = (AFIO->MAPR & ~AFIO_MAPR_CAN_REMAP_MASK) | AFIO_MAPR_CAN_REMAP_PB8_PB9;

    /* PA9 USART1_TX, PA10 USART1_RX. */
    GPIOA->CRH &= ~((0xFUL << 4) | (0xFUL << 8));
    GPIOA->CRH |= (GPIO_MODE_AF_PP_50MHZ << 4)
               |  (GPIO_MODE_INPUT_FLOATING << 8);

    /* PB8 CAN_RX, PB9 CAN_TX after AFIO CAN remap. */
    GPIOB->CRH &= ~((0xFUL << 0) | (0xFUL << 4));
    GPIOB->CRH |= (GPIO_MODE_INPUT_FLOATING << 0)
               |  (GPIO_MODE_AF_PP_50MHZ << 4);

    /* PB6 OLED_SCL, PB7 OLED_SDA, software I2C open-drain, matching the OLED example. */
    GPIOB->CRL &= ~((0xFUL << 24) | (0xFUL << 28));
    GPIOB->CRL |= (GPIO_MODE_OUTPUT_OD_50MHZ << 24)
               |  (GPIO_MODE_OUTPUT_OD_50MHZ << 28);
    GPIOB->BSRR = BIT(OLED_SCL_PIN) | BIT(OLED_SDA_PIN);

    /* PC13 Blue Pill LED, active low. On means not linked yet. */
    GPIOC->CRH &= ~(0xFUL << 20);
    GPIOC->CRH |= (GPIO_MODE_OUTPUT_PP_2MHZ << 20);
    status_led_set(1);

    g_last_can_rx_pin = (GPIOB->IDR & BIT(CAN_RX_PIN)) != 0U;
}

static void can_rx_pin_poll(void)
{
    const uint8_t rx_pin = (GPIOB->IDR & BIT(CAN_RX_PIN)) != 0U;

    if (rx_pin != g_last_can_rx_pin) {
        g_last_can_rx_pin = rx_pin;
        ++g_can_rx_pin_edges;
    }
}

static void systick_init(void)
{
    SYSTICK->LOAD = (SYSCLK_HZ / 1000UL) - 1UL;
    SYSTICK->VAL = 0;
    SYSTICK->CTRL = SYSTICK_CTRL_CLKSOURCE | SYSTICK_CTRL_TICKINT | SYSTICK_CTRL_ENABLE;
}

static uint8_t systick_elapsed(uint32_t *last_ms, uint32_t interval_ms)
{
    const uint32_t now_ms = millis();

    if ((uint32_t)(now_ms - *last_ms) < interval_ms) {
        return 0;
    }

    *last_ms = now_ms;
    return 1;
}

static uint32_t millis(void)
{
    return g_millis;
}

void SysTick_Handler(void)
{
    const uint32_t now_ms = g_millis + 1U;
    uint8_t link_active;

    g_millis = now_ms;

    link_active = (g_can_ready != 0U)
               && (g_can_link_ok != 0U)
               && ((CAN1->ESR & CAN_ESR_BOFF) == 0U)
               && ((uint32_t)(now_ms - g_can_link_last_ms) < CAN_LINK_OK_TIMEOUT_MS);

    if (link_active == 0U) {
        status_led_set(1);
        g_status_led_last_toggle_ms = now_ms;
        return;
    }

    if ((uint32_t)(now_ms - g_status_led_last_toggle_ms) >= STATUS_LED_BLINK_INTERVAL_MS) {
        status_led_set((g_status_led_on == 0U) ? 1U : 0U);
        g_status_led_last_toggle_ms = now_ms;
    }
}

static void usart1_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    USART1->CR1 = 0;
    USART1->CR2 = 0;
    USART1->CR3 = 0;
    USART1->BRR = (PCLK2_HZ + (USART1_BAUDRATE / 2UL)) / USART1_BAUDRATE;
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

static uint8_t can1_init(void)
{
    uint32_t timeout;

    (void)CAN_BITRATE;

    RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;
    RCC->APB1RSTR |= RCC_APB1RSTR_CAN1RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_CAN1RST;

    CAN1->MCR &= ~CAN_MCR_SLEEP;
    CAN1->MCR |= CAN_MCR_INRQ;
    timeout = 1000000U;
    while (((CAN1->MSR & CAN_MSR_INAK) == 0U) && (timeout != 0U)) {
        --timeout;
    }

    if (timeout == 0U) {
        return 0;
    }

    CAN1->MCR = CAN_MCR_INRQ | CAN_MCR_ABOM | CAN_MCR_AWUM;

    /* 36 MHz / (4 * (1 sync + 15 BS1 + 2 BS2)) = 500 kbit/s, sample point 88.9%. */
    CAN1->BTR = ((CAN_BTR_SJW - 1UL) << 24)
              | ((CAN_BTR_TS2 - 1UL) << 20)
              | ((CAN_BTR_TS1 - 1UL) << 16)
              | (CAN_BTR_BRP - 1UL);

    CAN1->FMR |= CAN_FMR_FINIT;
    CAN1->FA1R &= ~BIT(0);
    CAN1->FM1R &= ~BIT(0);
    CAN1->FS1R |= BIT(0);
    CAN1->FFA1R &= ~BIT(0);
    CAN1->sFilterRegister[0].FR1 = 0;
    CAN1->sFilterRegister[0].FR2 = 0;
    CAN1->FA1R |= BIT(0);
    CAN1->FMR &= ~CAN_FMR_FINIT;

    CAN1->MCR = CAN_MCR_ABOM | CAN_MCR_AWUM;
    timeout = 1000000U;
    while (((CAN1->MSR & CAN_MSR_INAK) != 0U) && (timeout != 0U)) {
        --timeout;
    }

    return (timeout != 0U) ? 1U : 0U;
}

static uint8_t can1_receive(CanFrame *frame)
{
    if ((CAN1->RF0R & CAN_RF0R_FMP0_MASK) == 0U) {
        return 0;
    }

    const uint32_t rir = CAN1->sFIFOMailBox[0].RIR;
    const uint32_t rdtr = CAN1->sFIFOMailBox[0].RDTR;
    const uint32_t rdlr = CAN1->sFIFOMailBox[0].RDLR;
    const uint32_t rdhr = CAN1->sFIFOMailBox[0].RDHR;

    frame->is_extended = (rir & CAN_RIR_IDE) != 0U;
    frame->is_remote = (rir & CAN_RIR_RTR) != 0U;
    frame->dlc = (uint8_t)(rdtr & 0x0FU);

    if (frame->dlc > 8U) {
        frame->dlc = 8U;
    }

    if (frame->is_extended != 0U) {
        frame->id = (rir >> 3) & 0x1FFFFFFFUL;
    } else {
        frame->id = (rir >> 21) & 0x7FFUL;
    }

    frame->data[0] = (uint8_t)(rdlr >> 0);
    frame->data[1] = (uint8_t)(rdlr >> 8);
    frame->data[2] = (uint8_t)(rdlr >> 16);
    frame->data[3] = (uint8_t)(rdlr >> 24);
    frame->data[4] = (uint8_t)(rdhr >> 0);
    frame->data[5] = (uint8_t)(rdhr >> 8);
    frame->data[6] = (uint8_t)(rdhr >> 16);
    frame->data[7] = (uint8_t)(rdhr >> 24);

    CAN1->RF0R |= CAN_RF0R_RFOM0;
    return 1;
}

static uint8_t can1_transmit(uint32_t id, const uint8_t *data, uint8_t dlc)
{
    CAN_TxMailBox_TypeDef *mailbox;
    uint32_t rqcp_bit;
    uint32_t txok_bit;
    uint32_t abrq_bit;
    uint32_t timeout;
    uint32_t tdlr = 0;
    uint32_t tdhr = 0;

    if (dlc > 8U) {
        dlc = 8U;
    }

    if ((CAN1->TSR & CAN_TSR_TME0) != 0U) {
        mailbox = &CAN1->sTxMailBox[0];
        rqcp_bit = CAN_TSR_RQCP0;
        txok_bit = CAN_TSR_TXOK0;
        abrq_bit = CAN_TSR_ABRQ0;
    } else if ((CAN1->TSR & CAN_TSR_TME1) != 0U) {
        mailbox = &CAN1->sTxMailBox[1];
        rqcp_bit = CAN_TSR_RQCP1;
        txok_bit = CAN_TSR_TXOK1;
        abrq_bit = CAN_TSR_ABRQ1;
    } else if ((CAN1->TSR & CAN_TSR_TME2) != 0U) {
        mailbox = &CAN1->sTxMailBox[2];
        rqcp_bit = CAN_TSR_RQCP2;
        txok_bit = CAN_TSR_TXOK2;
        abrq_bit = CAN_TSR_ABRQ2;
    } else {
        return 0;
    }

    for (uint8_t i = 0; i < dlc; ++i) {
        if (i < 4U) {
            tdlr |= (uint32_t)data[i] << (uint8_t)(i * 8U);
        } else {
            tdhr |= (uint32_t)data[i] << (uint8_t)((i - 4U) * 8U);
        }
    }

    CAN1->TSR = rqcp_bit;
    mailbox->TIR = 0;
    mailbox->TDTR = dlc;
    mailbox->TDLR = tdlr;
    mailbox->TDHR = tdhr;
    mailbox->TIR = ((id & 0x7FFUL) << 21) | CAN_TIR_TXRQ;

    timeout = CAN_TX_TIMEOUT;
    while (((CAN1->TSR & rqcp_bit) == 0U) && (timeout != 0U)) {
        --timeout;
    }

    if (timeout == 0U) {
        CAN1->TSR = abrq_bit;
        return 0;
    }

    if ((CAN1->TSR & txok_bit) == 0U) {
        CAN1->TSR = rqcp_bit;
        return 0;
    }

    CAN1->TSR = rqcp_bit;
    ++g_can_tx_count;
    return 1;
}

static void can_handle_link_frame(const CanFrame *frame)
{
    uint8_t response[8];

    if ((frame->is_extended != 0U) || (frame->is_remote != 0U) || (frame->id != CAN_LINK_PING_STD_ID)) {
        return;
    }

    response[0] = 'F';
    response[1] = '1';
    response[2] = '0';
    response[3] = '3';
    response[4] = 'O';
    response[5] = 'K';
    response[6] = (uint8_t)g_can_rx_count;
    response[7] = (uint8_t)(g_can_rx_count >> 8);

    if (can1_transmit(CAN_LINK_PONG_STD_ID, response, 8U) != 0U) {
        g_can_link_ok = 1;
        g_can_link_last_ms = millis();
        ++g_can_link_count;
        usart1_write_string("CAN link OK: replied id=0x701 bytes=46 31 30 33 4F 4B ");
        usart1_write_hex_u8(response[6]);
        usart1_write_char(' ');
        usart1_write_hex_u8(response[7]);
        usart1_write_string("\r\n");
    } else {
        usart1_write_string("CAN link reply failed\r\n");
    }
}

static uint8_t can_frame_temperature_decic(const CanFrame *frame, int16_t *temperature_decic)
{
    uint32_t raw;
    int32_t centi_c;

    if ((frame->is_remote != 0U) || (frame->is_extended != 0U) || (frame->id != CAN_TEMP_STD_ID)) {
        return 0;
    }

    if (frame->dlc >= 4U) {
        raw = ((uint32_t)frame->data[0] << 24)
            | ((uint32_t)frame->data[1] << 16)
            | ((uint32_t)frame->data[2] << 8)
            | (uint32_t)frame->data[3];
        centi_c = (int32_t)raw;
        *temperature_decic = (int16_t)((centi_c >= 0) ? ((centi_c + 5) / 10) : ((centi_c - 5) / 10));
        return 1;
    }

    if (frame->dlc < 2U) {
        return 0;
    }

    raw = (uint32_t)frame->data[0] | ((uint32_t)frame->data[1] << 8);
    *temperature_decic = (int16_t)raw;
    return 1;
}

static void print_latest_temperature(int16_t temperature_decic, uint32_t age_ms)
{
    usart1_write_string("CAN temp latest rx=0x");
    usart1_write_hex_u32(g_can_temperature_count, 8);
    usart1_write_string(" temp=");
    usart1_write_temperature(temperature_decic);
    usart1_write_string(" age=");
    usart1_write_unsigned(age_ms);
    usart1_write_string("ms");
    usart1_write_string("\r\n");
}

static void print_can_status(void)
{
    const uint32_t esr = CAN1->ESR;
    const uint32_t now_ms = millis();
    const uint8_t link_active = (g_can_link_ok != 0U)
                             && ((uint32_t)(now_ms - g_can_link_last_ms) < CAN_LINK_OK_TIMEOUT_MS);

    usart1_write_string("CAN stat rx=0x");
    usart1_write_hex_u32(g_can_rx_count, 8);
    usart1_write_string(" temp=0x");
    usart1_write_hex_u32(g_can_temperature_count, 8);
    usart1_write_string(" tx=0x");
    usart1_write_hex_u32(g_can_tx_count, 8);
    usart1_write_string(" link=");
    usart1_write_string((link_active != 0U) ? "OK" : "WAIT");
    usart1_write_string(" pong=0x");
    usart1_write_hex_u32(g_can_link_count, 8);
    usart1_write_string(" pb8=");
    usart1_write_char((GPIOB->IDR & BIT(CAN_RX_PIN)) != 0U ? 'H' : 'L');
    usart1_write_string(" pb9=");
    usart1_write_char((GPIOB->IDR & BIT(CAN_TX_PIN)) != 0U ? 'H' : 'L');
    usart1_write_string(" edges=0x");
    usart1_write_hex_u32(g_can_rx_pin_edges, 8);
    usart1_write_string(" msr=0x");
    usart1_write_hex_u32(CAN1->MSR, 8);
    usart1_write_string(" tsr=0x");
    usart1_write_hex_u32(CAN1->TSR, 8);
    usart1_write_string(" rf0r=0x");
    usart1_write_hex_u32(CAN1->RF0R, 8);
    usart1_write_string(" esr=0x");
    usart1_write_hex_u32(esr, 8);
    usart1_write_string(" btr=0x");
    usart1_write_hex_u32(CAN1->BTR, 8);
    usart1_write_string(" mapr=0x");
    usart1_write_hex_u32(AFIO->MAPR, 8);
    usart1_write_string(" lec=0x");
    usart1_write_hex_u8((uint8_t)((esr >> 4) & 0x07U));
    usart1_write_string(" rec=0x");
    usart1_write_hex_u8((uint8_t)(esr >> 24));
    usart1_write_string(" tec=0x");
    usart1_write_hex_u8((uint8_t)(esr >> 16));
    usart1_write_string("\r\n");
}

static void print_oled_status(void)
{
    usart1_write_string("OLED i2c addr=0x");
    usart1_write_hex_u8((uint8_t)(g_oled_i2c_write_address >> 1));
    usart1_write_string(" ack3c=");
    usart1_write_char((g_oled_ack_mask & OLED_ACK_3C) != 0U ? 'Y' : 'N');
    usart1_write_string(" ack3d=");
    usart1_write_char((g_oled_ack_mask & OLED_ACK_3D) != 0U ? 'Y' : 'N');
    usart1_write_string(" pb6=");
    usart1_write_char((GPIOB->IDR & BIT(OLED_SCL_PIN)) != 0U ? 'H' : 'L');
    usart1_write_string(" pb7=");
    usart1_write_char((GPIOB->IDR & BIT(OLED_SDA_PIN)) != 0U ? 'H' : 'L');
    usart1_write_string("\r\n");
}

static void usart1_write_char(char value)
{
    while ((USART1->SR & USART_SR_TXE) == 0U) {
    }

    USART1->DR = (uint32_t)(uint8_t)value;
}

static void usart1_write_string(const char *value)
{
    while (*value != '\0') {
        usart1_write_char(*value++);
    }
}

static void usart1_write_unsigned(uint32_t value)
{
    char buffer[10];
    uint8_t length = 0;

    do {
        buffer[length++] = (char)('0' + (value % 10U));
        value /= 10U;
    } while (value != 0U);

    while (length != 0U) {
        usart1_write_char(buffer[--length]);
    }
}

static void usart1_write_temperature(int16_t temperature_decic)
{
    uint32_t magnitude;

    if (temperature_decic < 0) {
        usart1_write_char('-');
        magnitude = (uint32_t)(-(int32_t)temperature_decic);
    } else {
        magnitude = (uint32_t)temperature_decic;
    }

    usart1_write_unsigned(magnitude / 10U);
    usart1_write_char('.');
    usart1_write_char((char)('0' + (magnitude % 10U)));
    usart1_write_string(" C");
}

static void usart1_write_hex_u32(uint32_t value, uint8_t digits)
{
    static const char hex[] = "0123456789ABCDEF";

    for (int8_t shift = (int8_t)((digits - 1U) * 4U); shift >= 0; shift -= 4) {
        usart1_write_char(hex[(value >> (uint8_t)shift) & 0x0FU]);
    }
}

static void usart1_write_hex_u8(uint8_t value)
{
    usart1_write_hex_u32(value, 2);
}

static void oled_delay(void)
{
    for (volatile uint32_t i = 0; i < 120U; ++i) {
    }
}

static void oled_scl(uint8_t high)
{
    if (high != 0U) {
        GPIOB->BSRR = BIT(OLED_SCL_PIN);
    } else {
        GPIOB->BRR = BIT(OLED_SCL_PIN);
    }
    oled_delay();
}

static void oled_sda(uint8_t high)
{
    if (high != 0U) {
        GPIOB->BSRR = BIT(OLED_SDA_PIN);
    } else {
        GPIOB->BRR = BIT(OLED_SDA_PIN);
    }
    oled_delay();
}

static void oled_i2c_start(void)
{
    oled_sda(1);
    oled_scl(1);
    oled_sda(0);
    oled_scl(0);
}

static void oled_i2c_stop(void)
{
    oled_sda(0);
    oled_scl(1);
    oled_sda(1);
}

static uint8_t oled_i2c_write(uint8_t value)
{
    for (uint8_t mask = 0x80U; mask != 0U; mask >>= 1) {
        oled_sda((value & mask) != 0U);
        oled_scl(1);
        oled_scl(0);
    }

    oled_sda(1);
    oled_scl(1);
    const uint8_t ack = (GPIOB->IDR & BIT(OLED_SDA_PIN)) == 0U;
    oled_scl(0);
    return ack;
}

static uint8_t oled_probe_address(uint8_t address)
{
    oled_i2c_start();
    const uint8_t ack = oled_i2c_write(address);
    oled_i2c_stop();
    return ack;
}

static void oled_select_address(void)
{
    g_oled_ack_mask = 0;

    if (oled_probe_address(OLED_I2C_WRITE_ADDRESS_3C) != 0U) {
        g_oled_ack_mask |= OLED_ACK_3C;
    }

    if (oled_probe_address(OLED_I2C_WRITE_ADDRESS_3D) != 0U) {
        g_oled_ack_mask |= OLED_ACK_3D;
    }

    g_oled_i2c_write_address = ((g_oled_ack_mask & OLED_ACK_3D) != 0U)
                             && ((g_oled_ack_mask & OLED_ACK_3C) == 0U)
                              ? OLED_I2C_WRITE_ADDRESS_3D
                              : OLED_I2C_WRITE_ADDRESS_3C;
}

static uint8_t oled_write_control(uint8_t control, uint8_t value)
{
    uint8_t ack;

    oled_i2c_start();
    ack = oled_i2c_write(g_oled_i2c_write_address);
    ack &= oled_i2c_write(control);
    ack &= oled_i2c_write(value);
    oled_i2c_stop();
    return ack;
}

static void oled_command(uint8_t command)
{
    (void)oled_write_control(0x00U, command);
}

static void oled_data(uint8_t data)
{
    (void)oled_write_control(0x40U, data);
}

static void oled_init(void)
{
    delay_cycles(7200000U);
    oled_select_address();

    oled_command(0xAEU);

    oled_command(0xD5U);
    oled_command(0x80U);

    oled_command(0xA8U);
    oled_command(0x3FU);

    oled_command(0xD3U);
    oled_command(0x00U);

    oled_command(0x40U);
    oled_command(0xA1U);
    oled_command(0xC8U);

    oled_command(0xDAU);
    oled_command(0x12U);

    oled_command(0x81U);
    oled_command(0xCFU);

    oled_command(0xD9U);
    oled_command(0xF1U);

    oled_command(0xDBU);
    oled_command(0x30U);

    oled_command(0xA4U);
    oled_command(0xA6U);

    oled_command(0x8DU);
    oled_command(0x14U);

    oled_command(0xAFU);

    oled_fill(0xFFU);
    delay_cycles(7200000U);
    oled_clear();
}

static void oled_clear(void)
{
    oled_fill(0x00U);
}

static void oled_fill(uint8_t pattern)
{
    for (uint8_t page = 0; page < OLED_PAGES; ++page) {
        oled_set_cursor(0, page);
        for (uint8_t column = 0; column < OLED_WIDTH; ++column) {
            oled_data(pattern);
        }
    }
}

static void oled_set_cursor(uint8_t column, uint8_t page)
{
    oled_command((uint8_t)(0xB0U | (page & 0x07U)));
    oled_command((uint8_t)(0x00U | (column & 0x0FU)));
    oled_command((uint8_t)(0x10U | ((column >> 4) & 0x0FU)));
}

static const uint8_t *oled_glyph(char value)
{
    static const uint8_t blank[5] = {0x00U, 0x00U, 0x00U, 0x00U, 0x00U};
    static const uint8_t colon[5] = {0x00U, 0x36U, 0x36U, 0x00U, 0x00U};
    static const uint8_t dot[5] = {0x00U, 0x60U, 0x60U, 0x00U, 0x00U};
    static const uint8_t minus[5] = {0x08U, 0x08U, 0x08U, 0x08U, 0x08U};
    static const uint8_t lower_m[5] = {0x7CU, 0x04U, 0x18U, 0x04U, 0x78U};
    static const uint8_t digits[10][5] = {
        {0x3EU, 0x51U, 0x49U, 0x45U, 0x3EU},
        {0x00U, 0x42U, 0x7FU, 0x40U, 0x00U},
        {0x42U, 0x61U, 0x51U, 0x49U, 0x46U},
        {0x21U, 0x41U, 0x45U, 0x4BU, 0x31U},
        {0x18U, 0x14U, 0x12U, 0x7FU, 0x10U},
        {0x27U, 0x45U, 0x45U, 0x45U, 0x39U},
        {0x3CU, 0x4AU, 0x49U, 0x49U, 0x30U},
        {0x01U, 0x71U, 0x09U, 0x05U, 0x03U},
        {0x36U, 0x49U, 0x49U, 0x49U, 0x36U},
        {0x06U, 0x49U, 0x49U, 0x29U, 0x1EU}
    };
    static const uint8_t letters[26][5] = {
        {0x7EU, 0x11U, 0x11U, 0x11U, 0x7EU},
        {0x7FU, 0x49U, 0x49U, 0x49U, 0x36U},
        {0x3EU, 0x41U, 0x41U, 0x41U, 0x22U},
        {0x7FU, 0x41U, 0x41U, 0x22U, 0x1CU},
        {0x7FU, 0x49U, 0x49U, 0x49U, 0x41U},
        {0x7FU, 0x09U, 0x09U, 0x09U, 0x01U},
        {0x3EU, 0x41U, 0x49U, 0x49U, 0x7AU},
        {0x7FU, 0x08U, 0x08U, 0x08U, 0x7FU},
        {0x00U, 0x41U, 0x7FU, 0x41U, 0x00U},
        {0x20U, 0x40U, 0x41U, 0x3FU, 0x01U},
        {0x7FU, 0x08U, 0x14U, 0x22U, 0x41U},
        {0x7FU, 0x40U, 0x40U, 0x40U, 0x40U},
        {0x7FU, 0x02U, 0x0CU, 0x02U, 0x7FU},
        {0x7FU, 0x04U, 0x08U, 0x10U, 0x7FU},
        {0x3EU, 0x41U, 0x41U, 0x41U, 0x3EU},
        {0x7FU, 0x09U, 0x09U, 0x09U, 0x06U},
        {0x3EU, 0x41U, 0x51U, 0x21U, 0x5EU},
        {0x7FU, 0x09U, 0x19U, 0x29U, 0x46U},
        {0x46U, 0x49U, 0x49U, 0x49U, 0x31U},
        {0x01U, 0x01U, 0x7FU, 0x01U, 0x01U},
        {0x3FU, 0x40U, 0x40U, 0x40U, 0x3FU},
        {0x1FU, 0x20U, 0x40U, 0x20U, 0x1FU},
        {0x3FU, 0x40U, 0x38U, 0x40U, 0x3FU},
        {0x63U, 0x14U, 0x08U, 0x14U, 0x63U},
        {0x07U, 0x08U, 0x70U, 0x08U, 0x07U},
        {0x61U, 0x51U, 0x49U, 0x45U, 0x43U}
    };

    if ((value >= '0') && (value <= '9')) {
        return digits[(uint8_t)(value - '0')];
    }

    if (value == 'm') {
        return lower_m;
    }

    if ((value >= 'a') && (value <= 'z')) {
        value = (char)(value - ('a' - 'A'));
    }

    if ((value >= 'A') && (value <= 'Z')) {
        return letters[(uint8_t)(value - 'A')];
    }

    if (value == ':') {
        return colon;
    }

    if (value == '.') {
        return dot;
    }

    if (value == '-') {
        return minus;
    }

    return blank;
}

static void oled_write_char(char value)
{
    const uint8_t *glyph = oled_glyph(value);

    for (uint8_t i = 0; i < 5U; ++i) {
        oled_data(glyph[i]);
    }
    oled_data(0x00U);
}

static void oled_write_string(const char *value)
{
    while (*value != '\0') {
        oled_write_char(*value++);
    }
}

static void field_write_unsigned_padded(char *field, uint8_t *position, uint32_t value, uint8_t width)
{
    char buffer[10];
    uint8_t length = 0;
    uint8_t padding;

    do {
        buffer[length++] = (char)('0' + (value % 10U));
        value /= 10U;
    } while ((value != 0U) && (length < (uint8_t)sizeof(buffer)));

    padding = (length < width) ? (uint8_t)(width - length) : 0U;
    while (padding != 0U) {
        field[*position] = ' ';
        ++(*position);
        --padding;
    }

    while (length != 0U) {
        field[*position] = buffer[--length];
        ++(*position);
    }
}

static void field_write_signed_integer(char *field, uint8_t *position, uint32_t magnitude,
                                       uint8_t integer_width, uint8_t negative)
{
    char buffer[10];
    uint8_t length = 0;
    uint8_t total_width = (uint8_t)(integer_width + 1U);
    uint8_t used_width;
    uint8_t padding;

    do {
        buffer[length++] = (char)('0' + (magnitude % 10U));
        magnitude /= 10U;
    } while ((magnitude != 0U) && (length < (uint8_t)sizeof(buffer)));

    used_width = (uint8_t)(length + ((negative != 0U) ? 1U : 0U));
    padding = (used_width < total_width) ? (uint8_t)(total_width - used_width) : 0U;

    while (padding != 0U) {
        field[*position] = ' ';
        ++(*position);
        --padding;
    }

    if (negative != 0U) {
        field[*position] = '-';
        ++(*position);
    }

    while (length != 0U) {
        field[*position] = buffer[--length];
        ++(*position);
    }
}

static void field_write_signed_fixed_1(char *field, uint8_t *position, int16_t value_deci,
                                       uint8_t integer_width)
{
    const int32_t signed_value = (int32_t)value_deci;
    const uint32_t magnitude = (signed_value < 0) ? (uint32_t)(-signed_value) : (uint32_t)signed_value;

    field_write_signed_integer(field, position, magnitude / 10U, integer_width,
                               (signed_value < 0) ? 1U : 0U);
    field[*position] = '.';
    ++(*position);
    field[*position] = (char)('0' + (magnitude % 10U));
    ++(*position);
}

static void field_write_unsigned_fixed_2(char *field, uint8_t *position, uint32_t value_centi,
                                         uint8_t integer_width)
{
    field_write_unsigned_padded(field, position, value_centi / 100U, integer_width);
    field[*position] = '.';
    ++(*position);
    field[*position] = (char)('0' + ((value_centi / 10U) % 10U));
    ++(*position);
    field[*position] = (char)('0' + (value_centi % 10U));
    ++(*position);
}

static void field_write_signed_fixed_2(char *field, uint8_t *position, int16_t value_centi,
                                       uint8_t integer_width)
{
    const int32_t signed_value = (int32_t)value_centi;
    const uint32_t magnitude = (signed_value < 0) ? (uint32_t)(-signed_value) : (uint32_t)signed_value;

    field_write_signed_integer(field, position, magnitude / 100U, integer_width,
                               (signed_value < 0) ? 1U : 0U);
    field[*position] = '.';
    ++(*position);
    field[*position] = (char)('0' + ((magnitude / 10U) % 10U));
    ++(*position);
    field[*position] = (char)('0' + (magnitude % 10U));
    ++(*position);
}

static void format_temperature_field(char *field, int16_t temperature_decic)
{
    uint8_t position = 0U;

    field_write_signed_fixed_1(field, &position, temperature_decic, 3U);
    field[position++] = ' ';
    field[position++] = 'C';
}

static void format_current_field(char *field, uint32_t current_centima)
{
    uint8_t position = 0U;

    field_write_unsigned_fixed_2(field, &position, current_centima, 2U);
    field[position++] = ' ';
    field[position++] = 'm';
    field[position++] = 'A';
}

static void format_voltage_field(char *field, int16_t voltage_centiv)
{
    uint8_t position = 0U;

    field_write_signed_fixed_2(field, &position, voltage_centiv, 1U);
    field[position++] = ' ';
    field[position++] = 'V';
}

static void oled_reset_temperature_field_cache(void)
{
    for (uint8_t i = 0U; i < OLED_TEMP_FIELD_LEN; ++i) {
        g_oled_temp_cache[i] = '\0';
    }

    for (uint8_t i = 0U; i < OLED_CURRENT_FIELD_LEN; ++i) {
        g_oled_current_cache[i] = '\0';
    }

    for (uint8_t i = 0U; i < OLED_VOLTAGE_FIELD_LEN; ++i) {
        g_oled_voltage_cache[i] = '\0';
    }
}

static void oled_update_field(uint8_t column, uint8_t page, char *cache, const char *next, uint8_t length)
{
    for (uint8_t i = 0U; i < length; ++i) {
        if (cache[i] != next[i]) {
            oled_set_cursor((uint8_t)(column + (i * OLED_CHAR_WIDTH)), page);
            oled_write_char(next[i]);
            cache[i] = next[i];
        }
    }
}

static int16_t clamp_temperature_decic(int16_t temperature_decic)
{
    if (temperature_decic < TEMP_MIN_DECIC) {
        return TEMP_MIN_DECIC;
    }

    if (temperature_decic > TEMP_MAX_DECIC) {
        return TEMP_MAX_DECIC;
    }

    return temperature_decic;
}

static uint32_t temperature_to_current_centima(int16_t temperature_decic)
{
    const int32_t clamped = (int32_t)clamp_temperature_decic(temperature_decic);
    const int32_t offset = clamped - (int32_t)TEMP_MIN_DECIC;
    const int32_t scaled = (offset * (int32_t)CURRENT_SPAN_CENTIMA + (TEMP_SPAN_DECIC / 2)) / TEMP_SPAN_DECIC;

    return (uint32_t)((int32_t)CURRENT_MIN_CENTIMA + scaled);
}

static int16_t temperature_to_voltage_centiv(int16_t temperature_decic)
{
    const int32_t clamped = (int32_t)clamp_temperature_decic(temperature_decic);
    const int32_t offset = clamped - (int32_t)TEMP_MIN_DECIC;
    const int32_t scaled = (offset * (int32_t)VOLTAGE_SPAN_CENTIV + (TEMP_SPAN_DECIC / 2)) / TEMP_SPAN_DECIC;

    return (int16_t)((int32_t)VOLTAGE_MIN_CENTIV + scaled);
}

static void oled_show_waiting(void)
{
    g_oled_temperature_layout_ready = 0;
    oled_reset_temperature_field_cache();
    oled_clear();
    oled_set_cursor(0, 0);
    oled_write_string("CAN TEMP");
    oled_set_cursor(0, 2);
    oled_write_string("WAITING");
}

static void oled_show_can_error(void)
{
    g_oled_temperature_layout_ready = 0;
    oled_reset_temperature_field_cache();
    oled_clear();
    oled_set_cursor(0, 0);
    oled_write_string("CAN TEMP");
    oled_set_cursor(0, 2);
    oled_write_string("CAN INIT ERR");
    oled_set_cursor(0, 4);
    oled_write_string("CHECK PB8 PB9");
}

static void oled_show_temperature_layout(void)
{
    oled_clear();
    oled_reset_temperature_field_cache();
    oled_set_cursor(0, 0);
    oled_write_string("CAN TEMP");
    oled_set_cursor(0, 2);
    oled_write_string("T:");
    oled_set_cursor(0, 4);
    oled_write_string("I:");
    oled_set_cursor(0, 6);
    oled_write_string("V:");
    g_oled_temperature_layout_ready = 1;
}

static void oled_show_temperature(int16_t temperature_decic)
{
    const uint32_t current_centima = temperature_to_current_centima(temperature_decic);
    const int16_t voltage_centiv = temperature_to_voltage_centiv(temperature_decic);
    char temperature_field[OLED_TEMP_FIELD_LEN];
    char current_field[OLED_CURRENT_FIELD_LEN];
    char voltage_field[OLED_VOLTAGE_FIELD_LEN];

    if (g_oled_temperature_layout_ready == 0U) {
        oled_show_temperature_layout();
    }

    format_temperature_field(temperature_field, temperature_decic);
    format_current_field(current_field, current_centima);
    format_voltage_field(voltage_field, voltage_centiv);

    oled_update_field(OLED_VALUE_COLUMN, 2, g_oled_temp_cache,
                      temperature_field, OLED_TEMP_FIELD_LEN);
    oled_update_field(OLED_VALUE_COLUMN, 4, g_oled_current_cache,
                      current_field, OLED_CURRENT_FIELD_LEN);
    oled_update_field(OLED_VALUE_COLUMN, 6, g_oled_voltage_cache,
                      voltage_field, OLED_VOLTAGE_FIELD_LEN);
}

static void status_led_set(uint8_t on)
{
    g_status_led_on = on;

    if (on != 0U) {
        GPIOC->BRR = BIT(STATUS_LED_PIN);
    } else {
        GPIOC->BSRR = BIT(STATUS_LED_PIN);
    }
}

static void delay_cycles(uint32_t cycles)
{
    while (cycles != 0U) {
        --cycles;
    }
}
