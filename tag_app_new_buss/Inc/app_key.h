
#ifndef __APP_KEY_H__
#define	__APP_KEY_H__

#include "app_config.h"
typedef struct key_function
{
  void (*func_cmd)(uint8 key_n);
}KEY_FUNC;


typedef enum 
{
  KEY_NORMAL = 0,
  KEY_WAIT   = 1,
	KEY_PANDIAN = 2
}KEY_StateDef;



#endif
