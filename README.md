# Maru Userbot 🤖 — Project Tracker & Roadmap

**Maru** is a high-performance, asynchronous Telegram userbot built from scratch in bleeding-edge **C++23** utilizing the official **TDLib** (Telegram Database Library). Compiled with **GCC 16.2.1**, it features a highly optimized modular architecture designed to bring lightning-fast automation and advanced activity analytics directly to your Telegram account.

---

## ⚠️ Proprietary Notice & License

**Look-But-Don't-Touch License**

This project is **proprietary software**. The source code is made publicly visible **for educational and auditing purposes only**. 

* **You ARE permitted to:** View, read, and analyze the source code to learn about advanced TDLib integration in modern C++.
* **You ARE STRICTLY PROHIBITED from:** Copying, modifying, distributing, selling, or deploying (running) this bot or any part of its code for personal or commercial use without prior written consent from the author (**qwexaro**).

For full legal terms, please read the accompanying `LICENSE` file.

---

## 🗺️ Development Status & Task Tracker

This section serves as the active roadmap and feature tracker for Maru. Items marked with `[x]` are completely stabilized in the current build.

### 📦 Core Architecture
- [x] **Native C++23 Core:** Initialized clean engine environment optimized for GCC 16.2.1.
- [x] **Cascaded Async Router:** Implemented internal request mapping via `request_id` to safely distribute multi-layered Telegram API callbacks.
- [x] **Smart Pointer Chains:** Guaranteed memory safety using `td::td_api::object_ptr` and `std::unique_ptr` moves (Zero-leak policy).
- [x] **Runtime Configuration:** Secured private credentials by importing `API_ID` and `API_HASH` from a local `config.txt` file at startup.

### 📥 Iris-Style Input Parser
- [x] **Basic Prefix Management:** Automated cutting of leading dots (`.`) and slashes (`/`).
- [x] **Argument Splitting:** Basic logic to separate the core command from trailing arguments via spaces.
- [ ] **Beta: Case-Insensitivity & UTF-8 Normalization** *(In Progress)*
  - [ ] Implement robust multi-byte character scanning to handle upper/lowercase Cyrillic (`А-Я`) safely.
  - [ ] Normalize raw command tokens before matching against the internal command map.

### 🛡️ Analytics & Weekly Statistics
- [x] **Sliding Window Engine:** Dynamically tracks user messaging history using Unix-timestamps.
- [ ] **State Persistence:** Automated serialization of weekly data into a local `stats_weekly.json` file.
- [ ] **Resilience Enhancements** *(In Progress)*
  - [ ] Refine automated 7-day sliding window cleanups when the bot is booting up after extended offline periods.

---

## 🛠️ Operational Commands

All commands are undergoing integration with the upcoming universal lowercase normalizer:

* `ping` — Instant execution check. Replies with an active text response (`ПОНГ 🏓`).
* `id` — Extracts and prints the current Chat ID. When used as a reply to a forwarded message, it recursively unpacks the original sender's User ID.
* `info` — Triggered as a reply to fetch and format a target user's profile card (Name, ID) via asynchronous nested `getUser` queries.
* `stats` — Returns weekly messaging volume stats over a rolling 7-day period for yourself or a targeted group participant.

---

## 🚀 Building the Project

### Prerequisites
* Compiler supporting **C++23** (Specifically verified on **GCC 16.2.1**)
* **CMake** (v3.15 or higher)
* **OpenSSL** development headers
* Pre-compiled **TDLib** binaries in adjacent directory structures (`../../td`)

### Compilation Steps
Initialize build environment:

```bash
mkdir build_cb && cd build_cb
cmake .. -G "CodeBlocks - Unix Makefiles"
make
```

Run the compiled executable directly from your build tree:
```bash
./mecha_drive
```

---

## 🔒 Security & Privacy Notice

Maru runs entirely client-side. Your authorization session data, local caches, and serialized JSON logs are stored strictly inside your local `tdlib_session/`, `tdlib_files/`, and `build_cb/` folders. **Never commit or share these directories.**
