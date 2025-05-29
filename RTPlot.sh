#!/bin/bash

# Force the use of X11
export GDK_BACKEND=x11
export XDG_SESSION_TYPE=x11

cd ~/dev/RTPlot
exec ./RTPlot
