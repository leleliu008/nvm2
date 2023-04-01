#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <jansson.h>

#include "core/regex/regex.h"

#include "nvm2.h"

int nvm2_check_if_the_given_argument_matches_version_name_pattern(const char * arg) {
    if (arg == NULL) {
        return NVM2_ERROR_ARG_IS_NULL;
    }

    if (strcmp(arg, "") == 0) {
        return NVM2_ERROR_ARG_IS_EMPTY;
    }

    if (regex_matched(arg, NVM2_VERSION_NAME_PATTERN) == 0) {
        return NVM2_OK;
    } else {
        if (errno == 0) {
            return NVM2_ERROR_ARG_IS_INVALID;
        } else {
            perror(NULL);
            return NVM2_ERROR;
        }
    }
}

int nvm2_check_if_the_given_version_is_available(const char * versionName) {
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

    size_t   nvm2TmpDirLength = nvm2HomeDirLength + 5U;
    char     nvm2TmpDir[nvm2TmpDirLength];
    snprintf(nvm2TmpDir, nvm2TmpDirLength, "%s/tmp", nvm2HomeDir);

    if (stat(nvm2TmpDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", nvm2TmpDir);
            return NVM2_ERROR;
        }
    } else {
        if (mkdir(nvm2TmpDir, S_IRWXU) != 0) {
            perror(nvm2TmpDir);
            return NVM2_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    const char * const indexJsonUrl = "https://nodejs.org/download/release/index.json";

    size_t   indexJsonFilePathLength = nvm2TmpDirLength + 13U;
    char     indexJsonFilePath[indexJsonFilePathLength];
    snprintf(indexJsonFilePath, indexJsonFilePathLength, "%s/index.json", nvm2TmpDir);

    int ret = nvm2_http_fetch_to_file(indexJsonUrl, indexJsonFilePath, false, false);

    if (ret != NVM2_OK) {
        return ret;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    json_error_t jsonError;

    json_t * jsonRoot = json_load_file(indexJsonFilePath, 0, &jsonError);

    if (jsonRoot == NULL) {
        fprintf(stderr, "json syntax error. file: %s line: %d reason: %s\n", indexJsonFilePath, jsonError.line, jsonError.text);
        return NVM2_ERROR;
    }

    if (!json_is_array(jsonRoot)) {
        fprintf(stderr, "json scheme error. file: %s , json root was expected to be an array, but it was not.\n", indexJsonFilePath);
        json_decref(jsonRoot);
        return NVM2_ERROR;
    }

    size_t size = json_array_size(jsonRoot);

    for (size_t i = 0U; i < size; i++) {
        json_t * jsonObject = json_array_get(jsonRoot, i);

        if(!json_is_object(jsonObject)) {
            fprintf(stderr, "json scheme error. file %s , the %luth json array element was expected to be an object, but it was not.\n", indexJsonFilePath, i);
            ret = NVM2_ERROR;
            goto finalize;
        }

        //////////////////////////////////////////////////////////////////////////////////

        json_t * jsonObjectversion = json_object_get(jsonObject, "version");

        if(jsonObjectversion == NULL) {
            fprintf(stderr, "json scheme error: file %s , 'version' field not found in the %luth json array element.\n", indexJsonFilePath, i);
            ret = NVM2_ERROR;
            goto finalize;
        }

        if(!json_is_string(jsonObjectversion)) {
            fprintf(stderr, "json scheme error: file %s , 'version' field's value was expected to be a string, but it was not.\n", indexJsonFilePath);
            ret = NVM2_ERROR;
            goto finalize;
        }

        //////////////////////////////////////////////////////////////////////////////////

        if (strcmp(versionName, &json_string_value(jsonObjectversion)[1]) == 0) {
            goto finalize;
        }
    }

    ret = NVM2_ERROR_VERSION_NOT_AVAILABLE;

finalize:
    json_decref(jsonRoot);
    return ret;
}

int nvm2_check_if_the_given_version_is_installed(const char * versionName) {
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

    struct stat st;

    size_t   nvm2HomeDirLength = userHomeDirLength + 7U;
    char     nvm2HomeDir[nvm2HomeDirLength];
    snprintf(nvm2HomeDir, nvm2HomeDirLength, "%s/.nvm2", userHomeDir);

    size_t   versionInstalledDirLength = nvm2HomeDirLength + strlen(versionName) + 20U;
    char     versionInstalledDir[versionInstalledDirLength];
    snprintf(versionInstalledDir, versionInstalledDirLength, "%s/versions/%s", nvm2HomeDir, versionName);

    if (stat(versionInstalledDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", versionInstalledDir);
            return NVM2_ERROR;
        }
    } else {
        return NVM2_ERROR_VERSION_NOT_INSTALLED;
    }

    size_t   receiptFilePathLength = nvm2HomeDirLength + versionInstalledDirLength + 19U;
    char     receiptFilePath[receiptFilePathLength];
    snprintf(receiptFilePath, receiptFilePathLength, "%s/receipt.yml", versionInstalledDir);

    if (stat(receiptFilePath, &st) == 0) {
        if (S_ISREG(st.st_mode)) {
            return NVM2_OK;
        } else {
            return NVM2_ERROR_VERSION_IS_BROKEN;
        }
    } else {
        return NVM2_ERROR_VERSION_IS_BROKEN;
    }
}
