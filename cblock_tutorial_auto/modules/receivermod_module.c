/*
 * module function declaration for library receivermod header (autogenerated)
 */

#include "receivermod_module.h"

int receivermod_mod_init(ubx_node_info_t* ni)
{
  int ret;
  ret=receiver_mod_init(ni);
  if(ret!=0)
    goto out;

out:
   return ret;
}

void receivermod_mod_cleanup(ubx_node_info_t* ni)
{
  receiver_mod_cleanup(ni);
}

