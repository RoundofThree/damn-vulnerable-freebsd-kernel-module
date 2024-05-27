# DVKM for FreeBSD and CheriBSD

Damn Vulnerable Kernel Module for FreeBSD and CheriBSD.

## Vulnerabilities

- Buffer overflow
    - Stack
        - Standard
        - Subobject
    - Heap
        - General purpose kmalloc
            - Standard
            - Subobject
        - Special bucket
            - Standard
            - Subobject
        - Page
- Double fetch


## Build instructions

In a FreeBSD host,
```sh
make
```

In a CheriBSD host, to build a A64 module for a hybrid kernel,
```sh
env MACHINE_ARCH=aarch64 MACHINE=arm64 make
```

And to build a C64 module for a purecap kernel,
```sh
make
```

Build artifacts are in `obj/`.
