#!/home/pmilewski/hw3-pmilewski/smash
echo Now running inside smash
ls -l >forTesting/newOutfile
ls /blah 2>forTesting/newErrfile
sort <forTesting/unsorted.txt >forTesting/sorted.txt
echo Now exiting smash
exit
