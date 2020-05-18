#!/bin/bash
# test script #1

echo test script 1
echo This test ensures that file redirection works for stdin, stdout, and stderr
echo Please look at testRun1.sh for the commands that will be put into the smash shell
echo Also running smash with -d to see the commands being run
echo Keep in mind some debug info will be in newErrfile since stderr is redirected
./testRun1.sh -d
echo newErrfile should contain an error since we cannot ls a file called /blah:
cat forTesting/newErrfile
echo newOutfile should just contain an ls -l of the current directory:
cat forTesting/newOutfile
echo sorted.txt should contain the contents of unsorted.txt but sorted alphabetically:
cat forTesting/sorted.txt
echo 
