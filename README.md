# webserv

A simple non-blocking single threaded server using Linux's `epoll` facility.

### Pre-requisites

Linux > 2.5.44

`g++` (or `clang++`) supporting C++98

### Installation

```
git clone https://github.com/3lswear/webserv
```

```
make
```

### Usage

```
./webserv your_config_file.toml
```

### Features

- HTTP 1.1 Compliant
  - Supported methods: `GET`, `POST`, `DELETE`, `PUT`
- Non-blocking
- Efficient resource usage (doesn't use CPU on idle)
- Fair client handling (1 slow client won't slow down 50 fast clients)
- listen on multiple ports/interfaces
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

A team project:

- [3lswear](https://github.com/3lswear)

- [talyx](https://github.com/talyx)

### Why ?

- Why epoll?
- Epoll is one of the modern event listeners/IO multiplexers out there (the other is kqueue). Epoll works on the assumption that set manipulation is not as frequent as event handling, and the performance trade-offs are made accordingly. Although it's not without it's problems (link)
- Why `TOML` for a config file?
- TOML was our format of choice because it provides strict structure of document, around which we could model our internal data structures. With a set file type like TOML the syntax is clear and unambiguous, and the tools like syntax highlighting and validation are already available. Also it isn't as hard to parse as YAML, although admittedly being less pretty. Lastly, we didn't want to just copy nginx's config structure.
- Why make this project in the first place?
- It was a great opportunity to learn writing bigger projects in C++, learn a little about network programming, how does OS handle socket IO, and to try tackling typical problems that come with socket IO and a need to serve a sizable amount of clients. Also, it was interesting to learn how software of similar purpose works (nginx uses `epoll` too!).