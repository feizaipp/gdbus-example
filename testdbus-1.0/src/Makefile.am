AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(GIO_CFLAGS) \
	-DPACKAGE_LIBEXEC_DIR=\""$(libexecdir)"\"

LIBS = \
	$(GLIB_LIBS) \
	$(GIO_LIBS) \
	$(NULL)

dbus_built_sources = testdbus-generated.h testdbus-generated.c

$(dbus_built_sources) : $(top_srcdir)/data/org.freedesktop.Testdbus.xml
	gdbus-codegen --interface-prefix org.freedesktop.Testdbus --c-namespace Testdbus --generate-c-code=testdbus-generated $<

BUILT_SOURCES =										\
	$(dbus_built_sources)								\
	$(NULL)

testdbusprivdir = $(libexecdir)/testdbus
testdbuspriv_PROGRAMS = testdbusd
testdbusd_SOURCES = main.c $(BUILT_SOURCES)

bin_PROGRAMS = testgdbus
testgdbus_SOURCES = testgdbus.c $(BUILT_SOURCES)

CLEANFILES = $(BUILT_SOURCES)


