#!/usr/bin/env python3

import os

def main():
    try:
        os.remove("Makefile.common")
    except FileNotFoundError:
        pass

    for root, _, files in os.walk("."):
        if "Makefile.json" in files:
            try:
                os.remove(os.path.join(root, "Makefile"))
            except FileNotFoundError:
                pass

if __name__ == "__main__":
    main()

