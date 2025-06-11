for k in {6..10}
do
   echo "ARRAY_SIZE $((10**$k))"
   #for i in {0..5}
   #do
   i=5
       echo "chapel-stream-copy"
       echo "CHPL_RT_NUM_THREADS_PER_LOCAL=$((2**$i)) --arraySize=$((10**$k))"
       for j in {1..10}
       do
	   CHPL_TASKS=qthreads CHPL_RT_NUM_THREADS_PER_LOCALE=$((2**$i)) ./publications/fall2023/stream_copy --arraySize=$((10**$k))
       done

       sleep 10
       echo "chplx-stream-copy"
       echo "--hpx:threads=$((2**$i)) --arraySize=$((10**$k))"
       for j in {1..10}
       do
           $HOME/experiment/chplx/build/backend/stream_copy/build/stream_cpp_copy --hpx:threads=$((2**$i)) --arraySize=$((10**$k))
       done

       sleep 10
       echo "chapel-stream-scale"
       echo "CHPL_RT_NUM_THREADS_PER_LOCAL=$((2**$i)) --arraySize=$((10**$k))"
       for j in {1..10}
       do
	   CHPL_TASKS=qthreads CHPL_RT_NUM_THREADS_PER_LOCALE=$((2**$i)) ./publications/fall2023/stream_scale --arraySize=$((10**$k))
       done

       sleep 10
       echo "chplx-stream-scale"
       echo "--hpx:threads=$((2**$i)) --arraySize=$((10**$k))"
       for j in {1..10}
       do
           $HOME/experiment/chplx/build/backend/stream_scale/build/stream_cpp_scale --hpx:threads=$((2**$i)) --arraySize=$((10**$k))
       done

       sleep 10
       echo "chapel-stream-add"
       echo "CHPL_RT_NUM_THREADS_PER_LOCAL=$((2**$i)) --arraySize=$((10**$k))"
       for j in {1..10}
       do
	   CHPL_TASKS=qthreads CHPL_RT_NUM_THREADS_PER_LOCALE=$((2**$i)) ./publications/fall2023/stream_add --arraySize=$((10**$k))
       done

       sleep 10
       echo "chplx-stream-add"
       echo "--hpx:threads=$((2**$i)) --arraySize=$((10**$k))"
       for j in {1..10}
       do
           $HOME/experiment/chplx/build/backend/stream_add/build/stream_cpp_add --hpx:threads=$((2**$i)) --arraySize=$((10**$k))
       done

       sleep 10
       echo "chapel-stream-triad"
       echo "CHPL_RT_NUM_THREADS_PER_LOCAL=$((2**$i)) --arraySize=$((10**$k))"
       for j in {1..10}
       do
	   CHPL_TASKS=qthreads CHPL_RT_NUM_THREADS_PER_LOCALE=$((2**$i)) ./publications/fall2023/stream_triad --arraySize=$((10**$k))
       done

       sleep 10
       echo "chplx-stream-triad"
       echo "--hpx:threads=$((2**$i)) --arraySize=$((10**$k))"
       for j in {1..10}
       do
           $HOME/experiment/chplx/build/backend/stream_triad/build/stream_cpp_triad --hpx:threads=$((2**$i)) --arraySize=$((10**$k))
       done
   #done
done
