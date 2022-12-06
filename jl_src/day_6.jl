import ArgParse

# Build ArgParsing Table
s = ArgParse.ArgParseSettings()
@ArgParse.add_arg_table s begin
    "--input"
        help = "Filepath to input data"
        arg_type = String
        required = true
    "--verbose"
        help = "Add additional reporting"
        action = :store_true
end

struct RuntimeError <: Exception
end

# Parse arguments
parsed_args = ArgParse.parse_args(ARGS, s)

function FirstFullyUniqueSequence(line::String, num::Integer)::Integer
    if length(line) < num
        throw(RuntimeError)
    end
    for i in num:length(line)
        char_set = Set(line[i-(num-1):i])
        if length(char_set) == num
            return i
        end
    end
end

# Process files
function ProcessFile(filepath::String)::Vector{String}
    result = Vector{String}()
    open(filepath) do f
        for line in readlines(f)
            push!(result, line)
        end
    end
    return result
end

# Get input filepath
input_filepath = parsed_args["input"]
verbose = parsed_args["verbose"]

lines = ProcessFile(input_filepath)

println("Task 1")
for i in 1:length(lines)
    println(FirstFullyUniqueSequence(lines[i], 4))
end

println("Task 2")
for i in 1:length(lines)
    println(FirstFullyUniqueSequence(lines[i], 14))
end
