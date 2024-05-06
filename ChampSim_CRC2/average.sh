#!/bin/bash

# Section 1
g++ -Wall --std=c++11 -o lru-config1 example/lru.cc lib/config1.a -static-libstdc++

calculate_miss_rate() {
    local grp_line="$1"
    total_access=$(echo "$grp_line" | awk '{print $4}')
    miss=$(echo "$grp_line" | awk '{print $8}')
    echo "scale=4; $miss / $total_access" | bc
}

iterations=4
line=""
total_miss_rate=0
#Section 2
for (( i=1; i<=$iterations; i++ )); do
    if [ "$i" -eq 1 ]; then
        echo "running bzip2"
        line=$(./lru-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/bzip2_10M.trace.gz | grep "LLC TOTAL")
    elif [ "$i" -eq 2 ]; then
        echo "running graph_analytics"
        line=$(./lru-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/graph_analytics_10M.trace.gz | grep "LLC TOTAL")
    elif [ "$i" -eq 3 ]; then
        echo "running libquantum"
        line=$(./lru-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/libquantum_10M.trace.gz | grep "LLC TOTAL")
    else
        echo "running mcf10M"
        line=$(./lru-config1 -warmup_instructions 1000000 -simulation_instructions 10000000 -traces trace/mcf_10M.trace.gz | grep "LLC TOTAL")
    fi
    cur_miss_rate=$(calculate_miss_rate "$line")
    echo "$cur_miss_rate"
    total_miss_rate=$(echo "scale=4; $cur_miss_rate + $total_miss_rate" | bc)
done

# calculate average miss rate
average_miss_rate=$(echo "scale=4; $total_miss_rate / 4" | bc)

echo "Average miss rate: $average_miss_rate"
echo "Baseline miss rate: .5118"