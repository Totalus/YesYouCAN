

## Description

YesYouCAN is a capture and analysis tool for *Controler Area Network* (CAN) systems, which are widely used in the automotive industry for communication between controllers.


<!-- MarkdownTOC -->

- [Features](#features)
- [Videos](#videos)
- [State of development](#state-of-development)
- [Contribute](#contribute)
- [CAN adaptors support](#can-adaptors-support)
- [Message Filtering](#message-filtering)

<!-- /MarkdownTOC -->


## Features


- Capture network traffic
- Import and view DBC files
- Decoding signals using your DBC file(s)
- Build custom dashboards for vewing signals into diffent types of widgets
- Message filtering using *C-style condition* (the best you can get !) (see [Message filtering](#message-filtering))
- Send CAN messages on interfaces
- Loopback interface for testing, simulating and playing around
- Replay trace on any interface that supports sending messages
- Import network captures from file (different formats supported)
- Export network captures in various formats
- Export signal values in CSV file or Matlab script

The main missing feature is the signal graphs. There is only a start of implementation in the *graphics* branch. The current way to go for plotting signals is to export the signals values into a Matlab format and load the data in Matlab.


## Videos

Checkout the [**Tutorial video**]() to see the program and features in action.

## State of development

This tool is not currently under development. The source code has been sitting in my hard drive for a while unused. I'm not working on it anymore as I have other projects going on and nobody is acutally using the tool, but I thought it could be a great tool for others to use, customize and/or contribute to.

YesYouCAN has been built from scratch in **C++** with **Qt library** and only very few other dependencies. It has a strong back-end implementation (message capturing, filtering, decoding, ...). The front-end... could definitely be improved, but it is fully functional.

If you noticed, there are two branches. The *graphics* branch contains a start of implementation for **signal graphics** (the main feature missing in this tool). It is a good start but is not complete and not yet usable. It uses *qwt* as library for graphics which I found the most suitable after testing a few Qt graphics libraries.

## Contribute

You want to contribute, great!

I don't have a lot of time to put on it, but I can definitely guide on where you should start, which files to look at, add some useful documentation, etc. So don't hesitate to message me for help.

There is no written documentation, but there are some useful block diagrams in the `docs` directory that will help you understand the software architecture. The *.graphml* files can be opened with the great and free [*yEd graphics editor*](https://www.yworks.com/products/yed)).

## CAN adaptors support

YesYouCAN currently only supports [*PEAK PCAN-USB*](https://www.peak-system.com/PCAN-USB.199.0.html) adaptor. However, you can add new adaptors easily by implementing a new drivers.

All code related to the capture interfaces is in `src/interfaces`. You mainly need to subclass the `HwInterface` and then integrate it to the available interface list. You also should implement a configuration dialog to configure the interface.

## Message Filtering

Filtering is implemented using C-style condition expressions. It gives the most flexibility you can get to filter the messages you are looking for.

Examples of filter expressions

- Filter messages containing a specified signal name: `contains(signal_name)`
- Filter messages containing a signal with the specified value: `signal(signal_name) > 103.8`
- Show only messages of a specific ID: `msg.id == 0x103`
- Filter a specific byte value in the message data: `msg.data[2] == 0b101`
- Combine expression: `msg.id == 0x103 && timestamp > 10.8 || msg.len > 3`
- Use decimal, binary or hexadecimal notation for numbers: `0b101`, `5`, `5.0` and `0x05` are all valid !