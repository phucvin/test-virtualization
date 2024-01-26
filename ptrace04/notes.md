TODO:
- Manage memory of the child using a custom manager similar to gVisor.
- Write a simple ptrace tool to pause process when it tries to exit, so we can view /proc/pid/* (e.g. maps, mem) of short-live processes.

References:
- https://github.com/google/gvisor/tree/master/pkg/sentry/mm
- https://groups.google.com/g/gvisor-users/c/9KOz4qBKl18?pli=1
- https://man7.org/linux/man-pages/man5/proc.5.html
- https://blog.cloudflare.com/diving-into-proc-pid-mem
- https://www.baeldung.com/linux/proc-id-maps
- https://groups.google.com/g/gvisor-users/c/9KOz4qBKl18?pli=1 (Sentry memory management)
- https://github.com/google/gvisor/blob/master/pkg/sentry/mm/README.md
