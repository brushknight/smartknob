# Smart Knob Development Kit by @Seedlabs.it
## Current Version: 0.1

### Table of Contents
1. [Introduction](#introduction)
2. [Why a Smart Knob Development Kit](#why)
3. [Product Specifications](#specs)
4. [What's in this codebase](#what)
5. [Build vs Buy](#build-vs-buy) 
6. [License](#license) 


### Introduction  <a name="introduction"></a>
The Smart Knob Dev Kit project is part of the family of products developed by Seedlabs. Seedlabs is focused in accellerating the transition to a modern grid: open source, distributed, digital and decarbonized. This particular product is designed for the community of developers and hardware tinkerers interested in contributing to the Seedlabs ecosystems.  

This project is based on [ScottBez](https://github.com/scottbez1)'s original [smart knob](https://github.com/scottbez1/smartknob/) design. Without his work and committment to the open source community, this codebase would not exist.

### Why a Smart Knob Development Kit <a name="why"></a>

Seedlabs' mission is to assist people in producing and consuming energy in a smarter way, encompassing everything from electricity to heating. To achieve this, we recognize the need for a certain level of automation in residential spaces, which necessitates dedicated software and hardware.

A key part of persuading people to be mindful of their energy consumption is to ensure they don't have to constantly think about it. We aim to avoid creating another complex energy monitoring system that demands extensive knowledge of software and hardware, has a complicated user interface, and is difficult to maintain.

Instead, our goal is to create delightful experiences for our users, helping them seamlessly integrate smart energy solutions into their daily lives.
One of our priorities is to craft natural user-machine interaction in our controllers. We're competing with solutions as straightforward as a traditional light switch. The original Smart Knob design innovatively combined four different interaction patterns: a rotary controller, a push button, a display, and force/torque-based haptic feedback. While there are many possibilities for what can be done, creating an intuitive user experience is a challenging yet rewarding task.

As Grigorii was implementing Scott's original design, we received hundreds of requests from around the world. People are eager to experiment with the knob, potentially developing software and hardware extensions for it. We saw this as a fantastic opportunity to engage a wider community in addressing the question of what makes an ideal residential controller. Therefore, we decided to manufacture a development kit specifically for this purpose.

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

In this codebase you have all the material needed to create your own smart knob. You have the kicad files for the pcb, the fusion 360 files for the enclosure, bill of materials for the components, and firmware to flash the device. 
We also offer a fully assembled (soldered PCB with plastic enclosure) and flashed with firmware version of the smart knob in our [store](https://store.seedlabs.it/products/smartknob-devkit-v1-0).
We aim to continue offering the device at a no profit cost. In other words, even with shipping cost, it's unluckly that you can end up spending less than just buying from our store.

That said, our goal is not manufacturing this kit at scale, nor to maintain constant backstock of it. As such it might take longer to get it. 
Additionally, if you have experience with PCB soldering, pick and place machinery, or you want to explore how to use 3rd parties to get this built for you, this is a great project get your hands dirty. Just make sure you fully understand what you are enbarking on, as, both from a mechanical and electronic standpoint, this is not an entry level project. 

Either ways, if you have any question drop by our Discord server. 


### License <a name="license"></a>

A more detailed license can be found in the license file. 

In this paragraph we wanted to give a layman version of the spirit behind it. 
Our goal is to enable as many contribution as possible to seedlabs ecosystem of open hardware. 
We also want/need to preserve the spirit behind the original project (Apache V2, Creative Common Attribution), and all the libraries that have been used in this codebase. 

As such we require that:

1. All the changes you make to the software and hardware are shared with the community in an open source manner, maintaining the same spirit of the license used here.
2. You can use the hardware and software of this project for educational and personal project. This mean you can build as many smart knobs as you want as long as you are using in an educational setting (ie.: if you are a university and want to offer the device as part of a course) or for personal use (ie.: you want to install 10 dev kits in your home)
3. You can use the hardware and software of this project for commercial use, but you first needs to get a partnership agreement with seedlabs. This allows to assure that correct attribution is in place, all the changes implemented remains open source, there is a reasonable redirection (royalties) of the value generated by the work done by the community towards the ultimate goal this project is intended for. This last point varies depending on the downstream utilization. The spirit behind this requirements is that we stay true to Seedlabs mission. You can imagine that we would charge a higher royalties fee should this projet be used in a petrol car, compared to, let's say, being used in a medical device. 

Ultimately, this work is derivative from the original Smart Knob work, and, should you feel the license is too stringent for your use case, we recommend going upstream, and build directly the original verison. 


  