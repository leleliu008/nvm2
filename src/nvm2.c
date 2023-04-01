#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "core/log.h"
#include "nvm2.h"

int nvm2_main(int argc, char* argv[]) {
    if (argc == 1) {
        nvm2_help();
        return NVM2_OK;
    }

    if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0)) {
        nvm2_help();
        return NVM2_OK;
    }

    if ((strcmp(argv[1], "-V") == 0) || (strcmp(argv[1], "--version") == 0)) {
        printf("%s\n", NVM2_VERSION);
        return NVM2_OK;
    }

    bool verbose = false;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = true;
            break;
        }
    }

    if (strcmp(argv[1], "sysinfo") == 0) {
        return nvm2_sysinfo();
    }

    if (strcmp(argv[1], "env") == 0) {
        int ret = nvm2_env(verbose);

        if (ret == NVM2_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == NVM2_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == NVM2_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "info") == 0) {
        int ret = nvm2_info(argv[2], argv[3]);

        if (ret == NVM2_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s info <VERSION-NAME> [KEY], <VERSION-NAME> is unspecified.\n", argv[0]);
        } else if (ret == NVM2_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s info <VERSION-NAME> [KEY], <VERSION-NAME> should be a non-empty string.\n", argv[0]);
        } else if (ret == NVM2_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s info <VERSION-NAME> [KEY], <VERSION-NAME> does not match pattern %s\n", argv[0], NVM2_VERSION_NAME_PATTERN);
        } else if (ret == NVM2_ERROR_ARG_IS_UNKNOWN) {
            fprintf(stderr, "Usage: %s info <VERSION-NAME> [KEY], unrecognized KEY: %s\n", argv[0], argv[3]);
        } else if (ret == NVM2_ERROR_VERSION_NOT_AVAILABLE) {
            fprintf(stderr, "version [%s] is not available.\n", argv[2]);
        } else if (ret == NVM2_ERROR_VERSION_NOT_INSTALLED) {
            fprintf(stderr, "version [%s] is not installed.\n", argv[2]);
        } else if (ret == NVM2_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == NVM2_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == NVM2_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "tree") == 0) {
        int ret = nvm2_tree(argv[2], argc - 3, &argv[3]);

        if (ret == NVM2_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s tree <VERSION-NAME> [KEY], <VERSION-NAME> is unspecified.\n", argv[0]);
        } else if (ret == NVM2_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s tree <VERSION-NAME> [KEY], <VERSION-NAME> should be a non-empty string.\n", argv[0]);
        } else if (ret == NVM2_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s tree <VERSION-NAME> [KEY], <VERSION-NAME> does not match pattern %s\n", argv[0], NVM2_VERSION_NAME_PATTERN);
        } else if (ret == NVM2_ERROR_VERSION_NOT_AVAILABLE) {
            fprintf(stderr, "version [%s] is not available.\n", argv[2]);
        } else if (ret == NVM2_ERROR_VERSION_NOT_INSTALLED) {
            fprintf(stderr, "version [%s] is not installed.\n", argv[2]);
        } else if (ret == NVM2_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == NVM2_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == NVM2_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "exec") == 0) {
        int ret = nvm2_exec(argv[2], argc - 3, &argv[3]);

        if (ret == NVM2_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s exec <VERSION-NAME> [CMD], <VERSION-NAME> is unspecified.\n", argv[0]);
        } else if (ret == NVM2_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s exec <VERSION-NAME> [CMD], <VERSION-NAME> should be a non-empty string.\n", argv[0]);
        } else if (ret == NVM2_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s exec <VERSION-NAME> [CMD], <VERSION-NAME> does not match pattern %s\n", argv[0], NVM2_VERSION_NAME_PATTERN);
        } else if (ret == NVM2_ERROR_VERSION_NOT_AVAILABLE) {
            fprintf(stderr, "version [%s] is not available.\n", argv[2]);
        } else if (ret == NVM2_ERROR_VERSION_NOT_INSTALLED) {
            fprintf(stderr, "version [%s] is not installed.\n", argv[2]);
        } else if (ret == NVM2_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == NVM2_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == NVM2_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }


    if (strcmp(argv[1], "install") == 0) {
        unsigned int jobs = 0U;

        NVM2LogLevel logLevel = NVM2LogLevel_normal;

        bool fromSource = false;

        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-q") == 0) {
                logLevel = NVM2LogLevel_silent;
            } else if (strcmp(argv[i], "-v") == 0) {
                logLevel = NVM2LogLevel_verbose;
            } else if (strcmp(argv[i], "-vv") == 0) {
                logLevel = NVM2LogLevel_very_verbose;
            } else if (strcmp(argv[i], "--from-source") == 0) {
                fromSource = true;
            } else if (strncmp(argv[i], "--jobs=", 7) == 0) {
                if (argv[i][7] == '\0') {
                    fprintf(stderr, "Usage: %s %s [-q | -v | -vv | --from-source --site-url=<URL> --jobs=<N>], <N> should be a non-empty string.\n", argv[0], argv[1]);
                    return NVM2_ERROR_ARG_IS_NULL;
                } else {
                    const char * jobsStr = argv[i] + 7;
                    jobs = atoi(jobsStr);
                }
            }
        }

        int ret = nvm2_install(argv[2], NULL, fromSource, logLevel, jobs);

        if (ret == NVM2_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s %s <VERSION-NAME>, <VERSION-NAME> is unspecified.\n", argv[0], argv[1]);
        } else if (ret == NVM2_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s %s <VERSION-NAME>, <VERSION-NAME> should be a non-empty string.\n", argv[0], argv[1]);
        } else if (ret == NVM2_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s %s <VERSION-NAME>, <VERSION-NAME> does not match pattern %s\n", argv[0], argv[1], NVM2_VERSION_NAME_PATTERN);
        } else if (ret == NVM2_ERROR_VERSION_NOT_AVAILABLE) {
            fprintf(stderr, "version [%s] is not available.\n", argv[2]);
        } else if (ret == NVM2_ERROR_VERSION_NOT_INSTALLED) {
            fprintf(stderr, "version [%s] is not installed.\n", argv[2]);
        } else if (ret == NVM2_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == NVM2_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == NVM2_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        if (ret != NVM2_OK) {
            return ret;
        }

        return NVM2_OK;
    }

    if (strcmp(argv[1], "uninstall") == 0) {
        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            }
        }

        int ret = nvm2_check_if_the_given_version_is_installed(argv[2]);

        if (ret == NVM2_ERROR_ARG_IS_INVALID) {
            LOG_ERROR4("version name [", argv[2], "] does not match pattern ", NVM2_VERSION_NAME_PATTERN);
        } else if (ret == NVM2_ERROR_VERSION_NOT_INSTALLED) {
            LOG_ERROR3("version [", argv[2], "] is not installed.");
        }

        if (ret != NVM2_OK) {
            return ret;
        }

        ret = nvm2_uninstall(argv[2], verbose);

        if (ret == NVM2_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s %s <VERSION-NAME>, <VERSION-NAME> is unspecified.\n", argv[0], argv[1]);
        } else if (ret == NVM2_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s %s <VERSION-NAME>, <VERSION-NAME> should be a non-empty string.\n", argv[0], argv[1]);
        } else if (ret == NVM2_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s %s <VERSION-NAME>, <VERSION-NAME> does not match pattern %s\n", argv[0], argv[1], NVM2_VERSION_NAME_PATTERN);
        } else if (ret == NVM2_ERROR_VERSION_NOT_AVAILABLE) {
            fprintf(stderr, "version [%s] is not available.\n", argv[2]);
        } else if (ret == NVM2_ERROR_VERSION_NOT_INSTALLED) {
            fprintf(stderr, "version [%s] is not installed.\n", argv[2]);
        } else if (ret == NVM2_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == NVM2_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == NVM2_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        if (ret != NVM2_OK) {
            return ret;
        }

        return NVM2_OK;
    }

    if (strcmp(argv[1], "reinstall") == 0) {
        unsigned int jobs = 0U;

        NVM2LogLevel logLevel = NVM2LogLevel_normal;

        bool fromSource = false;

        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-q") == 0) {
                logLevel = NVM2LogLevel_silent;
            } else if (strcmp(argv[i], "-v") == 0) {
                logLevel = NVM2LogLevel_verbose;
            } else if (strcmp(argv[i], "-vv") == 0) {
                logLevel = NVM2LogLevel_very_verbose;
            } else if (strcmp(argv[i], "--from-source") == 0) {
                fromSource = true;
            } else if (strncmp(argv[i], "--jobs=", 7) == 0) {
                if (argv[i][7] == '\0') {
                    fprintf(stderr, "Usage: %s %s [-q | -v | -vv | --from-source --site-url=<URL> --jobs=<N>], <N> should be a non-empty string.\n", argv[0], argv[1]);
                    return NVM2_ERROR_ARG_IS_NULL;
                } else {
                    const char * jobsStr = argv[i] + 7;
                    jobs = atoi(jobsStr);
                }
            }
        }

        int ret = nvm2_reinstall(argv[2], NULL, fromSource, logLevel, jobs);

        if (ret == NVM2_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s %s <VERSION-NAME>, <VERSION-NAME> is unspecified.\n", argv[0], argv[1]);
        } else if (ret == NVM2_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s %s <VERSION-NAME>, <VERSION-NAME> should be a non-empty string.\n", argv[0], argv[1]);
        } else if (ret == NVM2_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s %s <VERSION-NAME>, <VERSION-NAME> does not match pattern %s\n", argv[0], argv[1], NVM2_VERSION_NAME_PATTERN);
        } else if (ret == NVM2_ERROR_VERSION_NOT_AVAILABLE) {
            fprintf(stderr, "version [%s] is not available.\n", argv[2]);
        } else if (ret == NVM2_ERROR_VERSION_NOT_INSTALLED) {
            fprintf(stderr, "version [%s] is not installed.\n", argv[2]);
        } else if (ret == NVM2_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == NVM2_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == NVM2_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        if (ret != NVM2_OK) {
            return ret;
        }

        return NVM2_OK;
    }

    if (strcmp(argv[1], "ls-available") == 0) {
        bool onlyLTS = false;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else if (strcmp(argv[i], "--lts") == 0) {
                onlyLTS = true;
            } else {
                LOG_ERROR2("unrecognized option: ", argv[i]);
                return NVM2_ERROR_ARG_IS_UNKNOWN;
            }
        }

        int ret = nvm2_list_the_available_versions(verbose, onlyLTS);

        if (ret == NVM2_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == NVM2_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == NVM2_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "ls-installed") == 0) {
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else {
                LOG_ERROR2("unrecognized option: ", argv[i]);
                return NVM2_ERROR_ARG_IS_UNKNOWN;
            }
        }

        int ret = nvm2_list_the_installed_versions(verbose);

        if (ret == NVM2_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == NVM2_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == NVM2_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "is-available") == 0) {
        int ret = nvm2_check_if_the_given_version_is_available(argv[2]);

        if (ret == NVM2_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s is-available <VERSION-NAME>, <VERSION-NAME> is unspecified.\n", argv[0]);
        } else if (ret == NVM2_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s is-available <VERSION-NAME>, <VERSION-NAME> should be a non-empty string.\n", argv[0]);
        } else if (ret == NVM2_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s is-available <VERSION-NAME>, <VERSION-NAME> does not match pattern %s\n", argv[0], NVM2_VERSION_NAME_PATTERN);
        } else if (ret == NVM2_ERROR_VERSION_NOT_AVAILABLE) {
            fprintf(stderr, "version [%s] is not available.\n", argv[2]);
        } else if (ret == NVM2_ERROR_VERSION_NOT_INSTALLED) {
            fprintf(stderr, "version [%s] is not installed.\n", argv[2]);
        } else if (ret == NVM2_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == NVM2_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == NVM2_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "is-installed") == 0) {
        int ret = nvm2_check_if_the_given_version_is_installed(argv[2]);

        if (ret == NVM2_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s is-installed <VERSION-NAME>, <VERSION-NAME> is unspecified.\n", argv[0]);
        } else if (ret == NVM2_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s is-installed <VERSION-NAME>, <VERSION-NAME> should be a non-empty string.\n", argv[0]);
        } else if (ret == NVM2_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s is-installed <VERSION-NAME>, <VERSION-NAME> does not match pattern %s\n", argv[0], NVM2_VERSION_NAME_PATTERN);
        } else if (ret == NVM2_ERROR_VERSION_NOT_AVAILABLE) {
            fprintf(stderr, "version [%s] is not available.\n", argv[2]);
        } else if (ret == NVM2_ERROR_VERSION_NOT_INSTALLED) {
            fprintf(stderr, "version [%s] is not installed.\n", argv[2]);
        } else if (ret == NVM2_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == NVM2_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == NVM2_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "integrate") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s integrate <zsh|bash|fish>\n", argv[0]);
            return NVM2_ERROR_ARG_IS_NULL;
        } else if (strcmp(argv[2], "zsh") == 0) {
            char * outputDirPath = NULL;

            for (int i = 3; i < argc; i++) {
                if (strncmp(argv[i], "--output-dir=", 13) == 0) {
                    if (argv[i][13] == '\0') {
                        fprintf(stderr, "--output-dir=VALUE, VALUE should be a non-empty string.\n");
                        return NVM2_ERROR_ARG_IS_INVALID;
                    } else {
                        outputDirPath = &argv[i][13];
                    }
                } else if (strcmp(argv[i], "-v") == 0) {
                    verbose = true;
                } else {
                    fprintf(stderr, "unrecognized option: %s\n", argv[i]);
                    return NVM2_ERROR_ARG_IS_UNKNOWN;
                }
            }

            return nvm2_integrate_zsh_completion (outputDirPath, verbose);
        } else if (strcmp(argv[2], "bash") == 0) {
            return nvm2_integrate_bash_completion(NULL, verbose);
        } else if (strcmp(argv[2], "fish") == 0) {
            return nvm2_integrate_fish_completion(NULL, verbose);
        } else {
            LOG_ERROR2("unrecognized argument: ", argv[2]);
            return NVM2_ERROR_ARG_IS_INVALID;
        }
    }

    if (strcmp(argv[1], "show-release-schedule") == 0) {
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else {
                LOG_ERROR2("unrecognized option: ", argv[i]);
                return NVM2_ERROR_ARG_IS_UNKNOWN;
            }
        }

        int ret = nvm2_show_release_schedule(verbose);

        if (ret == NVM2_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == NVM2_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == NVM2_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "upgrade-self") == 0) {
        return nvm2_upgrade_self(verbose);
    }

    if (strcmp(argv[1], "cleanup") == 0) {
        return nvm2_cleanup(verbose);
    }

    if (strcmp(argv[1], "util") == 0) {
        return nvm2_util(argc, argv);
    }

    LOG_ERROR2("unrecognized action: ", argv[1]);
    return NVM2_ERROR_ARG_IS_UNKNOWN;
}

int main(int argc, char* argv[]) {
    return nvm2_main(argc, argv);
}
