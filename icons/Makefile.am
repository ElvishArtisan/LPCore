## automake.am
##
## icons/automake.am for LPCore
##
## Use automake to process this into a Makefile.in
##
##   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License as
##   published by the Free Software Foundation; either version 2 of
##   the License, or (at your option) any later version.
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

install-exec-am:	
	mkdir -p ${DESTDIR}/usr/share/icons/hicolor/16x16/apps
	cp lpcodecpool-16x16.png ${DESTDIR}/usr/share/icons/hicolor/16x16/apps/lpcodecpool.png
	mkdir -p ${DESTDIR}/usr/share/icons/hicolor/22x22/apps
	cp lpcodecpool-22x22.png ${DESTDIR}/usr/share/icons/hicolor/22x22/apps/lpcodecpool.png
	mkdir -p ${DESTDIR}/usr/share/icons/hicolor/32x32/apps
	cp lpcodecpool-32x32.png ${DESTDIR}/usr/share/icons/hicolor/32x32/apps/lpcodecpool.png
	mkdir -p ${DESTDIR}/usr/share/icons/hicolor/48x48/apps
	cp lpcodecpool-48x48.png ${DESTDIR}/usr/share/icons/hicolor/48x48/apps/lpcodecpool.png

uninstall:	
	rm -f ${DESTDIR}/usr/share/icons/hicolor/16x16/apps/lpcodecpool.png
	rm -f ${DESTDIR}/usr/share/icons/hicolor/22x22/apps/lpcodecpool.png
	rm -f ${DESTDIR}/usr/share/icons/hicolor/32x32/apps/lpcodecpool.png
	rm -f ${DESTDIR}/usr/share/icons/hicolor/48x48/apps/lpcodecpool.png

EXTRA_DIST = lpcodecpool-16x16.png\
             lpcodecpool-16x16.xpm\
             lpcodecpool-22x22.png\
             lpcodecpool-32x32.png\
             lpcodecpool-48x48.png

CLEANFILES = *~
MAINTAINERCLEANFILES = *~ Makefile.in configure aclocal.m4
