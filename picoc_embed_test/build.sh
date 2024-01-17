gcc -Wall host.c \
  ../picoc/table.o \
  ../picoc/lex.o \
  ../picoc/parse.o \
  ../picoc/expression.o \
  ../picoc/heap.o \
  ../picoc/type.o \
  ../picoc/variable.o \
  ../picoc/clibrary.o \
  ../picoc/platform.o \
  ../picoc/include.o \
  ../picoc/debug.o \
  ../picoc/platform/platform_unix.o \
  ../picoc/platform/library_unix.o \
  ../picoc/cstdlib/stdio.o \
  ../picoc/cstdlib/math.o \
  ../picoc/cstdlib/string.o \
  ../picoc/cstdlib/stdlib.o \
  ../picoc/cstdlib/time.o \
  ../picoc/cstdlib/errno.o \
  ../picoc/cstdlib/ctype.o \
  ../picoc/cstdlib/stdbool.o \
  ../picoc/cstdlib/unistd.o \
  -lm -lreadline \
  -o host \
  && ./host 
