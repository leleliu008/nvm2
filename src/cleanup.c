#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "core/rm-r.h"

#include "nvm2.h"

int nvm2_cleanup(bool verbose) {
    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return NVM2_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        return NVM2_ERROR_ENV_HOME_NOT_SET;
    }

    ////////////////////////////////////////////////////////////////

    struct stat st;

    size_t   nvm2TmpDirLength = userHomeDirLength + 11;
    char     nvm2TmpDir[nvm2TmpDirLength];
    snprintf(nvm2TmpDir, nvm2TmpDirLength, "%s/.nvm2/tmp", userHomeDir);

    if (stat(nvm2TmpDir, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            if (rm_r(nvm2TmpDir, verbose) == 0) {
                return NVM2_OK;
            } else {
                perror(nvm2TmpDir);
                return NVM2_ERROR;
            }
        } else {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", nvm2TmpDir);
            return NVM2_ERROR;
        }
    } else {
        return NVM2_OK;
    }
}
