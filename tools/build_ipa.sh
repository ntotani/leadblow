cd $(dirname $0)/../frameworks/runtime-src/proj.ios_mac
DIR=$(pwd)
CONF=$1
JOB=$2
PROJ=leadblow
MPROV=leadblow_AdHoc.mobileprovision

xcodebuild -sdk iphoneos7.1 -project $PROJ.xcodeproj -scheme leadblow -configuration $CONF $JOB CODE_SIGN_IDENTITY="iPhone Distribution: Maho Tsutsui (4VK3MQNCVY)"
xcrun -sdk iphoneos7.1 PackageApplication -v $DIR/DerivedData/$PROJ/Build/Products/$CONF-iphoneos/${PROJ}_iOS.app -o $DIR/DerivedData/$PROJ/Build/Products/$CONF-iphoneos/${PROJ}_iOS.ipa --embed $DIR/$MPROV --sign "iPhone Distribution: Maho Tsutsui (4VK3MQNCVY)"
