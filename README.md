# Mirai

[![Build Status](https://github.com/WaleedYaser/mirai/workflows/build/badge.svg)](https://github.com/WaleedYaser/mirai/actions)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/c19ca50a0d7e4f32a11b90e779a22210)](https://www.codacy.com/gh/WaleedYaser/mirai/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=WaleedYaser/mirai&amp;utm_campaign=Badge_Grade)
[![Project Status: Concept – Minimal or no implementation has been done yet, or the repository is only intended to be a limited example, demo, or proof-of-concept.](https://www.repostatus.org/badges/latest/concept.svg)](https://www.repostatus.org/#concept)
![Repo Size](https://img.shields.io/github/repo-size/WaleedYaser/mirai)
![Lines of Code](https://img.shields.io/tokei/lines/github/WaleedYaser/mirai)

Mirai is a side project I'm working on now for the purpose of learning more about game engines, rendering, and game development. I'm note sure yet what the end goal will be but hopefully it will be a game engine that has a good rendering quality and modern graphics APIs backend (Vulkan, DirectX12, ...), a simple physics engine, sound engine, and more.

I'm trying to write a decent amount of comments to explain most of the code so I make sure that I understand it well, and to help anyone reading the codebase to understand it. Please feel free to contact me if you have any questions or suggestions.

> Mirai (未来 or 未來) is the Japanese word for the future.
>
> -- <cite>Wikipedia</cite>

## Supported platforms
- Windows
- Linux

## Compilers
- Clang

# Build
## Windows
- Download and install Clang (if you don't have it): https://releases.llvm.org/download.html.
- Run `build-all.bat` which will build everything.
- The output will be in the `bin` folder.

## Linux
- Install Clang and the other dependencies:
```
sudo apt install clang libxcb1-dev libxcb-util-dev libxcb-keysyms1-dev
```
- Run `build-all.sh` which will build everything.
- The output will be in the `bin` folder.

# Resources
- [Kohi Game Engine series on YouTube.](https://www.youtube.com/playlist?list=PLv8Ddw9K0JPg1BEO-RS-0MYs423cvLVtj)