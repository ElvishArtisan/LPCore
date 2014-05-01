## automake.am
##
## by Fred Gleason <fredg@paravelsystems.com>
##
## (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -I$(top_srcdir)/lib
MOC = @MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

clean-local:	
	rm -rf debug release

instdir = @LOCAL_PREFIX@/lib

lib_LTLIBRARIES = liblpcore.la
dist_liblpcore_la_SOURCES = lpcmdswitch.cpp lpcmdswitch.h\
                            lpconfig.cpp lpconfig.h\
                            lpconnection.cpp lpconnection.h\
                            lpengine.cpp lpengine.h\
                            lpmessage.cpp lpmessage.h\
                            lpparser.cpp lpparser.h\
                            lpprofile.cpp lpprofile.h\
                            lpstreamcmdserver.cpp lpstreamcmdserver.h\
                            lpswitcher.cpp lpswitcher.h\
                            lpswitcherfactory.cpp lpswitcherfactory.h\
                            lpttydevice.cpp lpttydevice.h\
                            switcher_am16.cpp switcher_am16.h\
                            switcher_btss82.cpp switcher_btss82.h\
                            switcher_dummy.cpp switcher_dummy.h\
                            switcher_gpio.cpp switcher_gpio.h\
                            switcher_zephyrclassic.cpp switcher_zephyrclassic.h\
                            switcher_zephyrxtreme.cpp switcher_zephyrxtreme.h

nodist_liblpcore_la_SOURCES = moc_lpconnection.cpp\
                              moc_lpparser.cpp\
                              moc_lpttydevice.cpp\
                              moc_lpstreamcmdserver.cpp\
                              moc_lpswitcher.cpp\
                              moc_switcher_am16.cpp\
                              moc_switcher_btss82.cpp\
                              moc_switcher_dummy.cpp\
                              moc_switcher_gpio.cpp\
                              moc_switcher_zephyrclassic.cpp\
                              moc_switcher_zephyrxtreme.cpp

includedir = $(prefix)/include/lpcore
include_HEADERS = lpcmdswitch.h\
                  lpconfig.h\
                  lpconnection.h\
                  lpengine.h\
                  lpmessage.h\
                  lpparser.h\
                  lpprofile.h\
                  lpswitcher.h\
                  lpswitcherfactory.h\
                  lpttydevice.h

liblpcore_la_LDFLAGS = -version-info $(INTERFACE_CURRENT):$(INTERFACE_REVISION):$(INTERFACE_AGE)

EXTRA_DIST = 

CLEANFILES = *~\
             moc_*

DISTCLEANFILES = *~\
                 moc_* 

MAINTAINERCLEANFILES = *~\
                       Makefile.in\
                       moc_*
