#include <unistd.h>
#include "core/log.h"

int nvm2_help() {
    if (isatty(STDOUT_FILENO)) {
        const char * str = ""
        COLOR_GREEN
        "nvm2 (yet another Node.js Version Manager written in C)\n\n"
        "nvm2 <ACTION> [ARGUMENT...]\n\n"
        "nvm2 --help\n"
        "nvm2 -h\n"
        COLOR_OFF
        "    show help of this command.\n\n"
        COLOR_GREEN
        "nvm2 --version\n"
        "nvm2 -V\n"
        COLOR_OFF
        "    show version of this command.\n\n"
        COLOR_GREEN
        "nvm2 sysinfo\n"
        COLOR_OFF
        "    show your system's information.\n\n"
        COLOR_GREEN
        "nvm2 env\n"
        COLOR_OFF
        "    show your system's information and other information.\n\n"
        COLOR_GREEN
        "nvm2 integrate zsh [-v] [--output-dir=<DIR>]\n"
        COLOR_OFF
        "    download a zsh completion script file to a approprivate location.\n\n"
        "    to apply this feature, you may need to run the command 'autoload -U compinit && compinit' in your terminal (your current running shell must be zsh).\n\n"
        COLOR_GREEN
        "nvm2 info <VERSION-NAME>\n"
        COLOR_OFF
        "    show information of the given version.\n\n"
        COLOR_GREEN
        "nvm2 tree <VERSION-NAME> [--dirsfirst | -L N]\n"
        COLOR_OFF
        "    list files of the given installed version in a tree-like format.\n\n"
        COLOR_GREEN
        "nvm2   install <VERSION-NAME>\n"
        COLOR_OFF
        "    install the given version.\n\n"
        COLOR_GREEN
        "nvm2 reinstall <VERSION-NAME>\n"
        COLOR_OFF
        "    reinstall the given version.\n\n"
        COLOR_GREEN
        "nvm2 uninstall <VERSION-NAME>\n"
        COLOR_OFF
        "    uninstall the given version.\n\n"
        COLOR_GREEN
        "nvm2 ls-available [-v]\n"
        COLOR_OFF
        "    list the available versions.\n\n"
        COLOR_GREEN
        "nvm2 ls-installed [-v]\n"
        COLOR_OFF
        "    list the installed versions.\n\n"
        COLOR_GREEN
        "nvm2 is-available <VERSION-NAME>\n"
        COLOR_OFF
        "    check if the given version is available.\n\n"
        COLOR_GREEN
        "nvm2 is-installed <VERSION-NAME>\n"
        COLOR_OFF
        "    check if the given version is installed.\n\n"
        COLOR_GREEN
        "nvm2 cleanup\n"
        COLOR_OFF
        "    cleanup the unused cache.\n\n\n"
        COLOR_GREEN
        "nvm2 util zlib-deflate -L <LEVEL> < input/file/path\n"
        COLOR_OFF
        "    compress data using zlib deflate algorithm.\n\n"
        "    LEVEL >= 1 && LEVEL <= 9\n\n"
        "    The smaller the LEVEL, the faster the speed and the lower the compression ratio.\n\n"
        COLOR_GREEN
        "nvm2 util zlib-inflate < input/file/path\n"
        COLOR_OFF
        "    decompress data using zlib inflate algorithm.\n\n"
        COLOR_GREEN
        "nvm2 util base16-encode <STR>\n"
        COLOR_OFF
        "    encode <STR> using base16 algorithm.\n\n"
        COLOR_GREEN
        "nvm2 util base16-encode < input/file/path\n"
        COLOR_OFF
        "    encode data using base16 algorithm.\n\n"
        COLOR_GREEN
        "nvm2 util base16-decode <BASE16-ENCODED-SUM>\n"
        COLOR_OFF
        "    decode <BASE16-ENCODED-SUM> using base16 algorithm.\n\n"
        COLOR_GREEN
        "nvm2 util base16-decode < input/file/path\n"
        COLOR_OFF
        "    decode data using base16 algorithm.\n\n"
        COLOR_GREEN
        "nvm2 util base64-encode <STR>\n"
        COLOR_OFF
        "    encode <STR> using base64 algorithm.\n\n"
        COLOR_GREEN
        "nvm2 util base64-encode < input/file/path\n"
        COLOR_OFF
        "    encode data using base64 algorithm.\n\n"
        COLOR_GREEN
        "nvm2 util base64-decode <BASE64-ENCODED-SUM>\n"
        COLOR_OFF
        "    decode <BASE64-ENCODED-SUM> using base64 algorithm.\n\n"
        COLOR_GREEN
        "nvm2 util base64-decode < input/file/path\n"
        COLOR_OFF
        "    decode data using base64 algorithm.\n\n"
        COLOR_GREEN
        "nvm2 util sha256sum <input/file/path>\n"
        COLOR_OFF
        "    calculate sha256sum of file.\n\n"
        COLOR_GREEN
        "nvm2 util sha256sum < input/file/path\n"
        COLOR_OFF
        "    calculate sha256sum of file.\n\n"
        COLOR_GREEN
        "nvm2 util which <COMMAND-NAME> [-a]\n"
        COLOR_OFF
        "    find <COMMAND-NAME> in PATH.\n"
        ;

        printf("%s\n", str);
    } else {
        const char * str = ""
        "nvm2 (Universal Prebuild Package Manager)\n\n"
        "nvm2 <ACTION> [ARGUMENT...]\n\n"
        "nvm2 --help\n"
        "nvm2 -h\n"
        "    show help of this command.\n\n"
        "nvm2 --version\n"
        "nvm2 -V\n"
        "    show version of this command.\n\n"
        "nvm2 sysinfo\n"
        "    show your system's information.\n\n"
        "nvm2 env\n"
        "    show your system's information and other information.\n\n"
        "nvm2 integrate zsh [-v] [--output-dir=<DIR>]\n"
        "    download a zsh completion script file to a approprivate location.\n\n"
        "    to apply this feature, you may need to run the command 'autoload -U compinit && compinit' in your terminal (your current running shell must be zsh).\n\n"
        "nvm2 info <VERSION-NAME>\n"
        "    show information of the given version.\n\n"
        "nvm2 tree <VERSION-NAME> [--dirsfirst | -L N]\n"
        "    list files of the given installed version in a tree-like format.\n\n"
        "nvm2   install <VERSION-NAME>\n"
        "    install the given versions.\n\n"
        "nvm2 reinstall <VERSION-NAME>\n"
        "    reinstall the given versions.\n\n"
        "nvm2 uninstall <VERSION-NAME>\n"
        "    uninstall the given versions.\n\n"
        "nvm2 ls-available [-v]\n"
        "    list the available versions.\n\n"
        "nvm2 ls-installed [-v]\n"
        "    list the installed versions.\n\n"
        "nvm2 cleanup\n"
        "    cleanup the unused cache.\n\n\n"
        "nvm2 util zlib-deflate -L <LEVEL> < input/file/path\n"
        "    compress data using zlib deflate algorithm.\n\n"
        "    LEVEL >= 1 && LEVEL <= 9\n\n"
        "    The smaller the LEVEL, the faster the speed and the lower the compression ratio.\n\n"
        "nvm2 util zlib-inflate < input/file/path\n"
        "    decompress data using zlib inflate algorithm.\n\n"
        "nvm2 util base16-encode <STR>\n"
        "    encode <STR> using base16 algorithm.\n\n"
        "nvm2 util base16-encode < input/file/path\n"
        "    encode data using base16 algorithm.\n\n"
        "nvm2 util base16-decode <BASE16-ENCODED-SUM>\n"
        "    decode <BASE16-ENCODED-SUM> using base16 algorithm.\n\n"
        "nvm2 util base16-decode < input/file/path\n"
        "    decode data using base16 algorithm.\n\n"
        "nvm2 util base64-encode <STR>\n"
        "    encode <STR> using base64 algorithm.\n\n"
        "nvm2 util base64-encode < input/file/path\n"
        "    encode data using base64 algorithm.\n\n"
        "nvm2 util base64-decode <BASE64-ENCODED-SUM>\n"
        "    decode <BASE64-ENCODED-SUM> using base64 algorithm.\n\n"
        "nvm2 util base64-decode < input/file/path\n"
        "    decode data using base64 algorithm.\n\n"
        "nvm2 util sha256sum <input/file/path>\n"
        "    calculate sha256sum of file.\n\n"
        "nvm2 util sha256sum < input/file/path\n"
        "    calculate sha256sum of file.\n\n"
        "nvm2 util which <COMMAND-NAME> [-a]\n"
        "    find <COMMAND-NAME> in PATH.\n"
        ;

        printf("%s\n", str);
    }

    return 0;
}
