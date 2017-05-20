Rust Whitelist
==============

This software allows a rust server (modded or not) to manage with [Linux GSM](https://gameservermanagers.com/lgsm/rustserver/) in order to enable a whitelist.
It scans the log file to get the players' steamID and kicks the unknown players thanks to [ruco](https://github.com/nizig/ruco).
The program automaticly changes of log file when the server is stopped or restarted.

Dependencies
============

You must install ruco as mentioned [here](https://github.com/nizig/ruco#installation).

Installation
============

```sh
git clone https://github.com/ahugeat/rust_whitelist.git
cd rust_whitelist
mkdir build
cd build
cmake ..
make
```

Run program
===========

Before you run the program, you have to configure ruco ([some examples here](https://github.com/nizig/ruco#utility-configuration-example)).
The whitelist file must contain all allowed steamID (one by line). You can change the whitelist file in live, the daemon will reload it.

Launch the program:

```sh
./rust_whitelist rust_lgsm_path/log/server path_to_whitelist/whitelist.txt
```

After that, the program runs as a daemon. The output will be redirected to rust_whitelist.cout.txt and rust_whiteliste.cerr.txt. If you have any troubles, check those file.

