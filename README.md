# websnarf

## Description

This is a clone of the Websnarf tool in c, from http://www.unixwiz.net/tools/websnarf.html

## Todo

| Command | Works |
|--|--|
| `--help` | [x] |
| `--log FILE` | [ ] |
| `--port ##` | [x] |
| `--timeout ##` | [x] |
| `--apache` | [ ] |
| `--max=##` | [x] |
| `--save=DIR` | [ ] |

## Usage

`--help`

Show a brief summary of the command-line usage.

`--log FILE`

Append to the logfile named "FILE". It's created anew if required.

`--port ##`

Listen on TCP port ## instead of port 80. 

`--timeout ##`

Wait at most ## seconds for data once a connection is established.

`--apache`

Save logs in Apache format.

`--max=##`

Only show up to ## chars of the request.

`--save=DIR`

Save each full header in a file in directory DIR. The filename is named for the source and destination IP -- otherwise this would not be available -- and it's not so clear that this is really all that interesting. We need to find a better format for the filenames or decide if this functionality even belongs here.