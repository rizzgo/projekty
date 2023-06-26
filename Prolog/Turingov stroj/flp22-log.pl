% FLP - Logický projekt
% Turingov stroj
% Richard Seipel - xseipe00


:- dynamic rule/4, tm_input/1.

starts_with_upper(String) :- 
    atom_chars(String, [FirstChar|_]),
    char_type(FirstChar, upper).

remove_every_second([], []).
remove_every_second([H], [H]).
remove_every_second([H, _|T1], [H|T2]) :-
    remove_every_second(T1, T2).

process_line(Line) :-
    (starts_with_upper(Line) -> (
        atom_chars(Line, Chars),
        remove_every_second(Chars, Rule),
        length(Rule, 4),
        [State, Symbol, NState, NSymbol] = Rule,
        assertz(rule(State, Symbol, NState, NSymbol))
    ); (
        atom_chars(Line, TMInput),
        assertz(tm_input(TMInput))
    )).

process_input :-
    \+at_end_of_stream,
    read_line_to_codes(user_input, LineCodes),
    atom_codes(Line, LineCodes),
    process_line(Line),
    process_input.
process_input.

next_tape_config([], _, 'L', [], []).
next_tape_config(Left, Right, 'L', NLeft, [Moved|Right]) :- append(NLeft, [Moved], Left).
next_tape_config(Left, [Moved], 'R', NLeft, [' ']) :- append(Left, [Moved], NLeft).
next_tape_config(Left, [Moved|NRight], 'R', NLeft, NRight) :- append(Left, [Moved], NLeft).
next_tape_config(Left, [_|Right], Symbol, Left, [Symbol|Right]).

possible_configs(_, _, [], []).
possible_configs(
    Left, 
    Right, 
    [(_, _, NState, NSymbol)|Rules], 
    [(NState, NLeft, NRight)|Configs]
) :-
    next_tape_config(Left, Right, NSymbol, NLeft, NRight),
    possible_configs(Left, Right, Rules, Configs).

update_output(State, Left, Right, FinalConfigs, PossibleConfigs, NFinalConfigs) :- 
    [LastFinalConfig|_] = FinalConfigs,
    (member(LastFinalConfig, PossibleConfigs) -> (
        NFinalConfigs = [(State, Left, Right)|FinalConfigs]
    );(
        NFinalConfigs = FinalConfigs
    )).

next_step([('F', Left, Right)|_], [('F', Left, Right)]).
next_step([(State, Left, Right)|ConfigsToTry], NFinalConfigs) :-
    [Symbol|_] = Right,
    bagof((State, Symbol, NState, NSymbol), rule(State, Symbol, NState, NSymbol), Rules),
    possible_configs(Left, Right, Rules, PossibleConfigs),
    append(ConfigsToTry, PossibleConfigs, NConfigsToTry),
    next_step(NConfigsToTry, FinalConfigs),
    update_output(State, Left, Right, FinalConfigs, PossibleConfigs, NFinalConfigs).
next_step([_|ConfigsToTry], FinalConfigs) :-
    next_step(ConfigsToTry, FinalConfigs).

concat_configuration(State, Left, Right, Configuration) :-
    append([Left, [State], Right], ConfigArray),
    atomic_list_concat(ConfigArray, Configuration).

print_configurations([]).
print_configurations([(State, Left, Right)|Remaining]) :-
    concat_configuration(State, Left, Right, Configuration),
    writeln(Configuration),
    print_configurations(Remaining).

main :- 
    process_input,
    tm_input(TMInput), !,
    next_step([('S', [], TMInput)], Configurations),
    print_configurations(Configurations),
    halt.
