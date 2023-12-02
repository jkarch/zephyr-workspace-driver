# Zephyr Example Application, modified to support multiple out of tree driver examples, particularly the iis3wdb vibration sensor by Josh Karch
https://github.com/jkarch?tab=repositories  
Feel free to modify / contribute to your likings, this is a breakout point for getting started with Zephyr.  
This code served as a first cut opportunity to make a driver for the ST Micro iis3wdb vibration sensor, which does not yet exist in Zephyr.  
The driver is loosely based on [https://github.com/muhammadadeelzahid/LIS3DSH-Drivers-Zephyr-RTOS/tree/main](https://oxeltech.de/en/how-to-write-a-driver-for-accelerometer-lis3dsh-in-zephyr/), where the structure was modified to support the iis3wdb vibration sensor: https://www.st.com/en/mems-and-sensors/iis3dwb.html  

This repository contains a Zephyr example application. The main purpose of this
repository is to serve as a reference on how to structure Zephyr-based
applications. Some of the features demonstrated in this example are:

- Basic [Zephyr application][app_dev] skeleton
- [Zephyr workspace applications][workspace_app]
- [Zephyr modules][modules]
- [West T2 topology][west_t2]
- [Custom boards][board_porting]
- Custom [devicetree bindings][bindings]
- Out-of-tree [drivers][drivers]
- Out-of-tree libraries
- Example CI configuration (using Github Actions)
- Custom [west extension][west_ext]

This repository is versioned together with the [Zephyr main tree][zephyr]. This
means that every time that Zephyr is tagged, this repository is tagged as well
with the same version number, and the [manifest](west.yml) entry for `zephyr`
will point to the corresponding Zephyr tag. For example, the `example-application`
v2.6.0 will point to Zephyr v2.6.0. Note that the `main` branch always
points to the development branch of Zephyr, also `main`.

[app_dev]: https://docs.zephyrproject.org/latest/develop/application/index.html
[workspace_app]: https://docs.zephyrproject.org/latest/develop/application/index.html#zephyr-workspace-app
[modules]: https://docs.zephyrproject.org/latest/develop/modules.html
[west_t2]: https://docs.zephyrproject.org/latest/develop/west/workspaces.html#west-t2
[board_porting]: https://docs.zephyrproject.org/latest/guides/porting/board_porting.html
[bindings]: https://docs.zephyrproject.org/latest/guides/dts/bindings.html
[drivers]: https://docs.zephyrproject.org/latest/reference/drivers/index.html
[zephyr]: https://github.com/zephyrproject-rtos/zephyr
[west_ext]: https://docs.zephyrproject.org/latest/develop/west/extensions.html

## Getting Started

Before getting started, make sure you have a proper Zephyr development
environment. Follow the official
[Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/getting_started/index.html).
This is adapted from: https://github.com/nrfconnect/ncs-example-application/tree/main

### Initialization

The first step is to initialize the workspace folder (``my-workspace``) where
the ``example-application`` and all Zephyr modules will be cloned. Run the following
command:

```shell
# initialize my-workspace for the example-application (main branch)
west init -m https://github.com/jkarch/zephyr-workspace-driver --mr main my-exampledriver-workspace
# update Zephyr modules
cd my-exampledriver-workspace/zephyr-workspace-driver
west update
```

### Building and running

To build the application for the nrf52832 run the following command:

```shell
west build -b nrf52dk_nrf52832 app -- -DDTC_OVERLAY_FILE=$(PWD)/boards/nrf52dk_nrf52832.overlay 
```

Feel free to modify / add more boards


Once you have built the application, run the following command to flash it:

```shell
west flash
```

