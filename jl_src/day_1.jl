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

# Parse arguments
parsed_args = ArgParse.parse_args(ARGS, s)

struct Day1{T} end
function Day1{T}(elf_food::Vector{Vector{T}}) where T <: Integer
    elf_cals = sort([sum(v) for v in elf_food], rev=true)
    println("Task 1: ", elf_cals[1])
    println("Task 2: ", sum(elf_cals[1:3]))
end

# Process files
struct ProcessFile{T} end
function ProcessFile{T}(filepath::String) where T <: Integer
    elf_food = Vector{Vector{T}}()
    cur_el = Vector{T}()
    open(filepath) do f
        for line in readlines(f)
            if isempty(line)
                push!(elf_food, cur_el)
                cur_el = Vector{T}()
            else
                push!(cur_el, parse(T, line))
            end
        end
    end
    push!(elf_food, cur_el)
    Day1{T}(elf_food)
end

# Get input filepath
input_filepath = parsed_args["input"]
verbose = parsed_args["verbose"]

ProcessFile{UInt32}(input_filepath)
