#!/home/pmilewski/hw3-pmilewski/smash
echo Now running inside smash
pwd
cd forTesting
pwd
cd ..
pwd
echo testing ls *.sh
ls *.sh
echo here is the exit status for the last command: $?
echo here is the path: $PATH
echo here is the path: $PATH and the last exit status: $?
echo have fun grader :)
echo Now exiting smash
exit
