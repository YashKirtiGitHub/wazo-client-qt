#!/usr/bin/env bash

if [ -r versions.mak ]
then
    source versions.mak
else
    echo "No versions.mak file found. Please run qmake."
    exit 1
fi

function get_infos {
    UNAME_ARCH="$(uname -m)" # Gets i?86 or x86_64
    case "$UNAME_ARCH" in
        *86)
            ARCH="i386"
            ;;
        *64)
            ARCH="amd64"
            ;;
    esac
}
get_infos

function package-content {
    cd "$GIT_DIR"

    PKGDIR=pkg
    PKGROOT=$PKGDIR/root

    mkdir -p $PKGROOT/opt/xivoclient
    cp -r bin/* $PKGROOT/opt/xivoclient

    mkdir -p $PKGROOT/usr/share/icons/
    cp cross/resources/xivoclient.png $PKGROOT/usr/share/icons/

    mkdir -p $PKGROOT/usr/share/applications/
    cp cross/resources/xivoclient.desktop $PKGROOT/usr/share/applications/
    sed -i s/xivoicon/xivoclient/ $PKGROOT/usr/share/applications/xivoclient.desktop

    mkdir -p $PKGROOT/usr/bin
    cat > $PKGROOT/usr/bin/xivoclient <<EOF
#!/bin/bash

DEBUG="no"

while getopts ":dh" opt; do
  case \$opt in
      d)
          DEBUG="yes"
          ;;
      h)
          echo "Usage : \$0 [-dh] [profile]"
          echo
          echo "-d : Enable debug output"
          echo "-h : Help"
          echo "profile : Configuration profile"
          echo
          exit 0
          ;;
      \?)
          echo "Invalid option: -\$OPTARG" >&2
          ;;
  esac
done

shift \$(( OPTIND-1 ))

cd /opt/xivoclient

if [ "\$DEBUG" = "yes" ]
then
    LD_LIBRARY_PATH=".:\$LD_LIBRARY_PATH" ./xivoclient \$@
else
    LD_LIBRARY_PATH=".:\$LD_LIBRARY_PATH" ./xivoclient \$@ >& /dev/null
fi
EOF
    chmod 755 $PKGROOT/usr/bin/xivoclient
}
package-content

function package-control {
    mkdir -p pkg/control

    PKGSIZE=$(du -s $PKGROOT | cut -f 1)

    cat > pkg/control/control <<EOF
Package: xivoclient
Version: ${XC_VERSION}
Architecture: ${ARCH}
Maintainer: Proformatique Maintainance Team <technique@proformatique.com>
Installed-Size: ${PKGSIZE}
Depends: libc6 (>= 2.7-1), libgcc1 (>= 1:4.1.1-21), libstdc++6 (>= 4.1.1-21), libqtcore4 (>= 4.6.0), libqtgui4 (>= 4.6.0), libqt4-network (>= 4.6.0), libqt4-xml (>= 4.6.0)
Section: graphics
Priority: optional
Homepage: http://www.xivo.fr/
Description: CTI client for XiVO
 XiVO CTI (Computer Telephony Integration) client is the graphical
 front-end to the XiVO CTI services.
EOF
    find $PKGROOT/ -type f | xargs md5sum > pkg/control/md5sums
}
package-control

function package {
    cd $PKGROOT
    tar zcf ../data.tar.gz --owner=0 --group=0 .
    cd ../control
    tar zcf ../control.tar.gz --owner=0 --group=0 .
    cd ..

    echo "2.0" > debian-binary

    PACKAGE_PATH="$GIT_DIR/xivoclient-${XC_VERSION}-${ARCH}.deb"
    rm -f "$PACKAGE_PATH"
    ar -r "$PACKAGE_PATH" debian-binary control.tar.gz data.tar.gz
}
package

function clean {
    cd ..
    rm -rf $PKGDIR
}
clean
