#!/usr/bin/env python2
# -*- coding: utf8 -*-
import dateutil.parser, os, os.path, re, sys
from datetime import datetime, timedelta

messages = {}
base_dir = os.path.expanduser("~/.purple/logs")
for protocol in os.listdir(base_dir):
    for account in os.listdir(os.path.join(base_dir, protocol)):
        for buddy in os.listdir(os.path.join(base_dir, protocol, account)):
            if buddy not in messages:
                messages[buddy] = []

            for conversation in os.listdir(os.path.join(base_dir, protocol, account, buddy)):
                time = datetime.strptime(conversation.rsplit(".", 1)[0].split("+")[0], "%Y-%m-%d.%H%M%S")
                with open(os.path.join(base_dir, protocol, account, buddy, conversation)) as f:
                    for line in f:
                        try:
                            line = line.decode("utf-8")
                        except:
                            continue

                        line = re.sub('<[^<]+?>', '', line)
                        
                        if re.search(r"^Conversation with .+ at (.+) on ", line):
                            continue

                        timestamp = re.search(r"^\((.{8,}?)\)", line)
                        if timestamp:
                            try:
                                new_time = dateutil.parser.parse(timestamp.group(1), default=time)
                                if new_time < time:
                                    new_time += timedelta(days=1)
                                time = new_time

                                try:
                                    line = line.split(": ", 1)[1]
                                except IndexError:
                                    continue
                                messages[buddy].append((time, ""))
                            except ValueError:
                                timestamp = None
                        try:
                            messages[buddy][-1] = (messages[buddy][-1][0], messages[buddy][-1][1] + line)
                        except IndexError:
                            pass

from PidginHistoryStackedAreaChartConfig import my_uin
del messages[my_uin]

from PidginHistoryStackedAreaChartConfig import merge
for mergegroup in merge:
    for buddy in mergegroup[1:]:
        messages[mergegroup[0]] += messages[buddy]
        del messages[buddy]

for buddy in messages.keys():
    if not messages[buddy]:
        del messages[buddy]

import time

dates = []
lens = 0
for buddy in messages.keys():
    for message in messages[buddy]:
        i = (time.mktime(message[0].timetuple()))

        if not (i >= 1354294800 and i <= 1356973200):
            continue

        dates.append(i)
        lens += len(message[1])

dates = sorted(dates)

prev = 0
sum = 0
for i in dates:
    if i - prev < 60:
        sum += i - prev
    prev = i
print sum, lens, len(dates)