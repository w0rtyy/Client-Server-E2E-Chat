# Client-Server-E2E-Chat

A simple, terminal-based **chat application**\\ built using **sockets in C** with **AES-256-CBC encryption** powered by OpenSSL. The server runs on a Linux machine, and the client can run from another machine (e.g., macOS) over a network or the internet.

---

## ✨ Features

- 🔐 AES-256-CBC encrypted communication
- 💬 Bi-directional chat between client and server
- 🌐 Works across machines on local or public networks
- 🧪 Built with low-level socket APIs (no third-party frameworks)
- 🧵 Designed for future multi-client expansion

---

## 📁 Project Structure

```bash
.
├── client.c        # Client source code
├── server.c        # Server source code
├── README.md       # This file

```

🛠️ Prerequisites
On Both Server and Client Machines:
    GCC compiler
    OpenSSL development libraries

Install on Debian/Ubuntu-based systems:
```
  sudo apt update
  sudo apt install build-essential libssl-dev
```
For macOS (with Homebrew):
```
  brew install openssl
```
🔧 Compilation
Server (on Linux):
```
  gcc server.c -o server -lssl -lcrypto
```
Client (on macOS or Linux):
```
  gcc client.c -o client -lssl -lcrypto
```
