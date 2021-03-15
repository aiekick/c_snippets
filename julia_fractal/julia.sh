#!/bin/bash

clamp()
{
    # arg 1 must be the value to clamp
    # arg 2 ùmust be the inf value
    # arg 3 must be the sup value
    res=$1
    #echo "ini)res=$res"
    if [ "$(echo "$res < $2" | bc -l)" == 1 ] 
    then
        res=$2
        #echo "min)res=$res"
    fi
    if [ "$(echo "$res > $3" | bc -l)" == 1 ] 
    then
        res=$3
        #echo "max)res=$res"
    fi
    echo $res
}

sx=100
sy=80
px=0.3
py=1.0
iterations=5
charset="azertyuiop-"
zoom=1.4

d=0.0
tx=0.0
df=0.0

mns=$sy

zx=0.0
zy=0.0

str=" "
strlen=${#charset}-1
idx=0

y=0
while [ $y -lt $sy ]
do
    let y++

    x=0
    while [ $x -lt $sx ]
    do
        let x++
        
        zx=$(echo "(($x * 2.0 - $sx) / $mns * $zoom)" | bc -l)
        zy=$(echo "(($y * 2.0 - $sy) / $mns * $zoom)" | bc -l)

        i=0
        while [ $i -lt $iterations ]
        do
            let i++
            d=$(echo "$zx * $zx + $zy * $zy" | bc -l)
            if [ "$(echo "$d < 4.0" | bc -l)" == 1 ] 
            then
                tx=$zx
                zx=$(echo "($zx * $zx - $zy * $zy - $px)" | bc -l)
                zy=$(echo "(2.0 * $tx * $zy - $py)" | bc -l)
            fi
        done

        df=$(echo "0.14 / ($d + 0.00001) + sqrt($d) * l($d)" | bc -l)
        df=$(clamp $df 0.0 1.0)
        idx=$(echo "$df * $strlen" | bc -l)
        iidx=$(echo "$idx / 1" | bc)
        str=$str${charset:$iidx:1}
    done
    echo $str
    str=""
done
