
# https://help.github.com/articles/merging-an-upstream-repository-into-your-fork/
clear
echo Merging from AlexeyAB \"master\"
git checkout master
git pull https://github.com/AlexeyAB/darknet.git master
git push origin master

