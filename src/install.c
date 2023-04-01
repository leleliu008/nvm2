#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "core/sysinfo.h"
#include "core/sha256sum.h"
#include "core/rm-r.h"
#include "core/tar.h"
#include "core/log.h"
#include "core/exe.h"

#include "nvm2.h"

#define LOG_STEP(output2Terminal, isSilent, stepN, msg) \
    if (!isSilent) { \
        if (output2Terminal) { \
            fprintf(stderr, "\n%s=>> STEP %u : %s%s\n", COLOR_PURPLE, stepN, msg, COLOR_OFF); \
        } else { \
            fprintf(stderr, "\n=>> STEP %u : %s\n", stepN, msg); \
        } \
    }

#define LOG_RUN_CMD(output2Terminal, isSilent, cmd) \
    if (!isSilent) { \
        if (output2Terminal) { \
            fprintf(stderr, "%s==>%s %s%s%s\n", COLOR_PURPLE, COLOR_OFF, COLOR_GREEN, cmd, COLOR_OFF); \
        } else { \
            fprintf(stderr, "==> %s\n", cmd); \
        } \
    }

static int run_cmd(char * cmd, int redirectOutput2FD) {
    pid_t pid = fork();

    if (pid < 0) {
        perror(NULL);
        return NVM2_ERROR;
    }

    if (pid == 0) {
        if (redirectOutput2FD > 0) {
            if (dup2(redirectOutput2FD, STDOUT_FILENO) < 0) {
                perror(NULL);
                exit(125);
            }

            if (dup2(redirectOutput2FD, STDERR_FILENO) < 0) {
                perror(NULL);
                exit(126);
            }
        }

        ////////////////////////////////////////

        size_t argc = 0U;
        char*  argv[10] = {0};

        char * arg = strtok(cmd, " ");

        while (arg != NULL) {
            argv[argc] = arg;
            argc++;
            arg = strtok(NULL, " ");
        }

        ////////////////////////////////////////

        execv(argv[0], argv);
        perror(argv[0]);
        exit(127);
    } else {
        int childProcessExitStatusCode;

        if (waitpid(pid, &childProcessExitStatusCode, 0) < 0) {
            perror(NULL);
            return NVM2_ERROR;
        }

        if (childProcessExitStatusCode == 0) {
            return NVM2_OK;
        }

        if (WIFEXITED(childProcessExitStatusCode)) {
            fprintf(stderr, "running command '%s' exit with status code: %d\n", cmd, WEXITSTATUS(childProcessExitStatusCode));
        } else if (WIFSIGNALED(childProcessExitStatusCode)) {
            fprintf(stderr, "running command '%s' killed by signal: %d\n", cmd, WTERMSIG(childProcessExitStatusCode));
        } else if (WIFSTOPPED(childProcessExitStatusCode)) {
            fprintf(stderr, "running command '%s' stopped by signal: %d\n", cmd, WSTOPSIG(childProcessExitStatusCode));
        }

        return NVM2_ERROR;
    }
}

int nvm2_find_or_install_gmake(char ** pgmakePath, size_t * pgmakePathLength, const char * osKind, const char * osArch, const char * versionInstallingDir, size_t versionInstallingDirLength, const char * downloadsDir, size_t downloadsDirLength, int stepN) {
    const char * binUrlGmake = NULL;
    const char * binShaGmake = NULL;

    if (strcmp(osKind, "linux") == 0) {
        if (strcmp(osArch, "x86_64") == 0) {
            binUrlGmake = "https://github.com/leleliu008/gmake-build/releases/download/4.3/gmake-4.3-linux-x86_64.tar.xz";
            binShaGmake = "b68f0033f4163bd94af6fb93b281c61fc02bc4af2cc7e80e74722dbf4c639dd3";
        } else if (strcmp(osArch, "aarch64") == 0) {
            binUrlGmake = "https://github.com/leleliu008/gmake-build/releases/download/4.3/gmake-4.3-linux-aarch64.tar.xz";
            binShaGmake = "8ba11716b9d473dbc100cd87474d381bd2bcb7822cc029b50e5a8307c6695691";
        } else if (strcmp(osArch, "ppc64le") == 0) {
            binUrlGmake = "https://github.com/leleliu008/gmake-build/releases/download/4.3/gmake-4.3-linux-ppc64le.tar.xz";
            binShaGmake = "635c8e41683e318f39a81b964deac2ae1fa736009dc05a04f1110393fa0c9480";
        } else if (strcmp(osArch, "s390x") == 0) {
            binUrlGmake = "https://github.com/leleliu008/gmake-build/releases/download/4.3/gmake-4.3-linux-s390x.tar.xz";
            binShaGmake = "4e25857f78bb0a1932cf5e0de1ad7b424a42875775d174753362c3af61ceeb0d";
        }
    } else if (strcmp(osKind, "darwin") == 0) {
        if (strcmp(osArch, "x86_64") == 0) {
            binUrlGmake = "https://github.com/leleliu008/gmake-build/releases/download/4.3/gmake-4.3-macos11.0-x86_64.tar.xz";
            binShaGmake = "f22660038bc9e318fc37660f406767fe3e2a0ccc205defaae3f4b2bc0708e3a9";
        } else if (strcmp(osArch, "arm64") == 0) {
            binUrlGmake = "https://github.com/leleliu008/gmake-build/releases/download/4.3/gmake-4.3-macos11.0-arm64.tar.xz";
            binShaGmake = "41680f6d1270497f1a3c717ac6150b4239b44430cfbfde4b9f51ff4d4dd1d52c";
        }
    } else if (strcmp(osKind, "freebsd") == 0) {
        if (strcmp(osArch, "amd64") == 0) {
            binUrlGmake = "https://github.com/leleliu008/gmake-build/releases/download/4.3/gmake-4.3-freebsd-amd64.tar.xz";
            binShaGmake = "8bab8e9b83afc8d8e08a4406b2167f8f66efd05fa4d4ba4f8c2b14a543860888";
        }
    } else if (strcmp(osKind, "openbsd") == 0) {
        if (strcmp(osArch, "amd64") == 0) {
            binUrlGmake = "https://github.com/leleliu008/gmake-build/releases/download/4.3/gmake-4.3-openbsd-amd64.tar.xz";
            binShaGmake = "a7d61765f08d536942c6894c0d81fb7e7052906aa2590586237ada8d09cbdf45";
        }
    } else if (strcmp(osKind, "netbsd") == 0) {
        if (strcmp(osArch, "amd64") == 0) {
            binUrlGmake = "https://github.com/leleliu008/gmake-build/releases/download/4.3/gmake-4.3-netbsd-amd64.tar.xz";
            binShaGmake = "8ba11716b9d473dbc100cd87474d381bd2bcb7822cc029b50e5a8307c6695691";
        }
    }

    //////////////////////////////////////////////////////////////////////////////

    char * gmakePath = NULL;

    size_t gmakePathLength = 0U;

    if (binUrlGmake == NULL) {
        LOG_STEP(true, false, stepN, "finding gmake")

        switch (exe_lookup("gmake", &gmakePath, &gmakePathLength)) {
            case  0:
                break;
            case -1:
                perror("gmake");
                return NVM2_ERROR;
            case -2:
                return NVM2_ERROR_ENV_PATH_NOT_SET;
            case -3:
                return NVM2_ERROR_ENV_PATH_NOT_SET;
            default:
                return NVM2_ERROR;
        }

        if (gmakePath == NULL) {
            switch (exe_lookup("make", &gmakePath, &gmakePathLength)) {
                case  0:
                    break;
                case -1:
                    perror("make");
                    return NVM2_ERROR;
                case -2:
                    return NVM2_ERROR_ENV_PATH_NOT_SET;
                case -3:
                    return NVM2_ERROR_ENV_PATH_NOT_SET;
                default:
                    return NVM2_ERROR;
            }
        }

        if (gmakePath == NULL) {
            fprintf(stderr, "neither 'gmake' nor 'make' command was found in PATH.\n");
            return NVM2_ERROR;
        } else {
            (*pgmakePath) = gmakePath;
            (*pgmakePathLength) = gmakePathLength;
            return NVM2_OK;
        }
    } else {
        LOG_STEP(true, false, stepN, "installing gmake")

        size_t   urlLength = strlen(binUrlGmake);

        size_t   fetchPhaseCmdLength = urlLength + 10U;
        char     fetchPhaseCmd[fetchPhaseCmdLength];
        snprintf(fetchPhaseCmd, fetchPhaseCmdLength, "Fetching %s", binUrlGmake);

        LOG_RUN_CMD(true, false, fetchPhaseCmd)

        ////////////////////////////////////////////////////////////////////////

        char    urlCopy[urlLength + 1U];
        strncpy(urlCopy, binUrlGmake, urlLength + 1U);

        char *   fileName = basename(urlCopy);

        size_t   fileNameLength = strlen(urlCopy);

        size_t   filePathLength = downloadsDirLength + fileNameLength + 2U;
        char     filePath[filePathLength];
        snprintf(filePath, filePathLength, "%s/%s", downloadsDir, fileName);

        bool needFetch = true;

        struct stat st;

        if ((stat(filePath, &st) == 0) && S_ISREG(st.st_mode)) {
            char actualSHA256SUM[65] = {0};

            if (sha256sum_of_file(actualSHA256SUM, filePath) != 0) {
                perror(filePath);
                return NVM2_ERROR;
            }

            if (strcmp(actualSHA256SUM, binShaGmake) == 0) {
                needFetch = false;

                    fprintf(stderr, "%s already have been fetched.\n", filePath);
            }
        }

        if (needFetch) {
            int ret = nvm2_http_fetch_to_file(binUrlGmake, filePath, false, false);

            if (ret != NVM2_OK) {
                return ret;
            }

            char actualSHA256SUM[65] = {0};

            if (sha256sum_of_file(actualSHA256SUM, filePath) != 0) {
                perror(filePath);
                return NVM2_ERROR;
            }

            if (strcmp(actualSHA256SUM, binShaGmake) != 0) {
                fprintf(stderr, "sha256sum mismatch.\n    expect : %s\n    actual : %s\n", binShaGmake, actualSHA256SUM);
                return NVM2_ERROR_SHA256_MISMATCH;
            }
        }

        size_t   uncompressPhaseCmdLength = filePathLength + 36U;
        char     uncompressPhaseCmd[uncompressPhaseCmdLength];
        snprintf(uncompressPhaseCmd, uncompressPhaseCmdLength, "Uncompressing %s --strip-components=1", filePath);

        LOG_RUN_CMD(true, false, uncompressPhaseCmd)

        size_t   uncompressDirLength = versionInstallingDirLength + 7U;
        char     uncompressDir[uncompressDirLength];
        snprintf(uncompressDir, uncompressDirLength, "%s/gmake", versionInstallingDir);

        int ret = tar_extract(uncompressDir, filePath, ARCHIVE_EXTRACT_TIME, false, 1);

        if (ret != 0) {
            return abs(ret) + NVM2_ERROR_ARCHIVE_BASE;
        }

        size_t pLength = uncompressDirLength + 11U;
        char * p = (char*)malloc(pLength * sizeof(char));

        if (p == NULL) {
            return NVM2_ERROR_MEMORY_ALLOCATE;
        }

        snprintf(p, pLength, "%s/bin/gmake", uncompressDir);

        (*pgmakePath) = p;
        (*pgmakePathLength) = strlen(p);

        return NVM2_OK;
    }
}

// https://github.com/nodejs/node/blob/main/BUILDING.md
int nvm2_build_nodejs_from_source(const char * versionInstalledDir, size_t versionInstalledDirLength, int redirectOutput2FD, const char * gmakePath, size_t gmakePathLength, unsigned int jobs) {
    if (jobs == 0) {
        int ncpu = sysinfo_ncpu();

        if (ncpu <= 0) {
            perror(NULL);
            return NVM2_ERROR;
        }

        jobs = ncpu;
    }

    if (chdir(versionInstalledDir) != 0) {
        perror(versionInstalledDir);
        return NVM2_ERROR;
    }

    size_t   configurePhaseCmdLength = versionInstalledDirLength + 32U;
    char     configurePhaseCmd[configurePhaseCmdLength];
    snprintf(configurePhaseCmd, configurePhaseCmdLength, "./configure --prefix=%s", versionInstalledDir);

    LOG_RUN_CMD(redirectOutput2FD != STDOUT_FILENO, false, configurePhaseCmd)

    int ret = run_cmd(configurePhaseCmd, redirectOutput2FD);

    if (ret != NVM2_OK) {
        return ret;
    }

    //////////////////////////////////////////////////////////////////////////////

    size_t   buildPhaseCmdLength = gmakePathLength + 12U;
    char     buildPhaseCmd[buildPhaseCmdLength];
    snprintf(buildPhaseCmd, buildPhaseCmdLength, "%s --jobs=%u", gmakePath, jobs);

    LOG_RUN_CMD(redirectOutput2FD == STDOUT_FILENO, false, buildPhaseCmd)

    ret = run_cmd(buildPhaseCmd, redirectOutput2FD);

    if (ret != NVM2_OK) {
        return ret;
    }

    //////////////////////////////////////////////////////////////////////////////

    size_t   installPhaseCmdLength = gmakePathLength + versionInstalledDirLength + 9U;
    char     installPhaseCmd[installPhaseCmdLength];
    snprintf(installPhaseCmd, installPhaseCmdLength, "%s install", gmakePath);

    LOG_RUN_CMD(redirectOutput2FD == STDOUT_FILENO, false, installPhaseCmd)

    return run_cmd(installPhaseCmd, redirectOutput2FD);
}

int nvm2_install(const char * versionName, const char * siteUrl, bool fromSource, NVM2LogLevel logLevel, unsigned int jobs) {
    const char * const userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return NVM2_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0U) {
        return NVM2_ERROR_ENV_HOME_NOT_SET;
    }

    size_t   nvm2HomeDirLength = strlen(userHomeDir) + 7U;
    char     nvm2HomeDir[nvm2HomeDirLength];
    snprintf(nvm2HomeDir, nvm2HomeDirLength, "%s/.nvm2", userHomeDir);

    size_t   versionInstalledDirLength = nvm2HomeDirLength + strlen(versionName) + 11U;
    char     versionInstalledDir[versionInstalledDirLength];
    snprintf(versionInstalledDir, versionInstalledDirLength, "%s/versions/%s", nvm2HomeDir, versionName);

    size_t   receiptFilePathLength = versionInstalledDirLength + 12U;
    char     receiptFilePath[receiptFilePathLength];
    snprintf(receiptFilePath, receiptFilePathLength, "%s/receipt.yml", versionInstalledDir);

    //////////////////////////////////////////////////////////////////////////

    struct stat st;

    if (stat(receiptFilePath, &st) == 0 && S_ISREG(st.st_mode)) {
        fprintf(stderr, "version [%s] already has been installed.\n", versionName);
        return NVM2_OK;
    }

    //////////////////////////////////////////////////////////////////////////

    fprintf(stderr, "prepare to install version [%s].\n", versionName);

    //////////////////////////////////////////////////////////////////////////

    if (stat(nvm2HomeDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", nvm2HomeDir);
            return NVM2_ERROR;
        }
    } else {
        if (mkdir(nvm2HomeDir, S_IRWXU) != 0) {
            return NVM2_ERROR;
        }
    }

    //////////////////////////////////////////////////////////////////////////

    size_t   nvm2DownloadDirLength = nvm2HomeDirLength + 11U;
    char     nvm2DownloadDir[nvm2DownloadDirLength];
    snprintf(nvm2DownloadDir, nvm2DownloadDirLength, "%s/downloads", nvm2HomeDir);

    if (stat(nvm2DownloadDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", nvm2DownloadDir);
            return NVM2_ERROR;
        }
    } else {
        if (mkdir(nvm2DownloadDir, S_IRWXU) != 0) {
            return NVM2_ERROR;
        }
    }

    //////////////////////////////////////////////////////////////////////////

    char osKind[21] = {0};

    if (sysinfo_kind(osKind, 20) != 0) {
        perror(NULL);
        return NVM2_ERROR;
    }

    char osArch[21] = {0};

    if (sysinfo_arch(osArch, 20) != 0) {
        perror(NULL);
        return NVM2_ERROR;
    }

    size_t  tarballFileNameBufSize = 50U;
    char    tarballFileNameBuf[tarballFileNameBufSize];

    if (siteUrl == NULL) {
        siteUrl = "https://nodejs.org/dist";
    }

    if (fromSource) {
        snprintf(tarballFileNameBuf, tarballFileNameBufSize, "node-v%s.tar.xz", versionName);
    } else {
        if (strcmp(osKind, "darwin") == 0) {
            if (strcmp(osArch, "x86_64") == 0) {
                snprintf(tarballFileNameBuf, tarballFileNameBufSize, "node-v%s-darwin-x64.tar.xz", versionName);
            } else {
                snprintf(tarballFileNameBuf, tarballFileNameBufSize, "node-v%s-darwin-arm64.tar.xz", versionName);
            }
        } else if (strcmp(osKind, "linux") == 0) {
            int libc = sysinfo_libc();

            if (libc < 0) {
                perror(NULL);
                return NVM2_ERROR;
            }

            if (libc == 1) { //glibc
                if (strcmp(osArch, "x86_64") == 0) {
                    snprintf(tarballFileNameBuf, tarballFileNameBufSize, "node-v%s-linux-x64.tar.xz", versionName);
                } else if (strcmp(osArch, "aarch64") == 0) {
                    snprintf(tarballFileNameBuf, tarballFileNameBufSize, "node-v%s-linux-arm64.tar.xz", versionName);
                } else if (strcmp(osArch, "ppc64le") == 0) {
                    snprintf(tarballFileNameBuf, tarballFileNameBufSize, "node-v%s-linux-ppc64le.tar.xz", versionName);
                } else if (strcmp(osArch, "s390x") == 0) {
                    snprintf(tarballFileNameBuf, tarballFileNameBufSize, "node-v%s-linux-s390x.tar.xz", versionName);
                }
            } else {
                fromSource = true;
                snprintf(tarballFileNameBuf, tarballFileNameBufSize, "node-v%s.tar.xz", versionName);
            }
        } else {
            fromSource = true;
            snprintf(tarballFileNameBuf, tarballFileNameBufSize, "node-v%s.tar.xz", versionName);
        }
    }

    size_t   tarballFileNameLength = strlen(tarballFileNameBuf);

    size_t   tarballFilePathLength = nvm2DownloadDirLength + tarballFileNameLength + 2U;
    char     tarballFilePath[tarballFilePathLength];
    snprintf(tarballFilePath, tarballFilePathLength, "%s/%s", nvm2DownloadDir, tarballFileNameBuf);

    //////////////////////////////////////////////////////////////////////////

    size_t   siteUrlLength = strlen(siteUrl);

    size_t   versionNameLength = strlen(versionName);

    size_t   sha256sumTxtFileUrlLength = siteUrlLength + versionNameLength + 18U;
    char     sha256sumTxtFileUrl[sha256sumTxtFileUrlLength];
    snprintf(sha256sumTxtFileUrl, sha256sumTxtFileUrlLength, "%s/v%s/SHASUMS256.txt", siteUrl, versionName);

    size_t   sha256sumTxtFilePathLength = nvm2DownloadDirLength + versionNameLength + 22U;
    char     sha256sumTxtFilePath[sha256sumTxtFilePathLength];
    snprintf(sha256sumTxtFilePath, sha256sumTxtFilePathLength, "%s/node-v%s-SHASUMS256.txt", nvm2DownloadDir, versionName);

    int ret = nvm2_http_fetch_to_file(sha256sumTxtFileUrl, sha256sumTxtFilePath, logLevel >= NVM2LogLevel_verbose, logLevel >= NVM2LogLevel_verbose);

    if (ret != NVM2_OK) {
        return ret;
    }

    //////////////////////////////////////////////////////////////////////////

    char expectedSHA256SUM[65] = {0};

    FILE * sha256sumTxtFile = fopen(sha256sumTxtFilePath, "r");

    if (sha256sumTxtFile == NULL) {
        perror(sha256sumTxtFilePath);
        return NVM2_ERROR;
    }

    for (;;) {
        char buf[256] = {0};

        if (fgets(buf, 255, sha256sumTxtFile) == NULL) {
            if (ferror(sha256sumTxtFile)) {
                fclose(sha256sumTxtFile);
                return NVM2_ERROR;
            } else {
                fclose(sha256sumTxtFile);
                fprintf(stderr, "no sha256sum found for %s in %s\n", tarballFileNameBuf, sha256sumTxtFilePath);
                return NVM2_ERROR;
            }
        } else {
            if (strncmp(&buf[66], tarballFileNameBuf, tarballFileNameLength) == 0) {
                fclose(sha256sumTxtFile);
                strncpy(expectedSHA256SUM, buf, 64);
                expectedSHA256SUM[64] = '\0';
                break;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////

    bool needFetch = true;

    if (stat(tarballFilePath, &st) == 0) {
        if (S_ISREG(st.st_mode)) {
            char actualSHA256SUM[65] = {0};

            if (sha256sum_of_file(actualSHA256SUM, tarballFilePath) != 0) {
                perror(tarballFilePath);
                return NVM2_ERROR;
            }

            if (strcmp(actualSHA256SUM, expectedSHA256SUM) == 0) {
                needFetch = false;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////

    size_t   tarballFileUrlLength = siteUrlLength + versionNameLength + tarballFileNameLength + 4U;
    char     tarballFileUrl[tarballFileUrlLength];
    snprintf(tarballFileUrl, tarballFileUrlLength, "%s/v%s/%s", siteUrl, versionName, tarballFileNameBuf);

    if (needFetch) {
        int ret = nvm2_http_fetch_to_file(tarballFileUrl, tarballFilePath, logLevel >= NVM2LogLevel_verbose, logLevel >= NVM2LogLevel_verbose);

        if (ret != NVM2_OK) {
            return ret;
        }

        char actualSHA256SUM[65] = {0};

        if (sha256sum_of_file(actualSHA256SUM, tarballFilePath) != 0) {
            perror(tarballFilePath);
            return NVM2_ERROR;
        }

        if (strcmp(actualSHA256SUM, expectedSHA256SUM) == 0) {
            fprintf(stderr, "%s already have been fetched.\n", tarballFilePath);
        } else {
            fprintf(stderr, "sha256sum mismatch.\n    expect : %s\n    actual : %s\n", expectedSHA256SUM, actualSHA256SUM);
            return NVM2_ERROR_SHA256_MISMATCH;
        }
    }

    //////////////////////////////////////////////////////////////////////////

    size_t   installedDirLength = nvm2HomeDirLength + 20U;
    char     installedDir[installedDirLength];
    snprintf(installedDir, installedDirLength, "%s/versions", nvm2HomeDir);

    if (stat(installedDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", installedDir);
            return NVM2_ERROR;
        }
    } else {
        if (mkdir(installedDir, S_IRWXU) != 0) {
            return NVM2_ERROR;
        }
    }

    if (stat(versionInstalledDir, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            if (rm_r(versionInstalledDir, logLevel == NVM2LogLevel_very_verbose) != 0) {
                return NVM2_ERROR;
            }
        } else {
            if (unlink(versionInstalledDir) != 0) {
                perror(versionInstalledDir);
                return NVM2_ERROR;
            }
        }
    }

    if (mkdir(versionInstalledDir, S_IRWXU) != 0) {
        return NVM2_ERROR;
    }

    char * gmakePath = NULL;
    size_t gmakePathLength = 0U;

    if (fromSource) {
        size_t   versionInstallingRootDirLength = nvm2HomeDirLength + 12U;
        char     versionInstallingRootDir[versionInstallingRootDirLength];
        snprintf(versionInstallingRootDir, versionInstallingRootDirLength, "%s/installing", nvm2HomeDir);

        if (stat(versionInstallingRootDir, &st) == 0) {
            if (!S_ISDIR(st.st_mode)) {
                fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", versionInstallingRootDir);
                return NVM2_ERROR;
            }
        } else {
            if (mkdir(versionInstallingRootDir, S_IRWXU) != 0) {
                perror(versionInstallingRootDir);
                return NVM2_ERROR;
            }
        }

        //////////////////////////////////////////////////////////////////////////////

        size_t   versionInstallingDirLength = versionInstallingRootDirLength + 12U;
        char     versionInstallingDir[versionInstallingDirLength];
        snprintf(versionInstallingDir, versionInstallingDirLength, "%s/%d", versionInstallingRootDir, getpid());

        if (stat(versionInstallingDir, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                if (rm_r(versionInstallingDir, false) != 0) {
                    perror(versionInstallingDir);
                    return NVM2_ERROR;
                }
            } else {
                fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", versionInstallingDir);
                return NVM2_ERROR;
            }
        } else {
            if (mkdir(versionInstallingDir, S_IRWXU) != 0) {
                perror(versionInstallingDir);
                return NVM2_ERROR;
            }
        }

        //////////////////////////////////////////////////////////////////////////////

        int ret = tar_extract(versionInstallingDir, tarballFilePath, ARCHIVE_EXTRACT_TIME, logLevel == NVM2LogLevel_very_verbose, 1);

        if (ret != 0) {
            return abs(ret) + NVM2_ERROR_ARCHIVE_BASE;
        }

        ret = nvm2_find_or_install_gmake(&gmakePath, &gmakePathLength, osKind, osArch, versionInstallingDir, versionInstallingDirLength, nvm2DownloadDir, nvm2DownloadDirLength, 1);

        if (ret != 0) {
            return ret;
        }

        ret = nvm2_build_nodejs_from_source(versionInstallingDir, versionInstallingDirLength, -1, gmakePath, gmakePathLength, jobs);

        free(gmakePath);

        if (ret != 0) {
            return ret;
        }
    } else {
        int ret = tar_extract(versionInstalledDir, tarballFilePath, ARCHIVE_EXTRACT_TIME, logLevel == NVM2LogLevel_very_verbose, 1);

        if (ret != 0) {
            return abs(ret) + NVM2_ERROR_ARCHIVE_BASE;
        }
    }

    //////////////////////////////////////////////////////////////////////

    FILE * receiptFile = fopen(receiptFilePath, "w");

    if (receiptFile == NULL) {
        perror(receiptFilePath);
        return NVM2_ERROR;
    }

    if (fromSource) {
        fprintf(receiptFile, "sourceurl: %s\nsha256sum: %s\nsignature: %s\ntimestamp: %lu\n", tarballFileUrl, expectedSHA256SUM, NVM2_VERSION, time(NULL));
    } else {
        fprintf(receiptFile, "binaryurl: %s\nsha256sum: %s\nsignature: %s\ntimestamp: %lu\n", tarballFileUrl, expectedSHA256SUM, NVM2_VERSION, time(NULL));
    }

    fclose(receiptFile);

    fprintf(stderr, "\nversion [%s] successfully installed.\n", versionName);

    return NVM2_OK;
}
