SRCDIR=src
OBJDIR=$(SRCDIR)
LIBDIR=lib
INCDIR=include
BINDIR=bin
DOCDIR=doc
#LINUXINCDIR=/usr/include/linux
LOCALINCDIR=/usr/local/include
FIOINCDIR=$(BSPDIR)/usr/include
FPUIINCDIR=$(BSPDIR)/usr/include
TODINCDIR=$(BSPDIR)/usr/include
VT100INCDIR=include
EMFIOINCDIR=include
LOCALLIBDIR=/usr/local/lib
#EXPATLIBDIR=../lib/libexpat/lib/$(ARCH)
VT100LIBDIR=lib
FIOLIBDIR=$(BSPDIR)/usr/lib
FPUILIBDIR=$(BSPDIR)/usr/lib
TODLIBDIR=$(BSPDIR)/usr/lib
EMFIOLIBDIR=lib

SOURCES=  $(SRCDIR)/main.c $(SRCDIR)/vse.c $(SRCDIR)/outputXML.c $(SRCDIR)/inputXML.c $(SRCDIR)/pCode.c $(SRCDIR)/pInterpret.c $(SRCDIR)/argument.c $(SRCDIR)/function.c $(SRCDIR)/expression.c $(SRCDIR)/configFile.c

OBJECTS=  $(OBJDIR)/main.o $(OBJDIR)/vse.o $(OBJDIR)/outputXML.o $(OBJDIR)/inputXML.o $(OBJDIR)/pCode.o $(OBJDIR)/pInterpret.o $(OBJDIR)/argument.o $(OBJDIR)/function.o $(OBJDIR)/expression.o $(OBJDIR)/configFile.o

INCLUDES= $(INCDIR)/vse_stnd.h $(INCDIR)/vse.h $(INCDIR)/outputXML.h $(INCDIR)/inputXML.h $(INCDIR)/pCode.h $(INCDIR)/pInterpret.h $(INCDIR)/argument.h $(INCDIR)/function.h $(INCDIR)/expression.h $(INCDIR)/configFile.h

TARGET=   $(BINDIR)/vse

CFLAGS= -g -Wall -fPIC -I$(SRCDIR) -I$(INCDIR) -I$(LOCALINCDIR) -I$(VT100INCDIR) -I$(FIOINCDIR) -I$(EMFIOINCDIR) -I$(FPUIINCDIR) -I$(TODINCDIR)
LDFLAGS= -L$(LIBDIR) -L$(BSPDIR)/usr/lib -lexpat -lvt100 -lpthread -lemfio -lfio -lfpui -ltod #-lfiostub.$(ARCH) -lfpuistub.$(ARCH) -ltodstub.$(ARCH)
CC= $(CROSS_COMPILE)gcc
RANLIB= $(CROSS_COMPILE)ranlib
AR= $(CROSS_COMPILE)ar
NM= $(CROSS_COMPILE)nm

LIBVT100=libvt100.so
LIBEMFIO=libemfio.so

all: $(LIBDIR)/$(LIBVT100).1 $(LIBDIR)/$(LIBEMFIO).1 $(TARGET)

LIBVT100OBJS = $(OBJDIR)/vt100.o $(OBJDIR)/vt100_clearVD.o $(OBJDIR)/vt100_compareVD.o $(OBJDIR)/vt100_dataAccess.o $(OBJDIR)/vt100_scrollVD.o \
	$(OBJDIR)/vt100_dumpVD.o $(OBJDIR)/vt100_globalData.o $(OBJDIR)/vt100_serialPortComms.o $(OBJDIR)/vt100_showVD.o \
	$(OBJDIR)/vt100_processInput.o $(OBJDIR)/vt100_noFPUI.o $(OBJDIR)/vt100_convertToASCII.o $(OBJDIR)/vt100_fromDumpToVD.o 


LIBEMFIOOBJS = $(OBJDIR)/emfio.o $(OBJDIR)/emfio_globalData.o $(OBJDIR)/emfio_setupCommands.o \
	$(OBJDIR)/emfio_errors.o $(OBJDIR)/emfio_responses.o $(OBJDIR)/emfio_stats.o

$(LIBDIR)/$(LIBVT100).1: $(LIBVT100OBJS)
	mkdir -p $(LIBDIR)
	$(CC) -shared -fPIC -Wl,-soname,$(LIBVT100).1 -o $(LIBDIR)/$(LIBVT100).1.0 $(LIBVT100OBJS) -lc -Wall
	cd $(LIBDIR); ln -s  $(LIBVT100).1.0  $(LIBVT100).1
	cd $(LIBDIR); ln -s  $(LIBVT100).1.0  $(LIBVT100)

$(OBJDIR)/vt100.o: $(SRCDIR)/vt100.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/vt100_clearVD.o: $(SRCDIR)/vt100_clearVD.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/vt100_compareVD.o: $(SRCDIR)/vt100_compareVD.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/vt100_dataAccess.o: $(SRCDIR)/vt100_dataAccess.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/vt100_scrollVD.o: $(SRCDIR)/vt100_scrollVD.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/vt100_dumpVD.o: $(SRCDIR)/vt100_dumpVD.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/vt100_globalData.o: $(SRCDIR)/vt100_globalData.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/vt100_serialPortComms.o: $(SRCDIR)/vt100_serialPortComms.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/vt100_showVD.o: $(SRCDIR)/vt100_showVD.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/vt100_processInput.o: $(SRCDIR)/vt100_processInput.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/vt100_noFPUI.o: $(SRCDIR)/vt100_noFPUI.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/vt100_convertToASCII.o: $(SRCDIR)/vt100_convertToASCII.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/vt100_fromDumpToVD.o: $(SRCDIR)/vt100_fromDumpToVD.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@


$(LIBDIR)/$(LIBEMFIO).1: $(LIBEMFIOOBJS)
	mkdir -p $(LIBDIR)
	$(CC) -shared -fPIC -Wl,-soname,$(LIBEMFIO).1 -o $(LIBDIR)/$(LIBEMFIO).1.0 $(LIBEMFIOOBJS) -lc -Wall
	cd $(LIBDIR); ln -s  $(LIBEMFIO).1.0  $(LIBEMFIO).1
	cd $(LIBDIR); ln -s  $(LIBEMFIO).1.0  $(LIBEMFIO)

$(OBJDIR)/emfio.o: $(SRCDIR)/emfio.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/emfio_globalData.o: $(SRCDIR)/emfio_globalData.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/emfio_setupCommands.o: $(SRCDIR)/emfio_setupCommands.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/emfio_errors.o: $(SRCDIR)/emfio_errors.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/emfio_responses.o: $(SRCDIR)/emfio_responses.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/emfio_stats.o: $(SRCDIR)/emfio_stats.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@


$(TARGET): $(OBJECTS)
	mkdir -p $(BINDIR)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJDIR)/main.o: $(SRCDIR)/main.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/vse.o: $(SRCDIR)/vse.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/outputXML.o: $(SRCDIR)/outputXML.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/inputXML.o: $(SRCDIR)/inputXML.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/pCode.o: $(SRCDIR)/pCode.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/pInterpret.o: $(SRCDIR)/pInterpret.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/argument.o: $(SRCDIR)/argument.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/function.o: $(SRCDIR)/function.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/expression.o: $(SRCDIR)/expression.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/configFile.o: $(SRCDIR)/configFile.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

docs:
	doxygen $(DOCDIR)/Doxyfile.doxy

clean:
	rm -f $(TARGET) $(LIBDIR)/$(LIBVT100)* $(LIBDIR)/$(LIBEMFIO)* $(OBJDIR)/*.o
