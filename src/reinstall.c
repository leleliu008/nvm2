#include "nvm2.h"

int nvm2_reinstall(const char * versionName, const char * siteUrl, bool fromSource, NVM2LogLevel logLevel, unsigned int jobs) {
    int ret = nvm2_check_if_the_given_version_is_available(versionName);

    if (ret != NVM2_OK) {
        return ret;
    }

    ret = nvm2_check_if_the_given_version_is_installed(versionName);

    if (ret != NVM2_OK) {
        return ret;
    }

    ret = nvm2_uninstall(versionName, logLevel >= NVM2LogLevel_verbose);

    if (ret != NVM2_OK) {
        return ret;
    }

    return nvm2_install(versionName, siteUrl, fromSource, logLevel, jobs);
}
