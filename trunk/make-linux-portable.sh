# Run this from the build directory.

HASH=$(git rev-parse HEAD)
SHORT=${HASH:0:8}
DATE=$(date +%Y%m%d)
DIRNAME=chuffed-linux-$DATE-$SHORT

cmake -DCMAKE_BUILD_TYPE=Release ..
make
mkdir -p $DIRNAME
cp fzn_chuffed $DIRNAME
cp /usr/lib/x86_64-linux-gnu/libprotobuf.so.9 $DIRNAME
cp /usr/lib/x86_64-linux-gnu/libboost_system.so.1.58.0 $DIRNAME
cp -r ../chuffed/flatzinc/mznlib $DIRNAME
cp ../chuffed.sh $DIRNAME

zip -r $DIRNAME.zip $DIRNAME
