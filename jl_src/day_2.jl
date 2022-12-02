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
function ProcessFile(filepath::String)::Vector{Tuple{String, String}}
    guide = Vector{Tuple{String, String}}()
    open(filepath) do f
        for line in readlines(f)
            s = broadcast(String,split(line))
            push!(guide, (s[1], s[2]))
        end
    end
    return guide
end

function convert_with_map_1(guide::Vector{Tuple{String, String}}, move_map::Dict{String,Integer})::Vector{Tuple{Integer, Integer}}
    return [(move_map[a[1]], move_map[a[2]]) for a in guide]
end

function convert_with_map_2(guide::Vector{Tuple{String, String}}, move_map_1::Dict{String,Integer}, move_map_2::Dict{Tuple{Integer, String},Tuple{Integer,Integer}})
    return [move_map_2[(move_map_1[a[1]], a[2])] for a in guide]
end

function convert_pair_to_outcome(pair::Tuple{Integer,Integer})::Integer
    if pair[1] == pair[2]
        return 0
    end
end

function score_game(moves)
    score_map = Dict{Integer,Integer}(
        1 => 1,
        2 => 2,
        3 => 3,
    )
    outcome_score_map = Dict{Integer,Integer}(
        -1 => 0,
        0 => 3,
        1 => 6,
    )
    # 1 -> Rock
    # 2 -> Paper
    # 3 -> Scissors

    pair_outcome_map = Dict{Tuple{Integer,Integer}, Integer}(
# rock, paper
        (1, 2) => 1,
# paper, rock
        (2, 1) => -1,
# rock, scissors
        (1, 3) => -1,
# scissors, rock
        (3, 1) => 1,
# paper, scissors
        (2, 3) => 1,
# scissors, paper
        (3, 2) => -1,
# equals
        (1, 1) => 0,
        (2, 2) => 0,
        (3, 3) => 0,
    )

    move_scores = [m[2] for m in moves]
    game_scores = [outcome_score_map[pair_outcome_map[m]] for m in moves]

    return sum(broadcast(+, move_scores, game_scores))
end

function Task1(guide)
    # Map move letters to move integers.
    move_map = Dict{String,Integer}(
        "A" => 1,
        "B" => 2,
        "C" => 3,
        "X" => 1,
        "Y" => 2,
        "Z" => 3,
    )

    moves = convert_with_map_1(guide, move_map)

    score = score_game(moves)

    println("Task1: ", score)
end

function Task2(guide)
    # Map move letters to move integers.
    move_map_1 = Dict{String,Integer}(
        "A" => 1,
        "B" => 2,
        "C" => 3,
    )

    # 1 -> Rock
    # 2 -> Paper
    # 3 -> Scissors
    move_map_2 = Dict{Tuple{Integer,String},Tuple{Integer,Integer}}(
        (1, "X") => (1, 3),
        (1, "Y") => (1, 1),
        (1, "Z") => (1, 2),
        (2, "X") => (2, 1),
        (2, "Y") => (2, 2),
        (2, "Z") => (2, 3),
        (3, "X") => (3, 2),
        (3, "Y") => (3, 3),
        (3, "Z") => (3, 1),
    )

    moves = convert_with_map_2(guide, move_map_1, move_map_2)

    score = score_game(moves)

    println("Task2: ", score)
end

# Get input filepath
input_filepath = parsed_args["input"]
verbose = parsed_args["verbose"]

guide = ProcessFile(input_filepath)

Task1(guide)
Task2(guide)
