#ifndef _PROCESS_COMMAND_H
#define _PROCESS_COMMAND_H

#include "../mailbox/mailbox.h"

#ifdef __cplusplus
#define __BEGIN_CDECLS  extern "C" {
#define __END_CDECLS    }
#else
#define __BEGIN_CDECLS
#define __END_CDECLS
#endif


__BEGIN_CDECLS

mailbox_data_t* processCommand(mailbox_data_t* command);

__END_CDECLS


#endif