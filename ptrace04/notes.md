TODO:
- Manage memory of the child using a custom manager similar to gVisor.
- Write a simple ptrace tool to pause process when it tries to exit, so we can view /proc/pid/* (e.g. maps, mem) of short-live processes.

References:
- https://github.com/google/gvisor/tree/master/pkg/sentry/mm
- https://groups.google.com/g/gvisor-users/c/9KOz4qBKl18?pli=1
