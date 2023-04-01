#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "nvm2.h"

int nvm2_exec(const char * versionName, size_t argc, char* argv[]) {
    if (argc == 0) {
        return NVM2_ERROR_ENV_HOME_NOT_SET;
    }

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

    size_t   versionInstalledDirLength = userHomeDirLength + strlen(versionName) + 20U;
    char     versionInstalledDir[versionInstalledDirLength];
    snprintf(versionInstalledDir, versionInstalledDirLength, "%s/.nvm2/versions/%s", userHomeDir, versionName);

    struct stat st;

    if (stat(versionInstalledDir, &st) != 0) {
        return NVM2_ERROR_VERSION_NOT_INSTALLED;
    }

    size_t   receiptFilePathLength = versionInstalledDirLength + 20U;
    char     receiptFilePath[receiptFilePathLength];
    snprintf(receiptFilePath, receiptFilePathLength, "%s/receipt.yml", versionInstalledDir);

    if (stat(receiptFilePath, &st) != 0 || (!S_ISREG(st.st_mode))) {
        return NVM2_ERROR_VERSION_IS_BROKEN;
    }

    size_t   cmdPathLength = versionInstalledDirLength + strlen(argv[0]) + 6U;
    char     cmdPath[cmdPathLength];
    snprintf(cmdPath, cmdPathLength, "%s/bin/%s", versionInstalledDir, argv[0]);

    argv[0] = cmdPath;

    execv(cmdPath, argv);

    perror(cmdPath);

    return NVM2_ERROR;
}
