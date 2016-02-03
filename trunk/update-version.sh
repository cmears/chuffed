HASH=$(git rev-parse HEAD)
BRANCH=$(git rev-parse --abbrev-ref HEAD)
DIRTY=$(git status --porcelain)

if [ -n "$DIRTY" ] ; then
    DIRTYMSG=" (uncommited files present)"
else
    DIRTYMSG=""
fi

#echo "$BRANCH@$HASH$DIRTYMSG" > "$1"

VERSION="$BRANCH@$HASH$DIRTYMSG"

cat > "$1" <<EOF
#ifndef VERSION_H
#define VERSION_H

const std::string versionString = "chuffed $VERSION";

#endif
EOF
