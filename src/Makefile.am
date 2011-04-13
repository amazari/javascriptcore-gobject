JSCORE_GOBJECT_LIBRARY_VERSION=1:0:0

lib_LTLIBRARIES = libjavascriptcore-gobject-1.0.la
libjavascriptcore_gobject_1_0_la_SOURCES = jscore-class.c   \
									   jscore-context-group.c \
									   jscore-context.c \
									   jscore-object.c  \
									   jscore-value.c
									   
libjavascriptcore_gobject_1_0_la_includedir=$(includedir)/javascriptcore-gobject-1.0/javascriptcore-gobject
libjavascriptcore_gobject_1_0_la_include_HEADERS = jscore-class.h \
		  			      jscore-context-group.h \
						  jscore-context.h  \
						  jscore-object.h \
						  jscore-value.h
libjavascriptcore_gobject_1_0_la_CFLAGS = $(DEPENDENCIES_CFLAGS)

libjavascriptcore_gobject_1_0_la_LDFLAGS = -version-info $(JSCORE_GOBJECT_LIBRARY_VERSION) $(DEPENDENCIES_LIBS)
