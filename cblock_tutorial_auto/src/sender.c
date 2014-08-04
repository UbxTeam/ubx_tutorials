#include "sender.h"

/* define a structure for holding the block local state. By assigning an
 * instance of this struct to the block private_data pointer (see init), this
 * information becomes accessible within the hook functions.
 */
struct sender_info
{
        /* add custom block local data here */

        /* this is to have fast access to ports for reading and writing, without
         * needing a hash table lookup */
        struct sender_port_cache ports;
};

/* init */
int sender_init(ubx_block_t *b)
{
        int ret = -1;
        struct sender_info *inf;

        /* allocate memory for the block local state */
        if ((inf = (struct sender_info*)calloc(1, sizeof(struct sender_info)))==NULL) {
                ERR("sender: failed to alloc memory");
                ret=EOUTOFMEM;
                goto out;
        }
        b->private_data=inf;
        update_port_cache(b, &inf->ports);
        ret=0;
out:
        return ret;
}

/* start */
int sender_start(ubx_block_t *b)
{
        /* struct sender_info *inf = (struct sender_info*) b->private_data; */
        int ret = 0;
        return ret;
}

/* stop */
void sender_stop(ubx_block_t *b)
{
        /* struct sender_info *inf = (struct sender_info*) b->private_data; */
}

/* cleanup */
void sender_cleanup(ubx_block_t *b)
{
        free(b->private_data);
}

/* step */
void sender_step(ubx_block_t *b)
{
  struct sender_info *inf = (struct sender_info*) b->private_data;
  struct my_data data;
  strcpy(data.model, "image array");
  strcpy(data.uid,"image");
  strcpy(data.meta_model,"c array");
  strcpy(data.data,"test_data");

  write_output(inf->ports.output, &data);
}

