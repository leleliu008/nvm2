#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "nvm2.h"

int nvm2_prefix(const char * versionName) {
    int ret = nvm2_check_if_the_given_argument_matches_version_name_pattern(versionName);

    if (ret != NVM2_OK) {
        return ret;
    }

    const char * const userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return NVM2_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0U) {
        return NVM2_ERROR_ENV_HOME_NOT_SET;
    }

    size_t   versionInstalledDirLength = userHomeDirLength + strlen(versionName) + 17U;
    char     versionInstalledDir[versionInstalledDirLength];
    snprintf(versionInstalledDir, versionInstalledDirLength, "%s/.nvm2/versions/%s", userHomeDir, versionName);

    printf("%s\n", versionInstalledDir);

    return NVM2_OK;
}
