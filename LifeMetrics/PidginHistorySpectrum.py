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

                                if "themylogin" not in line.split(": ", 1)[0]:
                                    continue

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

cols = []
date = dateutil.parser.parse("00:00:00", default=min([min([message[0] for message in messages[buddy]]) for buddy in messages.keys()]))
while date < datetime.now():
    col = []
    for buddy in messages.keys():
        col += [message[0].strftime("%H:%M:%S") for message in messages[buddy] if date <= message[0] and message[0] < date + timedelta(days=1)]
    cols.append([date.strftime("%Y-%m-%d")] + col)
    date += timedelta(days=1)

rows = max([len(col) for col in cols])
print ";" + ";".join([col[0] for col in cols])
for i in range(1, rows):
    print ";" + ";".join([col[i] if i < len(col) else "" for col in cols])
