#!/bin/sh

#############################################################
# monitor server running									
# 1. restart server if detect it is not running				
# 2. check ERROR and WARN from logtime.log file
# 3. check admin server HTTP interface status
# 4. send email and SMS to related owner
# monitor.sh must be in the same directory with the server
#############################################################
function disp
{
    echo "$(date):   $1"
    return 0
}


# send email
function send_email() {

    #EMAIL_LIST='yf-wangyanpeng@sunlands.com'
    EMAIL_LIST='yuanjianjian@sunlands.com, yf-wangyanpeng@sunlands.com, yuanjintao@sunlands.com, liuyuejie@sunlands.com, zhangqiongying@sunlands.com, wuyujie@sunlands.com'

    [[ -z $1 ]] && echo "$1 is null" && return 1

    systemctl status sendmail > /dev/null 2>&1
    if (($? == 0)); then
        disp "$1" # write error to monitor log
        echo -e $1 | mail -s "IM Monitor: <$MY_IP>, $SERVER has error" $EMAIL_LIST
        disp "===email is sent.==="
    else
        disp "sendmail service is not enabled, skip mail sent."
        return 1
    fi

    return 0
}

function send_sms() {
    # yp: 13683013560, jt: 18612109634, jianjian: 18614064515, yuejie: 15210855382, qiongying, yujie
    #MOBILE_LIST='13683013560'
    MOBILE_LIST='13683013560 18612109634 18614064515 15210855382 13261127824 15210717453'
    SMS_URL='http://sms.shangdejigou.cn/gateway-war/gateway/sendSms/sendSms.action'
    # send short message
    [[ -z $1 ]] && echo "\$1 is null" && return 1

    msg1='{"head":{"productSystem":"基础服务","messageType":"EXTERNAL","groupNum":1,'
    msg1+='"timing":"2014916142000","operator":"yf-wangyanpeng","createTime":'
    msg1+='"2014-09-1614:19:30","signData":"6b60dcece9d455fa2af9733aef0661c1"},"body":[{"mobile":"'

    msg3='","message":'\"$1\"

    msg4=',"sentType":"MESSAGE","sendMarking":"【尚德机构】"'
    msg4+=',"category1":"北京校","category2":"自考","category3":"行政管理本科"}]}'

    typeset ret=0
    for phone in $MOBILE_LIST; do
        msg=$msg1"$phone"$msg3$msg4
        curl  -d "requestJson=$msg" $SMS_URL > /dev/null 2>&1
        (( ret+=$? ))
    done

    if (( ret == 0 )); then
        disp "---short message is sent.---"
    else
        disp "send short message error, ret=$ret"
    fi

    return $ret
}

function handle_error() {
    [[ -z $1 ]] && echo "$1 is null" && return 1

    typeset sms_keyword='[ERROR'
    [[ $SEND_MAIL == 'YES' ]] && send_email "$1"
    [[ $SEND_SMS == 'YES' && $1 == *$sms_keyword*  ]] && send_sms "$1"

    return 0
}

function monitor() {

    typeset last_occur_hms=""
    typeset err_info=""
    typeset err_to_count=0
    typeset err_to_count2=0

    while true
    do 
        ##############################################################
        # step 1: check if process die
        ##############################################################
    	pid=$(cat server.pid)  # get pid
    	process_count=$(pgrep -f $1|grep $pid | wc -l)
    	if [ $process_count == 0 ]
    	then
    		# add log
    		err_info="$(date): [ERROR] ip=$MY_IP, pwd=$MY_DIR, server $1 stopped, pid=$pid, process_cnt=$process_count, will restart..."

    		# restart server
    		../daeml ./$1

            # send email
            handle_error "$err_info" && sleep 1  # sleep 1s to avoid sent too quick if error log need send in same loop 
    	fi

        ##############################################################
        # step 2: check error log
        ##############################################################
        #
        # check error and warn log in log/logtime.log.
        # since the log name will change at 00:00:00, 
        # we need handle the corner case around that time.
        #
        last_chk_time=$(date "+%Y-%m-%d %H:%M:%S" -d "$INTERVAL seconds ago")  # e.g. 2017-05-11 15:03:56
        disp "last_chk_time = $last_chk_time"
        last_chk_day=${last_chk_time:0:10} # e.g. 2017-05-11
        last_chk_hms=${last_chk_time:11:8} # e.g. 15:03:56

        #
        # log/logtime.log format is like:
        # ...
        # 2017-05-08 20:03:43,947 [ERROR PUSH] - ...
        # ...
        #
        # To avoid log/logtime.log was too big, we tail the last 1000000 line , there should be no missing,
        # because there shouldn't be so much log printed within 15s even when debug is trun on.
        # 1000000 may be still big, but looks the speed is acceptable i.e within couple of seconds.
        #

        # empty the log file, at the begin of each loop
        cat /dev/null > $TMP_LOG

        # corner case, once a day, we have the new day's logtime now
        # need adjust 23:59:40 if INTERVAL is not 15 s
        # This may print duplicate msg if error within 23:59:41 - 23:59:45 (last_chk_hms)
        if [[ $last_chk_hms > '23:59:40' ]]; then

             disp "Entering corner case ..."

             # a. check log of last_chk_day
             if [[ -e log/logtime.log.$last_chk_day ]]; then
                 disp "check error from log/logtime.log.$last_chk_day"
                 tail -$TL log/logtime.log.$last_chk_day | \
                     awk -v val1=$last_chk_day -v val2=$last_chk_hms \
                     '{if (/\[WARN|\[ERROR/ && $1 == val1 && $2 > val2) print }' >> $TMP_LOG
             else
                 disp "error: log/logtime.log.$last_chk_day not found."
             fi

             # b. append log of new log/logtime.log
             tail -$TL log/logtime.log | awk -v val1=$last_chk_day \
                 -v val2=$last_chk_hms '{if (/\[WARN|\[ERROR/ && $1 > val1) print }' >> $TMP_LOG

             if [[ -s $TMP_LOG ]]; then
                 err_info="ip=$MY_IP, pwd=$MY_DIR.\n"
                 err_info+=$(cat $TMP_LOG)
                 handle_error "$err_info"
             fi

             last_occur_hms=""
             disp "last_occur_hms=$last_occur_hms"
        # normal case
        else
            #tail -$TL log/logtime.log |awk -v val1=$last_chk_day \
            tail -$TL log/logtime.log | grep -v "No valid messages" | awk -v val1=$last_chk_day \
                -v val2=$last_chk_hms -v val3=$last_occur_hms \
                '{if (/\[WARN|\[ERROR/ && !/DBPool.*Incorrect string value/ && $1 == val1 && $2 > val2 && $2 > val3) print }' >> $TMP_LOG
            if [[ -s $TMP_LOG ]]; then
                err_info="ip=$MY_IP, pwd=$MY_DIR.\n"
                err_info+=$(cat $TMP_LOG)
                handle_error "$err_info"
                last_occur_hms=$(tail -1 /tmp/im_$$_log.out |awk '{print $2}')
                disp "last_occur_hms=$last_occur_hms"
            fi
        fi

        ##############################################################
        # step 3: check admin server HTTP interface on push_server
        ##############################################################
        if [[ $SERVER == "sunlands_push_server" ]]; then
	          typeset send=0
	          
	          # 1. check HTTP server
            curl --connect-timeout 10  http://42.62.110.136/get_my_groups?user_iddf=111 > /dev/null 2>&1
	    ret36=$?
            curl --connect-timeout 10  http://42.62.110.137/get_my_groups?user_iddf=111 > /dev/null 2>&1
            ret=$?
            (( ret = ret + ret36 ))
            if (( $ret != 0 )); then
		            # 28 means time out
		            if (( $ret == 28 )); then
                   disp "get a curl time out, err_to_count = $err_to_count"
		               ((err_to_count = err_to_count + 1))
		               if (( $err_to_count == 3 )); then
                  			send=1
                  			err_to_count=0
                   fi
                else
                   send=1
		            fi

                err_info="ip=$MY_IP, pwd=$MY_DIR.\n"
                err_info+="$(date): [ERROR] check admin server interface: http://info.social.im.sunlands.com/get_my_groups?user_iddf=111 failed. ret=$ret"
                (( $send == 1 )) && handle_error "$err_info"
            else
		            err_to_count=0
                disp "check admin server HTTP, OK"
            fi

	          # 2. check xiaomi push server
	          typeset send2=0
	          curl --connect-timeout 10 https://api.xmpush.xiaomi.com/v2/multi_messages/regids  > /dev/null 2>&1
            ret=$?
            if (( $ret != 0 )); then
                 # 28 means time out
                 if (( $ret == 28 || $ret == 7 )); then
                   disp "get a curl time out, err_to_count2 = $err_to_count2"
		               ((err_to_count2 = err_to_count2 + 1))
		               if (( $err_to_count2 == 3 )); then
                  			send2=1
                  			err_to_count2=0
                   fi
                else
                   send2=1
            fi                 
                 err_info="ip=$MY_IP, pwd=$MY_DIR.\n"
                 err_info+="$(date): [ERROR] check xiaomi push server interface: https://api.xmpush.xiaomi.com/v2/multi_messages/regids failed. ret=$ret"
                 (( $send2 == 1 )) && handle_error "$err_info"
            else
                 err_to_count2=0
                 disp "check xiaomi push server interface, OK"
            fi

        fi
        
    	sleep $INTERVAL
    	
    done
}


#############################################################
# main
#############################################################
MY_IP=$(grep "ListenIP=" *.conf |grep -v "^#" |awk -F= '{print $2}' |awk '{print $1}'|tail -1)
MY_DIR=$(pwd)
TMP_LOG=/tmp/im_$$_log.out
INTERVAL=15
TL=1000000
SERVER=$1
SEND_MAIL='YES'
SEND_SMS='YES'


monitor $SERVER




