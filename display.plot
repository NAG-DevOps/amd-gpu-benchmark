set fit limit 1e-9
set logscale x
set logscale y

set key left top
set xlabel "Time (ms)"
set ylabel "Operations (*1e6)"

fgl(x) = agl*x + bgl
fit fgl(x) "bench_gpu_loops.txt" u ($2/1e6):($1/1e6) via agl,bgl

fcl(x) = acl*x + bcl
fit fcl(x) "bench_cpu_loops.txt" u ($2/1e6):($1/1e6) via acl,bcl

fgw(x) = agw*x + bgw
fit fgw(x) "bench_gpu_workers.txt" u ($2/1e6):($1/1e6) via agw,bgw

plot \
  "bench_gpu_workers.txt" using ($2/1e6):($1/1e6) with points title "GPU many workers", \
  fgw(x) with lines title "GPU many workers fn", \
  "bench_gpu_loops.txt" using ($2/1e6):($1/1e6) with points title "GPU 1 worker", \
  fgl(x) with lines title "GPU 1 worker fn", \
  "bench_cpu_loops.txt" using ($2/1e6):($1/1e6) with points title "CPU", \
  fcl(x) with lines title "CPU fn"

print ""
print "-= Millions operations per second =-"
print "CPU: ", int(fcl(1e3))
print "GPU 1 worker: ", int(fgl(1e3))
print "GPU many workers: ", int(fgw(1e3))
print ""
print "-= Ratios =-"
print "CPU / GPU 1 worker: ", int(fcl(1e3) / fgl(1e3))
print "GPU many workers / CPU: ", int(fgw(1e3) / fcl(1e3))
print "GPU many workers / GPU 1 worker: ", int(fgw(1e3) / fgl(1e3))

pause -1
