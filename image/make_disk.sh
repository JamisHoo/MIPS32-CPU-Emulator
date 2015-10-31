cp kernel  disk0

padding_size=$((4*1024*1024 - $(wc -c < kernel)))

head -c $padding_size /dev/zero >> disk0

cat sfs.img >> disk0
