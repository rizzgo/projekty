-- Knapsack problem
-- Module - Types
-- Richard Seipel (xseipe00)
-- 2023

module Types  (
    Item(..),
    Knapsack(..),
    Solution
) where

import Data.List (intercalate)

type Solution = [Int]

data Item = Item {weight :: Int, cost :: Int} deriving (Eq, Ord)
data Knapsack = Knapsack {maxWeight :: Int, minCost :: Int, items :: [Item]}

instance Show Item where
    show item = unlines
        ["  Item {"
        ,"  weight: " ++ show (weight item)
        ,"  cost: " ++ show (cost item)
        ,"  }"
        ]

instance Show Knapsack where
    show knapsack = unlines
        ["Knapsack {"
        ,"maxWeight: " ++ show (maxWeight knapsack)
        ,"minCost: " ++ show (minCost knapsack)
        ,"items: ["
        ,intercalate "" (map (indent . show) (items knapsack)) ++ "]"
        ,"}"
        ]

indent :: String -> String
indent = unlines . map ("  " ++) . lines
