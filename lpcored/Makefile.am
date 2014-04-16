## automake.am
##
## Makefile for the lpcored(8) daemon.
##
## (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.8 2013/12/31 19:31:05 cvs Exp $
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


sbin_PROGRAMS = lpcored

dist_lpcored_SOURCES = ae_engine.cpp ae_engine.h\
                       apphost.cpp apphost.h\
                       connection.cpp connection.h\
                       engine.cpp engine.h\
                       enginefactory.cpp enginefactory.h\
                       lpcored.cpp lpcored.h\
                       switcher_engine.cpp switcher_engine.h

nodist_lpcored_SOURCES = moc_ae_engine.cpp\
                         moc_apphost.cpp\
                         moc_connection.cpp\
                         moc_engine.cpp\
                         moc_lpcored.cpp\
                         moc_switcher_engine.cpp

lpcored_LDADD = @CLI_LPLIBS@
lpcored_LDFLAGS = @CLI_LPFLAGS@

CLEANFILES = *~\
             moc_*\
             *.obj\
             *.idb\
             *.pdb\
             *ilk

MAINTAINERCLEANFILES = *~\
                       Makefile.in
