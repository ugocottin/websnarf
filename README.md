# websnarf

## Description

This is a clone of the Websnarf tool in c, from http://www.unixwiz.net/tools/websnarf.html

## Todo

| Command | Works |
|--|--|
| `--help` | Yes |
| `--log FILE` | Yes |
| `--port ##` | Yes |
| `--timeout ##` | Yes |
| `--apache` | No |
| `--max ##` | Yes |
| `--save DIR` | Yes |

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

Save each request in a finle in directory DIR. The filename is named for the source IP or hostname if available