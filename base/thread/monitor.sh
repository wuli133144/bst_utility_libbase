#!/bin/sh


function usage {
		echo "Usage: "
		echo "sh monitor.sh (start|status|stop)"
}

function pre_check {

    # check if core file size is 'ulimited'
    cs=$(ulimit -c)
    if [[ $cs != 'unlimited' ]]; then
        echo "core file size for current shell is $cs not unlimited, please set to it."
        return 1
    fi
    
    # check if we are at executable file's dir
    if ! ls *.conf > /dev/null 2>&1; then
        echo "no config file, are you at server executable file dir?"
        return 1
    fi
    
    if [[ ! -e server.pid ]]; then 
        echo "no config file or server.pid file"
        return 1
    fi

    sunlands_proc_num=$(ls sunlands_*_server*  |wc -l)
    if ((sunlands_proc_num != 1)); then
        echo "proc num is not 1, is $sunlands_proc_num"
        return 1
    fi

    return 0
}

# check if there are existing monitor instances for current dir/executable file
function get_mon_pid {
        my_pid=$$
        my_dir=$(pwd)
        pids=$(ps -ef |grep -w $BG_MONI_SH |grep -v grep | grep -v "vim" | grep -v $my_pid | grep -w $SERVER_NAME|awk '{print $2}')
        for pid in $pids; do
            p_cwd=$(ls -l /proc/$pid/cwd |awk '{print $NF}')
            if [[ $my_dir == $p_cwd ]]; then
                echo $pid
                return
            fi
        done
}
    
# check if there are server instances for current dir/executable file
function get_server_pid {
        my_dir=$(pwd)
        pids=$(ps -ef |grep -w $SERVER_NAME |grep -v grep | grep -v "vim" |grep -v $BG_MONI_SH |awk '{print $2}')
        for pid in $pids; do
            p_cwd=$(ls -l /proc/$pid/cwd |awk '{print $NF}')
            if [[ $my_dir == $p_cwd ]]; then
                echo $pid
                return
            fi
        done
}
    
function check_and_start {

    if pre_check; then 

        # check if SERVER is already started
        saved_pid=$(cat server.pid)
        server_pid=$(get_server_pid)
        if (( saved_pid != server_pid )); then
            echo "$SERVER_NAME is not running or pid is mismatch with server.pid"
            return 1
        fi

        # check if there are existing monitor instances for current dir/executable file
        mon_pid=$(get_mon_pid)
        if [[ -n $mon_pid ]]; then
            echo "Erorr: monitor.sh for current server is already running, pid:$mon_pid"
            return 1
        fi
    
        nohup ./$BG_MONI_SH $SERVER_NAME > nohup.monitor.log 2>&1 &
        echo "monitor is successfully started in background."
    else
        echo "pre_check failed."
        return 1
    fi

    return 0
}

function stop_monitor {

    if pre_check; then 
        # check if there are existing monitor instances for current dir/executable file
        mon_pid=$(get_mon_pid)
        if [[ -n $mon_pid ]]; then
            echo "Will stop monitor for current server, monitor pid:$mon_pid."
            kill -9 $mon_pid
            chk_pid=$(get_mon_pid)
            if [[ -n $chk_pid ]]; then
                echo "Failed to stop monitor process."
                return 1
            else
                echo "monitor process is successfully stoped."
                return 0
            fi
        else
            echo "Erorr: There is no monitor running for current SERVER."
            return 1
        fi
    else
        echo "pre_check failed."
        return 1
    fi

    return 0
}

function show_status {

    if pre_check; then 
        # check if there are existing monitor instances for current dir/executable file
        my_pid=$$
        mon_pid=$(get_mon_pid)
        if [[ -n $mon_pid ]]; then
            echo "monitor is running for current server, pid:$mon_pid"
            return 0
        else
            echo "There is no monitor running for current SERVER."
            return 1
        fi
    else
        echo "pre_check failed."
        return 1
    fi

    return 0
}
##############################################################################################
# main
##############################################################################################

(($# != 1)) && usage && exit 1

SERVER_NAME=$(ls sunlands_*_server*)
BG_MONI_SH="__bg_monitor.sh"

case $1 in
	start)
        check_and_start  
		;;
	status)
		show_status
		;;
	stop)
		stop_monitor
		;;
	*)
        usage && exit 1
		;;
esac

