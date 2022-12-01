day=$1
year=2022

declare -A lang_dict
lang_dict["jl"]="jl_src/template.jl"
lang_dict["rs"]="rust_src/src/template.rs"
lang_dict["py"]="python_src/template.py"
lang_dict["hs"]="hs_src/template.hs"
lang_dict["cpp"]="cpp_src/src/template.cpp"

echo "${lang_dict[@]}"

langs=${@:2}
if [ "${langs}" == "" ]; then
    langs=("jl" "rs" "py" "hs" "cpp")
fi;

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

for l in ${langs[@]}; do
    prepare "${lang_dict[$l]}"
done
