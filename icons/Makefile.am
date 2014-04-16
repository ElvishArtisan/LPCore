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
	cp logitek-16x16.png ${DESTDIR}/usr/share/icons/hicolor/16x16/apps/logitek.png
	mkdir -p ${DESTDIR}/usr/share/icons/hicolor/22x22/apps
	cp logitek-22x22.png ${DESTDIR}/usr/share/icons/hicolor/22x22/apps/logitek.png
	mkdir -p ${DESTDIR}/usr/share/icons/hicolor/32x32/apps
	cp logitek-32x32.png ${DESTDIR}/usr/share/icons/hicolor/32x32/apps/logitek.png
	mkdir -p ${DESTDIR}/usr/share/icons/hicolor/48x48/apps
	cp logitek-48x48.png ${DESTDIR}/usr/share/icons/hicolor/48x48/apps/logitek.png

uninstall:	
	rm -f ${DESTDIR}/usr/share/icons/hicolor/16x16/apps/logitek.png
	rm -f ${DESTDIR}/usr/share/icons/hicolor/22x22/apps/logitek.png
	rm -f ${DESTDIR}/usr/share/icons/hicolor/32x32/apps/logitek.png
	rm -f ${DESTDIR}/usr/share/icons/hicolor/48x48/apps/logitek.png

EXTRA_DIST = logitek-16x16.png\
             logitek-16x16.xpm\
             logitek-22x22.png\
             logitek-22x22.xpm\
             logitek-32x32.png\
             logitek-32x32.xpm\
             logitek-48x48.png\
             logitek-48x48.xpm\
             logitek.res

CLEANFILES = *~
MAINTAINERCLEANFILES = *~ Makefile.in configure aclocal.m4
