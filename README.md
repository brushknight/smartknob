# Smart Knob Development Kit by @Seedlabs.it
## Current Version: 0.1

### Table of Contents
1. [Introduction](#introduction)
2. [Why a Smart Knob Development Kit](#why)
3. [Product Specifications](#specs)
4. [What's in this codebase](#what)
5. [Build vs Buy](#build-vs-buy) 
6. [License](#license) 
7. [FAQ](#faq)


### Introduction  <a name="introduction"></a>
The Smart Knob Dev Kit project is part of the family of products developed by Seedlabs. Seedlabs is focused in accellerating the transition to a modern grid: open source, distributed, digital and decarbonized. This particular product is designed for the community of developers and hardware tinkerers interested in contributing to the Seedlabs ecosystems.  

This project is based on [ScottBez](https://github.com/scottbez1)'s original [smart knob](https://github.com/scottbez1/smartknob/) design. Without his work and committment to the open source community, this codebase would not exist.

### Why a Smart Knob Development Kit <a name="why"></a>

Seedlabs' mission is to assist people in producing and consuming energy in a smarter way, encompassing everything from electricity to heating. To achieve this, we recognize the need for a certain level of automation in residential spaces, which necessitates dedicated software and hardware.

A key part of persuading people to be mindful of their energy consumption is to ensure they don't have to constantly think about it. We aim to avoid creating another complex energy monitoring system that demands extensive knowledge of software and hardware, has a complicated user interface, and is difficult to maintain.

Instead, our goal is to create delightful experiences for our users, helping them seamlessly integrate smart energy solutions into their daily lives.
One of our priorities is to craft natural user-machine interaction in our controllers. We're competing with solutions as straightforward as a traditional light switch. The original Smart Knob design innovatively combined four different interaction patterns: a rotary controller, a push button, a display, and force/torque-based haptic feedback. While there are many possibilities for what can be done, creating an intuitive user experience is a challenging yet rewarding task.

As [Grigorii](https://github.com/brushknight/) was implementing Scott's original design, we received hundreds of requests from around the world. People are eager to experiment with the knob, potentially developing software and hardware extensions for it. We saw this as a fantastic opportunity to engage a wider community in addressing the question of what makes an ideal residential controller. Therefore, we decided to manufacture a development kit specifically for this purpose.

### Product Specifications <a name="specs"></a>

This development kit is based on the original Smart Knob design, but features some changes to facilitate development and test new interaction pattern. 

Specifically: 

#### List of hardware and mechanical changes
- esp32s3-n16r8-u
- 72 addressable ec15 rgb leds (positions in cricle around the knob, at 5 degree angle from each other)
- Proximity sensor based on ToF VL53L0X
- 8 conductos 0.5mm pitch FPC calbe to drive screen
- reset and boot buttons
- 7 additional GPIOs on the back side
- 3 stemma qt connectors onthe back for i2c modules
- Slight change of the knob shape for better grip
- Redesign of the enclosure to allow for leds to light through 

 You can find all the electronics and mechanical design files in [./electronics_brushknight_esp32s3](./electronics_brushknight_esp32s3)

[Base PCB bill of materials (BOM)](https://github.com/brushknight/smartknob/releases/download/devkit_v0.1/base_ibom.html)

[Screen PCB bill of materials (BOM)](https://github.com/brushknight/smartknob/releases/download/devkit_v0.1/screen_ibom.html)



#### List of software changes (yet not merged #3)
- Firmware extended to support the new hardware
- WiFi support
- MQTT setup included in the onboarding flow
- Code refactored to allow the concept of Apps and Menu



### What is in this codebase <a name="what"></a>

Coming Soon

### Build vs Buy <a name="build-vs-buy"></a>

In this codebase, you will find all the materials needed to create your own smart knob. This includes KiCad files for the PCB, Fusion 360 files for the enclosure, a bill of materials for the components, and firmware to flash the device.
We also offer a fully assembled version of the smart knob (soldered PCB with a plastic enclosure) that is pre-flashed with firmware, available in our [store](https://store.seedlabs.it/products/smartknob-devkit-v1-0).
Our aim is to continue offering the device at a no-profit cost. In other words, even considering the shipping costs, it's unlikely that you'll spend less than what our store charges.

That said, our goal is not to manufacture this kit at scale, nor to maintain a constant backstock of it. As such, it might take longer to receive it from us.
Additionally, if you have experience with PCB soldering, using pick and place machinery, or if you are interested in exploring how to get this built through third-party services, this project is an excellent opportunity to get your hands dirty. However, make sure you fully understand what you are embarking on, as this project is not entry-level, both from a mechanical and electronic standpoint.

Either way, if you have any questions, feel free to drop by our [Discord server](https://discord.gg/8tgPc484nA).



### License <a name="license"></a>


Your text provides a clear explanation of the licensing terms for Seedlabs' open hardware ecosystem. Here's a proofread version for enhanced clarity and grammatical accuracy:

A more detailed license can be found in the license file.

In this paragraph, we aim to present a layman's version of the spirit behind it. Our goal is to enable as many contributions as possible to the Seedlabs ecosystem of open hardware. Additionally, we want to preserve the spirit behind the original project (Apache V2, Creative Commons Attribution) and all the libraries that have been used in this codebase.

As such, we require that:

1. All changes you make to the software and hardware are shared with the community in an open-source manner, maintaining the same spirit of the license used here.

2. You can use the hardware and software of this project for educational and personal projects. This means you can build as many smart knobs as you want, as long as they are used in an educational setting (e.g., if you are a university and want to offer the device as part of a course) or for personal use (e.g., if you want to install 10 dev kits in your home).

3. You can use the hardware and software of this project for commercial purposes, but you first need to obtain a partnership agreement with Seedlabs. This ensures that proper attribution is in place, all changes implemented remain open source, and there is a reasonable redirection (royalties) of the value generated by the work of the community towards the ultimate goal of this project. This arrangement varies depending on the downstream utilization. The spirit behind these requirements is to stay true to Seedlabs' mission. For instance, we might charge higher royalties if this project is used in a petrol car, as opposed to being used in a medical device.
Ultimately, this work is derivative of the original Smart Knob project. If you find the license too stringent for your use case, we recommend exploring the original version and building directly from it.


  
### FAQ <a name="faq"></a>

#### I want to understand better how the Smart Knob is assembled and works?

Scott's original "Inside the SmartKnob View" remains highly relevant, despite some differences in the design of the enclosure and PCB.
<a href="https://www.youtube.com/watch?v=Q76dMggUH1M">
    <img src="https://img.youtube.com/vi/Q76dMggUH1M/maxresdefault.jpg" width="480" />
</a>

#### How do I flash my Seedlabs' Smart Knob Dev Kit?

Our Smart Knob Dev Kit comes with pre-flashed firmware, the same version that you can find in this repository. The Smart Knob uses [PlatformIO](https://platformio.org/) to build the firmware. We recommend using [Microsoft Visual Studio Code](https://code.visualstudio.com/) as the development environment, where you can install the PlatformIO IDE.

To proceed, check out this codebase on your machine. Then, from Visual Studio Code, navigate to the PlatformIO tab. Select 'Upload and Monitor All' from the Project Tasks. This will open a terminal, compile the code, upload the firmware, and monitor the serial communication between your computer and the knob.