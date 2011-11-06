#!/bin/sh

EXT_NAME=portugaleid
BUILDS_DIR=builds
BUILD_DIR=_build

cd `dirname $0`

VERSION=`cat VERSION`
XPI_NAME=${EXT_NAME}-${VERSION}.xpi
CURRENT_NAME=${EXT_NAME}-CURRENT.xpi
OPTFORCE=0

# command line options
while getopts fh optionname
do
  case "$optionname" in
   h) echo "Usage: $0"
      echo "Options:"
      echo " -f: overwrite existing build"
      echo " -h: print this help message"
      exit 1;;
   f) OPTFORCE=1;;
esac
done

echo "starting xpi build.."

# check if build exists
if [ -e ${BUILDS_DIR}/${XPI_NAME} -a $OPTFORCE -ne 1  ]; then
  echo "A build for version ${VERSION} already exists. Run '$0 -f' to override."
  echo "  To change the version, edit VERSION and portugaleid/install.rdf"
  exit 1
fi
  
# check if build dir already exitst
if [ -e ${BUILD_DIR} ]; then
  echo "The build dir '${BUILD_DIR}' exists. Please remove the directory and run again."
  exit 1
fi

# create build dir
mkdir ${BUILD_DIR}
# copy files in build dir
cp -r $EXT_NAME/* ${BUILD_DIR}
# remove svn files
find ${BUILD_DIR} -path "*.svn" -type d -print0 | xargs -0 /bin/rm -rf

# create XPI
cd ${BUILD_DIR}
zip -r $XPI_NAME .
# copy XPI to builds dir
mkdir ../${BUILDS_DIR} 2> /dev/null
cp -f $XPI_NAME ../${BUILDS_DIR}

# create symbolic link to current dir
cd ../${BUILDS_DIR}
if [ -e ${CURRENT_NAME} ];then
	rm ${CURRENT_NAME}
fi
ln -s ${XPI_NAME} ${CURRENT_NAME}

cd ..

# clean up
rm -rf ${BUILD_DIR}

echo "xpi build completed"
