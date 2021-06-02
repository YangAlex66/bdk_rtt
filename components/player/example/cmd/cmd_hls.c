#include "player.h"
#include "hls_work.h"

static int hls_dump(void)
{
    hls_dump_music(); 
    return 0; 
}
MSH_CMD_EXPORT(hls_dump, dump player hls playing status.); 
