#!/bin/sh

file */*.png | grep 1-bit | awk '{print $1}' | sed s/:// | xargs rm

