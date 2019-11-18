#!/usr/bin/env bash
time dd if=/dev/zero of=/tmp/test.img oflag=dsync bs=4k count=1000
time dd if=/dev/zero of=/tmp/test.img  bs=4k count=1000