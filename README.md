# netJeep

A simple extensible asynchronous network framework.

A flexible, lightweight and asynchronous network framework based on standalone Asio.


# Features

### 1. Direct Internet Access & Multi-hop Chaining

netJeep nodes can connect directly to the Internet, or be chained together to form a multi-hop network topology.

This allows users to build flexible proxy networks, relay systems, or custom communication infrastructures.

Example:

```text
Client -> Node A -> Node B -> Node C -> Internet
```

![overall](docs/overall.svg)

---

### 2. Pluggable Strategy Module

The core communication logic in netJeep is abstracted as a `Strategy` interface.

By implementing the `Strategy` interface, users can customize:

- Encryption methods
- Handshake protocols
- Relay logic
- Packet transformation

This design allows communication methods to be replaced without modifying the core framework.

---

### 3. Built-in Strategies

Currently implemented strategies include:

#### AES Strategy

An encrypted communication protocol similar to early Shadowsocks implementations.

Features:

- AES encrypted data transport
- Lightweight protocol
- Suitable for relay and proxy communication

---

#### Socks5 Strategy

Allows inbound connections to communicate through the SOCKS5 protocol.

Suitable for:

- Browser proxy
- Local proxy applications
- Standard SOCKS5 clients

---

### 4. Fully Asynchronous I/O

netJeep is built on standalone Asio and uses fully asynchronous, non-blocking I/O.

---

# Architecture Overview

![framework](docs/framework.svg)

## Component Description

### InConn

Represents the inbound/local-side connection.

Examples:

- SOCKS5 client
- Browser connection
- Acceptor of other node

---

### OutConn

Represents the outbound/remote-side connection.

Examples:

- Remote server
- Next-hop node
- Internet target

---

### Strategy

Handles the communication logic between `InConn` and `OutConn`.

Examples:

- Encryption
- Handshake
- Protocol transformation
- Packet encapsulation

---

# I/O Processing Flow

netJeep uses an event-driven asynchronous state machine to process network traffic.

For `InConn`, the processing flow is:

```text
toInRead
    ↓
inRead
    ↓
onInRead
    ↓
toOutWrite
    ↓
outWrite
    ↓
onOutWrite
    ↓
toInRead (loop)
```

---

## Stage Description

### toInRead

Prepare for asynchronous reading from `InConn`.

Typical operations:

- Buffer allocation
- State checking
- Strategy preprocessing

---

### inRead

Perform asynchronous read operation.

Usually implemented using:

```cpp
asio::async_read_some(...)
```

---

### onInRead

Handle received data after reading.

Typical operations:

- Decryption
- Packet parsing
- Protocol processing
- Strategy dispatching

---

### toOutWrite

Prepare data before sending to `OutConn`.

Typical operations:

- Encryption
- Packet encapsulation
- Compression
- Buffer generation

---

### outWrite

Perform asynchronous write operation.

Usually implemented using:

```cpp
asio::async_write(...)
```

---

### onOutWrite

Handle post-write processing.

Typical operations:

- Continue next read cycle
- Release buffers
- State transition

---

# Bidirectional Data Flow

Depending on the current `Strategy`, netJeep may asynchronously trigger reading from `OutConn` at specific stages.

The `OutConn` processing flow is similar to `InConn`:

```text
toOutRead
    ↓
outRead
    ↓
onOutRead
    ↓
toInWrite
    ↓
inWrite
    ↓
onInWrite
    ↓
toOutRead (loop)
```

This design enables fully asynchronous bidirectional forwarding.

---

# Multi-hop Chaining

Since both inbound and outbound are abstracted connections, netJeep nodes can easily be chained together.

Example topology:

```text
Client
   ↓
Node A
   ↓
Node B
   ↓
Node C
   ↓
Internet
```

Each node may use different strategies.

For example:

```text
Client
   ↓ SOCKS5
Node A
   ↓ AES Strategy
Node B
   ↓ Custom Strategy
Node C
   ↓ Internet
```

---

# Future Plans

Potential future features:

- [ ] UDP support
- [x] ~~structured InConnection and OutConnection manager~~
- [x] json config parser
- [ ] logger
- [ ] TLS strategy
- [ ] QUIC strategy
- [ ] WebSocket transport
- [ ] Traffic statistics
- [ ] Dynamic routing

---

# requirements

[Boost](https://www.boost.org/) >= 1.66.0

[Asio non-Boost](https://think-async.com/Asio/) == 1.22.1

[CryptoPP](https://www.cryptopp.com/) == 8.6

other requirements workings with cmake.
