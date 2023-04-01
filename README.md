# nvm2
yet another Node Version Manager written in C.

<br>

**Note**: This project is being actively developed. It's in beta stage and may not be stable. Some features are subject to change without notice.

## dependences
|dependency|required?|purpose|
|----|---------|-------|
|[cmake](https://cmake.org/)|required |for generating `build.ninja`|
|[ninja](https://ninja-build.org/)|required |for doing jobs that read from `build.ninja`|
|[pkg-config>=0.18](https://www.freedesktop.org/wiki/Software/pkg-config/)|required|for finding libraries.|
||||
|[jansson](https://github.com/akheron/jansson)|required|for parsing and creating JSON.|
|[libyaml](https://github.com/yaml/libyaml/)|required|for parsing formulas whose format is YAML.|
|[libcurl](https://curl.se/)|required|for http requesting support.|
|[openssl](https://www.openssl.org/)|required|for https requesting support and SHA-256 sum checking support.|
|[libarchive](https://www.libarchive.org/)|required|for uncompressing .zip and .tar.* files.|
|[zlib](https://www.zlib.net/)|required|for compressing and uncompressing.|
|[pcre2](https://www.pcre.org/)||for Regular Expressions support.<br>only required on OpenBSD.|


## build and install nvm2 via [ppkg](https://github.com/leleliu008/ppkg)
```bash
ppkg install nvm2
```

## build and install nvm2 using [vcpkg](https://github.com/microsoft/vcpkg)

**Note:** This is the recommended way to build and install nvm2.

```bash
# install g++ curl zip unzip tar git

git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg install curl openssl libarchive libyaml jansson

cd -

git clone https://github.com/leleliu008/nvm2
cd nvm2

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
cmake --build   build.d
cmake --install build.d
```

## build and install nvm2 using your system's default package manager

**[Ubuntu](https://ubuntu.com/)**

```bash
apt -y update
apt -y install git cmake ninja-build pkg-config gcc libcurl4 libcurl4-openssl-dev libarchive-dev libyaml-dev libjansson-dev

git clone https://github.com/leleliu008/nvm2
cd nvm2

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[Fedora](https://getfedora.org/)**

```bash
dnf -y update
dnf -y install git cmake ninja-build pkg-config gcc libcurl-devel libarchive-devel libyaml-devel jansson-devel

git clone https://github.com/leleliu008/nvm2
cd nvm2

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[ArchLinux](https://archlinux.org/)**

```bash
pacman -Syyuu --noconfirm
pacman -S     --noconfirm git cmake ninja pkg-config gcc curl openssl libarchive libyaml jansson

git clone https://github.com/leleliu008/nvm2
cd nvm2

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[AlpineLinux](https://www.alpinelinux.org/)**

```bash
apk add git cmake ninja pkgconf gcc libc-dev curl-dev openssl-dev libarchive-dev yaml-dev jansson-dev

git clone https://github.com/leleliu008/nvm2
cd nvm2

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[VoidLinux](https://voidlinux.org/)**

```bash
xbps-install -Suy xbps
xbps-install -Suy cmake ninja gcc pkg-config libcurl-devel libarchive-devel libyaml-devel jansson-devel

git clone https://github.com/leleliu008/nvm2
cd nvm2

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[Gentoo Linux](https://www.gentoo.org/)**

```bash
emerge dev-vcs/git cmake dev-util/ninja gcc pkg-config net-misc/curl libarchive dev-libs/libyaml dev-libs/jansson

git clone https://github.com/leleliu008/nvm2
cd nvm2

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[openSUSE](https://www.opensuse.org/)**

```bash
zypper update  -y  
zypper install -y git cmake ninja gcc pkg-config libcurl-devel libarchive-devel libyaml-devel libjansson-devel

git clone https://github.com/leleliu008/nvm2
cd nvm2

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[macOS](https://www.apple.com/macos/)**

```bash
brew update
brew install git cmake pkg-config ninja curl jansson libyaml libarchive

git clone https://github.com/leleliu008/nvm2
cd nvm2

export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:/usr/local/opt/openssl@1.1/lib/pkgconfig:/usr/local/opt/curl/lib/pkgconfig:/usr/local/opt/libarchive/lib/pkgconfig"

CMAKE_EXE_LINKER_FLAGS='-L/usr/local/lib -L/usr/local/opt/openssl@1.1/lib -lssl -liconv -framework CoreFoundation -framework Security'
CMAKE_FIND_ROOT_PATH="$(brew --prefix openssl@1.1);$(brew --prefix curl);$(brew --prefix libarchive)"

cmake \
    -S . \
    -B build.d \
    -G Ninja \
    -DCMAKE_INSTALL_PREFIX=./output \
    -DCMAKE_EXE_LINKER_FLAGS="$CMAKE_EXE_LINKER_FLAGS" \
    -DCMAKE_FIND_ROOT_PATH="$CMAKE_FIND_ROOT_PATH"

cmake --build   build.d
cmake --install build.d
```

**[FreeBSD](https://www.freebsd.org/)**

```bash
pkg install -y git cmake ninja pkgconf gcc curl openssl libarchive libyaml jansson

git clone https://github.com/leleliu008/nvm2
cd nvm2

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[OpenBSD](https://www.openbsd.org/)**

```bash
pkg_add git cmake ninja pkgconf llvm curl libarchive libyaml jansson

git clone https://github.com/leleliu008/nvm2
cd nvm2

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```

**[NetBSD](https://www.netbsd.org/)**

```bash
pkgin -y install git mozilla-rootcerts cmake ninja-build pkg-config clang curl openssl libarchive libyaml jansson

mozilla-rootcerts install

git clone https://github.com/leleliu008/nvm2
cd nvm2

cmake -S . -B   build.d -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build   build.d
cmake --install build.d
```


## ~/.nvm2
all relevant dirs and files are located in `~/.nvm2` directory.

**Note**: Please do NOT place your own files in `~/.nvm2` directory, as `nvm2` will change files in `~/.nvm2` directory without notice.

## nvm2 command usage
*   **show help of this command**
        
        nvm2 -h
        nvm2 --help
        
*   **show version of this command**

        nvm2 -V
        nvm2 --version
        
*   **show your system's information**

        nvm2 sysinfo

*   **show your system's information and other information**

        nvm2 env
        
*   **integrate `zsh-completion` script**

        nvm2 integrate zsh
        nvm2 integrate zsh --output-dir=/usr/local/share/zsh/site-functions
        nvm2 integrate zsh -v
        
    I provide a zsh-completion script for `nvm2`. when you've typed `nvm2` then type `TAB` key, the rest of the arguments will be automatically complete for you.

    **Note**: to apply this feature, you may need to run the command `autoload -U compinit && compinit` in your terminal (your current running shell must be zsh).

        
*   **upgrade this software**

        nvm2 upgrade-self
        nvm2 upgrade-self -v
        
*   **show Node.js Release Schedule**
        
        nvm2 show-release-schedule
        nvm2 show-release-schedule -v
        
*   **list all available versions**
        
        nvm2 ls-available
        nvm2 ls-available -v
        
*   **list all available LTS(Long Term Support) versions**
        
        nvm2 ls-available --lts
        
*   **list all installed versions**
        
        nvm2 ls-installed
        nvm2 ls-installed -v
        
*   **check if the given version is available ?**
        
        nvm2 is-available 8.15.0
        
*   **check if the given version is installed ?**
        
        nvm2 is-installed 8.15.0
        
*   **install the given version**
        
        nvm2 install 8.15.0
        nvm2 install 8.15.0 -v
        nvm2 install 8.15.0 -v --from-source --site=https://mirrors.tuna.tsinghua.edu.cn/nodejs-release
        
*   **reinstall the given version**
        
        nvm2 reinstall 8.15.0
        nvm2 reinstall 8.15.0 -v
        
*   **uninstall the given version**

        nvm2 uninstall 8.15.0
        nvm2 uninstall 8.15.0 -v
        
*   **show information of the given installed version**
        
        nvm2 info 8.15.0
        nvm2 info 19.8.0 --yaml
        nvm2 info 19.8.0 --json
        nvm2 info 19.8.0 installed-dir
        nvm2 info 19.8.0 installed-timestamp-unix
        nvm2 info 19.8.0 installed-timestamp-iso-8601
        nvm2 info 19.8.0 installed-timestamp-rfc-3339
        
*   **list the installed files of the given installed version in a tree-like format**
        
        nvm2 tree 8.15.0
        nvm2 tree 8.15.0 -L 3
        
*   **run any arbitrary command in a subshell with the desired version of node**
        
        nvm2 exec 8.15.0 node --version
        nvm2 exec 8.15.0 npm  --version
        
*   **extra common used utilities**
        
        nvm2 util zlib-deflate -L 6 < input/file/path
        nvm2 util zlib-inflate      < input/file/path

        nvm2 util base16-encode "string to be encoded with base16 algorithm"
        nvm2 util base16-encode < input/file/path

        nvm2 util base16-decode ABCD
        nvm2 util base16-decode ABCD > output/file/path

        nvm2 util base64-encode "string to be encoded with base64 algorithm"
        nvm2 util base64-encode < input/file/path

        nvm2 util base64-decode YQ==
        nvm2 util base64-decode YQ== > output/file/path

        nvm2 util sha256sum   input/file/path
        nvm2 util sha256sum < input/file/path

        nvm2 util which tree
        nvm2 util which tree -a
        
*   **delete the unused cached files**
        
        nvm2 cleanup
        

## environment variables

*   **HOME**

    this environment variable already have been set on most systems, if not set or set a empty string, you will receive an error message.

*   **PATH**

    this environment variable already have been set on most systems, if not set or set a empty string, you will receive an error message.

*   **SSL_CERT_FILE**

    ```bash
    curl -LO https://curl.se/ca/cacert.pem
    export SSL_CERT_FILE="$PWD/cacert.pem"
    ```

    In general, you don't need to set this environment variable, but, if you encounter the reporting `the SSL certificate is invalid`, trying to run above commands in your terminal will do the trick.
