SRC=$(wildcard ../*/*.c)
OBJS=$(addprefix obj/,$(subst ../,,$(subst .c,.o,$(SRC))))
DIRS=$(sort $(dir $(OBJS)))
CC=mscl
CFLAGS=/I. /I../include /I../../inc_win
CFLAGS+=/ZI /Od

all:winnix.lib

winnix.lib:$(OBJS)
	mscl /lib /out:$@ $(OBJS)

$(DIRS):
	-@mkdir -p $@

obj/%.o:../%.c | $(DIRS)
	$(CC) $(CFLAGS) /c /Fo$@ $<

clean:
	-@ rm -fr obj *.lib *.obj *.pdb *.idb *.out test*

test.exe:test.obj winnix.lib
	mscl /debug /out:$@ $< winnix.lib ntdll.lib

%.obj:%.cpp
	mscl /c /ZI /Od /I../include $<
