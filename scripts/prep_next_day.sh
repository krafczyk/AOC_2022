day=$1
year=2022

function customize {
    sed -i "s/##DAY##/$day/" $1
    sed -i "s/##YEAR##/$year/" $1
}

cp jl_src/template.jl jl_src/day_$day.jl
cp rust_src/src/template.rs rust_src/src/day_$day.rs
customize rust_src/src/day_$day.rs
cp python_src/template.py python_src/day_$day.py
customize python_src/day_$day.py
cp hs_src/template.hs hs_src/day_$day.hs
customize hs_src/day_$day.hs
cp cpp_src/template.cpp cpp_src/day_$day.cpp
customize cpp_src/day_$day.cpp
