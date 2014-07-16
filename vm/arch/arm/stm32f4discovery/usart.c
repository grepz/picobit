#include <stdlib.h>
#include <stdint.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#include "usart.h"

usart_buf_t g_recv_buf = {
    .head = 0,
    .tail = 0,
};

void usart6_isr(void)
{
    uint16_t head;
    static uint8_t data = '\r';

    if ((USART_CR1(USART6) & USART_CR1_RXNEIE) != 0) {
        /* Reset recv buffer contents if its full */
        head = g_recv_buf.head + 1;
        if (head >= USART_RECV_BUF_LEN)
            head = 0;

        while ((USART_SR(USART6) & USART_SR_RXNE) != 0) {
            data = usart_recv(USART6);

            if (head != g_recv_buf.tail) {
                g_recv_buf.buf[g_recv_buf.head] = data;
                g_recv_buf.head = head;

                if (++head >= USART_RECV_BUF_LEN)
                    head = 0;
            }
        }

        /* Enable TX interrupt to send back. */
//        usart_enable_tx_interrupt(USART6);
    }
#if 0
    if (((USART_CR1(USART6) & USART_CR1_TXEIE) != 0) &&
        ((USART_SR(USART6) & USART_SR_TXE) != 0)) {
        usart_send(USART6, data);
        /* TX event handled */
        usart_disable_tx_interrupt(USART6);
    }
#endif
}

void usart_write(const uint8_t *data, size_t len)
{
    while (len--)
        usart_send_blocking(USART6, *data++);
}

int usart_read(char *buf, size_t len)
{
    uint16_t tail;
    uint16_t recv = 0;

    tail = g_recv_buf.tail;

    while (recv < len) {
        if (g_recv_buf.head != tail) {
            *buf++ = g_recv_buf.buf[tail];
            if (++ tail >= USART_RECV_BUF_LEN)
                tail = 0;

            g_recv_buf.tail = tail;
            recv ++;
        } else
            break;
    }

    return recv;
}

int usart_getchar(char *ch)
{
    uint16_t tail;

    tail = g_recv_buf.tail;

    if (g_recv_buf.head != tail) {
        *ch = g_recv_buf.buf[tail];
        if (++ tail >= USART_RECV_BUF_LEN)
            tail = 0;

        g_recv_buf.tail = tail;

        return 0;
    }

    return -1;
}

void usart_putchar(char c)
{
    usart_send_blocking(USART6, c);
}

void usart_start(void)
{
    /* GPIO init */
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6); /* Tx */
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO7); /* Rx */
    gpio_set_output_options(GPIOC, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, GPIO7);
    gpio_set_af(GPIOC, GPIO_AF8, GPIO6);
    gpio_set_af(GPIOC, GPIO_AF8, GPIO7);

    /* USART configuration */
    usart_set_baudrate(USART6, 115200);
    usart_set_databits(USART6, 8);
    usart_set_stopbits(USART6, USART_STOPBITS_1);
    usart_set_mode(USART6, USART_MODE_TX_RX);
    usart_set_parity(USART6, USART_PARITY_NONE);
    usart_set_flow_control(USART6, USART_FLOWCONTROL_NONE);

    /* Enable RX interrupts */
    usart_enable_rx_interrupt(USART6);

    usart_enable(USART6);
}
