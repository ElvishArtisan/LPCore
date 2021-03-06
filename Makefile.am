## automake.am
##
## Top level automake.am
##
## Use automake to process this into a Makefile.in
##
##   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.12 2013/11/21 22:54:39 cvs Exp $
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License version 2 as
##   published by the Free Software Foundation.
##
##   This program is distributed in the hope that it will be useful,
##   but WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
##
##   You should have received a copy of the GNU General Public
##   License along with this program; if not, write to the Free Software
##   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##

SUBDIRS = conf\
          docs\
          icons\
          lib\
          lpautocue\
          lpautoroute\
          lpcodecpool\
          lpcore\
          lpcored\
          lpgreenroom\
          lpmiccontrol\
          lpnetrouter\
          lpremotebutton\
          lpswitch\
          xdg

ACLOCAL_AMFLAGS = -I m4

maintainer-clean-local:	
	rm -rf m4

install-exec-local:	
	mkdir -p ${DESTDIR}/etc/init.d
	cp rc.lpcored ${DESTDIR}/etc/init.d/lpcored
	cp rc.lpcodecpool ${DESTDIR}/etc/init.d/lpcodecpool

uninstall-local:
	rm -f ${DESTDIR}/etc/init.d/lpcored
	rm -f ${DESTDIR}/etc/init.d/lpcodecpool

rpm:	dist
	cp lpcore-$(VERSION).tar.gz $(HOME)/rpmbuild/SOURCES/
	$(RPMBUILD) -ba --target $(ARCH)-$(VENDOR)-linux lpcore.spec
	mv $(HOME)/rpmbuild/RPMS/$(ARCH)/lpcore-*.rpm $(top_srcdir)/
	mv $(HOME)/rpmbuild/SRPMS/lpcore-*.src.rpm $(top_srcdir)/
	rm $(HOME)/rpmbuild/SOURCES/lpcore-$(VERSION).tar.gz
	rm -rf $(HOME)/rpmbuild/BUILD/lpcore-$(VERSION)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = lpcore.pc

EXTRA_DIST = lpcore.spec.in\
             autogen.sh\
             get_distro.sh\
             get_target.sh\
             lpcore.pc.in\
             PACKAGE_VERSION\
             rc.lpcodecpool.in\
             rc.lpcored.in

CLEANFILES = *~\
             *.deb\
             *.mo\
             *.rpm\
             *.tar.gz\
             *.tgz\
             *.zip

DISTCLEANFILES = lpcore.spec\
                 config.guess\
                 config.sub\
                 ltmain.sh\
                 lpcore.pc\
                 rc.lpcore

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       config.guess\
                       config.status\
                       config.sub\
                       configure\
                       depcomp\
                       install-sh\
                       libtool\
                       ltmain.sh\
                       Makefile.in\
                       missing\
                       mkinstalldirs
