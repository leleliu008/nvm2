#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#include "core/sysinfo.h"
#include "core/self.h"
#include "nvm2.h"

#include <zlib.h>
#include <yaml.h>
#include <jansson.h>
#include <archive.h>
#include <curl/curlver.h>
#include <openssl/opensslv.h>

//#define PCRE2_CODE_UNIT_WIDTH 8
//#include <pcre2.h>


int nvm2_env(bool verbose) {
    printf("build.utctime: %s\n\n", NVM2_BUILD_TIMESTAMP);

    //printf("pcre2   : %d.%d\n", PCRE2_MAJOR, PCRE2_MINOR);
    printf("build.libyaml: %s\n", yaml_get_version_string());
    printf("build.libcurl: %s\n", LIBCURL_VERSION);

//https://www.openssl.org/docs/man3.0/man3/OPENSSL_VERSION_BUILD_METADATA.html
//https://www.openssl.org/docs/man1.1.1/man3/OPENSSL_VERSION_TEXT.html
#ifdef OPENSSL_VERSION_STR
    printf("build.openssl: %s\n", OPENSSL_VERSION_STR);
#else
    printf("build.openssl: %s\n", OPENSSL_VERSION_TEXT);
#endif

    printf("build.jansson: %s\n", JANSSON_VERSION);
    printf("build.archive: %s\n", ARCHIVE_VERSION_ONLY_STRING);
    printf("build.zlib:    %s\n\n", ZLIB_VERSION);

    SysInfo sysinfo = {0};

    if (sysinfo_make(&sysinfo) != 0) {
        perror(NULL);
        return NVM2_ERROR;
    }

    sysinfo_dump(sysinfo);
    sysinfo_free(sysinfo);

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return NVM2_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        return NVM2_ERROR_ENV_HOME_NOT_SET;
    }

    size_t   nvm2HomeDirLength = userHomeDirLength + 7;
    char     nvm2HomeDir[nvm2HomeDirLength];
    snprintf(nvm2HomeDir, nvm2HomeDirLength, "%s/.nvm2", userHomeDir);

    printf("\n");
    printf("nvm2.vers : %s\n", NVM2_VERSION);
    printf("nvm2.home : %s\n", nvm2HomeDir);

    char * selfRealPath = self_realpath();

    if (selfRealPath == NULL) {
        perror(NULL);
        return NVM2_ERROR;
    }

    printf("nvm2.path : %s\n", selfRealPath);

    free(selfRealPath);

    if (!verbose) {
        return NVM2_OK;
    }
   
    return NVM2_OK;
}
