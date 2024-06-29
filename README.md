# Date Converter Library

The dateconv library is a free and open-source C library that converts various types of calendars to each other (Persian, Gregorian, Islamic, Hebrew and Julian Calendars). This functions was originally written in JavaScript and placed in the public domain by [John Walker](https://www.fourmilab.ch/documents/calendar/) and converted to C by [Aboutaleb Roshan](https://www.rosybit.com/products/dateconv/) and added some minor features.

This library supports 2 types of Persian calendars (Astronomical and Algorithmic). The algorithmic version is based on Ahmad Birashk proposed algorithm.

This library is in the public domain.

## Requirements

- A supported compiler like GCC and Clang
- make

## Building and Installing the Library

`make` is the officially maintained build system of this project. In general, all you should have to do is open a terminal, cd to the dateconv source directory, and then:

```
make
make test
make install
```

The command above builds both static and shared libraries by default. If you want to build only shared or static version, perform:

```
make shared
# OR
make static
```

Optionally, you can install the library with the following command. This will be to `/usr/local` by default, and you'll probably need to be "root" to be able to write there.

```
make install
```

To install the library into a user selected directory, perform:

```
make install PREFIX=$PWD/dist
```

You can remove the program binaries and object files from the source code directory with the following command:

```
make clean
```

## Test the Library

Once the library is built, it should be tested to ensure that it works.

```
make test
./test
```

## PGP Public Key

The source file is signed with the following key:

- [abroshan39_PGP_public_key.asc](https://www.rosybit.com/abroshan39/abroshan39_PGP_public_key.asc) (Key ID: B0E5D23797D2D8CB)

## Reporting Bugs

If you find a bug please report it to ab.roshan39@gmail.com.
