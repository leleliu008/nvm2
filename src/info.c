#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <jansson.h>

#include "core/log.h"
#include "nvm2.h"

int nvm2_info(const char * versionName, const char * key) {
    int ret = nvm2_check_if_the_given_argument_matches_version_name_pattern(versionName);

    if (ret != NVM2_OK) {
        return ret;
    }

    if ((key == NULL) || (strcmp(key, "") == 0) || (strcmp(key, "--yaml") == 0)) {
        const char * const userHomeDir = getenv("HOME");

        if (userHomeDir == NULL) {
            return NVM2_ERROR_ENV_HOME_NOT_SET;
        }

        size_t userHomeDirLength = strlen(userHomeDir);

        if (userHomeDirLength == 0U) {
            return NVM2_ERROR_ENV_HOME_NOT_SET;
        }

        ////////////////////////////////////////////////////////////////

        size_t   versionsInstalledDirLength = userHomeDirLength + strlen(versionName) + 17U;
        char     versionsInstalledDir[versionsInstalledDirLength];
        snprintf(versionsInstalledDir, versionsInstalledDirLength, "%s/.nvm2/versions/%s", userHomeDir, versionName);

        ////////////////////////////////////////////////////////////////

        NVM2Receipt * receipt = NULL;

        int ret = nvm2_receipt_parse(versionName, &receipt);

        if (ret != NVM2_OK) {
            return ret;
        }

        if (isatty(STDOUT_FILENO)) {
            printf("version:   %s%s%s\n", COLOR_GREEN, versionName, COLOR_OFF);
        } else {
            printf("version:   %s\n", versionName);
        }

        printf("location:  %s\n", versionsInstalledDir);

        if (receipt->sourceurl != NULL) {
            printf("sourceurl: %s\n", receipt->sourceurl);
        }

        if (receipt->binaryurl != NULL) {
            printf("binaryurl: %s\n", receipt->binaryurl);
        }

        printf("sha256sum: %s\n", receipt->sha256sum);
        printf("signature: nvm2-%s\n", receipt->signature);

        time_t tt = (time_t)atol(receipt->timestamp);
        struct tm *tms = localtime(&tt);

        char buff[26] = {0};
        strftime(buff, 26, "%Y-%m-%d %H:%M:%S%z", tms);

        buff[24] = buff[23];
        buff[23] = buff[22];
        buff[22] = ':';

        printf("timestamp: %s\n", buff);

        nvm2_receipt_free(receipt);
    } else if (strcmp(key, "--json") == 0) {
        NVM2Receipt * receipt = NULL;

        int ret = nvm2_receipt_parse(versionName, &receipt);

        if (ret != NVM2_OK) {
            return ret;
        }

        json_t * root = json_object();

        json_object_set_new(root, "version", json_string(versionName));
        json_object_set_new(root, "sourceurl", json_string(receipt->sourceurl));
        json_object_set_new(root, "binaryurl", json_string(receipt->binaryurl));
        json_object_set_new(root, "sha256sum", json_string(receipt->sha256sum));
        json_object_set_new(root, "signature", json_string(receipt->signature));
        json_object_set_new(root, "timestamp", json_string(receipt->timestamp));

        char * jsonStr = json_dumps(root, 0);

        if (jsonStr == NULL) {
            ret = NVM2_ERROR;
        } else {
            printf("%s\n", jsonStr);
            free(jsonStr);
        }

        json_decref(root);

        nvm2_receipt_free(receipt);
    } else if (strcmp(key, "installed-timestamp-unix") == 0) {
        NVM2Receipt * receipt = NULL;

        int ret = nvm2_receipt_parse(versionName, &receipt);

        if (ret != NVM2_OK) {
            return ret;
        }

        printf("%s\n", receipt->timestamp);

        nvm2_receipt_free(receipt);
    } else if (strcmp(key, "installed-timestamp-rfc-3339") == 0) {
        NVM2Receipt * receipt = NULL;

        int ret = nvm2_receipt_parse(versionName, &receipt);

        if (ret != NVM2_OK) {
            return ret;
        }

        time_t tt = (time_t)atol(receipt->timestamp);
        struct tm *tms = localtime(&tt);

        char buff[26] = {0};
        strftime(buff, 26, "%Y-%m-%d %H:%M:%S%z", tms);

        buff[24] = buff[23];
        buff[23] = buff[22];
        buff[22] = ':';

        printf("%s\n", buff);

        nvm2_receipt_free(receipt);
    } else if (strcmp(key, "installed-timestamp-iso-8601") == 0) {
        NVM2Receipt * receipt = NULL;

        int ret = nvm2_receipt_parse(versionName, &receipt);

        if (ret != NVM2_OK) {
            return ret;
        }

        time_t tt = (time_t)atol(receipt->timestamp);
        struct tm *tms = localtime(&tt);

        char buff[26] = {0};
        strftime(buff, 26, "%Y-%m-%dT%H:%M:%S%z", tms);

        buff[24] = buff[23];
        buff[23] = buff[22];
        buff[22] = ':';

        printf("%s\n", buff);

        nvm2_receipt_free(receipt);
    } else {
        return NVM2_ERROR_ARG_IS_UNKNOWN;
    }

    return ret;
}
