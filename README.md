# The COVID-19 App Scanner Project

Software and hardware hacks for building smart devices scanning for Bluetooth
(BLE) signals of nearby smartphones running e.g. the Corona-Warn-App.

## Idea

Imagine a device that helps to identify people having the Corona-Warn-App
installed on their smartphones. This could be useful in various scenarios.

For example, a party location where only people get access having the app on
their phones. Or giving people a free drink when they have the app
active and running. Or implement a counter to detect more than one smartphone
at a time.

## How it works

An COVID-19 app scanner device scans for Bluetooth Low Energy (BLE) signals
from a "Contact Detection Service" running on a smartphone (iOS or Android).
The contact detection service is a BLE service used by the Corona-Warn-App.

By checking the signal strength (RSSI) the scanner device can decide how close
the smartphone is and update the detection result.

Bringing the smartphone close to the detection device narrows the signals down
to one device if several are in the room. Once the device is close enough the
scanner device may light up green to indicate that this device has the
Corona-Warn-App installed.

## Example

The first example of a COVID-19 App Scanner device is the "Green Tower" made
by [verlinked](https://www.verlinked.com).

![The 'Green Tower' by verlinked](site/greentower/images/greentower-makingof-18.jpg)

## How to build one

We do not have ready to use kits (yet) to make it easy for everyone to build
a smart scanning device. You need knowledge about hardware, embedded software,
3D design and 3D printing to create your own device.

But for all the hackers out there it should not be hard to create one from the
examples in this repository.

### Hardware

The scanner device requires a BLE hardware. We are using the widely used ESP32
chip for this purpose. An Internet search reveals many different variants of
these Arduino compatible chips. We are using the Arduino IDE for our code
examples.

Additionally some LED or an LED strip is required to indicate the scan
result. Some sort of display would also be possible. There are endless
possiblities.

### Software

This repository contains source code examples for building a scan device. The
code is open source an can be used for free. Just acknolwedge the copyright
notice when publishing the code somewhere else.

### Cases

The examples may also include files for 3D printable parts. Those can also
be used for your own projects.

## Background

It is everyones responsibility to keep distance from each other in times of the
COVID-19 pandemic. Smartphones help us to track contacts with other people and
to be informed about contacts we may have had with persons infected with
COVID-19.

In Germany the "Corona-Warn-App" was published to make it easy and secure for
such a tracking system. Read more about the app and get it from
[here](https://www.corona-warn-app.de).

The app uses Bluetooth Low Eneger (BLE) signals to indicate to other smartphones
that it is nearby. By exchanging encrypted IDs in an anonymous way, the phones
track when they have met other phones.

A device that checks whether a smartphone is equipped with the app has to check
for the BLE signals. If a smartphone is sending certain kinds of BLE packets
the scanning device can assume that the Corona-Warn-App is installed on that
device.

The details about the BLE communication is documented in the
[Contact Tracing - Bluetooth Specification](https://blog.google/documents/58/Contact_Tracing_-_Bluetooth_Specification_v1.1_RYGZbKW.pdf)
and the
[Contact Tracing - Cryptography Specification](https://blog.google/documents/56/Contact_Tracing_-_Cryptography_Specification.pdf).

## About

This project was initiated by the [verlinked GmbH](https://www.verlinked.com) -
a software company located in Paderborn, Germany.
