# copied from http://sleepycoders.blogspot.ie/2013/03/sharing-travis-ci-generated-files.html

if [ "$TRAVIS_PULL_REQUEST" == "false" ]; then
  echo -e "Starting to update gh-pages\n"
  
  doxygen Doxyfile

  #copy data we're interested in to other place
  cp -R html $HOME/html

  #go to home and setup git
  cd $HOME
  git config --global user.email "travis@travis-ci.org"
  git config --global user.name "Travis"

  #using token clone gh-pages branch
  git clone --quiet --branch=gh-pages https://${GH_TOKEN}@github.com/wheybags/freeablo.git  gh-pages > /dev/null
 
  # clear old files
  find . -depth 1 ! -iname ".git" -exec rm -rf {} \;

  #go into diractory and copy data we're interested in to that directory
  cd gh-pages
  cp -Rf $HOME/html/* .

  #add, commit and push files
  git add -f .
  git commit -m "Travis build $TRAVIS_BUILD_NUMBER pushed to gh-pages"
  git push -fq origin gh-pages > /dev/null
fi
