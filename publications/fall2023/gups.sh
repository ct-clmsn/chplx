for k in {1..5}
do
   for i in {0..5}
   do
       echo "CHPL_RT_NUM_THREADS_PER_LOCAL=$((2**$i)) --memRatio=$((2**$k))"
       for j in {1..10}
       do
	   CHPL_TASKS=qthreads CHPL_RT_NUM_THREADS_PER_LOCALE=$((2**$i)) ./publications/fall2023/gups --memRatio=$((2**$k))
       done
       sleep 10
       echo "--hpx:threads=$((2**$i)) --memRatio=$((2**$k))"
       for j in {1..10}
       do
	   $HOME/experiment/chplx/build/backend/gups/build/gups_cpp --hpx:threads=$((2**$i)) --memRatio=$((2**$k))
       done
   done
done
