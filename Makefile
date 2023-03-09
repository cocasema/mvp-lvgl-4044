all: run

NAME = mvp-issue-4044

FLAGS = \
 -I . \
 -DLV_CONF_INCLUDE_SIMPLE \
 -DLV_CONF_PATH="lv_conf.h"

CSRCS = $(shell find -L src/ -name \*.c)
COBJS = $(CSRCS:.c=.o)

CASEFLAG=
ifneq ($(CASE),)
	CASEFLAG = -DCASE=$(CASE)
endif

%.o: %.c lv_conf.h
	gcc -std=c99 ${FLAGS} -c -o $@ $<

lvgl.a: ${COBJS}
	ar rcu $@ $+
	ranlib $@

${NAME}: ${NAME}.cpp lvgl.a lv_conf.h
	g++ -std=c++14 ${FLAGS} ${CASEFLAG} -o $@ $< lvgl.a

run: ${NAME}
	./$<

.PHONY: all ${NAME} run