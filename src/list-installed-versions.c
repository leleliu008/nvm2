#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#include "core/log.h"

#include "nvm2.h"

int nvm2_list_the_installed_versions(bool verbose) {
    const char * const userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return NVM2_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0U) {
        return NVM2_ERROR_ENV_HOME_NOT_SET;
    }

    ////////////////////////////////////////////////////////////////

    struct stat st;

    size_t   versionsInstalledRootDirLength = userHomeDirLength + 17U; 
    char     versionsInstalledRootDir[versionsInstalledRootDirLength];
    snprintf(versionsInstalledRootDir, versionsInstalledRootDirLength, "%s/.nvm2/versions", userHomeDir);

    if (stat(versionsInstalledRootDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", versionsInstalledRootDir);
            return NVM2_OK;
        }
    } else {
        return NVM2_OK;
    }

    ////////////////////////////////////////////////////////////////


    DIR * dir = opendir(versionsInstalledRootDir);

    if (dir == NULL) {
        perror(versionsInstalledRootDir);
        return NVM2_ERROR;
    }

    for (;;) {
        errno = 0;

        struct dirent * dir_entry = readdir(dir);

        if (dir_entry == NULL) {
            if (errno == 0) {
                closedir(dir);
                break;
            } else {
                perror(versionsInstalledRootDir);
                closedir(dir);
                return NVM2_ERROR;
            }
        }

        if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
            continue;
        }

        size_t   receiptFilePathLength = versionsInstalledRootDirLength + + strlen(dir_entry->d_name) + 14U;
        char     receiptFilePath[receiptFilePathLength];
        snprintf(receiptFilePath, receiptFilePathLength, "%s/%s/receipt.yml", versionsInstalledRootDir, dir_entry->d_name);

        if (stat(receiptFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
            if (verbose) {
                int ret = nvm2_info(dir_entry->d_name, NULL);

                if (ret != NVM2_OK) {
                    closedir(dir);
                    return ret;
                }
            } else {
                printf("%s\n", dir_entry->d_name);
            }
        }
    }

    return NVM2_OK;
}
