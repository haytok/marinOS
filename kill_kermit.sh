ps aux | grep kermit | grep -v grep | awk '{ print "kill -9", $2 }' | sh
