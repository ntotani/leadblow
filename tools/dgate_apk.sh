CONF=$1

curl -F "file=@frameworks/runtime-src/proj.android/bin/leadblow-${CONF}.apk" \
     -F "token=dd1e7dd4e30de752e3db487b9fa96ab9990adf5e" \
     -F "message=$CONF" \
     https://deploygate.com/api/users/sepgame/apps
