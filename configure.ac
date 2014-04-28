dnl configure.in
dnl
dnl Autoconf configuration
dnl
dnl Use autoconf to process this into a configure script
dnl
dnl   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
dnl
dnl      $Id: configure.in,v 1.21 2014/01/13 15:34:13 cvs Exp $
dnl
dnl   This program is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License version 2 as
dnl   published by the Free Software Foundation.
dnl
dnl   This program is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details.
dnl
dnl   You should have received a copy of the GNU General Public
dnl   License along with this program; if not, write to the Free Software
dnl   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
dnl

AC_INIT(lpcore,`cat PACKAGE_VERSION`,Fred Gleason <fredg@paravelsystems.com>)
AM_INIT_AUTOMAKE(lpcore,`cat PACKAGE_VERSION`)
AC_SUBST(RPM_RELEASE,1)
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

#
# Some Fully Resolved Install Paths
# (for the packaging system and friends)
#
if test ${prefix} = "NONE" ; then
  AC_SUBST(LOCAL_PREFIX,/usr/local)
else
  AC_SUBST(LOCAL_PREFIX,${prefix})
fi

#
# Option Switches
#
AC_ARG_ENABLE(hpi,[  --disable-mc-gpio       disable MeasurementComputing GPIO device support],
		      [MC_GPIO_DISABLED=yes],[])

#
# Basic Compiler Checks
#
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_LANG(C++)

#
# Determine Library API Versioning
#
# From http://www.gnu.org/software/libtool/manual.html#Updating-version-info
#
#   1. Start with version information of 0:0:0 for each libtool library.
#   2. Update the version information only immediately before a public
#      release of your software. More frequent updates are unnecessary,
#      and only guarantee that the current interface number gets larger
#      faster.
#   3. If the library source code has changed at all since the last update,
#      then increment 'REVISION' (c:r:a becomes c:r+1:a).
#   4. If any interfaces have been added, removed, or changed since the last
#      update, increment 'CURRENT', and set 'REVISION' to 0.
#   5. If any interfaces have been added since the last public release, then
#      increment 'AGE'.
#   6. If any interfaces have been removed since the last public release,
#      then set 'AGE' to 0.
CURRENT=6
REVISION=0
AGE=6
POINT=$(($CURRENT - $AGE))

#
# Calculate the Library Version Values
#
AC_SUBST(INTERFACE_CURRENT,$CURRENT)
AC_SUBST(INTERFACE_REVISION,$REVISION)
AC_SUBST(INTERFACE_AGE,$AGE)
AC_SUBST(INTERFACE_MAJOR,$POINT)
AC_SUBST(INTERFACE_MINOR,$AGE)
AC_SUBST(INTERFACE_POINT,$REVISION)

# to avoid rpath usage :
# http://wiki.debian.net/index.cgi?RpathIssue
case ${host} in
  *-pc-linux-gnu)
    AC_MSG_RESULT([Fixing libtool for -rpath problems.])
    sed < libtool > libtool-2 \
    's/^hardcode_libdir_flag_spec.*$/hardcode_libdir_flag_spec=" -D__LIBTOOL_IS_A_FOOL__ "/'
    mv libtool-2 libtool
    chmod 755 libtool
  ;;
esac

#
# Check for Qt
#
AC_CHECK_HEADER(QtCore/QtCore,[],[AC_MSG_ERROR([*** Qt4 not found, unable to continue ***])])
AC_CHECK_PROG(MOC_NAME,moc-qt4,[moc-qt4],[moc])
AC_SUBST(MOC,$MOC_NAME)

#
# Check for GPIO dernel driver
#
AC_CHECK_HEADER(linux/gpio.h,[MC_GPIO_FOUND=yes],[])
if test $MC_GPIO_FOUND ; then
  if test -z $MC_GPIO_DISABLED ; then
    USING_MC_GPIO=yes
    AC_DEFINE(HAVE_MC_GPIO,yes)
  fi
fi

#
# Determine the target architecture
#
AR_GCC_TARGET()
AC_SUBST(VENDOR,$ar_gcc_distro)
AC_SUBST(ARCH,$ar_gcc_arch)

#
# Determine Distro
#
AR_GET_DISTRO()
AC_SUBST(DISTRO,$ar_gcc_distro)
if test $ar_gcc_distro = suse ; then
  AC_SUBST(QT4_MYSQL_PKG,"libqt4-sql-mysql")
  AC_SUBST(MYSQL_PKG,"mysql")
  AC_SUBST(DIST_VER,"")
  AC_SUBST(DEBUG_RPM,"no")
else
  AC_SUBST(QT4_MYSQL_PKG,"qt4-mysql")
  AC_SUBST(MYSQL_PKG,"mysqld")
  DISTRO_MAJOR_VER=`echo $ar_distro_version | awk 'split($1,fields,".") {print fields[[1]]}'`
  if test $DISTRO_MAJOR_VER -ge 6 ; then
    AC_SUBST(DIST_VER,".el$DISTRO_MAJOR_VER")
    AC_SUBST(DEBUG_RPM,"yes")
  else
    AC_SUBST(DIST_VER,"")
    AC_SUBST(DEBUG_RPM,"no")
  fi
fi

#
# Linux vs. OS X Setup
#
if test $ar_gcc_os = "darwin10" ; then
  AC_SUBST(GUI_LPLIBS,"-llpcore")
  AC_SUBST(GUI_LPFLAGS,"-framework QtCore -framework QtGui -framework QtNetwork")
  AC_SUBST(CLI_LPLIBS,"-llpcore")
  AC_SUBST(CLI_LPFLAGS,"-framework QtCore -framework -framework QtNetwork")
  AC_SUBST(INSTALL_INIT,"install-init-osx.sh")
  AC_DEFINE(OSX)
else
  AC_SUBST(GUI_LPLIBS,"-llpcore -lQtCore -lQtGui -lQtNetwork")
  AC_SUBST(GUI_LPFLAGS,"")
  AC_SUBST(CLI_LPLIBS,"-llpcore -lQtCore -lQtNetwork")
  AC_SUBST(CLI_LPFLAGS,"")
  AC_SUBST(INSTALL_INIT,"install-init-linux.sh")
  AC_DEFINE(LINUX)
fi

#
# Configure RPM Build
#
AC_CHECK_PROG(RPMBUILD_FOUND,rpmbuild,[yes],[])
if test -z $RPMBUILD_FOUND ; then
  AC_SUBST(RPMBUILD,rpm)
else
  AC_SUBST(RPMBUILD,rpmbuild)
fi
if test -d /usr/src/redhat ; then
  AC_SUBST(RPM_ROOT,/usr/src/redhat)
  AC_SUBST(VENDOR,redhat)
else
  AC_SUBST(RPM_ROOT,/usr/src/packages)
  AC_SUBST(VENDOR,suse)
fi 

AC_CONFIG_FILES([ \
    conf/Makefile \
    docs/Makefile \
    icons/Makefile \
    lib/Makefile \
    lib/lpcore/Makefile \
    lpautocue/Makefile \
    lpautoroute/Makefile \
    lpcodecpool/Makefile \
    lpcore/Makefile \
    lpcored/Makefile \
    lpgreenroom/Makefile \
    lpmiccontrol/Makefile \
    lpnetrouter/Makefile \
    lpremotebutton/Makefile \
    lpswitch/Makefile \
    lpcore.pc \
    lpcore.spec \
    rc.lpcodecpool \
    rc.lpcored \
    xdg/Makefile \
    Makefile ])
AC_OUTPUT()
chmod 755 rc.lpcodecpool
chmod 755 rc.lpcored

#
# Configuration Results
#
AC_MSG_NOTICE()
AC_MSG_NOTICE("|-----------------------------------------------------|")
AC_MSG_NOTICE("|         *** LPCORE CONFIGURATION SUMMARY ***        |")
AC_MSG_NOTICE("|-----------------------------------------------------|")
AC_MSG_NOTICE("| Hardware Support:                                   |")
if test -z $USING_MC_GPIO ; then
AC_MSG_NOTICE("|         MeasurementComputing GPIO Devices ... No    |")
else
AC_MSG_NOTICE("|         MeasurementComputing GPIO Devices ... Yes   |")
fi
AC_MSG_NOTICE("|-----------------------------------------------------|")
AC_MSG_NOTICE()
AC_MSG_NOTICE(Now enter 'make' to build the software.)
AC_MSG_NOTICE()
