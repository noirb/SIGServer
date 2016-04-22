#!/bin/sh

if [ ! `echo $0 | grep 'release.sh'` ] ; then
  echo '[Error] Please execute the shell script file directly without the dot or the source command.'
  return 2>&- || exit
fi

# Go to the directory of this shell file.
cd `dirname $0`

# Delete the unnecessary files.
echo 'deleting the unnecessary files...'
rm -rf build/
rm -r .git/
rm .gitignore
rm .gitmodules
rm release.sh
rm -f *~

# Create a release file.
echo 'compressing...'
cd ..
tar cfz SIGServer-X.X.X.tar.gz SIGServer/

# Delete the original directory.
echo 'deleting the original directory...'
rm -rf SIGServer/

echo 'finished.'

