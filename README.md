Pololu Mini Maestro 12 Servo Controller Code for the Genode framework

## Project Directory

```
.
+-- include
+-- run
+-- src
|   +-- server
|   +-- test
    |   +-- ctrl
    +-- app
    |   +-- net\_servo\_adapter
    |   +-- net\_servo\_client
```

## Install Walkthrough

1. Follow the instructions under (skip the last step ```make jenkins_run```)

https://argos-research.github.io/documentation/install.html#operating-system-on-local-machine

2. Clone the genode-world under genode/repos (assuming you are in the operating-system directory)
```
git clone https://github.com/argos-research/genode-world.git genode/repos/genode-world
```

3. Clone this repository into genode/repos (assuming you are in the operating-system directory)
```
git clone https://github.com/argos-research/genode-pololuServoController.git 
```

4. Create a build directory for a rpi and a panda build
```
GENODE_TARGET=focnados_rpi make jenkins_build_dir
GENODE_TARGET=focnados_panda make jenkins_build_dir
```

5. Edit the ```etc/build.conf``` in each build directory by adding the following lines
```
REPOSITORIES += $(GENODE_DIR)/repos/genode-world
REPOSITORIES += $(GENODE_DIR)/../genode-pololuServoController
```

6. Prepare the missing ports by executing (again in operating-system directory)
```
./genode/tool/ports/prepare_port libmosquitto openssl
```

7. Change to the individual build directories and trigger the compilation

- In ```genode-focnados_rpi``` build directory:

```
make run/net_servo_client
```

- In ```genode-focnados_panda``` build directory:
```
make run/net_servo_adapter
```

## Prerequisites (legacy)

- uncomment the following line in your build.conf
```
REPOSITORIES += $(GENODE_DIR)/repos/libports
REPOSITORIES += $(GENODE_DIR)/repos/dde_linux
```

- further proceeding 
```
cd repos/libports && make prepare
cd repos/dde_linux && make prepare
```
