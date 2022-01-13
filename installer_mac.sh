#!/bin/zsh -e

Usage () {
    echo "Usage: installer_mac.sh [[--build | --build-cmake] | [--sign] | [--create-dmg] | [--notarize] | [--full-pkg | --full-pkg-cmake]]"
    echo "    --build          : Builds the app and creates the bundle using qmake."
    echo "    --build-cmake    : Idem but using cmake"
    echo "    --sign           : Sign the app"
    echo "    --create-dmg     : Create the dmg package"
    echo "    --notarize       : Notarize package against Apple systems."
    echo "    --full-pkg       : Implies and overrides all the above using qmake"
    echo "    --full-pkg-cmake : Idem but using cmake"
    echo ""
    echo "Environment variables needed to build:"
    echo "    MEGAQTPATH : Point it to a valid Qt installation path"
    echo "    VCPKGPATH : Point it to a directory containing a valid vcpkg installation"
    echo ""
    echo "Note: --build and --build-cmake are mutually exclusive."
    echo "      --full-pkg and --full-pkg-cmake are mutually exclusive."
    echo ""
}

if [ $# -eq 0 ]; then
   Usage
   exit 1
fi

APP_NAME=MEGAsync
ID_BUNDLE=mega.mac
MOUNTDIR=tmp
RESOURCES=installer/resourcesDMG
MSYNC_PREFIX=MEGASync/
MLOADER_PREFIX=MEGALoader/
MUPDATER_PREFIX=MEGAUpdater/

full_pkg=0
full_pkg_cmake=0
build=0
build_cmake=0
sign=0
createdmg=0
notarize=0

while [ "$1" != "" ]; do
    case $1 in
        --build )
            build=1
            if [ ${build_cmake} -eq 1 ]; then Usage; echo "Error: --build and --build-cmake are mutually exclusive."; exit 1; fi
            ;;
        --build-cmake )
            build_cmake=1
            if [ ${build} -eq 1 ]; then Usage; echo "Error: --build and --build-cmake are mutually exclusive."; exit 1; fi
            ;;
        --sign )
            sign=1
            ;;
        --create-dmg )
            createdmg=1
            ;;
        --notarize )
            notarize=1
            ;;
        --full-pkg )
            if [ ${full_pkg_cmake} -eq 1 ]; then Usage; echo "Error: --full-pkg and --full-pkg-cmake are mutually exclusive."; exit 1; fi
            full_pkg=1
            ;;
        --full-pkg-cmake )
            if [ ${full_pkg} -eq 1 ]; then Usage; echo "Error: --full-pkg and --full-pkg-cmake are mutually exclusive."; exit 1; fi
            full_pkg_cmake=1
            ;;
        -h | --help )
            Usage
            exit
            ;;
        * )
            Usage
            exit 1
    esac
    shift
done

if [ ${full_pkg} -eq 1 ]; then
    build=1
    build_cmake=0
    sign=1
    createdmg=1
    notarize=1
fi
if [ ${full_pkg_cmake} -eq 1 ]; then
    build=0
    build_cmake=1
    sign=1
    createdmg=1
    notarize=1
fi

if [ ${build} -eq 1 -o ${build_cmake} -eq 1 ]; then
    if [ -z "${MEGAQTPATH}" ] || [ ! -d "${MEGAQTPATH}/bin" ]; then
        echo "Please set MEGAQTPATH env variable to a valid QT installation path!"
        exit 1;
    fi
    if [ -z "${VCPKGPATH}" ] || [ ! -d "${VCPKGPATH}/vcpkg/installed" ]; then
        echo "Please set VCPKGPATH env variable to a directory containing a valid vcpkg installation!"
        exit 1;
    fi

    MEGAQTPATH="$(cd "$MEGAQTPATH" && pwd -P)"
    echo "Building with:"
    echo "  MEGAQTPATH : ${MEGAQTPATH}"
    echo "  VCPKGPATH  : ${VCPKGPATH}"

    if [ ${build_cmake} -ne 1 ]; then
        AVCODEC_VERSION=libavcodec.58.dylib
        AVFORMAT_VERSION=libavformat.58.dylib
        AVUTIL_VERSION=libavutil.56.dylib
        SWSCALE_VERSION=libswscale.5.dylib
        CARES_VERSION=libcares.2.dylib
        CURL_VERSION=libcurl.dylib

        AVCODEC_PATH=${VCPKGPATH}/vcpkg/installed/x64-osx-mega/lib/$AVCODEC_VERSION
        AVFORMAT_PATH=${VCPKGPATH}/vcpkg/installed/x64-osx-mega/lib/$AVFORMAT_VERSION
        AVUTIL_PATH=${VCPKGPATH}/vcpkg/installed/x64-osx-mega/lib/$AVUTIL_VERSION
        SWSCALE_PATH=${VCPKGPATH}/vcpkg/installed/x64-osx-mega/lib/$SWSCALE_VERSION
        CARES_PATH=${VCPKGPATH}/vcpkg/installed/x64-osx-mega/lib/$CARES_VERSION
        CURL_PATH=${VCPKGPATH}/vcpkg/installed/x64-osx-mega/lib/$CURL_VERSION
    fi

    # Clean previous build
    rm -rf Release_x64
    mkdir Release_x64
    cd Release_x64

    # Build binaries
    if [ ${build_cmake} -eq 1 ]; then
        cmake -DUSE_THIRDPARTY_FROM_VCPKG=1 -DUSE_PREBUILT_3RDPARTY=0 -DCMAKE_PREFIX_PATH=${MEGAQTPATH} -DVCPKG_TRIPLET=x64-osx-mega -DMega3rdPartyDir=${VCPKGPATH} -S ../contrib/cmake
        cmake --build ./ --target MEGAsync -j`sysctl -n hw.ncpu`
        cmake --build ./ --target MEGAloader -j`sysctl -n hw.ncpu`
        cmake --build ./ --target MEGAupdater -j`sysctl -n hw.ncpu`
        MSYNC_PREFIX=""
        MLOADER_PREFIX=""
        MUPDATER_PREFIX=""
    else
        [ ! -f src/MEGASync/mega/include/mega/config.h ] && cp ../src/MEGASync/mega/contrib/official_build_configs/macos/config.h ../src/MEGASync/mega/include/mega/config.h
        ${MEGAQTPATH}/bin/lrelease ../src/MEGASync/MEGASync.pro
        ${MEGAQTPATH}/bin/qmake "CONFIG += FULLREQUIREMENTS" "THIRDPARTY_VCPKG_BASE_PATH=${VCPKGPATH}" -r ../src -spec macx-clang CONFIG+=release CONFIG+=x86_64 -nocache
        make -j`sysctl -n hw.ncpu`
    fi

    # Prepare bundle
    cp -R ${MSYNC_PREFIX}MEGAsync.app ${MSYNC_PREFIX}MEGAsync_orig.app
    ${MEGAQTPATH}/bin/macdeployqt ${MSYNC_PREFIX}MEGAsync.app -no-strip
    dsymutil ${MSYNC_PREFIX}MEGAsync.app/Contents/MacOS/MEGAsync -o MEGAsync.app.dSYM
    strip ${MSYNC_PREFIX}MEGAsync.app/Contents/MacOS/MEGAsync
    dsymutil ${MLOADER_PREFIX}MEGAloader.app/Contents/MacOS/MEGAloader -o MEGAloader.dSYM
    strip ${MLOADER_PREFIX}MEGAloader.app/Contents/MacOS/MEGAloader
    dsymutil ${MUPDATER_PREFIX}MEGAupdater.app/Contents/MacOS/MEGAupdater -o MEGAupdater.dSYM
    strip ${MUPDATER_PREFIX}MEGAupdater.app/Contents/MacOS/MEGAupdater

    mv ${MSYNC_PREFIX}MEGAsync.app/Contents/MacOS/MEGAsync ${MSYNC_PREFIX}MEGAsync.app/Contents/MacOS/MEGAclient
    mv ${MLOADER_PREFIX}MEGAloader.app/Contents/MacOS/MEGAloader ${MSYNC_PREFIX}MEGAsync.app/Contents/MacOS/MEGAsync
    mv ${MUPDATER_PREFIX}MEGAupdater.app/Contents/MacOS/MEGAupdater ${MSYNC_PREFIX}MEGAsync.app/Contents/MacOS/MEGAupdater

    if [ ${build_cmake} -ne 1 ]; then
        [ ! -f MEGASync/MEGAsync.app/Contents/Frameworks/$AVCODEC_VERSION ] && cp -L $AVCODEC_PATH MEGASync/MEGAsync.app/Contents/Frameworks/
        [ ! -f MEGASync/MEGAsync.app/Contents/Frameworks/$AVFORMAT_VERSION ] && cp -L $AVFORMAT_PATH MEGASync/MEGAsync.app/Contents/Frameworks/
        [ ! -f MEGASync/MEGAsync.app/Contents/Frameworks/$AVUTIL_VERSION ] && cp -L $AVUTIL_PATH MEGASync/MEGAsync.app/Contents/Frameworks/
        [ ! -f MEGASync/MEGAsync.app/Contents/Frameworks/$SWSCALE_VERSION ] && cp -L $SWSCALE_PATH MEGASync/MEGAsync.app/Contents/Frameworks/
        [ ! -f MEGASync/MEGAsync.app/Contents/Frameworks/$CARES_VERSION ] && cp -L $CARES_PATH MEGASync/MEGAsync.app/Contents/Frameworks/
        [ ! -f MEGASync/MEGAsync.app/Contents/Frameworks/$CURL_VERSION ] && cp -L $CURL_PATH MEGASync/MEGAsync.app/Contents/Frameworks/

        if [ ! -f MEGASync/MEGAsync.app/Contents/Frameworks/$AVCODEC_VERSION ]  \
            || [ ! -f MEGASync/MEGAsync.app/Contents/Frameworks/$AVFORMAT_VERSION ]  \
            || [ ! -f MEGASync/MEGAsync.app/Contents/Frameworks/$AVUTIL_VERSION ]  \
            || [ ! -f MEGASync/MEGAsync.app/Contents/Frameworks/$SWSCALE_VERSION ];
        then
            echo "Error copying FFmpeg libs to app bundle."
            exit 1
        fi
    fi

    MEGASYNC_VERSION=`grep "const QString Preferences::VERSION_STRING" ../src/MEGASync/control/Preferences.cpp | awk -F '"' '{print $2}'`
    /usr/libexec/PlistBuddy -c "Set :CFBundleShortVersionString $MEGASYNC_VERSION" "${MSYNC_PREFIX}$APP_NAME.app/Contents/Info.plist"

    if [ ${build_cmake} -ne 1 ]; then
        install_name_tool -change @loader_path/$AVCODEC_VERSION @executable_path/../Frameworks/$AVCODEC_VERSION MEGASync/MEGAsync.app/Contents/MacOS/MEGAclient
        install_name_tool -change @loader_path/$AVFORMAT_VERSION @executable_path/../Frameworks/$AVFORMAT_VERSION MEGASync/MEGAsync.app/Contents/MacOS/MEGAclient
        install_name_tool -change @loader_path/$AVUTIL_VERSION @executable_path/../Frameworks/$AVUTIL_VERSION MEGASync/MEGAsync.app/Contents/MacOS/MEGAclient
        install_name_tool -change @loader_path/$SWSCALE_VERSION @executable_path/../Frameworks/$SWSCALE_VERSION MEGASync/MEGAsync.app/Contents/MacOS/MEGAclient

        rm -r $APP_NAME.app || :
        mv $MSYNC_PREFIX/$APP_NAME.app ./
    fi

    otool -L MEGAsync.app/Contents/MacOS/MEGAclient

    #Attach shell extension
    xcodebuild clean build CODE_SIGN_IDENTITY="-" CODE_SIGNING_REQUIRED=NO -jobs "$(sysctl -n hw.ncpu)" -configuration Release -target MEGAShellExtFinder -project ../src/MEGAShellExtFinder/MEGAFinderSync.xcodeproj/
    cp -a ../src/MEGAShellExtFinder/build/Release/MEGAShellExtFinder.appex $APP_NAME.app/Contents/Plugins/
    cd ..
fi

if [ "$sign" = "1" ]; then
	cd Release_x64
	cp -R $APP_NAME.app ${APP_NAME}_unsigned.app
	echo "Signing 'APPBUNDLE'"
	codesign --force --verify --verbose --preserve-metadata=entitlements --options runtime --sign "Developer ID Application: Mega Limited" --deep $APP_NAME.app
	echo "Checking signature"
	spctl -vv -a $APP_NAME.app
	cd ..
fi

if [ "$createdmg" = "1" ]; then
	cd Release_x64
	[ -f $APP_NAME.dmg ] && rm $APP_NAME.dmg
	echo "DMG CREATION PROCESS..."
	echo "Creating temporary Disk Image (1/7)"
	#Create a temporary Disk Image
	/usr/bin/hdiutil create -srcfolder $APP_NAME.app/ -volname $APP_NAME -ov $APP_NAME-tmp.dmg -fs HFS+ -format UDRW >/dev/null

	echo "Attaching the temporary image (2/7)"
	#Attach the temporary image
	mkdir $MOUNTDIR
	/usr/bin/hdiutil attach $APP_NAME-tmp.dmg -mountroot $MOUNTDIR >/dev/null

	echo "Copying resources (3/7)"
	#Copy the background, the volume icon and DS_Store files
	unzip -d $MOUNTDIR/$APP_NAME ../$RESOURCES.zip
	/usr/bin/SetFile -a C $MOUNTDIR/$APP_NAME

	echo "Adding symlinks (4/7)"
	#Add a symbolic link to the Applications directory
	ln -s /Applications/ $MOUNTDIR/$APP_NAME/Applications

	echo "Detaching temporary Disk Image (5/7)"
	#Detach the temporary image
	/usr/bin/hdiutil detach $MOUNTDIR/$APP_NAME >/dev/null

	echo "Compressing Image (6/7)"
	#Compress it to a new image
	/usr/bin/hdiutil convert $APP_NAME-tmp.dmg -format UDZO -o $APP_NAME.dmg >/dev/null

	echo "Deleting temporary image (7/7)"
	#Delete the temporary image
	rm $APP_NAME-tmp.dmg
	rmdir $MOUNTDIR
	cd ..
fi

if [ "$notarize" = "1" ]; then

	cd Release_x64
	if [ ! -f $APP_NAME.dmg ];then
		echo ""
		echo "There is no dmg to be notarized."
		echo ""
		exit 1
	fi

	rm querystatus.txt staple.txt || :

	echo "NOTARIZATION PROCESS..."
	echo "Getting USERNAME for notarization commands (1/7)"

	AC_USERNAME=$(security find-generic-password -s AC_PASSWORD | grep  acct | cut -d '"' -f 4)
	if [[ -z "$AC_USERNAME" ]]; then
		echo "Error USERNAME not found for notarization process. You should add item named AC_PASSWORD with and account value matching the username to macOS keychain"
		false
	fi

	echo "Sending dmg for notarization (2/7)"
	xcrun altool --notarize-app -t osx -f $APP_NAME.dmg --primary-bundle-id $ID_BUNDLE -u $AC_USERNAME -p "@keychain:AC_PASSWORD" --output-format xml 2>&1 > staple.txt
	RUUID=$(cat staple.txt | grep RequestUUID -A 1 | tail -n 1 | awk -F "[<>]" '{print $3}')
	echo $RUUID
	if [ ! -z "$RUUID" ] ; then
		echo "Received UUID for notarization request. Checking state... (3/7)"
		attempts=60
		while [ $attempts -gt 0 ]
		do
			echo "Querying state of notarization..."
			xcrun altool --notarization-info $RUUID -u $AC_USERNAME -p "@keychain:AC_PASSWORD" --output-format xml  2>&1 > querystatus.txt
			RUUIDQUERY=$(cat querystatus.txt | grep RequestUUID -A 1 | tail -n 1 | awk -F "[<>]" '{print $3}')
			if [[ "$RUUID" != "$RUUIDQUERY" ]]; then
				echo "UUIDs missmatch"
				false
			fi

			STATUS=$(cat querystatus.txt  | grep -i ">Status<" -A 1 | tail -n 1  | awk -F "[<>]" '{print $3}')

			if [[ $STATUS == "invalid" ]]; then
				echo "INVALID status. Check file querystatus.txt for further information"
				echo $STATUS
				break
			elif [[ $STATUS == "success" ]]; then
				echo "Notarized ok. Stapling dmg file..."
				xcrun stapler staple -v $APP_NAME.dmg
				echo "Stapling ok"

				#Mount dmg volume to check if app bundle is notarized
				echo "Checking signature and notarization"
				mkdir $MOUNTDIR || :
				hdiutil attach $APP_NAME.dmg -mountroot $MOUNTDIR >/dev/null
				spctl -v -a $MOUNTDIR/$APP_NAME/$APP_NAME.app
				hdiutil detach $MOUNTDIR/$APP_NAME >/dev/null
				rmdir $MOUNTDIR
				break
			else
				echo $STATUS
			fi

			attempts=$((attempts - 1))
			sleep 30
		done

		if [[ $attempts -eq 0 ]]; then
			echo "Notarization still in process, timed out waiting for the process to end"
			false
		fi
	fi
	cd ..
fi

echo "DONE"
