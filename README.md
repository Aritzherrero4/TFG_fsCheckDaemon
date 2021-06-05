# fsCheckDaemon
This repository contains all the files needed by the daemon fsCheck. 
This particular daemon is responsible for generating a Merkle tree starting in the indicated directory and updating the root hash whenever a change notifications is received.

For more information, check out the complementary daemon to [detect changes made to the filesystem in real time](https://github.com/Aritzherrero4/TFG_fsNotifierDaemon) or the complementary project, [Distributed integrity monitoring system based on Blockchain technology](https://github.com/AitorB16/Blockchain-based-integrity-monitoring-distributed-net-protocol).

## Configuration
The configuration file fsCheck.config allows the user to specify the base path that will be the root directory of the system and the hash algorithm to use. 
There are two algorithms supported: Blake3 and SHA-256. This file must be in the same directory of the executable. 

Apart of this, systemd and daemon specific configurations are managed though the file fsCheck.service. There, the `ExecPath` and `WorkingDirectory` should be indicated in order to use it with systemctl.  
The `ExecPath` parameter must point to the executable file generated after the compilation and the `WorkingDirectory` should be the directory with the configuration file, usually the same directory that has the executable file.

## Dependencies

The code must be compiled with C++17, since it uses the implementation of the Filesystem library included in that release. Apart of that, two components must be installed before compiling:

* libsystemd-dev: used for sd_bus and logging.
* libcrypto++-dev: Used for the sha-256 implementation.

In addition, The C Blake3 implementation is used but included in this repository. See the [Blake3 GitHub repository](https://github.com/BLAKE3-team/BLAKE3) for more details.


## Building the program

With the repository, a makefile is included in order to facilitate the compilation process. To compile it, run:

```bash
make
```
If Blake3 is not already compiled, it will compile it automatically. 
### Other make options

* `make clean`: Clean all the files related to the program but does not remove blake3 library if compiled.
* `make clean-all`: Remove all the files, including the blake3 library.
* `make all`: Compile all, including Blake3 even if it is already there.
* `make fresh`: Shortcut for `make clean-all` and `make all`. 


### ARM support
The code is multiplatform ready but Blake3 needs different compilation for each platform since it depends on architecture specific instructions. For x86, AVX, SSE and so on are used that will not work on ARM.
The makefile will detect the architecture and, if ARM is detected, the ARM specific Blake3 compilation will be used that requires NEON support. If NEON is not supported, this can be used to compile a generic version of BLAKE3:

``` bash
gcc -shared -O3 -o libblake3.so blake3.c blake3_dispatch.c blake3_portable.c
```

This can be automatically done by running:

```
make generic
```

This has not been properly tested. The generic version should only be used if all the other commands fail due to incompatibilities with the architecture. For more details check [Blake3 C building](https://github.com/BLAKE3-team/BLAKE3/tree/master/c#building).

## Executing the program

The program can be run as a normal program by just executing it in a shell:

```
./fsCheckDaemon &
```

But it can also be run as a daemon. To do it, first build the program and indicate the correct path to the executable and the working directory in fsCheck.service as indicated in the configuration section. 

Then add it with systemctl:
```
systemctl --user enable /path/to/the/daemon/fsCheck.service

```

Now, the daemon can be managed as a normal daemon with systemctl:

```
systemctl --user start fsCheck.service
systemctl --user stop fsCheck.service
systemctl --user status fsCheck.service
```

## Checking the logs

The program writes to the system log directly when it is completely initialized or every time there is a change. This logs can be checked directly with:

```
journalctl -n20 | grep fsCheck
```

Or, if the program was launched with systemctl, in addition to that, systemctl can be used:

```
systemctl --user status fsCheck.service
```

## Intellectual property
The code is copyright 2021 Aritz Herrero and released under MIT License.
For more information, check [LICENCE](https://github.com/Aritzherrero4/TFG_fsCheckDaemon/blob/master/LICENSE) file.

### External libraries
The C implementation of Blake3 in external/blake3 is released as CCO 1.0 or alternatively under Apache 2.0 license as indicated in [Blake3 License](https://github.com/BLAKE3-team/BLAKE3/blob/master/LICENSE). This is included as a dependency and it is not developed or modified by me.

