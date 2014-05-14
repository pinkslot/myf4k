# Get video information
# Parameter list:
# $1: video path
function get_video_info(){
    FFMPEG="ffmpeg"
    MEDIAINFO="mediainfo"
    input="$1"

    # Get duration and fps
    duration=$($FFMPEG -i $input 2>&1 | sed -n "s/.* Duration: \([^,]*\), start: .*/\1/p")
    if [[ $duration != '' ]]; then
        fps=$($MEDIAINFO -i $input 2>&1 | sed -n "s/\(Frame rate *:\) \([0-9]*\).*/\2/p")
        hours=$(echo $duration | cut -d":" -f1)
        minutes=$(echo $duration | cut -d":" -f2)
        seconds=$(echo $duration | cut -d":" -f3)
        # Get the integer part with cut
        # frames=$(echo "($hours*3600+$minutes*60+$seconds)*$fps" | bc | cut -d"." -f1)
        pattern="frame="
	frames=$($FFMPEG -i $input -vcodec copy -acodec copy -f null /dev/null 2>&1 |grep $pattern | sed "s/$pattern//" | sed "s/^ //" | cut -f 1 -d ' ')
	echo "$fps $frames"
        return 0
    else
        return 1
    fi
}
