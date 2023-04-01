#include <stdio.h>
#include <string.h>
#include <jansson.h>
#include <sys/stat.h>

#include "nvm2.h"

int nvm2_list_the_available_versions(bool verbose, bool onlyLTS) {
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

    int ret = nvm2_http_fetch_to_file(indexJsonUrl, indexJsonFilePath, verbose, verbose);

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

        json_t * jsonObjectlts = json_object_get(jsonObject, "lts");

        bool isLTS = false;

        if(jsonObjectlts == NULL) {
            isLTS = false;
        } else if(json_is_boolean(jsonObjectlts)) {
            isLTS = json_boolean_value(jsonObjectlts);
        } else {
            isLTS = true;
        }

        //////////////////////////////////////////////////////////////////////////////////

        if (onlyLTS) {
            if (!isLTS) {
                continue;
            }
        }

        printf("%s %s\n", isLTS ? "*" : " ", &json_string_value(jsonObjectversion)[1]);
    }

finalize:
    json_decref(jsonRoot);
    return ret;
}
