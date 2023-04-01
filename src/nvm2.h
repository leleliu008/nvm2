#ifndef NVM2_H
#define NVM2_H

#include <config.h>
#include <stdlib.h>
#include <stdbool.h>


#define NVM2_VERSION_NAME_PATTERN "^[0-9][0-9]?[.][0-9][0-9]?[.][0-9][0-9]?$"


#define NVM2_OK                     0
#define NVM2_ERROR                  1

#define NVM2_ERROR_ARG_IS_NULL      2
#define NVM2_ERROR_ARG_IS_EMPTY     3
#define NVM2_ERROR_ARG_IS_INVALID   4
#define NVM2_ERROR_ARG_IS_UNKNOWN   5

#define NVM2_ERROR_MEMORY_ALLOCATE  6

#define NVM2_ERROR_SHA256_MISMATCH  7

#define NVM2_ERROR_ENV_HOME_NOT_SET 8
#define NVM2_ERROR_ENV_PATH_NOT_SET 9

#define NVM2_ERROR_EXE_NOT_FOUND    10

#define NVM2_ERROR_VERSION_NOT_AVAILABLE 20
#define NVM2_ERROR_VERSION_NOT_INSTALLED 21
#define NVM2_ERROR_VERSION_NOT_OUTDATED  22
#define NVM2_ERROR_VERSION_IS_BROKEN     23

#define NVM2_ERROR_FORMULA_REPO_NOT_FOUND 30
#define NVM2_ERROR_FORMULA_REPO_HAS_EXIST 31
#define NVM2_ERROR_FORMULA_REPO_IS_BROKEN 32
#define NVM2_ERROR_FORMULA_REPO_CONFIG_SYNTAX 34
#define NVM2_ERROR_FORMULA_REPO_CONFIG_SCHEME 35

#define NVM2_ERROR_FORMULA_SYNTAX     40
#define NVM2_ERROR_FORMULA_SCHEME     41

#define NVM2_ERROR_RECEIPT_SYNTAX     45
#define NVM2_ERROR_RECEIPT_SCHEME     46

#define NVM2_ERROR_URL_TRANSFORM_ENV_NOT_SET           50
#define NVM2_ERROR_URL_TRANSFORM_ENV_VALUE_IS_EMPTY    51
#define NVM2_ERROR_URL_TRANSFORM_ENV_POINT_TO_PATH_NOT_EXIST 52
#define NVM2_ERROR_URL_TRANSFORM_RUN_NO_RESULT         53

// libgit's error [-35, -1]
#define NVM2_ERROR_LIBGIT2_BASE    70

// libarchive's error [-30, 1]
#define NVM2_ERROR_ARCHIVE_BASE    110

// libcurl's error [1, 99]
#define NVM2_ERROR_NETWORK_BASE    150

/*
 * This macro should be employed only if there is no memory should be freed before returing.
 */
#define NVM2_RETURN_IF_MEMORY_ALLOCATION_FAILED(ptr) if ((ptr) == NULL) { return NVM2_ERROR_MEMORY_ALLOCATE; }

#define NVM2_PERROR(ret, versionName, ...) \
    if (ret == NVM2_ERROR) { \
        fprintf(stderr, "occurs error.\n"); \
    } else if (ret == NVM2_ERROR_ARG_IS_NULL) { \
        fprintf(stderr, "version name not specified.\n"); \
    } else if (ret == NVM2_ERROR_ARG_IS_EMPTY) { \
        fprintf(stderr, "version name should be a non-empty string.\n"); \
    } else if (ret == NVM2_ERROR_ARG_IS_INVALID) { \
        fprintf(stderr, "version name not match pattern: %s, %s\n", versionName, NVM2_VERSION_NAME_PATTERN); \
    } else if (ret == NVM2_ERROR_VERSION_NOT_AVAILABLE) { \
        fprintf(stderr, "version not available: %s\n", versionName); \
    } else if (ret == NVM2_ERROR_VERSION_NOT_INSTALLED) { \
        fprintf(stderr, "version not installed: %s\n", versionName); \
    } else if (ret == NVM2_ERROR_VERSION_NOT_OUTDATED) { \
        fprintf(stderr, "version not outdated: %s\n", versionName); \
    } else if (ret == NVM2_ERROR_VERSION_IS_BROKEN) { \
        fprintf(stderr, "version is broken: %s\n", versionName); \
    } else if (ret == NVM2_ERROR_ENV_HOME_NOT_SET) { \
        fprintf(stderr, "%s\n", "HOME environment variable not set.\n"); \
    } else if (ret == NVM2_ERROR_ENV_PATH_NOT_SET) { \
        fprintf(stderr, "%s\n", "PATH environment variable not set.\n"); \
    } else if (ret == NVM2_ERROR_URL_TRANSFORM_ENV_NOT_SET) { \
        fprintf(stderr, "%s\n", "NVM2_URL_TRANSFORM environment variable not set.\n"); \
    } else if (ret == NVM2_ERROR_URL_TRANSFORM_ENV_VALUE_IS_EMPTY) { \
        fprintf(stderr, "%s\n", "NVM2_URL_TRANSFORM environment variable's value should be a non-empty string.\n"); \
    } else if (ret == NVM2_ERROR_URL_TRANSFORM_ENV_POINT_TO_PATH_NOT_EXIST) { \
        fprintf(stderr, "%s\n", "NVM2_URL_TRANSFORM environment variable's value point to path not exist.\n"); \
    } else if (ret == NVM2_ERROR_URL_TRANSFORM_RUN_NO_RESULT) { \
        fprintf(stderr, "%s\n", "NVM2_URL_TRANSFORM environment variable's value point to path runs no result.\n"); \
    } else if (ret > NVM2_ERROR_NETWORK_BASE) { \
        fprintf(stderr, "network error.\n"); \
    }

//////////////////////////////////////////////////////////////////////

typedef struct {
    char * sourceurl;
    char * binaryurl;
    char * sha256sum;

    char * signature;
    char * timestamp;
} NVM2Receipt;

int  nvm2_receipt_parse(const char * versionName, NVM2Receipt * * receipt);
void nvm2_receipt_free(NVM2Receipt * receipt);
void nvm2_receipt_dump(NVM2Receipt * receipt);

//////////////////////////////////////////////////////////////////////

int nvm2_main(int argc, char* argv[]);

int nvm2_util(int argc, char* argv[]);

int nvm2_help();

int nvm2_sysinfo();

int nvm2_env(bool verbose);

int nvm2_info(const char * versionName, const char * key);

int nvm2_tree(const char * versionName, size_t argc, char* argv[]);

int nvm2_exec(const char * versionName, size_t argc, char* argv[]);

int nvm2_prefix(const char * versionName);

typedef enum {
    NVM2LogLevel_silent,
    NVM2LogLevel_normal,
    NVM2LogLevel_verbose,
    NVM2LogLevel_very_verbose
} NVM2LogLevel;

int nvm2_install(const char * versionName, const char * siteUrl, bool fromSource, NVM2LogLevel logLevel, unsigned int jobs);

int nvm2_reinstall(const char * versionName, const char * siteUrl, bool fromSource, NVM2LogLevel logLevel, unsigned int jobs);

int nvm2_uninstall(const char * versionName, bool verbose);

int nvm2_upgrade_self(bool verbose);

int nvm2_integrate_zsh_completion (const char * outputDir, bool verbose);
int nvm2_integrate_bash_completion(const char * outputDir, bool verbose);
int nvm2_integrate_fish_completion(const char * outputDir, bool verbose);

int nvm2_cleanup(bool verbose);

int nvm2_check_if_the_given_argument_matches_version_name_pattern(const char * arg);

int nvm2_check_if_the_given_version_is_available(const char * versionName);
int nvm2_check_if_the_given_version_is_installed(const char * versionName);
int nvm2_check_if_the_given_version_is_outdated (const char * versionName);

int nvm2_list_the_available_versions(bool verbose, bool onlyLTS);
int nvm2_list_the_installed_versions(bool verbose);

int nvm2_show_release_schedule(bool verbose);

int nvm2_http_fetch_to_file(const char * url, const char * outputFilePath, bool verbose, bool showProgress);

#endif
