#
# daemon - http://libslack.org/daemon/
#
# Copyright (C) 1999, 2000 raf <raf@raf.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
# or visit http://www.gnu.org/copyleft/gpl.html
#

# 20000902 raf <raf@raf.org>

# Uncomment this to override the default value of 600 seconds
# as the minimum amount of time that a client can live if it
# is to be respawned
#
# DAEMON_DEFINES += -DRESPAWN_THRESHOLD=600

# Uncomment this to override the default configuration file path
#
# DAEMON_DEFINES += -DCONFIG_PATH=\"/etc/daemon.conf\"

# Uncomment this to disable debugging completely
#
# DAEMON_DEFINES += -DNDEBUG

DAEMON_NAME := daemon
DAEMON_VERSION := 0.3
DAEMON_ID := $(DAEMON_NAME)-$(DAEMON_VERSION)
DAEMON_DIST := $(DAEMON_ID).tar.gz
DAEMON_HAS_SUBTARGETS := 1

DAEMON_TARGET := $(DAEMON_SRCDIR)/$(DAEMON_NAME)
DAEMON_MODULES := daemon

DAEMON_CFILES := $(patsubst %, $(DAEMON_SRCDIR)/%.c, $(DAEMON_MODULES))
DAEMON_OFILES := $(patsubst %, $(DAEMON_SRCDIR)/%.o, $(DAEMON_MODULES))
DAEMON_PODFILES := $(DAEMON_CFILES)
DAEMON_MANFILES := $(patsubst %.c, %.$(APP_MANSECT), $(DAEMON_PODFILES))
DAEMON_HTMLFILES := $(patsubst %.c, %.$(APP_MANSECT).html, $(DAEMON_PODFILES))

TAG_FILES += $(DAEMON_HFILES) $(DAEMON_CFILES)
DEPEND_CFILES += $(DAEMON_CFILES)
DEPEND_HFILES += $(DAEMON_HFILES)

ALL_TARGETS += daemon
MAN_TARGETS += man-daemon
HTML_TARGETS += html-daemon
INSTALL_TARGETS += install-daemon
UNINSTALL_TARGETS += uninstall-daemon

CLEAN_FILES += $(DAEMON_OFILES) $(DAEMON_MANFILES) $(DAEMON_HTMLFILES)
CLOBBER_FILES += $(DAEMON_TARGET) $(DAEMON_SRCDIR)/tags

DAEMON_DEFINES += $(SLACK_DEFINES)
DAEMON_CPPFLAGS += $(DAEMON_DEFINES) $(patsubst %, -I%, $(DAEMON_INCDIRS))
DAEMON_CCFLAGS += $(CCFLAGS)
DAEMON_CFLAGS += $(DAEMON_CPPFLAGS) $(DAEMON_CCFLAGS)
DAEMON_LIBS += slack

# Uncomment this on Solaris for getsockopt(3)
#
# DAEMON_LIBS += xnet

DAEMON_LDFLAGS += $(patsubst %, -L%, $(DAEMON_LIBDIRS)) $(patsubst %, -l%, $(DAEMON_LIBS))

SLACK_SRCDIR := libslack
SLACK_INCDIRS := libslack
SLACK_LIBDIRS := libslack
include $(SLACK_SRCDIR)/macros.mk
DAEMON_SUBMODULES := $(SLACK_TARGET)

