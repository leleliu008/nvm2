#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "core/http.h"
#include "nvm2.h"

int nvm2_integrate_zsh_completion(const char * outputDir, bool verbose) {
    const char * url = "https://raw.githubusercontent.com/leleliu008/nvm2/master/nvm2-zsh-completion";

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return NVM2_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0U) {
        return NVM2_ERROR_ENV_HOME_NOT_SET;
    }

    ////////////////////////////////////////////////////////////////

    struct stat st;

    size_t   nvm2HomeDirLength = userHomeDirLength + 7U;
    char     nvm2HomeDir[nvm2HomeDirLength];
    snprintf(nvm2HomeDir, nvm2HomeDirLength, "%s/.nvm2", userHomeDir);

    if (stat(nvm2HomeDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", nvm2HomeDir);
            return NVM2_ERROR;
        }
    } else {
        if (mkdir(nvm2HomeDir, S_IRWXU) != 0) {
            perror(nvm2HomeDir);
            return NVM2_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////

    size_t   zshCompletionDirLength = nvm2HomeDirLength + 16U;
    char     zshCompletionDir[zshCompletionDirLength];
    snprintf(zshCompletionDir, zshCompletionDirLength, "%s/zsh_completion", nvm2HomeDir);

    if (stat(zshCompletionDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", zshCompletionDir);
            return NVM2_ERROR;
        }
    } else {
        if (mkdir(zshCompletionDir, S_IRWXU) != 0) {
            perror(zshCompletionDir);
            return NVM2_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////

    size_t   zshCompletionFilePathLength = zshCompletionDirLength + 7U;
    char     zshCompletionFilePath[zshCompletionFilePathLength];
    snprintf(zshCompletionFilePath, zshCompletionFilePathLength, "%s/_nvm2", zshCompletionDir);

    int ret = http_fetch_to_file(url, zshCompletionFilePath, verbose, verbose);

    if (ret != NVM2_OK) {
        return ret;
    }

    if (outputDir == NULL) {
        return NVM2_OK;
    }

    if (stat(outputDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", outputDir);
            return NVM2_ERROR;
        }
    } else {
        fprintf(stderr, "'%s\n' directory was expected to be exist, but it was not.\n", outputDir);
        return NVM2_ERROR;
    }

    size_t   destFilePathLength = strlen(outputDir) + 7U;
    char     destFilePath[destFilePathLength];
    snprintf(destFilePath, destFilePathLength, "%s/_nvm2", outputDir);

    if (symlink(zshCompletionFilePath, destFilePath) != 0) {
        perror(destFilePath);
        return NVM2_ERROR;
    } else {
        return NVM2_OK;
    }
}

int nvm2_integrate_bash_completion(const char * outputDir, bool verbose) {
    (void)outputDir;
    (void)verbose;
    return NVM2_OK;
}

int nvm2_integrate_fish_completion(const char * outputDir, bool verbose) {
    (void)outputDir;
    (void)verbose;
    return NVM2_OK;
}
