AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src include tools docs

EXTRA_DIST = \
	README.md \
	git-version-gen

dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version
