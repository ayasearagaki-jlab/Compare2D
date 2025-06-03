
#!/bin/bash

name=$1
date=$2

target_dir="/home/fermi/rshibata/data/${name}/${date}"

file_number=$(ls -1 ${target_dir}/data_*.txt 2>/dev/null | wc -l)
file=$(ls -1 ${target_dir}/data_*.txt | head -n 1)
filename=$(basename "$file")

upperpath=$(echo "$filename" | sed -E 's/^data_([0-9]{12})_.*/\1/')
lowerpath=$(echo "$filename" | sed -E 's/^data_[0-9]{12}_[0-9]+_([0-9]+s)_test\.txt$/_\1_test.txt/')

json_path="/home/fermi/rshibata/config/dataconfig/datapathconfig_${date}.json"
[ -e "$json_path" ] && rm "$json_path"

cat <<EOF > "$json_path"
{
  "type": "${name}",
  "date": "${date}",
  "file_number": ${file_number},
  "mode":"Not",
  "paths": {
    "upper": "${target_dir}/data_${upperpath}_",
    "lower": "${lowerpath}"
  }
}
EOF
