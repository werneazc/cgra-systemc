#!bin/bash

# Automatically install required packages, gSysC, SystemC and
# set environment variables. 

# Make sure that the whole script is running without root privileges  
if [[ $EUID -eq 0 ]]; then
    echo    "Please run do not run the whole script with root privileges."
    echo -e "Use something like [bash install_requirements.sh] instead.\n"
    exit 1
fi

echo "Running setup. This may take some time..."
sleep 2

DISTRO=$(cat /etc/os-release \
        | awk '{split($0,a,"\""); print a[2]}' \
        | head -n 1)

# Ubuntu support
if [[ $DISTRO == 'Ubuntu' ]]; then
    sudo apt update -y || sudo apt upgrade -y
    sudo apt install build-essential libssl-dev clang qt5-default git cmake

    wget https://www.accellera.org/images/downloads/standards/systemc/systemc-2.3.3.tar.gz -O /tmp/systemc-2.3.3.tar.gz
    tar -C /opt -xvf /tmp/systemc-2.3.3.tar.gz 
    sudo ln -s /usr/bin/make /usr/bin/gmake
    pushd /opt/systemc-2.3.3 && mkdir objdir && cd objdir && ../configure --prefix=/opt/systemc-2 --with-unix-layout --enable-pthreads && gmake && sudo gmake install && popd
    git clone https://github.com/werneazc/gsysc.git /opt/gsysc
    mkdir -p /opt/gsysc/build
    pushd /opt/gsysc && cmake -B build -S ./ -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_STANDARD=14 -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles"
    cmake --build ./build -- -j2 && popd

    echo "" >> ~/.bashrc
    echo "#Environment variables:" >> ~/.bashrc
    echo "SYSTEMC_ROOT=\"/opt/systemc-2\"" >> ~/.bashrc
    echo "GSYSC_ROOT=\"/opt/gsysc\"" >> ~/.bashrc
    echo "CXX=${CXX:-clang++}" >> ~/.bashrc
    echo "CXX_FOR_BUILD=${CXX_FOR_BUILD:-clang++}" >> ~/.bashrc
    echo "CC=${CC:-clang}" >> ~/.bashrc
    echo "CC_FOR_BUILD=${CC_FOR_BUILD:-clang}" >> ~/.bashrc
    echo

    read -p "Restart now to complete the setup? " ANSWER

    if [ $ANSWER == "y" ] ||
       [ $ANSWER == "yes" ] ||
       [ $ANSWER == "Y" ] ||
       [ $ANSWER == "YES" ] ||
       [ $ANSWER == "Yes" ] ||
       [ $ANSWER == "" ] 
    then
        reboot
    fi
fi

