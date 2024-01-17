cd build
cc -o bloom \
   ../src/main.c glfw_build/src/libglfw3.a \
   -Iglfw/deps -Iglfw/include \
   -lm
./bloom
