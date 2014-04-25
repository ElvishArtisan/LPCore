## automake.am
##
## Makefile for the lpcodecpool(1) application.
##
## (C) Copyright 2013-2014 Fred Gleason <fredg@paravelsystems.com>
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


bin_PROGRAMS = lpcodecpool lpcodecpanel

dist_lpcodecpool_SOURCES = codecs.cpp codecs.h\
                           devices.cpp devices.h\
                           lpcodecpool.cpp lpcodecpool.h\
                           rooms.cpp rooms.h
nodist_lpcodecpool_SOURCES = moc_lpcodecpool.cpp

dist_lpcodecpanel_SOURCES = codecwidget.cpp codecwidget.h\
                            lpcodecpanel.cpp lpcodecpanel.h
nodist_lpcodecpanel_SOURCES = moc_codecwidget.cpp\
                              moc_lpcodecpanel.cpp

lpcodecpool_LDADD = @CLI_LPLIBS@
lpcodecpool_LDFLAGS = @CLI_LPFLAGS@

lpcodecpanel_LDADD = @GUI_LPLIBS@
lpcodecpanel_LDFLAGS = @GUI_LPFLAGS@

CLEANFILES = *~\
             moc_*\
             *.obj\
             *.idb\
             *.pdb\
             *ilk

MAINTAINERCLEANFILES = *~\
                       Makefile.in
