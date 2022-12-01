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

# Process files
function ProcessFile(filepath::String)
    open(filepath) do f
        for line in readlines(f)
        end
    end
end

# Get input filepath
input_filepath = parsed_args["input"]
verbose = parsed_args["verbose"]

ProcessFile(input_filepath)
