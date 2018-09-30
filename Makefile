OPENSTADIR = module/OpenSTA
SIZERDIR = src

all: sta
	$(MAKE) -C $(SIZERDIR);

sta:
	cd $(OPENSTADIR) && mkdir -p install-sp && ./bootstrap && ./configure --prefix=$(CURDIR)/$(OPENSTADIR)/install-sp && $(MAKE) install;


clean:
	cd $(OPENSTADIR) && $(MAKE) distclean && rm -rf install-sp;
	$(MAKE) -C $(SIZERDIR) clean;
