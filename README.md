.:Strelko Server:.
=============================

Strelko is a small footprint HTTP server. 

The goal of **Strelko** is to provide a swift solution for testing your html/css project 
or just a fast way to share some files over HTTP. It could be useful to web developers,
web designers, testers and others.

Running Strelko in production is highly **unrecommended**!

# Why do I need this?
You probably don't. ;) Python does pretty much the same, if you run:

    python -m SimpleHTTPServer 8000

However, **Strelko** is smaller and has no dependencies.

# Usage

    $ strelkosrv -h <listen address> -p <listen port>

# Development
## Installation
To build Strelko run:

    cmake -G "Unix Makefiles"

To build a debug binary use:

    cmake -G "Unix Makefiles"  -DCMAKE_BUILD_TYPE:STRING=Debug
    
## Libraries Used

  * [APR](http://apr.apache.org/) - Apache Portable Runtime.
  * [HTTP Parser](https://github.com/joyent/http-parser) - Parser for HTTP messages written in C.

# License
This project is under [GPL license](COPYING).
