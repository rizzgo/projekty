-- Knapsack problem
-- Module - Parser
-- Richard Seipel (xseipe00)
-- 2023

module Parser (
    parse
) where

import Data.Maybe (isJust)
import Types (Item(..), Knapsack(..))

parse :: String -> Maybe Knapsack
parse input = parseKnapsack slicedInput parsedItems 
    where
        slicedInput = map words $ lines input
        parsedItems = convertItems . parseItems $ drop 4 slicedInput
        
parseKnapsack :: [[String]] -> Maybe [Item] -> Maybe Knapsack
parseKnapsack input parsedItems = case (input, parsedItems) of
    (["Knapsack", "{"]:["maxWeight:", maxWeightValue]:["minCost:", minCostValue]:["items:", "["]:_, Just validItems) ->
        Just (Knapsack (read maxWeightValue) (read minCostValue) validItems)
    (["Knapsack", "{"]:["minCost:", minCostValue]:["maxWeight:", maxWeightValue]:["items:", "["]:_, Just validItems) ->
        Just (Knapsack (read maxWeightValue) (read minCostValue) validItems)
    _ -> Nothing
        
parseItems :: [[String]] -> [Maybe Item]
parseItems input = case input of
    (["Item", "{"]:["weight:", weightValue]:["cost:", costValue]:["}"]:remainingItems) ->
        Just (Item (read weightValue) (read costValue)) : parseItems remainingItems
    (["Item", "{"]:["cost:", costValue]:["weight:", weightValue]:["}"]:remainingItems) ->
        Just (Item (read weightValue) (read costValue)) : parseItems remainingItems
    (["]"]:["}"]:_) -> []
    _ -> [Nothing]

convertItems :: [Maybe Item] -> Maybe [Item]
convertItems parsedItems
    | null parsedItems = Just []
    | isJust (last parsedItems) = sequence parsedItems
    | otherwise = Nothing
