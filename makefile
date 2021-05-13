CC = emcc

FLAGS_COMMON =-s USE_SDL=2 -s INITIAL_MEMORY=134217728
FLAGS_DEBUG =-g -s ASSERTIONS=1 -s SAFE_HEAP=1
FLAGS_RELEASE =-s USE_CLOSURE_COMPILER=1 -s STACK_OVERFLOW_CHECK=0 -s MINIFY_HTML=1 -O2
ACCOUNT = nora-sellisa
PROJECT = bike-island-kiwami
CHANNEL = make-build

BUTLER_PATH = $(ACCOUNT)/$(PROJECT):$(CHANNEL)

CFLAGS = $(FLAGS_COMMON)

CONF_REL = release
CONF_DEB = debug

TOKEN = _t

ifeq ($(f), d)
CFLAGS += $(FLAGS_DEBUG)
CONF_DIR = $(CONF_DEB)
CONF_OTHER = $(CONF_REL)
else
CFLAGS += $(FLAGS_RELEASE)
CONF_DIR = $(CONF_REL)
CONF_OTHER = $(CONF_DEB)
HTML_SHELL = --shell-file shell.html
endif

DEP_DIR = $(CONF_DIR)/deps

CONFIG = $(CONF_DIR)/$(TOKEN)

objects = $(patsubst %.c,$(CONF_DIR)/%.o,$(wildcard *.c))
levels = $(patsubst %.svg,%.lvl,$(wildcard *.svg))
dependencies := $(patsubst %.c,$(DEP_DIR)/%.d,$(wildcard *.c))

index.html: $(CONFIG) $(objects) $(levels)
	$(CC) $(objects) -o $@ $(CFLAGS) $(HTML_SHELL) -lidbfs.js

%.o : %.c

$(CONF_DIR)/%.o : %.c $(DEP_DIR)/%.d $(CONFIG)
	$(eval DEPS := $(shell $(CC) -MM $< ))
	@echo $(patsubst %.c,$(DEP_DIR)/%.d, $<) $(patsubst %.o:,$(CONF_DIR)/%.o:,$(DEPS)) > $(patsubst %.c,$(DEP_DIR)/%.d, $<)
	$(CC) -c $(CFLAGS) -o $@ $<

$(CONFIG):
	rm -rf $(CONF_OTHER)
	mkdir $(CONF_DIR)
	mkdir $(DEP_DIR)
	touch $(CONFIG)

.PHONY:
host: index.html
	konsole -e python server.py &

%.lvl: %.svg
	python levelBuilder.py $< $@

game.zip: index.html
	rm -f game.zip
	zip game index.* *.lvl *.bmp *.wav

.PHONY:
clean:
	rm -rf index.html *.js *.wasm *.zip *.lvl $(CONF_REL) $(CONF_DEB)

.PHONY:
publish: game.zip
ifeq ($(f), d)
	$(error Trying to publish a Debug build!)
else
	butler push $< $(BUTLER_PATH)
endif

.PHONY:
status:
	butler status $(BUTLER_PATH)

.PHONY:
open: host
	python -m webbrowser localhost:8000

.PHONY:
itch:	
	python -m webbrowser https://$(ACCOUNT).itch.io/$(PROJECT)

$(dependencies):
	
include $(wildcard $(dependencies))