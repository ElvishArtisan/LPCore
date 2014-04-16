## automake.am
##
## Makefile for the lpswitch(1) switcher control applet.
##
## (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.4 2013/09/09 18:13:48 cvs Exp $
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
##
## Use automake to process this into a Makefile.in

AM_CPPFLAGS = -Wall -I$(top_srcdir)/lib -DPREFIX=\"$(prefix)\"
LIBS = -L$(top_srcdir)/lib/lpcore

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	@MOC@ $< -o $@


bin_PROGRAMS = lpswitch

dist_lpswitch_SOURCES = dialer.cpp dialer.h\
                        gpiindicator.cpp gpiindicator.h\
                        lpswitch.cpp lpswitch.h\
                        statuslight.cpp statuslight.h

nodist_lpswitch_SOURCES = moc_dialer.cpp\
                          moc_gpiindicator.cpp\
                          moc_lpswitch.cpp\
                          moc_statuslight.cpp

lpswitch_LDADD = @GUI_LPLIBS@
lpswitch_LDFLAGS = @GUI_LPFLAGS@

CLEANFILES = *~\
             moc_*\
             *.obj\
             *.idb\
             *.pdb\
             *ilk

MAINTAINERCLEANFILES = *~\
                       Makefile.in
