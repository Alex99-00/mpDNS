
TEMPDIRS=$(shell find ./ -type d -maxdepth 1)
SUBDIRS=$(subst ./, , $(TEMPDIRS))

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

