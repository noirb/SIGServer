#!/bin/sh

if [ ! `echo $0 | grep 'release.sh'` ] ; then
  echo '[Error] Please execute the shell script file directly without the dot or the source command.'
  return 2>&- || exit
fi

# Go to the directory of this shell file.
cd `dirname $0`

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
rm -f model/.git
rm -f model/.gitignore
rm -f srcs/sigverse/x3d/.git
rm -f srcs/sigverse/x3d/.gitignore
rm -f release.sh
rm -f *~

# Create a release file.
echo 'compressing...'
cd ..
tar cfz SIGServer-X.X.X.tar.gz SIGServer/

# Delete the original directory.
echo 'deleting the original directory...'
rm -rf SIGServer/

echo 'finished.'

