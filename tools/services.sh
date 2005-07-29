FOUND=" "
for i in $(for i in $(cd rc5.d; echo S*); do find rc[56].d -name "[SK]??${i:3}"; done)
do 
	A=$(basename $i); 
	PROG=${A:3}
	case $FOUND in 
		*" ${PROG}"*)
			echo -n ${A:1:2}
			continue
		;;
	esac
	echo
	echo -n "$PROG:${A:1:2}:"
	FOUND="$FOUND $PROG"
done
echo
