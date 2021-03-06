# Run this from the build directory.

HASH=$(git rev-parse HEAD)
SHORT=${HASH:0:8}
DATE=$(date +%Y%m%d)
DIRNAME=chuffed-mac-$DATE-$SHORT

cmake -DCMAKE_BUILD_TYPE=Release ..
make
mkdir -p $DIRNAME
cp fzn_chuffed $DIRNAME
cp /usr/local/lib/libprotobuf.9.dylib $DIRNAME
cp -r ../chuffed/flatzinc/mznlib $DIRNAME

cd $DIRNAME
install_name_tool -change /usr/local/lib/libprotobuf.9.dylib @executable_path/libprotobuf.9.dylib fzn_chuffed
cd ..

zip -r $DIRNAME.zip $DIRNAME
