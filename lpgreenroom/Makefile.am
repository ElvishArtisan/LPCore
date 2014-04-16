## automake.am
##
## Makefile for the lpgreenroom(1) driver.
##
## (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.1 2013/11/21 22:31:45 cvs Exp $
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


bin_PROGRAMS = lpgreenroom

dist_lpgreenroom_SOURCES = room.cpp room.h\
                           lpgreenroom.cpp lpgreenroom.h

nodist_lpgreenroom_SOURCES = moc_lpgreenroom.cpp

lpgreenroom_LDADD = @CLI_LPLIBS@
lpgreenroom_LDFLAGS = @CLI_LPFLAGS@

CLEANFILES = *~\
             moc_*\
             *.obj\
             *.idb\
             *.pdb\
             *ilk

MAINTAINERCLEANFILES = *~\
                       Makefile.in
