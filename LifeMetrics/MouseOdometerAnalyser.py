#!/usr/bin/env python
import sys
start = int(sys.argv[1])
end = int(sys.argv[2])

total_time = 0
total_pixels = 0
for (timestamp, pixels) in map(lambda s: map(int, s.split(" ")), open("MouseOdometer.log").read().strip().split("\n")):
	if timestamp >= start and timestamp <= end:
		if pixels > 0:
			total_time += 300
		total_pixels += pixels
print(total_time, total_pixels)
