OPENSTADIR = module/OpenSTA
TCLDIR = module/tcl
SIZERDIR = src

TCL_INST_DIR=$(CURDIR)/$(TCLDIR)/unix/install-sp

all: sta
	$(MAKE) -C $(SIZERDIR);

tcl8.4: 
	cd $(TCLDIR)/unix && mkdir -p install-sp && ./configure --prefix=$(CURDIR)/$(TCLDIR)/unix/install-sp && $(MAKE) install;

sta: tcl8.4
	cd $(OPENSTADIR) && mkdir -p install-sp && \
		./bootstrap && ./configure --prefix=$(CURDIR)/$(OPENSTADIR)/install-sp \
		--with-lib=$(TCL_INST_DIR)/lib \
		--with-include=$(TCL_INST_DIR)/include \
		--with-tcl=$(TCL_INST_DIR)/lib/tcl8.4 && \
		$(MAKE) install;


clean:
	cd $(TCLDIR)/unix/install-sp && $(MAKE) clean && rm -rf install-sp; > /dev/null 2>&1; true
	cd $(OPENSTADIR) && $(MAKE) distclean && rm -rf install-sp > /dev/null 2>&1; true
	$(MAKE) -C $(SIZERDIR) clean;
