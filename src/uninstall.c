#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "core/rm-r.h"

#include "nvm2.h"

int nvm2_uninstall(const char * versionName, bool verbose) {
    int ret = nvm2_check_if_the_given_argument_matches_version_name_pattern(versionName);

    if (ret != NVM2_OK) {
        return ret;
    }

    char * userHomeDir = getenv("HOME");

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

    struct stat st;

    if (stat(versionInstalledDir, &st) != 0) {
        return NVM2_ERROR_VERSION_NOT_INSTALLED;
    }

    size_t   receiptFilePathLength = versionInstalledDirLength + 14U;
    char     receiptFilePath[receiptFilePathLength];
    snprintf(receiptFilePath, receiptFilePathLength, "%s/receipt.yml", versionInstalledDir);

    if (stat(receiptFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
        if (rm_r(versionInstalledDir, verbose) == 0) {
            return NVM2_OK;
        } else {
            perror(versionInstalledDir);
            return NVM2_ERROR;
        }
    } else {
        // version is broken. is not installed completely?
        return NVM2_ERROR_VERSION_NOT_INSTALLED;
    }
}
