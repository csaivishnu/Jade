#include <stdbool.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "include/mailbox/mailbox.h"

void mailbox_init(mailbox_t* mailbox)
{
    mailbox->head = 0;
    mailbox->tail = 0;
}

int mailbox_queue_data(mailbox_t* mailbox, const mailbox_data_t* data)
{
    int ret = 0;
    uint16_t size;
    uint16_t bufferSizeUsed;
    size = (data->header) & 255;
    bufferSizeUsed = (((238 + mailbox->head) - mailbox->tail) % 238);
    if ((bufferSizeUsed + size) < 238)
    {
        mailbox->buffer[mailbox->head] = ((data->header) >> 24) & 255;
        mailbox->head = (mailbox->head + 1) % 238;
        mailbox->buffer[mailbox->head] = ((data->header) >> 16) & 255;
        mailbox->head = (mailbox->head + 1) % 238;
        mailbox->buffer[mailbox->head] = ((data->header) >> 8) & 255;
        mailbox->head = (mailbox->head + 1) % 238;
        mailbox->buffer[mailbox->head] = (data->header) & 255;
        mailbox->head = (mailbox->head + 1) % 238;
        for (int i = 0; i < size - 4; i++)
        {
            mailbox->buffer[mailbox->head] = data->payload[i];
            mailbox->head = (mailbox->head + 1) % 238;
        }
    }
    else
    {
        ret = ERR_NO_MEMORY;
    }
    return ret;
}

int mailbox_dequeue_data(mailbox_t* mailbox, mailbox_data_t* data)
{
    int ret = 0;
    uint16_t size;
    uint16_t bufferSizeUsed;
    uint32_t temp;
    bufferSizeUsed = (((238 + mailbox->head) - mailbox->tail) % 238);
    if (bufferSizeUsed == 0)
    {
        ret = ERR_NO_MSG;
    }
    else
    {
        data->header = 0;
        temp = mailbox->buffer[mailbox->tail];
        data->header |= (temp << 24);
        mailbox->tail = (mailbox->tail + 1) % 238;
        temp = mailbox->buffer[mailbox->tail];
        data->header |= (temp << 16);
        mailbox->tail = (mailbox->tail + 1) % 238;
        temp = mailbox->buffer[mailbox->tail];
        data->header |= (temp << 8);
        mailbox->tail = (mailbox->tail + 1) % 238;
        temp = mailbox->buffer[mailbox->tail];
        size = temp;
        data->header |= (temp);
        mailbox->tail = (mailbox->tail + 1) % 238;
        for (int i = 0; i < size - 4; i++)
        {
            data->payload[i] = mailbox->buffer[mailbox->tail];
            mailbox->tail = (mailbox->tail + 1) % 238;
        }
    }
    return ret;
}

void printData(mailbox_data_t* data)
{
    printf("Header - %u, ", data->header);
    int size = data->header & 255;
    printf("Payload - ");
    for (int i = 0; i < size - 4; i++)
    {
        printf("%u, ", data->payload[i]);
    }
    printf("\n");
}