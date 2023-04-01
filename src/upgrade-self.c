#include <math.h>
#include <string.h>
#include <sys/stat.h>

#include "core/sysinfo.h"
#include "core/self.h"
#include "core/tar.h"
#include "core/log.h"
#include "core/cp.h"

#include "nvm2.h"

int nvm2_upgrade_self(bool verbose) {
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

    const char * githubApiUrl = "https://api.github.com/repos/leleliu008/nvm2/releases/latest";

    size_t   githubApiResultJsonFilePathLength = nvm2TmpDirLength + 13U;
    char     githubApiResultJsonFilePath[githubApiResultJsonFilePathLength];
    snprintf(githubApiResultJsonFilePath, githubApiResultJsonFilePathLength, "%s/latest.json", nvm2TmpDir);

    int ret = nvm2_http_fetch_to_file(githubApiUrl, githubApiResultJsonFilePath, verbose, verbose);

    if (ret != NVM2_OK) {
        return ret;
    }

    FILE * file = fopen(githubApiResultJsonFilePath, "r");

    if (file == NULL) {
        perror(githubApiResultJsonFilePath);
        return NVM2_ERROR;
    }

    char * latestVersion = NULL;

    char * p = NULL;

    char line[30];

    for (;;) {
        p = fgets(line, 30, file);

        if (p == NULL) {
            if (ferror(file)) {
                perror(githubApiResultJsonFilePath);
                goto finalize;
            } else {
                break;
            }
        }

        for (;;) {
            if (p[0] <= 32) { // non-printable ASCII characters and space
                p++;
            } else {
                break;
            }
        }

        if (strncmp(p, "\"tag_name\"", 10) == 0) {
            p += 10;

            for (;;) {
                if (p[0] == '\0') {
                    fprintf(stderr, "%s return invalid json.\n", githubApiUrl);
                    return NVM2_ERROR;
                }

                if (p[0] == '"') { // found left double quote
                    p++;
                    break;
                } else {
                    p++;
                }
            }

            size_t n = 0;
            char * q = p;

            for (;;) {
                if (q[n] == '\0') {
                    fprintf(stderr, "%s return invalid json.\n", githubApiUrl);
                    return NVM2_ERROR;
                }

                if (q[n] == '+') { // found right double quote
                    q[n] = '\0';
                    latestVersion = &q[0];
                    goto finalize;
                } else {
                    n++;
                }
            }
        }
    }

finalize:
    fclose(file);

    printf("latestVersion=%s\n", latestVersion);

    if (latestVersion == NULL) {
        fprintf(stderr, "%s return json has no tag_name key.\n", githubApiUrl);
        return NVM2_ERROR;
    }

    size_t latestVersionCopyLength = strlen(latestVersion) + 1U;
    char   latestVersionCopy[latestVersionCopyLength];
    strncpy(latestVersionCopy, latestVersion, latestVersionCopyLength);

    char * latestVersionMajorStr = strtok(latestVersionCopy, ".");
    char * latestVersionMinorStr = strtok(NULL, ".");
    char * latestVersionPatchStr = strtok(NULL, ".");

    int latestVersionMajor = 0;
    int latestVersionMinor = 0;
    int latestVersionPatch = 0;

    if (latestVersionMajorStr != NULL) {
        latestVersionMajor = atoi(latestVersionMajorStr);
    }

    if (latestVersionMinorStr != NULL) {
        latestVersionMinor = atoi(latestVersionMinorStr);
    }

    if (latestVersionPatchStr != NULL) {
        latestVersionPatch = atoi(latestVersionPatchStr);
    }

    if (latestVersionMajor == 0 && latestVersionMinor == 0 && latestVersionPatch == 0) {
        fprintf(stderr, "invalid version format: %s\n", latestVersion);
        return NVM2_ERROR;
    }

    if (latestVersionMajor < NVM2_VERSION_MAJOR) {
        LOG_SUCCESS1("this software is already the latest version.");
        return NVM2_OK;
    } else if (latestVersionMajor == NVM2_VERSION_MAJOR) {
        if (latestVersionMinor < NVM2_VERSION_MINOR) {
            LOG_SUCCESS1("this software is already the latest version.");
            return NVM2_OK;
        } else if (latestVersionMinor == NVM2_VERSION_MINOR) {
            if (latestVersionPatch <= NVM2_VERSION_PATCH) {
                LOG_SUCCESS1("this software is already the latest version.");
                return NVM2_OK;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    char osType[31] = {0};

    if (sysinfo_type(osType, 30) < 0) {
        perror(NULL);
        return NVM2_ERROR;
    }

    char osArch[31] = {0};

    if (sysinfo_arch(osArch, 30) < 0) {
        perror(NULL);
        return NVM2_ERROR;
    }

    size_t latestVersionLength = strlen(latestVersion);

    size_t   tarballFileNameLength = latestVersionLength + strlen(osType) + strlen(osArch) + 26U;
    char     tarballFileName[tarballFileNameLength];
    snprintf(tarballFileName, tarballFileNameLength, "nvm2-%s-%s-%s.tar.xz", latestVersion, osType, osArch);

    size_t   tarballUrlLength = tarballFileNameLength + latestVersionLength + 66;
    char     tarballUrl[tarballUrlLength];
    snprintf(tarballUrl, tarballUrlLength, "https://github.com/leleliu008/nvm2/releases/download/%s/%s", latestVersion, tarballFileName);

    size_t   tarballFilePathLength = nvm2TmpDirLength + tarballFileNameLength + 2;
    char     tarballFilePath[tarballFilePathLength];
    snprintf(tarballFilePath, tarballFilePathLength, "%s/%s", nvm2TmpDir, tarballFileName);

    ret = nvm2_http_fetch_to_file(tarballUrl, tarballFilePath, verbose, verbose);

    if (ret != NVM2_OK) {
        return ret;
    }

    //////////////////////////////////////////////////////////////////////////////////

    size_t   tarballExtractDirLength = tarballFilePathLength + 3U;
    char     tarballExtractDir[tarballExtractDirLength];
    snprintf(tarballExtractDir, tarballExtractDirLength, "%s.d", tarballFilePath);

    ret = tar_extract(tarballExtractDir, tarballFilePath, 0, verbose, 1);

    if (ret != 0) {
        return abs(ret) + NVM2_ERROR_ARCHIVE_BASE;
    }

    size_t   upgradableExecutableFilePathLength = tarballExtractDirLength + 10;
    char     upgradableExecutableFilePath[upgradableExecutableFilePathLength];
    snprintf(upgradableExecutableFilePath, upgradableExecutableFilePathLength, "%s/bin/nvm2", tarballExtractDir);

    char * selfRealPath = self_realpath();

    if (selfRealPath == NULL) {
        perror(NULL);
        ret = NVM2_ERROR;
        goto finally;
    }

    if (unlink(selfRealPath) != 0) {
        perror(selfRealPath);
        ret = NVM2_ERROR;
        goto finally;
    }

    if (copy_file(upgradableExecutableFilePath, selfRealPath) != 0) {
        perror(NULL);
        ret = NVM2_ERROR;
        goto finally;
    }

    if (chmod(selfRealPath, S_IRWXU) != 0) {
        perror(selfRealPath);
        ret = NVM2_ERROR;
    }

finally:
    free(selfRealPath);

    if (ret == NVM2_OK) {
        fprintf(stderr, "nvm2 is up to date with version %s\n", latestVersion);
    } else {
        fprintf(stderr, "Can't upgrade self. the latest version of executable was downloaded to %s, you can manually replace the current running program with it.\n", upgradableExecutableFilePath);
    }

    return ret;
}
