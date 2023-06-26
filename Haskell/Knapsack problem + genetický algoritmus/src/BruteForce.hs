-- Knapsack problem
-- Module - Brute force
-- Richard Seipel (xseipe00)
-- 2023

module BruteForce (
    solve
) where

import Types (Item(..), Knapsack(..), Solution)

type Cost = Int

solve :: Knapsack -> (Cost, Solution)
solve knapsack =
    convertItems (getSubSolution knapsack 0 0) $ items knapsack

getSubSolution :: Knapsack -> Int -> Int -> (Int, [Item])
getSubSolution (Knapsack _ _ []) _ costCount = (costCount, [])
getSubSolution knapsack weightCount costCount = maximum $ zipWith startsFrom [0..length (items knapsack) - 1] $ items knapsack
    where
        startsFrom index item =
            if weightCount + weight item <= maxWeight knapsack then
                let remainingItems = drop (1+index) $ items knapsack
                    (subSolutionCost, subSolutionList) =
                        getSubSolution (knapsack {items=remainingItems}) (weightCount + weight item) (costCount + cost item)
                in (subSolutionCost, item : subSolutionList)
            else
                (costCount, [])

convertItems :: (Cost, [Item]) -> [Item] -> (Cost, Solution)
convertItems (solutionCost, solutionItems) allItems = (solutionCost, map inSolution allItems) 
    where
        inSolution item
            | item `elem` solutionItems = 1
            | otherwise = 0
