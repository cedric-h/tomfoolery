cd build
cc -o bloom \
   -ansi -Wall -pedantic -Werror -Wno-overlength-strings -Wno-error=unused-function \
   -fsanitize=address,undefined -g \
   ../src/main.c glfw_build/src/libglfw3.a \
   -Iglfw/deps -Iglfw/include \
   -framework Cocoa -framework IOKit \
   && ./bloom
