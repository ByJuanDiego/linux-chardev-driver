# Read/Write Character Device Driver for Linux

## Team Members

- Juan Diego Castro
- Joaquin Jordan

## Introduction

This project implements a simple character device driver for Linux that supports basic **read** and **write** operations. The driver interacts with user-space programs to demonstrate kernel-level programming.

---

## Getting Started

### Prerequisites

- Linux system with kernel development tools.
- Basic understanding of Linux kernel modules and user-space interaction.

---

## Building and Running the Driver

### 1. **Driver Setup**

#### Compile the driver:
```bash
make
```

#### Load the driver:
```bash
sudo insmod chardev.ko
```

#### Unload the driver:
```bash
sudo rmmod chardev
```

### 2. **User-Space Interaction**

#### Compile the user-space programs:
```bash
cd user
make
```

#### Write data to the driver:
```bash
sudo ./chardev_write "Hello\!"
```

#### Read data from the driver:
```bash
sudo ./chardev_read
```

#### Check system logs:
```bash
sudo dmesg
```

```
[83634.777294] chardev: [write] Successfully wrote 6 bytes
[83658.524777] chardev: [read] Successfully read 6 bytes
```
