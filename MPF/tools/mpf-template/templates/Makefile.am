EXTRA_DIST = component-autotools multipipe-autotools data-type-autotools

dist-hook:
	rm -rf `find $(distdir) -name .svn`

install-component-autotools:
	install -d $(DESTDIR)${datarootdir}/mpf/templates/component-autotools
	cp -rv $(top_srcdir)/templates/component-autotools $(DESTDIR)${datarootdir}/mpf/templates/
	find $(DESTDIR)${datarootdir}/mpf/templates/ -name \*.svn | xargs rm -rf

install-multipipe-autotools:
	install -d $(DESTDIR)${datarootdir}/mpf/templates/multipipe-autotools
	cp -rv $(top_srcdir)/templates/multipipe-autotools $(DESTDIR)${datarootdir}/mpf/templates/
	find $(DESTDIR)${datarootdir}/mpf/templates/ -name \*.svn | xargs rm -rf

install-data-type-autotools:
	install -d $(DESTDIR)${datarootdir}/mpf/templates/data-type-autotools
	cp -rv $(top_srcdir)/templates/data-type-autotools $(DESTDIR)${datarootdir}/mpf/templates/
	find $(DESTDIR)${datarootdir}/mpf/templates/ -name \*.svn | xargs rm -rf

svnclean:
	rm -f Makefile.in

install: install-component-autotools install-multipipe-autotools install-data-type-autotools

uninstall:
	rm -rf ${datarootdir}/mpf/templates/

.PHONY: install-component-autotools \
	install-data-type-autotools \
	svnclean 
