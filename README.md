# webserv

A simple non-blocking web server written in C++98 using Linux's `epoll` facility .

### Pre-requisites

Linux > 2.5.44

`g++` (or `clang++`) supporting C++98

### Installation

```
git clone https://github.com/3lswear/webserv
```

```
cd webserv && make
```

### Usage

```
webserv your_config_file.toml
```

![Usage](https://raw.githubusercontent.com/3lswear/webserv/fb4e2853a84ef57d68beafb5a4e688188c8d030b/assets/usage.svg)



### Features

- HTTP 1.1 compliant
  - Supported methods: `GET`, `POST`, `DELETE`, `PUT`
- Non-blocking
- Efficient resource usage (doesn't use CPU on idle)
- Fair client handling (1 slow client won't slow down 50 fast clients)
- Can listen on multiple ports/interfaces
- TOML config format
  - multiple servers, with unique` server_name`s
  - multiple location (route) blocks
    - location matching by path and wildcard
    - ability to limit allowed methods
  - redirections
  - toggleable directory listing support 
  - set up custom error pages
  - set up a limit for a request body size

### Collaborators

A team project made by:

- [sunderle](https://github.com/3lswear)

- [talyx](https://github.com/talyx)

### Why ?

#### Why epoll?

Epoll is one of the modern event listeners/IO multiplexers out there (the other one is kqueue). Epoll works on the assumption that set manipulation is not as frequent as event handling, and the performance trade-offs are made accordingly. [Although, it's not without its problems.](https://idea.popcount.org/2017-02-20-epoll-is-fundamentally-broken-12/)

#### Why `TOML` for a config file?

TOML was our format of choice because it provides strict structure for a configuration, around which we could model our internal data structures. With a set file type like TOML the syntax is clear and unambiguous, and the tools like syntax highlighting and validation are already available. Also it isn't as hard to parse as YAML, although admittedly being less pretty. Lastly, we didn't just want to copy nginx's config structure.

#### Why make this project in the first place?

It was a great opportunity to learn writing bigger projects in C++, learn a little about network programming, how does OS handle network interaction, and to try tackling typical problems that come with socket IO and a need to serve a sizable amount of clients. Also, it was interesting to learn how software of similar purpose works (nginx uses `epoll` too!).