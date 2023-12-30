mkdir -p build
cd build

git clone https://github.com/glfw/glfw
cmake -S glfw -B glfw_build
cd glfw_build && make
cd ..

cp "/Applications/Google Chrome.app/Contents/Frameworks/Google Chrome Framework.framework/Versions/120.0.6099.129/Libraries/libEGL.dylib" libEGL.dylib
cp "/Applications/Google Chrome.app/Contents/Frameworks/Google Chrome Framework.framework/Versions/120.0.6099.129/Libraries/libGLESv2.dylib" libGLESv2.dylib

cd ..

./macos_build.sh
