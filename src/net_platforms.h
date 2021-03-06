#pragma once
/*

   net_platforms.h : helper header file which just bring physical network type to source code 

*/

#include "../cfg_basic.h"

#if defined(NETWORK_ETHERNET_W5100)
   #define NETWORK_ETHERNET_WIZNET
   #define PHY_MODULE_NAME                                   "W5100"
#elif defined(NETWORK_ETHERNET_W5500)
   #define NETWORK_ETHERNET_WIZNET
   #define PHY_MODULE_NAME                                   "W5500"
#elif defined(NETWORK_ETHERNET_ENC28J60)
   #define PHY_MODULE_NAME                                   "ENC28J60"
#elif defined(NETWORK_SERIAL_INTERFACE)
   #define PHY_MODULE_NAME                                   "UART"
#endif

