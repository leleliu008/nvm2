#include <stdio.h>
#include <string.h>

#include "core/http.h"

#include "nvm2.h"

int nvm2_http_fetch_to_file(const char * url, const char * outputFilePath, bool verbose, bool showProgress) {
    int ret = http_fetch_to_file(url, outputFilePath, verbose, showProgress);

    if (ret == -1) {
        perror(outputFilePath);
        return NVM2_ERROR;
    }

    if (ret > 0) {
        return NVM2_ERROR_NETWORK_BASE + ret;
    }

    return NVM2_OK;
}
