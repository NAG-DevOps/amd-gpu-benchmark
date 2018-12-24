set fit limit 1e-9
set logscale x
set logscale y

fgl(x) = agl*x + bgl
fit fgl(x) "bench_gpu_loops.txt" via agl,bgl

fcl(x) = acl*x + bcl
fit fcl(x) "bench_cpu_loops.txt" via acl,bcl

fcw(x) = acw*x + bcw
fit fcw(x) "bench_cpu_workers.txt" via acw,bcw

fgw(x) = agw*x + bgw
fit fgw(x) "bench_gpu_workers.txt" via agw,bgw

plot \
  "bench_gpu_loops.txt" with points, fgl(x) with lines, \
  "bench_cpu_loops.txt" with points, fcl(x) with lines, \
  "bench_cpu_workers.txt" with points, fcw(x) with lines, \
  "bench_gpu_workers.txt" with points, fgw(x) with lines

print fgl(1e9)
print fcl(1e9)
print fcw(1e9)
print fgw(1e9)
print fgl(1e9) / fcl(1e9)
print fcl(1e9) / fgw(1e9)
print fgl(1e9) / fgw(1e9)

pause -1