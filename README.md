# OS Jackfruit Mini Container Runtime

## Problem Statement
This project implements a lightweight container runtime using Linux system primitives such as namespaces, chroot, and cgroups.

## Features
- Process creation using fork and exec
- Inter-process communication using UNIX sockets
- Filesystem isolation using chroot
- Memory control using cgroups
- Workload testing using memory_hog

## How to Run

### Build
```bash
