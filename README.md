# multi-delogo

multi-delogo is an application that helps you remove logos or other watermarks from videos. Even if the position of the logo changes from time to time, multi-delogo allows you to mark all the positions and generate a video without the logos.

There's an automatic logo detection feature that can identify automatically the logos and their positions, especially text logos.

For more details and usage instructions, see [here](docs/en/README.md).

Para informações em português, [clique aqui](README.pt_BR.md).


## Installation on Windows

Download the latest release from the [releases page](https://github.com/wernerturing/multi-delogo/releases) and unzip the file. Then run the `multi-delogo.exe` file to start the application.


## Installation on Linux

To compile the software on Linux, you'll need development files for the following libraries:

* gtkmm
* goocanvasmm
* opencv
* boost

You'll also need a C++11 compiler and `make`.

Download the latest release from the [releases page](https://github.com/wernerturing/multi-delogo/releases), extract it, and run

```sh
./configure
make
make install
```

Then run `multi-delogo` to start the application.


## Installation on Mac OS

There's a [Homebrew formula](https://github.com/wernerturing/homebrew-multi-delogo) that installs multi-delogo automatically.


## Installation from github

This is very similar to installing on Linux as described above. You'll need some additional dependencies:

* autoconf
* automake
* autopoint
* gettext

Clone the project with

```sh
git clone https://github.com/wernerturing/multi-delogo.git
```

Enter the `multi-delogo` directory, run `autogen.sh` to generate the `configure` script and the Makefiles, and finally run
```sh
./configure
make
make install
```

Then run `multi-delogo` to start the application.


## Bugs, suggestions, etc.

You can use [issues](https://github.com/wernerturing/multi-delogo/issues) to report bugs and suggestions.

Pull requests are welcome; please try to follow the style of the code.

You can also send me an email at werner.turing@protonmail.com.


## Copyright

multi-delogo is Copyright (C) 2018-2019 Werner Turing <werner.turing@protonmail.com>

multi-delogo is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

multi-delogo is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the [GNU General Public License](COPYING) for more details.
