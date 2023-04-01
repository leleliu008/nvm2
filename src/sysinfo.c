#include <stdio.h>
#include "core/sysinfo.h"
#include "nvm2.h"

int nvm2_sysinfo() {
    SysInfo sysinfo = {0};

    if (sysinfo_make(&sysinfo) != 0) {
        perror(NULL);
        return NVM2_ERROR;
    }

    sysinfo_dump(sysinfo);
    sysinfo_free(sysinfo);
   
    return NVM2_OK;
}
