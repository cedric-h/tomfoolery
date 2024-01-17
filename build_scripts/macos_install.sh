mkdir -p build
cd build

git clone https://github.com/glfw/glfw
cmake -S glfw -B glfw_build
cd glfw_build && make
cd ..

chrome_versions=("/Applications/Google Chrome.app/Contents/Frameworks/Google Chrome Framework.framework/Versions/"*)
cp "${chrome_versions[0]}/Libraries/libEGL.dylib" libEGL.dylib
cp "${chrome_versions[0]}/Libraries/libGLESv2.dylib" libGLESv2.dylib

cd ..

./macos_build.sh
