#include <stdio.h>
#include <string.h>
#include <jansson.h>
#include <sys/stat.h>

#include "nvm2.h"

int nvm2_show_release_schedule(bool verbose) {
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

    const char * const scheduleJsonUrl = "https://raw.githubusercontent.com/nodejs/Release/main/schedule.json";

    size_t   scheduleJsonFilePathLength = nvm2TmpDirLength + 13U;
    char     scheduleJsonFilePath[scheduleJsonFilePathLength];
    snprintf(scheduleJsonFilePath, scheduleJsonFilePathLength, "%s/nodejs-release-schedule.json", nvm2TmpDir);

    int ret = nvm2_http_fetch_to_file(scheduleJsonUrl, scheduleJsonFilePath, verbose, verbose);

    if (ret != NVM2_OK) {
        return ret;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    json_error_t jsonError;

    json_t * jsonRoot = json_load_file(scheduleJsonFilePath, 0, &jsonError);

    if (jsonRoot == NULL) {
        fprintf(stderr, "syntax error: on line %d: %s\n", jsonError.line, jsonError.text);
        return NVM2_ERROR;
    }

    if (!json_is_object(jsonRoot)) {
        fprintf(stderr, "syntax error: json root was expected to be an object, but it was not.\n");
        json_decref(jsonRoot);
        return NVM2_ERROR;
    }

    for (int i = 1; ; i++) {
        size_t   jsonObjectKeyLength = 12U;
        char     jsonObjectKey[jsonObjectKeyLength];

        switch (i) {
            case 1:
                snprintf(jsonObjectKey, jsonObjectKeyLength, "v0.8");
                break;
            case 2:
                snprintf(jsonObjectKey, jsonObjectKeyLength, "v0.10");
                break;
            case 3:
                snprintf(jsonObjectKey, jsonObjectKeyLength, "v0.12");
                break;
            default:
                snprintf(jsonObjectKey, jsonObjectKeyLength, "v%d", i);
        }

        json_t * jsonObject = json_object_get(jsonRoot, jsonObjectKey);

        if(NULL == jsonObject) {
            break;
        }

        if(json_is_null(jsonObject)) {
            break;
        }

        if(!json_is_object(jsonObject)) {
            fprintf(stderr, "syntax error: '%s' field's value was expected to be an object, but it was not.\n", jsonObjectKey);
            ret = NVM2_ERROR;
            goto finalize;
        }

        json_t * jsonObjectcodename = json_object_get(jsonObject, "codename");

        const char * codename;

        if (jsonObjectcodename == NULL) {
            codename = "";
        } else if (json_is_null(jsonObjectcodename)) {
            codename = "";
        } else if(json_is_string(jsonObjectcodename)) {
            codename = json_string_value(jsonObjectcodename);
        } else {
            fprintf(stderr, "syntax error: 'codename' field's value was expected to be a string, but it was not.\n");
            ret = NVM2_ERROR;
            goto finalize;
        }

        json_t * jsonObjectstart = json_object_get(jsonObject, "start");

        if(!json_is_string(jsonObjectstart)) {
            fprintf(stderr, "syntax error: 'start' field's value was expected to be a string, but it was not.\n");
            ret = NVM2_ERROR;
            goto finalize;
        }

        json_t * jsonObjectend = json_object_get(jsonObject, "end");

        if(!json_is_string(jsonObjectend)) {
            fprintf(stderr, "syntax error: 'end' field's value was expected to be a string, but it was not.\n");
            ret = NVM2_ERROR;
            goto finalize;
        }

        printf("%-5s  name:%-10s  start:%s  end:%s\n", jsonObjectKey, codename, json_string_value(jsonObjectstart), json_string_value(jsonObjectend));
    }

finalize:
    json_decref(jsonRoot);
    return ret;
}
