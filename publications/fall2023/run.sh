echo "chapel-heat"
for i in {0..5}
do
	echo "CHPL_RT_NUM_THREADS_PER_LOCAL=$((2**$i))"
   for j in {1..10}
   do
	   CHPL_TASKS=qthreads CHPL_RT_NUM_THREADS_PER_LOCALE=$((2**$i)) ./publications/fall2023/heat
   done
done
sleep 10
echo "chplx-heat"
for i in {0..5}
do
	echo "--hpx:threads=$((2**$i))"
   for j in {1..10}
   do
	   $HOME/experiment/chplx/build/backend/heat/build/heateqn --hpx:threads=$((2**$i))
   done
done
sleep 10
echo "chapel-gups"
for i in {0..5}
do
   for k in {1..5}
   do
	echo "CHPL_RT_NUM_THREADS_PER_LOCAL=$((2**$i)) --memRatio=$((2**$k))"
       for j in {1..10}
       do
	   CHPL_TASKS=qthreads CHPL_RT_NUM_THREADS_PER_LOCALE=$((2**$i)) ./publications/fall2023/gups --memRatio=$((2**$k))
       done
   done
done
sleep 10
echo "chplx-gups"
for i in {0..5}
do
   for k in {1..5}
   do
	echo "--hpx:threads=$((2**$i)) --memRatio=$((2**$k))"
       for j in {1..10}
       do
	   $HOME/experiment/chplx/build/backend/gups/build/gups_cpp --hpx:threads=$((2**$i)) --memRatio=$((2**$k))
       done
   done
done
sleep 10
echo "chapel-stream"
for i in {0..5}
do
   for k in {2..7}
   do
	echo "CHPL_RT_NUM_THREADS_PER_LOCAL=$((2**$i)) --arraySize=$((10**$k))"
       for j in {1..10}
       do
	   CHPL_TASKS=qthreads CHPL_RT_NUM_THREADS_PER_LOCALE=$((2**$i)) ./publications/fall2023/stream --arraySize=$((10**$k))
       done
   done
done
sleep 10
echo "chplx-gups"
for i in {0..5}
do
   for k in {2..7}
   do
        echo "--hpx:threads=$((2**$i)) --arraySize=$((10**$k))"
       for j in {1..10}
       do
           $HOME/experiment/chplx/build/backend/stream/build/stream_cpp --hpx:threads=$((2**$i)) --arraySize=$((10**$k))
       done
   done
done
