Pololu Mini Maestro 12 Servo Controller Code for the Genode framework

## Project Directory

```
.
+-- include
+-- run
+-- src
|   +-- server
|   +-- test
    |   +-- simple
```

## Prerequisites

- uncomment the following line in your build.conf
```
REPOSITORIES += $(GENODE_DIR)/repos/dde_linux
```

- further proceeding 
```
cd repos/dde_linux
make prepare
```
