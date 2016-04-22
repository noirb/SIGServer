#!/bin/sh

if [ ! `echo $0 | grep 'release.sh'` ] ; then
  echo '[Error] Please execute the shell script file directly without the dot or the source command.'
  return 2>&- || exit
fi

# Go to the directory of this shell file.
cd `dirname $0`

BASENAME=`basename \`pwd\``
VERSION_STR=`head -n 1 version.txt | tr -d " "`


# Update the git submodules.
echo 'updating the git submodules...'
git submodule init
git submodule update

# Delete the unnecessary files.
echo 'deleting the unnecessary files...'
rm -rf build/
rm -rf .git/
rm -f .gitignore
rm -f .gitmodules
rm -f release.sh
rm -f *~

# Create a release file.
echo 'compressing...'
cd ..
mv ${BASENAME}/ SIGServer-${VERSION_STR}/
tar cfz SIGServer-${VERSION_STR}.tar.gz SIGServer-${VERSION_STR}/

# Delete the original directory.
echo 'deleting the original directory...'
rm -rf SIGServer-${VERSION_STR}/

echo 'finished.'

