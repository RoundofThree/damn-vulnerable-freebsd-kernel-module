# DVKM for CheriBSD

Damn Vulnerable Kernel Module for FreeBSD (untested) and CheriBSD (purecap and hybrid kernels).
This is a work-in-progress research prototype. Analysis in another repository.

## Vulnerabilities

- Buffer overflow (write)
    - Stack
        - Standard
        - Subobject
    - Heap
        - General purpose kmem
            - Standard
            - Subobject
        - Dedicated UMA zone
            - Standard
            - Subobject
- Linear memory disclosure (read)
    - Stack
        - Standard
        - Subobject
    - Heap
        - General purpose kmem
            - Standard
            - Subobject
        - Dedicated UMA zone
            - Standard
            - Subobject
- Heap use after free (trigger malloc, free, read and write in separate invocations)
    - General purpose kmem zone (speify the size)
    - Dedicated UMA zone (specify the name)
- Stack use after free (TODO: hardcoded, how do I make it more flexible?)
- Arbitrary memory disclosure (read)
- Arbitrary overwrite (write)
- Arbitrary integer/pointer increment
- Pointer hijack (TODO) <!-- I use it to test ret2dir in CheriBSD -->
- Uninitialized memory (read/write)
    - Stack
    - Heap
- Double fetch
- Leak pagetable l0

## Build instructions

In a FreeBSD host, (untested)
```sh
make
```

In a CheriBSD host, to build a A64 module for a hybrid kernel,
```sh
env MACHINE_ARCH=aarch64 make
```

And to build a C64 module for a purecap kernel,
```sh
make
```

Build artifacts are in `obj/`.

## References

- [Damn Vulnerable Kernel Module for kernel fuzzing](https://github.com/hardik05/Damn_Vulnerable_Kernel_Module/tree/main)
- [HackSys Extreme Vulnerable Driver (HEVD)](https://github.com/hacksysteam/HackSysExtremeVulnerableDriver/tree/master)
