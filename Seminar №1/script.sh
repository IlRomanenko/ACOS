function genQuery {
    echo -e "GET " $2 " HTTP/1.1\nHost: "\
     $1 "\nAccept: text/html\nConnection: close\n\n"  >query
}

echo "/" >internallinks

home="$1"
ndir=""

mkdir -p "$home"

while read line; do
    ndir=$home$line
    echo $ndir
    mkdir -p $ndir
    genQuery $1 $line
    nc $1 80 <query >webpage
    cp webpage $ndir$webpage
    
    awk 'match($0, /href=\"[^"]+\">/){print substr($0, RSTART, RLENGTH)}' webpage >links
    awk 'match($0, /\"\/[^"]*\"/){print substr($0, RSTART+1, RLENGTH-2)}' links > n_internallinks

    while read new_line; do
        if ! grep -q "$new_line" internallinks ; then
            echo $new_line >> internallinks
        fi
    done < n_internallinks
done < internallinks
