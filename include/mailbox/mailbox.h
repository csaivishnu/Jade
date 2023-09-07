#ifndef _MAILBOX_H_
#define _MAILBOX_H_

#include <stdint.h>

#define ERR_NO_MEMORY 1
#define ERR_NO_MSG 2

#ifdef __cplusplus
#define __BEGIN_CDECLS  extern "C" {
#define __END_CDECLS    }
#else
#define __BEGIN_CDECLS
#define __END_CDECLS
#endif


__BEGIN_CDECLS

typedef struct _mailbox_data_t
{
    uint32_t header;                 /* command/response header */
    uint8_t payload[10];             /* variable length payload */

} mailbox_data_t;

typedef struct _mailbox_t
{

    uint32_t IRQ_IN_SET;                       // 0-3 bytes
    uint32_t IRQ_OUT_SET;                      // 4-7 bytes
    uint32_t IRQ_IN_CLR;                       // 8-11 bytes
    uint32_t IRQ_OUT_CLR;                      // 12-15 bytes
    uint8_t head;                              // 16th byte
    uint8_t tail;                              // 17th byte
    uint8_t buffer[238];                       // 18-255 bytes

} mailbox_t;

void mailbox_init(mailbox_t* mailbox);
int mailbox_queue_data(mailbox_t* mailbox, const mailbox_data_t* data);
int mailbox_dequeue_data(mailbox_t* mailbox, mailbox_data_t* data);
void printData(mailbox_data_t* data);

__END_CDECLS


#endif