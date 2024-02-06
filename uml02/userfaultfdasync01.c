#include <fcntl.h>
#include <linux/userfaultfd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/poll.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#define UFFD_USER_MODE_ONLY 1
#define UFFD_FEATURE_WP_ASYNC (1<<15)
#define UFFDIO_COPY_MODE_WP ((__u64)1<<1)

struct uffdio_writeprotect {
  struct uffdio_range range;
#define UFFDIO_WRITEPROTECT_MODE_WP ((__u64)1 << 0)
#define UFFDIO_WRITEPROTECT_MODE_DONTWAKE ((__u64)1 << 1)
  __u64 mode;
};

#define _UFFDIO_WRITEPROTECT (0x06)
#define UFFDIO_WRITEPROTECT \
  _IOWR(UFFDIO, _UFFDIO_WRITEPROTECT, struct uffdio_writeprotect)


typedef long long int int64_t;

void SetWriteProtect(int uffd, int64_t start, int64_t len, int write_protect) {
  struct uffdio_writeprotect wp;
  wp.range.start = start;
  wp.range.len = len;
  wp.mode = write_protect ? UFFDIO_WRITEPROTECT_MODE_WP : 0;
  if (ioctl(uffd, UFFDIO_WRITEPROTECT, &wp) != 0)
    printf("UFFDIO_WRITEPROTECT failed: %s\n", strerror(errno));
}

int main() {
  int uffd =
      syscall(__NR_userfaultfd, O_CLOEXEC | O_NONBLOCK | UFFD_USER_MODE_ONLY);
  if (uffd <= 0) printf("__NR_userfaultfd failed: %s\n", strerror(errno));

  struct uffdio_api api;
  api.api = UFFD_API;
  api.features = 0;  // 0 probably means all features
  if (ioctl(uffd, UFFDIO_API, &api) != 0)
    printf("uffdio_api failed: %s\n", strerror(errno));
  if ((api.ioctls & UFFD_API_IOCTLS) != UFFD_API_IOCTLS)
    printf("api.ioctls mismatched, expected: %llx, actual: %llx\n",
           UFFD_API_IOCTLS, api.ioctls);
  if ((api.features & UFFD_FEATURE_WP_ASYNC) == UFFD_FEATURE_WP_ASYNC)
    printf("api.features missing UFFD_FEATURE_WP_ASYNC, features: %llx\n",
           api.features);

  return 0;
}
