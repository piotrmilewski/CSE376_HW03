#!/home/pmilewski/hw3-pmilewski/smash
echo Now running inside smash
jobs
sleep 1 &
sleep 2 &
jobs
sleep 2
jobs
sleep 2 &
jobs
fg 2
jobs
sleep 2 &
kill -20 3
jobs
bg 3
jobs
sleep 2
jobs
echo Now exiting smash
exit
