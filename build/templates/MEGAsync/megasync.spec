Name:		megasync
Version:	MEGASYNC_VERSION
Release:	%(cat MEGA_BUILD_ID || echo "1").1
Summary:	Easy automated syncing between your computers and your MEGA cloud drive
License:	Freeware
Group:		Applications/Others
Url:		https://mega.nz
Source0:	megasync_%{version}.tar.gz
Vendor:		MEGA Limited
Packager:	MEGA Linux Team <linux@mega.co.nz>

BuildRequires: zlib-devel, autoconf, automake, libtool, gcc-c++
BuildRequires: hicolor-icon-theme, unzip, wget
BuildRequires: ffmpeg-mega

#OpenSUSE
%if 0%{?suse_version} || 0%{?sle_version}

    BuildRequires: libopenssl-devel, sqlite3-devel
    BuildRequires: libbz2-devel
    BuildRequires: libudev-devel

    # disabling post-build-checks that ocassionally prevent opensuse rpms from being generated
    # plus it speeds up building process
    #!BuildIgnore: post-build-checks

    %if 0%{?sle_version} >= 150000
        BuildRequires: libcurl4
    %endif

    %if 0%{?suse_version}>=1550
        BuildRequires: pkgconf-pkg-config
    %else
        BuildRequires: pkg-config
    %endif

    %if 0%{?suse_version}>=1550 || (0%{?is_opensuse} && 0%{?sle_version} > 120300 )
        BuildRequires: c-ares-devel
    %else
        BuildRequires: libcares-devel
    %endif

    %if !( ( "%{_target_cpu}" == "i586" && ( 0%{?sle_version} == 120200 || 0%{?sle_version} == 120300) ) || 0%{?suse_version} == 1230 )
        BuildRequires: libraw-devel
    %endif
        BuildRequires: update-desktop-files

    %if 0%{?sle_version} >= 120200 || 0%{?suse_version} > 1320
        BuildRequires: libqt5-qtbase-devel, libqt5-linguist, libqt5-qtsvg-devel, libqt5-qtx11extras-devel
        Requires: libQt5Core5
    %else
        BuildRequires: libqt4-devel, qt-devel
    %endif

    %if 0%{?suse_version} <= 1320
        BuildRequires: libcryptopp-devel
    %endif

%else

    %if 0%{?rhel_version} == 0 && 0%{?centos_version} < 800
        #if !RHEL
        BuildRequires: LibRaw-devel
    %endif

%endif

%if 0%{?is_opensuse} && (0%{?sle_version} && 0%{?sle_version} <= 120300)
    BuildRequires: gcc5, gcc5-c++
%endif

#Fedora specific
%if 0%{?fedora}
    BuildRequires: openssl-devel, sqlite-devel, c-ares-devel
    BuildRequires: desktop-file-utils
    BuildRequires: bzip2-devel
    BuildRequires: systemd-devel

    %if 0%{?fedora_version} < 33
        BuildRequires: cryptopp-devel
    %endif

    %if 0%{?fedora_version} >= 26
        Requires: cryptopp >= 5.6.5
    %endif
    %if 0%{?fedora_version}==25
        BuildRequires: lz4-libs
    %endif

    %if 0%{?fedora_version} >= 31
        BuildRequires: fonts-filesystem
    %else
        BuildRequires: fontpackages-filesystem
    %endif

    %if 0%{?fedora_version} >= 23
        BuildRequires: qt5-qtbase-devel qt5-qttools-devel, qt5-qtsvg-devel, qt5-qtx11extras-devel
        Requires: qt5-qtbase >= 5.6, qt5-qtsvg
        BuildRequires: terminus-fonts
    %else
        BuildRequires: qt, qt-x11, qt-devel
        BuildRequires: terminus-fonts
    %endif
%endif

#centos/scientific linux
%if 0%{?centos_version} || 0%{?scientificlinux_version}
    BuildRequires: openssl-devel, sqlite-devel, c-ares-devel, bzip2-devel
    BuildRequires: desktop-file-utils
    BuildRequires: systemd-devel

    %if 0%{?centos_version} >= 800
        BuildRequires: bzip2-devel
        BuildRequires: qt5-qtbase-devel qt5-qttools-devel, qt5-qtsvg-devel, qt5-qtx11extras-devel
    %else
        BuildRequires: qt-mega, mesa-libGL-devel
        Requires: freetype >= 2.8
    %endif
%endif

#red hat
%if 0%{?rhel_version}
    BuildRequires: openssl-devel, sqlite-devel, desktop-file-utils
    %if 0%{?rhel_version} < 800
        BuildRequires: qt, qt-x11, qt-devel
    %else
        BuildRequires: qt5-qtbase-devel qt5-qttools-devel, qt5-qtsvg-devel, qt5-qtx11extras-devel
        BuildRequires: bzip2-devel
    %endif
%endif


#Pdfium: required for 64 bits only
%if "%{_target_cpu}" != "i586" &&  "%{_target_cpu}" != "i686"
    BuildRequires: pdfium-mega
%endif

### Specific buildable dependencies ###

#Media info
%define flag_disablemediainfo -i

%if 0%{?fedora_version}==21 || 0%{?fedora_version}==22 || 0%{?fedora_version}>=25 || !(0%{?sle_version} < 120300)
    BuildRequires: libzen-devel, libmediainfo-devel
%endif

%if 0%{?fedora_version}==19 || 0%{?fedora_version}==20 || 0%{?fedora_version}==23 || 0%{?fedora_version}==24 || 0%{?centos_version} || 0%{?scientificlinux_version} || 0%{?rhel_version} || ( 0%{?suse_version} && 0%{?sle_version} < 120300)
    %define flag_disablemediainfo %{nil}
%endif

#Build cryptopp?
%define flag_cryptopp %{nil}

%if 0%{?centos_version} || 0%{?scientificlinux_version} || 0%{?rhel_version} || 0%{?suse_version} > 1320 || 0%{?fedora_version} >= 33
    %define flag_cryptopp -q
%endif

#Build cares?
%define flag_cares %{nil}

%if 0%{?rhel_version}
    %define flag_cares -e
%endif

#Build libraw?
%define flag_libraw %{nil}

%if ( "%{_target_cpu}" == "i586" && ( 0%{?sle_version} == 120200 || 0%{?sle_version} == 120300) ) || 0%{?centos_version} >= 800 || 0%{?rhel_version}
    %define flag_libraw -W
%endif

#Build zlib?
%define flag_disablezlib %{nil}

%if 0%{?fedora_version} == 23
    %define flag_disablezlib -z
%endif

%description
Secure:
Your data is encrypted end to end. Nobody can intercept it while in storage or in transit.

Flexible:
Sync any folder from your PC to any folder in the cloud. Sync any number of folders in parallel.

Fast:
Take advantage of MEGA's high-powered infrastructure and multi-connection transfers.

Generous:
Store up to 50 GB for free!

%prep
%setup -q

mega_build_id=`echo %{release} | sed "s/\.[^.]*$//" | sed "s/[^.]*\.//" | sed "s/[^0-9]//g"`
sed -i -E "s/USER_AGENT([^\/]*)\/(([0-9][0-9]*\.){3})(.*)\";/USER_AGENT\1\/\2${mega_build_id}\";/g" MEGASync/control/Preferences.cpp;
sed -i -E "s/BUILD_ID = ([0-9]*)/BUILD_ID = ${mega_build_id}/g" MEGASync/control/Preferences.cpp;

%build

%if 0%{?is_opensuse} && (0%{?sle_version} && 0%{?sle_version} <= 120300)
    # ln to gcc/g++ v5, instead of default 4.8
    mkdir userPath
    ln -sf /usr/bin/gcc-5 userPath/gcc
    ln -sf /usr/bin/g++-5 userPath/g++
    export PATH=`pwd`/userPath:$PATH
%endif

%if 0%{?centos_version} && 0%{?centos_version} < 800
    export PATH=/opt/mega/bin:${PATH}
%endif

export DESKTOP_DESTDIR=$RPM_BUILD_ROOT/usr

./configure %{flag_cryptopp} -g %{flag_disablezlib} %{flag_cares} %{flag_disablemediainfo} %{flag_libraw}

# Link dynamically with freeimage
ln -sfr $PWD/MEGASync/mega/bindings/qt/3rdparty/libs/libfreeimage*.so $PWD/MEGASync/mega/bindings/qt/3rdparty/libs/libfreeimage.so.3
ln -sfn libfreeimage.so.3 $PWD/MEGASync/mega/bindings/qt/3rdparty/libs/libfreeimage.so

# Fedora uses system Crypto++ header files
%if 0%{?fedora_version} < 33
    rm -fr MEGASync/mega/bindings/qt/3rdparty/include/cryptopp
%endif

%if ( 0%{?fedora_version} && 0%{?fedora_version}<=32 ) || ( 0%{?centos_version} == 600 ) || ( 0%{?sle_version} && 0%{?sle_version} < 150000 )
    %define extraqmake DEFINES+=MEGASYNC_DEPRECATED_OS
%else
    %define extraqmake %{nil}
%endif

%if 0%{?fedora_version} >= 35
    %define extraconfig CONFIG+=FFMPEG_WITH_LZMA
%else
    %define extraconfig %{nil}
%endif


%if 0%{?suse_version} != 1230
    %define fullreqs "CONFIG += FULLREQUIREMENTS"
%else
    sed -i "s/USE_LIBRAW/NOT_USE_LIBRAW/" MEGASync/MEGASync.pro
    %define fullreqs %{nil}
%endif


%if 0%{?fedora} || 0%{?sle_version} >= 120200 || 0%{?suse_version} > 1320 || 0%{?rhel_version} >=800 || 0%{?centos_version} >=800
    %if 0%{?fedora_version} >= 23 || 0%{?sle_version} >= 120200 || 0%{?suse_version} > 1320 || 0%{?rhel_version} >=800 || 0%{?centos_version} >=800
        qmake-qt5 %{fullreqs} DESTDIR=%{buildroot}%{_bindir} THE_RPM_BUILD_ROOT=%{buildroot} %{extraqmake} QMAKE_RPATHDIR="/opt/mega/lib" %{extraconfig}
        lrelease-qt5  MEGASync/MEGASync.pro
    %else
        qmake-qt4 %{fullreqs} DESTDIR=%{buildroot}%{_bindir} THE_RPM_BUILD_ROOT=%{buildroot} %{extraqmake} QMAKE_RPATHDIR="/opt/mega/lib" %{extraconfig}
        lrelease-qt4  MEGASync/MEGASync.pro
    %endif
%else
    %if 0%{?rhel_version} || 0%{?scientificlinux_version}
        qmake-qt4 %{fullreqs} DESTDIR=%{buildroot}%{_bindir} THE_RPM_BUILD_ROOT=%{buildroot} %{extraqmake} QMAKE_RPATHDIR="/opt/mega/lib" %{extraconfig}
        lrelease-qt4  MEGASync/MEGASync.pro
    %else
        qmake %{fullreqs} DESTDIR=%{buildroot}%{_bindir} THE_RPM_BUILD_ROOT=%{buildroot} %{extraqmake} QMAKE_RPATHDIR="/opt/mega/lib" %{extraconfig}
        lrelease MEGASync/MEGASync.pro
    %endif
%endif

make


%install
make install DESTDIR=%{buildroot}%{_bindir}

%if 0%{?suse_version}
    %suse_update_desktop_file -n -i %{name} Network System
%else
    desktop-file-install \
        --add-category="Network" \
        --dir %{buildroot}%{_datadir}/applications \
    %{buildroot}%{_datadir}/applications/%{name}.desktop
%endif

%if 0%{?centos_version} && 0%{?centos_version} < 800
    for i in `ldd %{buildroot}%{_bindir}/megasync | grep opt | awk '{print $3}'`; do
        install -D $i %{buildroot}/$i
    done
    install -pD /opt/mega/lib/libQt5XcbQpa.so.*.*.* %{buildroot}/opt/mega/lib/libQt5XcbQpa.so.5 || :
    install -D /opt/mega/plugins/platforms/libqxcb.so %{buildroot}/opt/mega/plugins/platforms/libqxcb.so
    install -D /opt/mega/plugins/platforms/libqvnc.so %{buildroot}/opt/mega/plugins/platforms/libqvnc.so
    install -D /opt/mega/plugins/platforms/libqoffscreen.so %{buildroot}/opt/mega/plugins/platforms/libqoffscreen.so
    install -D /opt/mega/plugins/platforms/libqminimal.so %{buildroot}/opt/mega/plugins/platforms/libqminimal.so
    install -D /opt/mega/plugins/platforms/libqlinuxfb.so %{buildroot}/opt/mega/plugins/platforms/libqlinuxfb.so
    install -D /opt/mega/plugins/platforminputcontexts/libqtvirtualkeyboardplugin.so %{buildroot}/opt/mega/plugins/platforminputcontexts/libqtvirtualkeyboardplugin.so
    install -D /opt/mega/plugins/platforminputcontexts/libibusplatforminputcontextplugin.so %{buildroot}/opt/mega/plugins/platforminputcontexts/libibusplatforminputcontextplugin.so
    install -D /opt/mega/plugins/platforminputcontexts/libcomposeplatforminputcontextplugin.so %{buildroot}/opt/mega/plugins/platforminputcontexts/libcomposeplatforminputcontextplugin.so
    install -D /opt/mega/plugins/imageformats/libqwebp.so %{buildroot}/opt/mega/plugins/imageformats/libqwebp.so
    install -D /opt/mega/plugins/imageformats/libqwbmp.so %{buildroot}/opt/mega/plugins/imageformats/libqwbmp.so
    install -D /opt/mega/plugins/imageformats/libqtiff.so %{buildroot}/opt/mega/plugins/imageformats/libqtiff.so
    install -D /opt/mega/plugins/imageformats/libqtga.so %{buildroot}/opt/mega/plugins/imageformats/libqtga.so
    install -D /opt/mega/plugins/imageformats/libqsvg.so %{buildroot}/opt/mega/plugins/imageformats/libqsvg.so
    install -D /opt/mega/plugins/imageformats/libqjpeg.so %{buildroot}/opt/mega/plugins/imageformats/libqjpeg.so
    install -D /opt/mega/plugins/imageformats/libqico.so %{buildroot}/opt/mega/plugins/imageformats/libqico.so
    install -D /opt/mega/plugins/imageformats/libqicns.so %{buildroot}/opt/mega/plugins/imageformats/libqicns.so
    install -D /opt/mega/plugins/imageformats/libqgif.so %{buildroot}/opt/mega/plugins/imageformats/libqgif.so
    install -D /opt/mega/plugins/iconengines/libqsvgicon.so %{buildroot}/opt/mega/plugins/iconengines/libqsvgicon.so
    install -D /opt/mega/plugins/bearer/libqnmbearer.so %{buildroot}/opt/mega/plugins/bearer/libqnmbearer.so
    install -D /opt/mega/plugins/bearer/libqgenericbearer.so %{buildroot}/opt/mega/plugins/bearer/libqgenericbearer.so
    install -D /opt/mega/plugins/bearer/libqconnmanbearer.so %{buildroot}/opt/mega/plugins/bearer/libqconnmanbearer.so

    install -D /opt/mega/lib/libQt5VirtualKeyboard.so.*.*.* %{buildroot}/opt/mega/lib/libQt5VirtualKeyboard.so.5
    install -D /opt/mega/lib/libQt5Qml.so.*.*.* %{buildroot}/opt/mega/lib/libQt5Qml.so.5
    install -D /opt/mega/lib/libQt5Quick.so.*.*.* %{buildroot}/opt/mega/lib/libQt5Quick.so.5

%endif

mkdir -p  %{buildroot}/opt/mega/lib
install -D MEGASync/mega/bindings/qt/3rdparty/libs/libfreeimage.so.* %{buildroot}/opt/mega/lib

mkdir -p  %{buildroot}/etc/sysctl.d/
echo "fs.inotify.max_user_watches = 524288" > %{buildroot}/etc/sysctl.d/99-megasync-inotify-limit.conf

mkdir -p  %{buildroot}/etc/udev/rules.d/
echo "SUBSYSTEM==\"block\", ATTRS{idDevtype}==\"partition\"" > %{buildroot}/etc/udev/rules.d/99-megasync-udev.rules

%post
%if 0%{?suse_version} >= 1140
    %desktop_database_post
    %icon_theme_cache_post
%else
    /bin/touch --no-create %{_datadir}/icons/hicolor &>/dev/null || :
    /bin/touch --no-create %{_datadir}/icons/ubuntu-mono-dark &>/dev/null || :
%endif


## Configure repository ##

%if 0%{?fedora}

    YUM_FILE="/etc/yum.repos.d/megasync.repo"
    cat > "$YUM_FILE" << DATA
[MEGAsync]
name=MEGAsync
baseurl=https://mega.nz/linux/MEGAsync/Fedora_\$releasever/
gpgkey=https://mega.nz/linux/MEGAsync/Fedora_\$releasever/repodata/repomd.xml.key
gpgcheck=1
enabled=1
DATA

%endif

%if 0%{?rhel_version} || 0%{?centos_version} || 0%{?scientificlinux_version}

    %if 0%{?rhel_version} == 800
        %define reponame RHEL_8
    %endif

    %if 0%{?rhel_version} == 700
        %define reponame RHEL_7
    %endif

    %if 0%{?scientificlinux_version} == 700
        %define reponame ScientificLinux_7
    %endif

    %if 0%{?centos_version} == 700
        %define reponame CentOS_7
    %endif

    %if 0%{?centos_version} == 800
        %define reponame CentOS_8
    %endif

    YUM_FILE="/etc/yum.repos.d/megasync.repo"
    cat > "$YUM_FILE" << DATA
[MEGAsync]
name=MEGAsync
baseurl=https://mega.nz/linux/MEGAsync/%{reponame}/
gpgkey=https://mega.nz/linux/MEGAsync/%{reponame}/repodata/repomd.xml.key
gpgcheck=1
enabled=1
DATA

%endif

%if 0%{?sle_version} || 0%{?suse_version}
    %if 0%{?sle_version} == 120300
        %define reponame openSUSE_Leap_42.3
    %endif

    %if 0%{?sle_version} == 120200
        %define reponame openSUSE_Leap_42.2
    %endif

    %if 0%{?sle_version} == 120100
        %define reponame openSUSE_Leap_42.1
    %endif

    %if 0%{?sle_version} == 150000 || 0%{?sle_version} == 150100 || 0%{?sle_version} == 150200
        %define reponame openSUSE_Leap_15.0
    %endif

    %if 0%{?sle_version} == 150300
        %define reponame openSUSE_Leap_15.3
    %endif

    %if 0%{?sle_version} == 0 && 0%{?suse_version} >= 1550
        %define reponame openSUSE_Tumbleweed
    %endif

    %if 0%{?suse_version} == 1320
        %define reponame openSUSE_13.2
    %endif

    %if 0%{?suse_version} == 1310
        %define reponame openSUSE_13.1
    %endif

    %if 0%{?suse_version} == 1230
        %define reponame openSUSE_12.3
    %endif
    %if 0%{?suse_version} == 1220
        %define reponame openSUSE_12.2
    %endif

    if [ -d "/etc/zypp/repos.d/" ]; then
        ZYPP_FILE="/etc/zypp/repos.d/megasync.repo"
        cat > "$ZYPP_FILE" << DATA
[MEGAsync]
name=MEGAsync
type=rpm-md
baseurl=https://mega.nz/linux/MEGAsync/%{reponame}/
gpgcheck=1
autorefresh=1
gpgkey=https://mega.nz/linux/MEGAsync/%{reponame}/repodata/repomd.xml.key
enabled=1
DATA
    fi

%endif

### include public signing key #####
# Install new key if it's not present
# Notice, for openSuse, postinst is checked (and therefore executed) when creating the rpm
# we need to ensure no command results in fail (returns !=0)
rpm -q gpg-pubkey-7f068e5d-563dc081 > /dev/null 2>&1 || KEY_NOT_FOUND=1

if [ ! -z "$KEY_NOT_FOUND" ]; then

    KEYFILE=$(mktemp /tmp/megasync.XXXXXX || :)
    if [ -n "$KEYFILE" ]; then

            cat > "$KEYFILE" <<KEY || :
-----BEGIN PGP PUBLIC KEY BLOCK-----
Version: GnuPG v2

mI0EVj3AgQEEAO2XyJgpvE5HDRVsggcrMhf5+KpQepl7m7OyrPSgxLi72Wuy5GWp
hO64BX1UzmdUirIEOc13YxdeuhwJ3YP0wnKUyUrdWA0r2HjOz555vN6ldrPlSCBI
RxKBWRMQaR4cwNKQ8V4xV9tVdPGgrQ9L/4H3fM9fYqCwEMKBxxLZsF3PABEBAAG0
IE1lZ2FMaW1pdGVkIDxzdXBwb3J0QG1lZ2EuY28ubno+iL8EEwECACkFAlY9wIEC
GwMFCRLMAwAHCwkIBwMCAQYVCAIJCgsEFgIDAQIeAQIXgAAKCRADw606fwaOXfOS
A/998rh6f0wsrHmX2LTw2qmrWzwPj4m+vp0m3w5swPZw1x4qSNsmNsIXX8J0ZcSE
qymOwHZ0B9imBS3iz+U496NSfPNWABbIBnUAu8zq0IR28Q9pUcLe5MWFsw9NO+w2
5dByoN9JKeUftZt1x76NHn5wmxB9fv7WVlCnZJ+T16+nh7iNBFY9wIEBBADHpopM
oXNkrGZLI6Ok1F5N7+bSgiyZwkvBMAqCkPawUgwJztFKGf8F/sSbydsKRC2aQcuJ
eOj0ZPUtJ80+o3w8MsHRtZDSxDIxqqiHeupoDRI3Be9S544vI5/UmiiygTuhmNTT
NWgStoZz7hEK4IsELAG1EFodIMtBSkptDL92HwARAQABiKUEGAECAA8FAlY9wIEC
GwwFCRLMAwAACgkQA8OtOn8Gjl3HlAQAoOckF5JBJWekmlX+k2RYwtgfszk31Gq+
Jjiho4rUEW8c1EUPvK8v1jRGwjYED3ihJ6510eblYFPl+6k91OWlScnxuVVAmSn4
35RW3vR+nYUvf3s8rctbw97gJJZAA7p5oAowTux3oHotKSYhhxKcz15goMXzSb5G
/h7fJRhMnw4=
=fp/e
-----END PGP PUBLIC KEY BLOCK-----
KEY

        mv /var/lib/rpm/.rpm.lock /var/lib/rpm/.rpm.lock_moved || : #to allow rpm import within postinst

        %if 0%{?suse_version}
            #Key import would hang and fail due to lock in /var/lib/rpm/Packages. We create a copy
            mv /var/lib/rpm/Packages{,_moved}
            cp /var/lib/rpm/Packages{_moved,}
        %endif

        rpm --import "$KEYFILE" 2>&1 || FAILED_IMPORT=1
        %if 0%{?suse_version}
            rm /var/lib/rpm/Packages_moved  #remove the old one
        %endif

        mv /var/lib/rpm/.rpm.lock_moved /var/lib/rpm/.rpm.lock || : #take it back

        rm $KEYFILE || :
    fi
fi

sysctl -p /etc/sysctl.d/99-megasync-inotify-limit.conf

### END of POSTINST


%preun
[ "$1" == "1" ] && killall -s SIGUSR1 megasync 2> /dev/null || true
sleep 2


%postun
%if 0%{?suse_version} >= 1140
    %desktop_database_postun
    %icon_theme_cache_postun
%else
    if [ $1 -eq 0 ] ; then
        /bin/touch --no-create %{_datadir}/icons/hicolor &>/dev/null || :
        /bin/touch --no-create %{_datadir}/icons/ubuntu-mono-dark &>/dev/null || :
        /usr/bin/gtk-update-icon-cache %{_datadir}/icons/* &>/dev/null || :
    fi
%endif

# kill running MEGAsync instance when uninstall (!upgrade)
[ "$1" == "0" ] && killall megasync 2> /dev/null || true


%posttrans
%if 0%{?fedora} || 0%{?rhel_version} || 0%{?centos_version} || 0%{?scientificlinux_version}
    /bin/touch --no-create %{_datadir}/icons/hicolor &>/dev/null || :
    /bin/touch --no-create %{_datadir}/icons/ubuntu-mono-dark &>/dev/null || :
    /usr/bin/gtk-update-icon-cache %{_datadir}/icons/* &>/dev/null || :
%endif
# to restore dormant MEGAsync upon updates
killall -s SIGUSR2 megasync 2> /dev/null || true


%clean
%{?buildroot:%__rm -rf "%{buildroot}"}


%files
%defattr(-,root,root)
%{_bindir}/%{name}
%{_datadir}/applications/megasync.desktop
%{_datadir}/icons/hicolor/*/*/mega.png
%{_datadir}/icons/hicolor/*/*/*
%{_datadir}/icons/*/*/*/*
%{_datadir}/doc/megasync
%{_datadir}/doc/megasync/*
/etc/sysctl.d/99-megasync-inotify-limit.conf
/etc/udev/rules.d/99-megasync-udev.rules
/opt/*

%changelog
