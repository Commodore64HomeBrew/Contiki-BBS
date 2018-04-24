ifndef CONTIKI
CONTIKI = ../../../contiki
endif

CONTIKI_PROJECT = contiki-bbs

all: $(CONTIKI_PROJECT)

TARGET = c64

#PROJECTDIRS = ../.. $(CONTIKI)/apps/telnetd

PROJECT_SOURCEFILES = bbs-shell.c bbs-file.c bbs-read.c bbs-post.c bbs-setboard.c bbs-telnetd.c

DEFINES = WITH_LOGGING,WITH_BOOST,TELNETD_CONF_REJECT,TELNETD_CONF_MAX_IDLE_TIME=0

CONTIKI_WITH_IPV4 = 1

CFLAGS += --static-locals

include $(CONTIKI)/Makefile.include
