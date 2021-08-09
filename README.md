# Chess <!-- omit in toc -->

This is a C++ command line Chess game.  
You can play chess against a human or an AI, locally or remotely.

- [Installation](#installation)
  - [Install Boost](#install-boost)
  - [Install Chess](#install-chess)
- [Start a game](#start-a-game)
  - [Locally](#locally)
  - [Remotely](#remotely)
  - [Continue a game](#continue-a-game)

## Installation

### Install Boost
First install Boost version 1.66.0
  1. Download the archive here https://sourceforge.net/projects/boost/files/boost-binaries/ and extract it.
  2. Run `./bootstrap.sh`
  3. Run `sudo ./b2 --with=all install`.  
   To speed up the compilation, you can use all your CPU cores with:  
   `sudo ./b2 --with=all -j $(nproc) install`  
  It will take several minutes
  1. Run `sudo ldconfig` to update the cache of library on your computer

### Install Chess
To install Chess, type the following command:
  1. `./configure`
  2. `make -j $(nproc)`

## Start a game
### Locally
To start a game with both player on the same computer, first run `./chessengine`.  
Then start the players in two other terminals:
  * If you want to add a human player: `./human`
  * If you want to add a computer player: `./ai`

### Remotely
Start the server with `./chessengine` as before.  
When lauching the player add the `--ip <SERVER_IP>` argument.  
For instance, to connect a human player to the server running on ip 10.11.12.13, run: `./human --ip 10.11.12.13`.  
By default the port `4242` will be used. You can change it with the `--port` option.

### Continue a game
By default, the server will create a new game.
But you can specify a PGN file to start a game as if all the moves specified in the PGN file have been played.

`./chessengine --pgn my_game.pgn`

See https://en.wikipedia.org/wiki/Portable_Game_Notation for more information on PGN format.
