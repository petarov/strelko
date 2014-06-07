.:Strelko Server:.
=============================

Strelko is a small footprint HTTP server. 

The goal of **Strelko** is to provide swift solution for testing your html/css project 
or just a fast way to share some files over HTTP. Could be useful to web developers,
web designers, testers and others.

**Note** that it is highly **unrecommended** to use this HTTP server on any productive system!

# Why do I need this?
You probably don't. ;) Python does pretty much the same, if you run:

    $ python -m SimpleHTTPServer 8000

However, **Strelko** has no dependencies and it's also smaller.

# Installation

  $ cmake -G "Unix Makefiles"

To build a debug binary use:

  $ cmake -G "Unix Makefiles"  -DCMAKE_BUILD_TYPE:STRING=Debug

# Usage

    $ strelko_srv -h <listen address> -p <listen port>

# Development
## Libraries Used
  * [APR](http://apr.apache.org/) - Apache Portable Runtime.
  * [HTTP Parser](https://github.com/joyent/http-parser) - Parser for HTTP messages written in C.

# License
This project is under [GPL license](COPYING).
