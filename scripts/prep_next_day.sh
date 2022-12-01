day=$1
year=2022

function prepare {
    template=$1
    filepath=$(dirname $template)
    filename=$(basename $template)
    extension="${filename##*.}"
    new_file=$filepath/day_$day.$extension
    cp $filepath/$filename $new_file
    sed -i "s/##DAY##/$day/" $new_file
    sed -i "s/##YEAR##/$year/" $new_file
}

prepare jl_src/template.jl
prepare rust_src/src/template.rs
prepare python_src/template.py
prepare hs_src/template.hs
prepare cpp_src/src/template.cpp
