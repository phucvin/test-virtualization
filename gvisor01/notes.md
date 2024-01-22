In Github codespaces:

- Follow https://gvisor.dev/docs/user_guide/install/
- But `sudo systemctl reload docker` won't work, do `ps aux` find `dockerd` pid and `sudo kill -9 <pid>`, then `sudo dockerd &` to restart it
- Then continue from https://gvisor.dev/docs/user_guide/quick_start/docker/