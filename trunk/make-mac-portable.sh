# Run this from the build directory.

HASH=$(git show-ref -s HEAD)
SHORT=${HASH:0:8}
DATE=$(date +%Y%m%d)
DIRNAME=chuffed-portable-$DATE-$SHORT

make
mkdir -p $DIRNAME
cp fzn_chuffed $DIRNAME
cp ~/local/lib/libboost_system.dylib $DIRNAME
cp /usr/local/lib/libprotobuf.9.dylib $DIRNAME
cp -r ../chuffed/flatzinc/mznlib $DIRNAME

cd $DIRNAME
install_name_tool -change /usr/local/lib/libprotobuf.9.dylib @executable_path/libprotobuf.9.dylib fzn_chuffed
install_name_tool -change libboost_system.dylib @executable_path/libboost_system.dylib fzn_chuffed
cd ..

zip -r chuffed-portable-$SHORT.zip $DIRNAME
